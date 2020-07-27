//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <cassert>
#include <cmath>
#include <functional>
#include <memory>
#include <numeric>
#include <queue>
#include <range/v3/view/filter.hpp>
#include <type_traits>
#include <vector>

#include "details.h"
#include "functors.h"

namespace manavrion::segment_tree {

template <typename T, typename Reducer = functors::default_reducer,
          typename Mapper = functors::deduce_mapper<T, Reducer>,
          typename Allocator = std::allocator<T>>
class node_based_segment_tree {
  static_assert(std::is_invocable_v<Mapper, T>);
  using mapper_result = std::decay_t<std::invoke_result_t<Mapper, T>>;
  static_assert(std::is_invocable_v<Reducer, mapper_result, mapper_result>);
  static_assert(std::is_convertible_v<
                std::invoke_result_t<Reducer, mapper_result, mapper_result>,
                mapper_result>);

  using container_type = std::vector<T, Allocator>;

 public:
  using value_type = T;
  using allocator_type = Allocator;
  using size_type = typename container_type::size_type;
  using difference_type = typename container_type::difference_type;
  using reference = typename container_type::reference;
  using const_reference = typename container_type::const_reference;
  using pointer = typename container_type::pointer;
  using const_pointer = typename container_type::const_pointer;

  using const_iterator = typename container_type::const_iterator;
  using const_reverse_iterator =
      typename container_type::const_reverse_iterator;

  // segment_tree specific.
  using reduced_type = mapper_result;
  using mapper_type = Mapper;
  using reducer_type = Reducer;

 private:
  using iterator = typename container_type::iterator;
  using reverse_iterator = typename container_type::reverse_iterator;

  iterator remove_const(const const_iterator it) {
    return std::next(data_.begin(), std::distance(data_.begin(), it));
  }

  struct node_t {
   public:
    reduced_type value;

   private:
    size_t first_index_;
    size_t last_index_;
    node_t* parent_ = nullptr;
    std::unique_ptr<node_t> left_;
    std::unique_ptr<node_t> right_;

   public:
    node_t(reduced_type value, size_t first_index, size_t last_index)
        : value(std::move(value)),
          first_index_(first_index),
          last_index_(last_index) {
      assert(first_index_ < last_index_);
    }

    node_t(std::unique_ptr<node_t> left, std::unique_ptr<node_t> right,
           const Reducer& reducer)
        : value(reducer(left->value, right->value)),
          first_index_(left->first_index_),
          last_index_(right->last_index_),
          left_(std::move(left)),
          right_(std::move(right)) {
      // Nodes must be adjacent.
      assert(left_->last_index_ == right_->first_index_);
      assert(first_index_ < last_index_);
      assert(left_ && right_);
      left_->parent_ = this;
      right_->parent_ = this;
    }

    size_t first_index() const { return first_index_; }
    size_t last_index() const { return last_index_; }
    node_t* parent() const { return parent_; }
    size_t size() const { return last_index_ - first_index_; }

    bool is_leaf() const {
      bool result = size() <= 2;
      if (result) {
        assert(!left_);
        assert(!right_);
      }
      return result;
    }

    bool is_part_of(size_t first, size_t last) const {
      assert(first <= last);
      return first <= first_index_ && last_index_ <= last;
    }

    auto childs() { return std::array<node_t*, 2>{left_.get(), right_.get()}; }
  };

  void rebuild_tree() {
    tails_.clear();
    head_.reset();
    build_tree();
  }

