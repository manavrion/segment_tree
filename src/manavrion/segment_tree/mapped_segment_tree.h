//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the mapped_segment_tree header-only library.
//

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iterator>
#include <optional>
#include <type_traits>
#include <vector>

#include "manavrion/segment_tree/details.h"
#include "manavrion/segment_tree/functors.h"

namespace manavrion::segment_tree {

template <typename T, typename Reducer = std::plus<T>,
          typename Mapper = functors::deduce_mapper<T, Reducer>,
          typename Allocator = std::allocator<T>,
          typename TreeAllocator =
              std::allocator<std::decay_t<std::invoke_result_t<Mapper, T>>>>
class mapped_segment_tree : private Reducer, private Mapper {
  static_assert(std::is_invocable_v<Mapper, T>);
  using mapper_result = std::decay_t<std::invoke_result_t<Mapper, T>>;
  static_assert(std::is_invocable_v<Reducer, mapper_result, mapper_result>);
  static_assert(std::is_convertible_v<
                std::invoke_result_t<Reducer, mapper_result, mapper_result>,
                mapper_result>);

 public:
  using allocator_type = Allocator;
  using value_type = T;
  using container_type = std::vector<value_type, allocator_type>;
  using size_type = typename container_type::size_type;
  using difference_type = typename container_type::difference_type;
  using reference = typename container_type::reference;
  using const_reference = typename container_type::const_reference;
  using pointer = typename container_type::pointer;
  using const_pointer = typename container_type::const_pointer;
  using iterator = typename container_type::iterator;
  using const_iterator = typename container_type::const_iterator;
  using reverse_iterator = typename container_type::reverse_iterator;
  using const_reverse_iterator =
      typename container_type::const_reverse_iterator;

  using tree_allocator_type = TreeAllocator;
  using tree_value_type = mapper_result;
  using tree_container_type = std::vector<tree_value_type, tree_allocator_type>;
  using tree_size_type = typename tree_container_type::size_type;
  using tree_difference_type = typename tree_container_type::difference_type;
  using tree_reference = typename tree_container_type::reference;
  using tree_const_reference = typename tree_container_type::const_reference;
  using tree_pointer = typename tree_container_type::pointer;
  using tree_const_pointer = typename tree_container_type::const_pointer;
  using tree_iterator = typename tree_container_type::iterator;
  using tree_const_iterator = typename tree_container_type::const_iterator;
  using tree_reverse_iterator = typename tree_container_type::reverse_iterator;
  using tree_const_reverse_iterator =
      typename tree_container_type::const_reverse_iterator;

  using mapper_type = Mapper;
  using reducer_type = Reducer;

 private:
  const Reducer& reducer() const& { return *static_cast<const Reducer*>(this); }

  Reducer&& reducer() && { return std::move(*static_cast<Reducer*>(this)); }

  const Mapper& mapper() const& { return *static_cast<const Mapper*>(this); }

  Mapper&& mapper() && { return std::move(*static_cast<Mapper*>(this)); }

  struct scoped_rebuild {
    scoped_rebuild(mapped_segment_tree* that) : that(that) {}
    ~scoped_rebuild() { that->rebuild_tree(); }
    mapped_segment_tree* that;
  };

  size_t parent(size_t node_index) const {
    assert(node_index != 0);
    return (node_index - 1) / 2;
  }

  size_t left_child(size_t node_index) const { return node_index * 2 + 1; }
  size_t right_child(size_t node_index) const { return node_index * 2 + 2; }

  bool is_left_child(size_t node_index) const { return node_index % 2 != 0; }
  bool is_right_child(size_t node_index) const { return node_index % 2 == 0; }

  size_t shift_up(size_t shift) const { return shift / 2; }

  size_t left_data_child(size_t node_index) const {
    assert(node_index == std::clamp(node_index, shift_up(shift_), shift_ - 1));
    return left_child(node_index) - shift_;
  }
  size_t right_data_child(size_t node_index) const {
    assert(node_index == std::clamp(node_index, shift_up(shift_), shift_ - 1));
    return right_child(node_index) - shift_;
  }

  size_t parent_of_data(size_t data_index) const {
    assert(data_index < data_.size());
    return (data_index + shift_ - 1) / 2;
  }

  size_t get_shift(size_t n) const {
    if (n == 0) return 0;
    return std::pow(2, std::ceil(std::log2(n))) - 1;
  }

