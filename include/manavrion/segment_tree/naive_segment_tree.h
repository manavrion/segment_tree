//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <cassert>
#include <cmath>
#include <functional>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <type_traits>
#include <vector>

#include "manavrion/segment_tree/details.h"
#include "manavrion/segment_tree/functors.h"

namespace manavrion::segment_tree {

template <typename T, typename Reducer = std::plus<T>,
          typename Allocator = std::allocator<T>>
class naive_segment_tree {
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

  using iterator = typename container_type::iterator;
  using reverse_iterator = typename container_type::reverse_iterator;
  using const_iterator = typename container_type::const_iterator;
  using const_reverse_iterator =
      typename container_type::const_reverse_iterator;

  // naive_segment_tree specific.
  using reducer_type = Reducer;

 private:
  // For special methods. Only for internal use.
  iterator remove_const(const const_iterator it) {
    return std::next(data_.begin(), std::distance(data_.cbegin(), it));
  }

 public:
  naive_segment_tree() = default;

  explicit naive_segment_tree(const Allocator& allocator) : data_(allocator) {}

  explicit naive_segment_tree(Reducer reducer, const Allocator& allocator = {})
      : reducer_(std::move(reducer)), data_(allocator) {}

  template <typename InputIt, typename = details::require_input_iter<InputIt>>
  naive_segment_tree(InputIt first, InputIt last, Reducer reducer = {},
                     const Allocator& allocator = {})
      : reducer_(std::move(reducer)), data_(first, last, allocator) {}

  // Time complexity - O(n).
  template <typename InputIt, typename = details::require_input_iter<InputIt>>
  naive_segment_tree(InputIt first, InputIt last, const Allocator& allocator)
      : data_(first, last, allocator) {}

  // Time complexity - O(n).
  naive_segment_tree(const naive_segment_tree& other,
                     const Allocator& allocator = {})
      : reducer_(other.reducer_), data_(other.data_, allocator) {}

  naive_segment_tree(naive_segment_tree&& other,
                     const Allocator& allocator = {}) noexcept
      : reducer_(std::move(other.reducer_)),
        data_(std::move(other.data_), allocator) {}

  // Time complexity - O(n).
  naive_segment_tree(std::initializer_list<T> init_list, Reducer reducer = {},
                     const Allocator& alloc = {})
      : reducer_(std::move(reducer)), data_(init_list, alloc) {}

  // Time complexity - O(n).
  naive_segment_tree(std::initializer_list<T> init_list, const Allocator& alloc)
      : data_(init_list, alloc) {}

  // Time complexity - O(n).
  naive_segment_tree& operator=(const naive_segment_tree& other) {
    data_ = other.data_;
    return *this;
  }

  naive_segment_tree& operator=(naive_segment_tree&& other) noexcept {
    reducer_ = std::move(other.reducer_);
    data_ = std::move(other.data_);
    return *this;
  }

  // Time complexity - O(n).
  naive_segment_tree& operator=(std::initializer_list<T> init_list) {
    data_ = init_list;
    return *this;
  }

  // Time complexity - O(n). Can be reduced to O(1) with dynamic_segment_tree.
  void assign(size_type count, const T& value) { data_.assign(count, value); }

  // Time complexity - O(n).
  template <class InputIt, typename = details::require_input_iter<InputIt>>
  void assign(InputIt first, InputIt last) {
    data_.assign(first, last);
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
  [[nodiscard]] bool empty() const noexcept { return data_.empty(); }

  // Time complexity - O(1).
  [[nodiscard]] size_type size() const noexcept { return data_.size(); }

  void reserve(size_t capacity) { data_.reserve(capacity); }

  [[nodiscard]] size_type capacity() const noexcept { return data_.capacity(); }

  // Time complexity - O(1).
  [[nodiscard]] size_type max_size() const noexcept { return data_.max_size(); }

  // Time complexity - O(n).
  void clear() noexcept {
    data_.clear();
    assert(empty());
  }

  // Time complexity - O(n).
  const_iterator insert(const_iterator pos, const T& value) {
    return data_.insert(pos, value);
  }

  // Time complexity - O(n).
  const_iterator insert(const_iterator pos, T&& value) {
    return data_.insert(pos, std::move(value));
  }

  // Time complexity - O(n).
  const_iterator insert(const_iterator pos, size_type count, const T& value) {
    return data_.insert(pos, count, value);
  }

  // Time complexity - O(n).
  template <typename InputIt, typename = details::require_input_iter<InputIt>>
  const_iterator insert(const_iterator pos, InputIt first, InputIt last) {
    return data_.insert(pos, first, last);
  }

  // Time complexity - O(n).
  const_iterator insert(const_iterator pos,
                        std::initializer_list<T> init_list) {
    return data_.insert(pos, init_list);
  }

  // Time complexity - O(n).
  template <typename... Args>
  const_iterator emplace(const_iterator pos, Args&&... args) {
    return data_.emplace(pos, std::forward<Args>(args)...);
  }

  // Time complexity - O(n).
  const_iterator erase(const_iterator pos) { return data_.erase(pos); }

  // Time complexity - O(n).
  const_iterator erase(const_iterator first, const_iterator last) {
    return data_.erase(first, last);
  }

  // Time complexity - O(n).
  void resize(size_type count) { data_.resize(count); }

  // Time complexity - O(1).
  void swap(naive_segment_tree& other) noexcept {
    auto tmp = std::move(other);
    other = std::move(*this);
    *this = std::move(tmp);
  }

  // Time complexity - O(1).
  template <typename V>
  void update(size_t index, V&& v) {
    data_[index] = std::forward<V>(v);
  }

  template <typename R, typename A>
  bool operator==(const naive_segment_tree<T, R, A>& other) const {
    return data_ == other.data_;
  }

  template <typename R, typename A>
  bool operator!=(const naive_segment_tree<T, R, A>& other) const {
    return data_ != other.data_;
  }

  template <typename R, typename A>
  bool operator<(const naive_segment_tree<T, R, A>& other) const {
    return data_ < other.data_;
  }

  template <typename R, typename A>
  bool operator<=(const naive_segment_tree<T, R, A>& other) const {
    return data_ <= other.data_;
  }

  template <typename R, typename A>
  bool operator>(const naive_segment_tree<T, R, A>& other) const {
    return data_ > other.data_;
  }

  template <typename R, typename A>
  bool operator>=(const naive_segment_tree<T, R, A>& other) const {
    return data_ >= other.data_;
  }

  // Make a query on [first_index, last_index) segment.
  // Time complexity - O(n).
  [[nodiscard]] T query(size_t first_index, size_t last_index) const {
    assert(first_index <= last_index);
    assert(last_index <= data_.size());

    std::optional<T> result;
    auto add_result = [&](const auto& value) {
      if (result) {
        result.emplace(reducer_(std::move(*result), value));
      } else {
        result.emplace(value);
      }
    };

    for (size_t i = first_index; i < last_index; ++i) {
      add_result(data_[i]);
    }

    if (!result) {
      result.emplace();
    }
    return std::move(*result);
  }

  void update_range(const_iterator, const_iterator) {}

 private:
  Reducer reducer_;
  std::vector<value_type, Allocator> data_;
};

}  // namespace manavrion::segment_tree