  // Creates segment tree nodes, time complexity - O(n).
  void build_tree() {
    assert(tails_.empty());
    assert(!head_);

    const size_t tail_size = data_.size() / 2 + bool(data_.size() % 2);
    std::vector<std::unique_ptr<node_t>> line;
    line.reserve(tail_size);
    tails_.reserve(tail_size);

    for (size_t i = 0; i < data_.size(); i += 2) {
      if (i + 1 < data_.size()) {
        line.emplace_back(std::make_unique<node_t>(
            reducer_(mapper_(data_[i]), mapper_(data_[i + 1])), i, i + 2));
      } else {
        line.emplace_back(
            std::make_unique<node_t>(mapper_(data_[i]), i, i + 1));
      }
      tails_.emplace_back(line.back().get());
    }

    assert(tails_.size() == tail_size);

    while (line.size() > 1) {
      std::vector<std::unique_ptr<node_t>> new_line;
      new_line.reserve(line.size() / 2 + bool(line.size() % 2));
      for (size_t i = 0; i < line.size(); i += 2) {
        if (i + 1 < line.size()) {
          new_line.emplace_back(std::make_unique<node_t>(
              std::move(line[i]), std::move(line[i + 1]), reducer_));
        } else {
          new_line.emplace_back(std::move(line[i]));
        }
      }
      line = std::move(new_line);
    }
    if (!line.empty()) {
      assert(line.size() == 1);
      head_ = std::move(line.front());
    }
  }

  // Updates unique element.
  // Time complexity - O(log n).
  void update(size_t index) {
    const size_t tail_size = data_.size() / 2 + bool(data_.size() % 2);
    assert(tails_.size() == tail_size);

    assert(index < data_.size());
    if (tails_.empty()) {
      assert(data_.size() <= 1);
      return;
    }
    const size_t tail_index = index / 2;
    assert(tail_index < tails_.size());
    node_t* node = tails_[tail_index];
    assert(node->is_leaf());
    // TODO: Do it with ranges!
    if (node->size() == 2) {
      node->value = reducer_(mapper_(data_[node->first_index()]),
                             mapper_(data_[node->first_index() + 1]));
    } else {
      node->value = mapper_(data_[node->first_index()]);
    }
    node = node->parent();
    while (node) {
      auto childs = node->childs();
      node->value = reducer_(childs[0]->value, childs[1]->value);
      node = node->parent();
    }
  }

  // Time complexity - O(min(n, k log n)) where k is (last_index - first_index).
  void update(size_t first_index, size_t last_index) {
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

  void update(const_iterator first, const_iterator last) {
    update(std::distance(data_.begin(), first),
           std::distance(data_.begin(), last));
  }

 public:
  node_based_segment_tree() = default;

  explicit node_based_segment_tree(const Allocator& allocator)
      : data_(allocator) {}

  explicit node_based_segment_tree(Reducer reducer, Mapper mapper = {},
                                   const Allocator& allocator = {})
      : reducer_(std::move(reducer)),
        mapper_(std::move(mapper)),
        data_(allocator) {}

  template <typename InputIt, typename = details::require_input_iter<InputIt>>
  node_based_segment_tree(InputIt first, InputIt last, Reducer reducer = {},
                          Mapper mapper = {}, const Allocator& allocator = {})
      : reducer_(std::move(reducer)),
        mapper_(std::move(mapper)),
        data_(first, last, allocator) {
    build_tree();
  }

  // Time complexity - O(n).
  template <typename InputIt, typename = details::require_input_iter<InputIt>>
  node_based_segment_tree(InputIt first, InputIt last,
                          const Allocator& allocator)
      : data_(first, last, allocator) {
    build_tree();
  }

  // Time complexity - O(n).
  node_based_segment_tree(const node_based_segment_tree& other,
                          const Allocator& allocator = {})
      : reducer_(other.reducer_),
        mapper_(other.mapper_),
        data_(other.data_, allocator) {
    build_tree();
  }

  node_based_segment_tree(node_based_segment_tree&& other,
                          const Allocator& allocator = {}) noexcept
      : reducer_(std::move(other.reducer_)),
        mapper_(std::move(other.mapper_)),
        data_(std::move(other.data_), allocator),
        head_(std::move(other.head_)),
        tails_(std::move(other.tails_)) {}

  // Time complexity - O(n).
  node_based_segment_tree(std::initializer_list<T> init_list,
                          Reducer reducer = {}, Mapper mapper = {},
                          const Allocator& alloc = {})
      : reducer_(std::move(reducer)),
        mapper_(std::move(mapper)),
        data_(init_list, alloc) {
    build_tree();
  }

  // Time complexity - O(n).
  node_based_segment_tree(std::initializer_list<T> init_list,
                          const Allocator& alloc)
      : data_(init_list, alloc) {
    build_tree();
  }

  // Time complexity - O(n).
  node_based_segment_tree& operator=(const node_based_segment_tree& other) {
    data_ = other.data_;
    rebuild_tree();
    return *this;
  }

  node_based_segment_tree& operator=(node_based_segment_tree&& other) noexcept {
    reducer_ = std::move(other.reducer_);
    mapper_ = std::move(other.mapper_);
    data_ = std::move(other.data_);
    head_ = std::move(other.head_);
    tails_ = std::move(other.tails_);
    return *this;
  }

  // Time complexity - O(n).
  node_based_segment_tree& operator=(std::initializer_list<T> init_list) {
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
  [[nodiscard]] const_iterator begin() const noexcept { return data_.begin(); }

  // Time complexity - O(1).
  [[nodiscard]] const_iterator cbegin() const noexcept {
    return data_.cbegin();
  }

  // Time complexity - O(1).
  [[nodiscard]] const_iterator end() const noexcept { return data_.end(); }

  // Time complexity - O(1).
  [[nodiscard]] const_iterator cend() const noexcept { return data_.cend(); }

  // Time complexity - O(1).
  [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
    return data_.rbegin();
  }

  // Time complexity - O(1).
  [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
    return data_.crbegin();
  }

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
      assert(!head_);
      assert(tails_.empty());
    }
    return data_.empty();
  }