  size_t get_tree_size(size_t shift, size_t n) const { return (shift + n) / 2; }

  size_t get_tree_capacity(size_t shift) const { return shift; }

  void init_tree() {
    assert(tree_.empty());
    const size_t n = data_.size();

    shift_ = get_shift(n);
    tree_.resize(get_tree_size(shift_, n));
  }

  void rebuild_tree() {
    tree_.clear();
    build_tree();
  }

  // Creates segment tree nodes, time complexity - O(n).
  void build_tree() {
    init_tree();
    const size_t tree_size = tree_.size();
    const size_t data_size = data_.size();
    const auto& reduce = reducer();
    const auto& map = mapper();

    for (size_t i = shift_up(shift_); i < tree_size; ++i) {
      const size_t child_1 = left_data_child(i);
      const size_t child_2 = child_1 + 1;
      assert(child_2 == right_data_child(i));
      if (child_2 < data_size) {
        tree_[i] = reduce(map(data_[child_1]), map(data_[child_2]));
      } else if (child_1 < data_size) {
        tree_[i] = map(data_[child_1]);
      } else {
        assert(false);
      }
    }

    size_t last = tree_size ? tree_size - 1 : 0;
    size_t shift = shift_up(shift_);

    while (last != 0) {
      const size_t prev_last = last;
      last = parent(last);
      shift = shift_up(shift);
      for (size_t i = shift; i <= last; ++i) {
        const size_t child_1 = left_child(i);
        const size_t child_2 = child_1 + 1;
        assert(child_2 == right_child(i));
        if (child_2 <= prev_last) {
          tree_[i] = reduce(tree_[child_1], tree_[child_2]);
        } else if (child_1 <= prev_last) {
          tree_[i] = tree_[child_1];
        }
      }
    }
  }

  // Updates unique element.
  // Time complexity - O(log n).
  void update(size_t i) {
    if (data_.size() == 1) {
      assert(tree_.empty());
      return;
    }
    const size_t data_size = data_.size();
    const size_t tree_size = tree_.size();
    const auto& reduce = reducer();
    const auto& map = mapper();

    assert(i < data_size);
    i = parent_of_data(i);
    assert(i < tree_size);

    const size_t child_1 = left_data_child(i);
    const size_t child_2 = child_1 + 1;
    assert(child_2 == right_data_child(i));
    if (child_2 < data_size) {
      tree_[i] = reduce(map(data_[child_1]), map(data_[child_2]));
    } else if (child_1 < data_size) {
      tree_[i] = map(data_[child_1]);
    } else {
      assert(true);
    }

    while (i != 0) {
      i = parent(i);
      const size_t child_1 = left_child(i);
      const size_t child_2 = child_1 + 1;
      assert(child_2 == right_child(i));
      if (child_2 < tree_size) {
        tree_[i] = reduce(tree_[child_1], tree_[child_2]);
      } else {
        assert(child_1 < tree_size);
        tree_[i] = tree_[child_1];
      }
    }
  }

  // Time complexity - O(min(n, k log n)) where k is (last_index - first_index).
  void update_range(size_t first_index, size_t last_index) {
    assert(first_index <= last_index);
    const size_t n = data_.size();
    const size_t k = last_index - first_index;
    const bool make_rebuild = n < 1000 || n < k * std::log(n);
    if (make_rebuild) {
      rebuild_tree();
    } else {
      while (first_index != last_index) {
        update(first_index++);
      }
    }
  }

  // Make a query on [first_index, last_index) segment.
  // Time complexity - O(log n).
  tree_value_type query_impl(size_t first_index, size_t last_index) const {
    assert(first_index <= last_index);
    assert(last_index <= data_.size());

    const auto& reduce = reducer();
    const auto& map = mapper();

    std::optional<tree_value_type> result;
    auto add_result = [&](const auto& value) {
      if (result) {
        result.emplace(reduce(std::move(*result), value));
      } else {
        result.emplace(value);
      }
    };

    if (first_index < last_index && first_index % 2 != 0) {
      assert(first_index < data_.size());
      add_result(map(data_[first_index]));
      ++first_index;
    }

    if (first_index < last_index && last_index % 2 != 0) {
      assert(last_index - 1 < data_.size());
      add_result(map(data_[last_index - 1]));
      --last_index;
    }

    first_index /= 2;
    last_index /= 2;
    size_t shift = shift_up(shift_);

    while (first_index < last_index) {
      if (first_index < last_index && is_right_child(shift + first_index)) {
        assert(shift + first_index < tree_.size());
        add_result(tree_[shift + first_index]);
        ++first_index;
      }
      if (first_index < last_index && is_left_child(shift + last_index - 1)) {
        assert(shift + last_index - 1 < tree_.size());
        add_result(tree_[shift + last_index - 1]);
        --last_index;
      }
      if (first_index + 1 == last_index) {
        assert(shift + first_index < tree_.size());
        add_result(tree_[shift + first_index]);
        break;
      }
      first_index /= 2;
      last_index /= 2;
      shift /= 2;
    }

    if (!result) {
      result.emplace();
    }
    return std::move(*result);
  }

