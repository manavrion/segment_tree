//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
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

namespace manavrion::segment_tree {

template <typename T, typename Reducer = std::plus<T>,
          typename Allocator = std::allocator<T>>
class segment_tree : private Reducer {
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

  using reducer_type = Reducer;

 private:
  const Reducer& reducer() const& { return *static_cast<const Reducer*>(this); }
  Reducer&& reducer() && { return std::move(*static_cast<Reducer*>(this)); }

  struct scoped_rebuild {
    scoped_rebuild(segment_tree* that) : that(that) {}
    ~scoped_rebuild() { that->rebuild_tree(); }
    segment_tree* that;
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

  size_t get_shift(size_t n) const {
    if (n == 0) return 0;
    return std::pow(2, std::ceil(std::log2(n))) - 1;
  }

  size_t get_tree_size(size_t shift, size_t n) const { return shift + n; }

  size_t get_tree_capacity(size_t shift) const { return left_child(shift); }

  void init_tree_impl(size_t n) {
    assert(tree_.empty());

    shift_ = get_shift(n);
    tree_.resize(get_tree_size(shift_, n));
  }

  template <typename InputIt>
  void init_tree(InputIt first, InputIt last) {
    init_tree_impl(std::distance(first, last));
    std::copy(first, last, std::next(tree_.begin(), shift_));
  }

  void init_tree(size_t n, const T& value) {
    init_tree_impl(n);
    std::fill(std::next(tree_.begin(), shift_), tree_.end(), value);
  }

  // Creates segment tree nodes, time complexity - O(n).
  void build_tree() {
    assert(!tree_.empty());
    const size_t tree_size = tree_.size();
    const auto& reduce = reducer();

    size_t last = tree_size ? tree_size - 1 : 0;
    size_t shift = shift_;
    assert(shift <= last);

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
    const size_t tree_size = tree_.size();
    const auto& reduce = reducer();

    i += shift_;
    assert(i < tree_size);

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
    const size_t n = tree_.size() - shift_;
    const size_t k = last_index - first_index;
    const bool make_rebuild = n < 1000 || n < k * std::log(n);
    if (make_rebuild) {
      build_tree();
    } else {
      while (first_index != last_index) {
        update(first_index++);
      }
    }
  }