  // Time complexity - O(1).
  [[nodiscard]] size_type size() const noexcept { return data_.size(); }

  // Time complexity - O(1).
  [[nodiscard]] size_type max_size() const noexcept { return data_.max_size(); }

  // Time complexity - O(n).
  void clear() noexcept {
    data_.clear();
    head_.reset();
    tails_.clear();
    assert(empty());
  }

  // Time complexity - O(n).
  const_iterator insert(const_iterator pos, const T& value) {
    details::scoped{[this] { rebuild_tree(); }};
    return data_.insert(pos, value);
  }

  // Time complexity - O(n).
  const_iterator insert(const_iterator pos, T&& value) {
    details::scoped{[this] { rebuild_tree(); }};
    return data_.insert(pos, std::move(value));
  }

  // Time complexity - O(n).
  const_iterator insert(const_iterator pos, size_type count, const T& value) {
    details::scoped{[this] { rebuild_tree(); }};
    return data_.insert(pos, count, value);
  }

  // Time complexity - O(n).
  template <typename InputIt, typename = details::require_input_iter<InputIt>>
  const_iterator insert(const_iterator pos, InputIt first, InputIt last) {
    details::scoped{[this] { rebuild_tree(); }};
    return data_.insert(pos, first, last);
  }

  // Time complexity - O(n).
  const_iterator insert(const_iterator pos,
                        std::initializer_list<T> init_list) {
    details::scoped{[this] { rebuild_tree(); }};
    return data_.insert(pos, init_list);
  }

  // Time complexity - O(n).
  template <typename... Args>
  const_iterator emplace(const_iterator pos, Args&&... args) {
    details::scoped{[this] { rebuild_tree(); }};
    return data_.emplace(pos, std::forward<Args>(args)...);
  }

  // Time complexity - O(n).
  const_iterator erase(const_iterator pos) {
    details::scoped{[this] { rebuild_tree(); }};
    return data_.erase(pos);
  }

  // Time complexity - O(n).
  const_iterator erase(const_iterator first, const_iterator last) {
    details::scoped{[this] { rebuild_tree(); }};
    return data_.erase(first, last);
  }