 public:
  mapped_segment_tree() = default;

  explicit mapped_segment_tree(const Allocator& allocator,
                               const TreeAllocator& tree_allocator = {})
      : data_(allocator), tree_(tree_allocator) {}

  explicit mapped_segment_tree(Reducer reducer, Mapper mapper = {},
                               const Allocator& allocator = {},
                               const TreeAllocator& tree_allocator = {})
      : Reducer(std::move(reducer)),
        Mapper(std::move(mapper)),
        data_(allocator),
        tree_(tree_allocator) {}

  template <typename InputIt, typename = details::require_input_iter<InputIt>>
  mapped_segment_tree(InputIt first, InputIt last, Reducer reducer = {},
                      Mapper mapper = {}, const Allocator& allocator = {},
                      const TreeAllocator& tree_allocator = {})
      : Reducer(std::move(reducer)),
        Mapper(std::move(mapper)),
        data_(first, last, allocator),
        tree_(tree_allocator) {
    build_tree();
  }

  // Time complexity - O(n).
  template <typename InputIt, typename = details::require_input_iter<InputIt>>
  mapped_segment_tree(InputIt first, InputIt last, const Allocator& allocator,
                      const TreeAllocator& tree_allocator = {})
      : data_(first, last, allocator), tree_(tree_allocator) {
    build_tree();
  }

  // Time complexity - O(n).
  mapped_segment_tree(const mapped_segment_tree& other,
                      const Allocator& allocator = {},
                      const TreeAllocator& tree_allocator = {})
      : Reducer(other.reducer()),
        Mapper(other.mapper()),
        data_(other.data_, allocator),
        tree_(tree_allocator) {
    build_tree();
  }

  mapped_segment_tree(mapped_segment_tree&& other,
                      const Allocator& allocator = {},
                      const TreeAllocator& tree_allocator = {}) noexcept
      : Reducer(std::move(other).reducer()),
        Mapper(std::move(other).mapper()),
        data_(std::move(other.data_), allocator),
        tree_(std::move(other.tree_), tree_allocator),
        shift_(other.shift_) {}

  // Time complexity - O(n).
  mapped_segment_tree(std::initializer_list<T> init_list, Reducer reducer = {},
                      Mapper mapper = {}, const Allocator& allocator = {},
                      const TreeAllocator& tree_allocator = {})
      : Reducer(std::move(reducer)),
        Mapper(std::move(mapper)),
        data_(init_list, allocator),
        tree_(tree_allocator) {
    build_tree();
  }

  // Time complexity - O(n).
  mapped_segment_tree(std::initializer_list<T> init_list,
                      const Allocator& allocator,
                      const TreeAllocator& tree_allocator = {})
      : data_(init_list, allocator), tree_(tree_allocator) {
    build_tree();
  }

  // Time complexity - O(n).
  mapped_segment_tree& operator=(const mapped_segment_tree& other) {
    data_ = other.data_;
    rebuild_tree();
    return *this;
  }

  mapped_segment_tree& operator=(mapped_segment_tree&& other) = default;

  // Time complexity - O(n).
  mapped_segment_tree& operator=(std::initializer_list<T> init_list) {
    data_ = init_list;
    rebuild_tree();
    return *this;
  }

  // Time complexity - O(n). Can be reduced to O(1) with dynamic_segment_tree.
  void assign(size_type count, const T& value) {
    data_.assign(count, value);
    rebuild_tree();
  }

  // Time complexity - O(n).
  template <class InputIt, typename = details::require_input_iter<InputIt>>
  void assign(InputIt first, InputIt last) {
    data_.assign(first, last);
    rebuild_tree();
  }