  // Make a query on [first_index, last_index) segment.
  // Time complexity - O(log n).
  T query_impl(size_t first_index, size_t last_index) const {
    assert(first_index <= last_index);
    assert(last_index + shift_ <= tree_.size());

    const auto& reduce = reducer();

    std::optional<T> result;
    auto add_result = [&](const auto& value) {
      if (result) {
        result.emplace(reduce(std::move(*result), value));
      } else {
        result.emplace(value);
      }
    };

    size_t shift = shift_;

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
  segment_tree() = default;

  explicit segment_tree(const Allocator& allocator) : tree_(allocator) {}

  explicit segment_tree(Reducer reducer, const Allocator& allocator = {})
      : Reducer(std::move(reducer)), tree_(allocator) {}

  template <typename InputIt, typename = details::require_input_iter<InputIt>>
  segment_tree(InputIt first, InputIt last, Reducer reducer = {},
               const Allocator& allocator = {})
      : Reducer(std::move(reducer)), tree_(allocator) {
    init_tree(first, last);
    build_tree();
  }

  // Time complexity - O(n).
  template <typename InputIt, typename = details::require_input_iter<InputIt>>
  segment_tree(InputIt first, InputIt last, const Allocator& allocator)
      : tree_(allocator) {
    init_tree(first, last);
    build_tree();
  }

  // Time complexity - O(n).
  segment_tree(const segment_tree& other, const Allocator& allocator = {})
      : Reducer(other.reducer()),
        tree_(other.tree_, allocator),
        shift_(other.shift_) {}

  segment_tree(segment_tree&& other, const Allocator& allocator = {}) noexcept
      : Reducer(std::move(other).reducer()),
        tree_(std::move(other.tree_), allocator),
        shift_(other.shift_) {}

  // Time complexity - O(n).
  segment_tree(std::initializer_list<T> init_list, Reducer reducer = {},
               const Allocator& allocator = {})
      : Reducer(std::move(reducer)), tree_(allocator) {
    init_tree(init_list.begin(), init_list.end());
    build_tree();
  }

  // Time complexity - O(n).
  segment_tree(std::initializer_list<T> init_list, const Allocator& allocator)
      : tree_(allocator) {
    init_tree(init_list.begin(), init_list.end());
    build_tree();
  }

  // Time complexity - O(n).
  segment_tree& operator=(const segment_tree& other) = default;
  segment_tree& operator=(segment_tree&& other) = default;

  // Time complexity - O(n).
  segment_tree& operator=(std::initializer_list<T> init_list) {
    init_tree(init_list.begin(), init_list.end());
    build_tree();
    return *this;
  }

  // Time complexity - O(n). Can be reduced to O(1) with dynamic_segment_tree.
  void assign(size_type count, const T& value) {
    init_tree(count, value);
    build_tree();
  }

  // Time complexity - O(n).
  template <class InputIt, typename = details::require_input_iter<InputIt>>
  void assign(InputIt first, InputIt last) {
    init_tree(first, last);
    build_tree();
  }

  // Time complexity - O(n).
  void assign(std::initializer_list<T> init_list) { operator=(init_list); }

  // Time complexity - O(1).
  [[nodiscard]] allocator_type get_allocator() const noexcept {
    return tree_.get_allocator();
  }

  // Time complexity - O(1).
  [[nodiscard]] const_reference at(size_type pos) const {
    return tree_.at(pos + shift_);
  }

  // Time complexity - O(1).
  [[nodiscard]] const_reference operator[](size_type pos) const {
    assert(pos + shift_ < tree_.size());
    return tree_[pos + shift_];
  }

  // Time complexity - O(1).
  [[nodiscard]] const_reference front() const { return tree_[shift_]; }

  // Time complexity - O(1).
  [[nodiscard]] const_reference back() const { return tree_.back(); }

  // Time complexity - O(1).
  [[nodiscard]] const T* data() const noexcept { return tree_.data() + shift_; }

  // Time complexity - O(1).
  [[nodiscard]] iterator begin() noexcept { return tree_.begin() + shift_; }

  // Time complexity - O(1).
  [[nodiscard]] const_iterator begin() const noexcept {
    return tree_.begin() + shift_;
  }

  // Time complexity - O(1).
  [[nodiscard]] const_iterator cbegin() const noexcept {
    return tree_.cbegin() + shift_;
  }

  // Time complexity - O(1).
  [[nodiscard]] iterator end() noexcept { return tree_.end(); }

  // Time complexity - O(1).
  [[nodiscard]] const_iterator end() const noexcept { return tree_.end(); }

  // Time complexity - O(1).
  [[nodiscard]] const_iterator cend() const noexcept { return tree_.cend(); }

  // Time complexity - O(1).
  [[nodiscard]] reverse_iterator rbegin() noexcept { return tree_.rbegin(); }

  // Time complexity - O(1).
  [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
    return tree_.rbegin();
  }

  // Time complexity - O(1).
  [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
    return tree_.crbegin();
  }

  // Time complexity - O(1).
  [[nodiscard]] reverse_iterator rend() noexcept {
    return tree_.rend() - shift_;
  }

  // Time complexity - O(1).
  [[nodiscard]] const_reverse_iterator rend() const noexcept {
    return tree_.rend() - shift_;
  }

  // Time complexity - O(1).
  [[nodiscard]] const_reverse_iterator crend() const noexcept {
    return tree_.crend() - shift_;
  }

  // Time complexity - O(1).
  [[nodiscard]] bool empty() const noexcept { return tree_.empty(); }

  // Time complexity - O(1).
  [[nodiscard]] size_type size() const noexcept {
    return tree_.size() - shift_;
  }

  // Time complexity - O(1).
  [[nodiscard]] size_type max_size() const noexcept { return tree_.max_size(); }

  // Time complexity - O(n).
  void clear() noexcept { tree_.clear(); }
#if 1
  // Time complexity - O(n).
  const_iterator insert(const_iterator pos, const T& value) {
    // scoped_rebuild scoped{this};
    return tree_.insert(pos, value);
  }

  // Time complexity - O(n).
  const_iterator insert(const_iterator pos, T&& value) {
    // scoped_rebuild scoped{this};
    return tree_.insert(pos, std::move(value));
  }

  // Time complexity - O(n).
  const_iterator insert(const_iterator pos, size_type count, const T& value) {
    // scoped_rebuild scoped{this};
    return tree_.insert(pos, count, value);
  }

  // Time complexity - O(n).
  template <typename InputIt, typename = details::require_input_iter<InputIt>>
  const_iterator insert(const_iterator pos, InputIt first, InputIt last) {
    // scoped_rebuild scoped{this};
    return tree_.insert(pos, first, last);
  }

  // Time complexity - O(n).
  const_iterator insert(const_iterator pos,
                        std::initializer_list<T> init_list) {
    // scoped_rebuild scoped{this};
    return tree_.insert(pos, init_list);
  }

  // Time complexity - O(n).
  template <typename... Args>
  const_iterator emplace(const_iterator pos, Args&&... args) {
    // scoped_rebuild scoped{this};
    return tree_.emplace(pos, std::forward<Args>(args)...);
  }

  // Time complexity - O(n).
  const_iterator erase(const_iterator pos) {
    // scoped_rebuild scoped{this};
    return tree_.erase(pos);
  }

  // Time complexity - O(n).
  const_iterator erase(const_iterator first, const_iterator last) {
    // scoped_rebuild scoped{this};
    return tree_.erase(first, last);
  }

  // Time complexity - O(n).
  void resize(size_type count) {
    tree_.resize(count);
    // rebuild_tree();
  }
#endif

  void reserve(size_type size) { tree_.reserve(get_tree_capacity(size)); }

  // Time complexity - O(1).
  void swap(segment_tree& other) noexcept {
    auto tmp = std::move(other);
    other = std::move(*this);
    *this = std::move(tmp);
  }

  // Time complexity - O(log n).
  template <typename V>
  void update(size_t index, V&& v) {
    tree_[index + shift_] = std::forward<V>(v);
    update(index);
  }

  // Make a query on [first_index, last_index) segment.
  // Time complexity - O(log n).
  [[nodiscard]] T query(size_t first_index, size_t last_index) const {
    return query_impl(first_index, last_index);
  }

  // Time complexity - O(min(n, k log n)) where k is (last_index - first_index).
  void update_range(const_iterator first, const_iterator last) {
    update_range(std::distance(cbegin(), first), std::distance(cbegin(), last));
  }

  template <typename T1, typename T2, typename R, typename A>
  friend bool operator==(const segment_tree<T1, R, A>& lhs,
                         const segment_tree<T2, R, A>& rhs);

  template <typename T1, typename T2, typename R, typename A>
  friend bool operator!=(const segment_tree<T1, R, A>& lhs,
                         const segment_tree<T2, R, A>& rhs);

  template <typename T1, typename T2, typename R, typename A>
  friend bool operator<(const segment_tree<T1, R, A>& lhs,
                        const segment_tree<T2, R, A>& rhs);

  template <typename T1, typename T2, typename R, typename A>
  friend bool operator<=(const segment_tree<T1, R, A>& lhs,
                         const segment_tree<T2, R, A>& rhs);

  template <typename T1, typename T2, typename R, typename A>
  friend bool operator>(const segment_tree<T1, R, A>& lhs,
                        const segment_tree<T2, R, A>& rhs);

  template <typename T1, typename T2, typename R, typename A>
  friend bool operator>=(const segment_tree<T1, R, A>& lhs,
                         const segment_tree<T2, R, A>& rhs);

 private:
  std::vector<T, Allocator> tree_;
  size_t shift_ = 0;
};

template <typename T1, typename T2, typename R, typename A>
bool operator==(const segment_tree<T1, R, A>& lhs,
                const segment_tree<T2, R, A>& rhs) {
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T1, typename T2, typename R, typename A>
bool operator!=(const segment_tree<T1, R, A>& lhs,
                const segment_tree<T2, R, A>& rhs) {
  return !(lhs == rhs);
}

template <typename T1, typename T2, typename R, typename A>
bool operator<(const segment_tree<T1, R, A>& lhs,
               const segment_tree<T2, R, A>& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T1, typename T2, typename R, typename A>
bool operator<=(const segment_tree<T1, R, A>& lhs,
                const segment_tree<T2, R, A>& rhs) {
  return (lhs < rhs) || (lhs == rhs);
}

template <typename T1, typename T2, typename R, typename A>
bool operator>(const segment_tree<T1, R, A>& lhs,
               const segment_tree<T2, R, A>& rhs) {
  return !(lhs <= rhs);
}

template <typename T1, typename T2, typename R, typename A>
bool operator>=(const segment_tree<T1, R, A>& lhs,
                const segment_tree<T2, R, A>& rhs) {
  return !(lhs < rhs);
}

}  // namespace manavrion::segment_tree