  // Time complexity - O(n).
  void resize(size_type count) {
    data_.resize(count);
    rebuild_tree();
  }

  // Time complexity - O(1).
  void swap(node_based_segment_tree& other) noexcept {
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

  template <typename R, typename M, typename A>
  bool operator==(const node_based_segment_tree<T, R, M, A>& other) const {
    return data_ == other.data_;
  }

  template <typename R, typename M, typename A>
  bool operator!=(const node_based_segment_tree<T, R, M, A>& other) const {
    return data_ != other.data_;
  }

  template <typename R, typename M, typename A>
  bool operator<(const node_based_segment_tree<T, R, M, A>& other) const {
    return data_ < other.data_;
  }

  template <typename R, typename M, typename A>
  bool operator<=(const node_based_segment_tree<T, R, M, A>& other) const {
    return data_ <= other.data_;
  }

  template <typename R, typename M, typename A>
  bool operator>(const node_based_segment_tree<T, R, M, A>& other) const {
    return data_ > other.data_;
  }

  template <typename R, typename M, typename A>
  bool operator>=(const node_based_segment_tree<T, R, M, A>& other) const {
    return data_ >= other.data_;
  }

  // Make a query on [first_index, last_index) segment.
  // Time complexity - O(log n).
  [[nodiscard]] reduced_type query(size_t first_index,
                                   size_t last_index) const {
    assert(head_);
    assert(first_index <= last_index);
    assert(last_index <= data_.size());
    //
    const size_t max_size = 4;
    std::vector<reduced_type> results;
    std::vector<node_t*> q;
    results.reserve(max_size);
    q.reserve(max_size);

    q.emplace_back(head_.get());
    while (!q.empty()) {
      assert(q.size() <= max_size);
      assert(results.size() <= max_size);

      auto node = q.back();
      q.pop_back();
      if (!node) {
        continue;
      }
      if (node->is_part_of(first_index, last_index)) {
        results.emplace_back(node->value);
        continue;
      }
      if (node->is_leaf()) {
        if (first_index == node->first_index()) {
          results.emplace_back(mapper_(data_[first_index]));
        }
        if (first_index == node->first_index() + 1) {
          assert(first_index < node->last_index());
          results.emplace_back(mapper_(data_[first_index]));
        }
        continue;
      }
      for (auto child : node->childs()) {
        q.emplace_back(child);
      }
    }
    assert(!results.empty());
    reduced_type result = std::move(results.back());
    results.pop_back();
    for (auto& node : results) {
      result = reducer_(std::move(result), std::move(node));
    }
    return result;
  }

  // Time complexity - O(min(n, k log n)) where k is size of Range.
  template <typename Range>
  const_iterator copy(const Range& range, const_iterator dist_first) {
    const_iterator dist_last =
        std::copy(range.begin(), range.end(), remove_const(dist_first));
    update(dist_first, dist_last);
    return dist_last;
  }

  // Time complexity - O(min(n, k log n)) where k is size of Range.
  template <typename Range>
  const_iterator move(Range&& range, const_iterator dist_first) {
    const_iterator dist_last =
        std::move(range.begin(), range.end(), remove_const(dist_first));
    update(dist_first, dist_last);
    return dist_last;
  }

  // Time complexity - O(min(n, k log n)) where k is distance(first, last).
  void fill(const_iterator first, const_iterator last, const T& value) {
    std::fill(remove_const(first), remove_const(last), value);
    update(first, last);
  }

  template <class Generator>
  void generate(const_iterator first, const_iterator last, Generator g) {
    std::generate(remove_const(first), remove_const(last), g);
    update(first, last);
  }

 private:
  Reducer reducer_;
  Mapper mapper_;
  std::vector<T, Allocator> data_;

  std::unique_ptr<node_t> head_;
  std::vector<node_t*> tails_;
};

}  // namespace manavrion::segment_tree