  // Time complexity - O(n).
  void assign(std::initializer_list<T> init_list) { operator=(init_list); }

  // Time complexity - O(1).
  [[nodiscard]] allocator_type get_allocator() const noexcept {
    return data_.get_allocator();
  }

  // Time complexity - O(1).
  [[nodiscard]] const_reference at(size_type pos) const {
    return data_.at(pos);
  }

  // Time complexity - O(1).
  [[nodiscard]] const_reference operator[](size_type pos) const {
    assert(pos < data_.size());
    return data_[pos];
  }

  // Time complexity - O(1).
  [[nodiscard]] const_reference front() const { return data_.front(); }

  // Time complexity - O(1).
  [[nodiscard]] const_reference back() const { return data_.back(); }

  // Time complexity - O(1).
  [[nodiscard]] const T* data() const noexcept { return data_.data(); }

  // Time complexity - O(1).
  [[nodiscard]] iterator begin() noexcept { return data_.begin(); }

  // Time complexity - O(1).
  [[nodiscard]] const_iterator begin() const noexcept { return data_.begin(); }

  // Time complexity - O(1).
  [[nodiscard]] const_iterator cbegin() const noexcept {
    return data_.cbegin();
  }

  // Time complexity - O(1).
  [[nodiscard]] iterator end() noexcept { return data_.end(); }

  // Time complexity - O(1).
  [[nodiscard]] const_iterator end() const noexcept { return data_.end(); }

  // Time complexity - O(1).
  [[nodiscard]] const_iterator cend() const noexcept { return data_.cend(); }

  // Time complexity - O(1).
  [[nodiscard]] reverse_iterator rbegin() noexcept { return data_.rbegin(); }

  // Time complexity - O(1).
  [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
    return data_.rbegin();
  }

  // Time complexity - O(1).
  [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
    return data_.crbegin();
  }

  // Time complexity - O(1).
  [[nodiscard]] reverse_iterator rend() noexcept { return data_.rend(); }

  // Time complexity - O(1).
  [[nodiscard]] const_reverse_iterator rend() const noexcept {
    return data_.rend();
  }

  // Time complexity - O(1).
  [[nodiscard]] const_reverse_iterator crend() const noexcept {
    return data_.crend();
  }

  // Time complexity - O(1).
  [[nodiscard]] bool empty() const noexcept {
    if (data_.empty()) {
      assert(tree_.empty());
    }
    return data_.empty();
  }

  // Time complexity - O(1).
  [[nodiscard]] size_type size() const noexcept { return data_.size(); }

  void reserve(size_t n) {
    data_.reserve(n);
    tree_.reserve(get_tree_capacity(get_shift(n)));
  }

  // Time complexity - O(1).
  [[nodiscard]] size_type max_size() const noexcept { return data_.max_size(); }

  // Time complexity - O(n).
  void clear() noexcept {
    data_.clear();
    tree_.clear();
    assert(empty());
  }

  // Time complexity - O(n).
  const_iterator insert(const_iterator pos, const T& value) {
    scoped_rebuild scoped{this};
    return data_.insert(pos, value);
  }

  // Time complexity - O(n).
  const_iterator insert(const_iterator pos, T&& value) {
    scoped_rebuild scoped{this};
    return data_.insert(pos, std::move(value));
  }

  // Time complexity - O(n).
  const_iterator insert(const_iterator pos, size_type count, const T& value) {
    scoped_rebuild scoped{this};
    return data_.insert(pos, count, value);
  }

  // Time complexity - O(n).
  template <typename InputIt, typename = details::require_input_iter<InputIt>>
  const_iterator insert(const_iterator pos, InputIt first, InputIt last) {
    scoped_rebuild scoped{this};
    return data_.insert(pos, first, last);
  }

  // Time complexity - O(n).
  const_iterator insert(const_iterator pos,
                        std::initializer_list<T> init_list) {
    scoped_rebuild scoped{this};
    return data_.insert(pos, init_list);
  }

  // Time complexity - O(n).
  template <typename... Args>
  const_iterator emplace(const_iterator pos, Args&&... args) {
    scoped_rebuild scoped{this};
    return data_.emplace(pos, std::forward<Args>(args)...);
  }

  // Time complexity - O(n).
  const_iterator erase(const_iterator pos) {
    scoped_rebuild scoped{this};
    return data_.erase(pos);
  }

  // Time complexity - O(n).
  const_iterator erase(const_iterator first, const_iterator last) {
    scoped_rebuild scoped{this};
    return data_.erase(first, last);
  }

  // Time complexity - O(n).
  void resize(size_type count) {
    data_.resize(count);
    rebuild_tree();
  }

  // Time complexity - O(1).
  void swap(mapped_segment_tree& other) noexcept {
    auto tmp = std::move(other);
    other = std::move(*this);
    *this = std::move(tmp);
  }

  // Time complexity - O(log n).
  template <typename V>
  void update(size_t index, V&& v) {
    data_[index] = std::forward<V>(v);
    update(index);
  }

  // Make a query on [first_index, last_index) segment.
  // Time complexity - O(log n).
  [[nodiscard]] tree_value_type query(size_t first_index,
                                      size_t last_index) const {
    return query_impl(first_index, last_index);
  }

  // Time complexity - O(min(n, k log n)) where k is (last_index - first_index).
  void update_range(const_iterator first, const_iterator last) {
    update_range(std::distance(data_.cbegin(), first),
                 std::distance(data_.cbegin(), last));
  }

  template <typename T1, typename T2, typename R, typename M, typename A>
  friend bool operator==(const mapped_segment_tree<T1, R, M, A>& lhs,
                         const mapped_segment_tree<T2, R, M, A>& rhs);

  template <typename T1, typename T2, typename R, typename M, typename A>
  friend bool operator!=(const mapped_segment_tree<T1, R, M, A>& lhs,
                         const mapped_segment_tree<T2, R, M, A>& rhs);

  template <typename T1, typename T2, typename R, typename M, typename A>
  friend bool operator<(const mapped_segment_tree<T1, R, M, A>& lhs,
                        const mapped_segment_tree<T2, R, M, A>& rhs);

  template <typename T1, typename T2, typename R, typename M, typename A>
  friend bool operator<=(const mapped_segment_tree<T1, R, M, A>& lhs,
                         const mapped_segment_tree<T2, R, M, A>& rhs);

  template <typename T1, typename T2, typename R, typename M, typename A>
  friend bool operator>(const mapped_segment_tree<T1, R, M, A>& lhs,
                        const mapped_segment_tree<T2, R, M, A>& rhs);

  template <typename T1, typename T2, typename R, typename M, typename A>
  friend bool operator>=(const mapped_segment_tree<T1, R, M, A>& lhs,
                         const mapped_segment_tree<T2, R, M, A>& rhs);

 private:
  std::vector<value_type, allocator_type> data_;

  std::vector<tree_value_type, tree_allocator_type> tree_;
  size_t shift_ = 0;
};

template <typename T1, typename T2, typename R, typename M, typename A>
bool operator==(const mapped_segment_tree<T1, R, M, A>& lhs,
                const mapped_segment_tree<T2, R, M, A>& rhs) {
  return lhs.data_ == rhs.data_;
}

template <typename T1, typename T2, typename R, typename M, typename A>
bool operator!=(const mapped_segment_tree<T1, R, M, A>& lhs,
                const mapped_segment_tree<T2, R, M, A>& rhs) {
  return lhs.data_ != rhs.data_;
}

template <typename T1, typename T2, typename R, typename M, typename A>
bool operator<(const mapped_segment_tree<T1, R, M, A>& lhs,
               const mapped_segment_tree<T2, R, M, A>& rhs) {
  return lhs.data_ < rhs.data_;
}

template <typename T1, typename T2, typename R, typename M, typename A>
bool operator<=(const mapped_segment_tree<T1, R, M, A>& lhs,
                const mapped_segment_tree<T2, R, M, A>& rhs) {
  return lhs.data_ <= rhs.data_;
}

template <typename T1, typename T2, typename R, typename M, typename A>
bool operator>(const mapped_segment_tree<T1, R, M, A>& lhs,
               const mapped_segment_tree<T2, R, M, A>& rhs) {
  return lhs.data_ > rhs.data_;
}

template <typename T1, typename T2, typename R, typename M, typename A>
bool operator>=(const mapped_segment_tree<T1, R, M, A>& lhs,
                const mapped_segment_tree<T2, R, M, A>& rhs) {
  return lhs.data_ >= rhs.data_;
}

}  // namespace manavrion::segment_tree
