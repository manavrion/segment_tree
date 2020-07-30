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
#include <stack>
#include <type_traits>
#include <variant>
#include <vector>
#include <stdexcept>

#include "details.h"
#include "functors.h"

namespace manavrion::segment_tree {

template <typename T, typename Reducer = functors::default_reducer,
          typename Mapper = functors::deduce_mapper<T, Reducer>,
          typename Allocator = std::allocator<T>>
class segment_tree {
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

  class node_t;

  struct leaf_data_t {
    // Copies one node with out relations or childs.
    leaf_data_t(const leaf_data_t& other)
        : value_(std::make_unique<value_type>(*other.value_)) {}

    leaf_data_t(std::unique_ptr<value_type> value) : value_(std::move(value)) {
      assert(value_);
    }

    std::unique_ptr<value_type> value_;
    node_t* prev_ = nullptr;
    node_t* next_ = nullptr;
  };

  struct regular_data_t {
    // Copies one node with out relations or childs.
    regular_data_t(const regular_data_t& other)
        : value_(std::make_unique<value_type>(*other.value_)) {}

    regular_data_t(std::unique_ptr<reduced_type> value,
                   std::unique_ptr<node_t> left, std::unique_ptr<node_t> right,
                   node_t* base)
        : value_(std::move(value)),
          left_(std::move(left)),
          right_(std::move(right)) {
      assert(value_);
      assert(left_);
      assert(right_);
      // Nodes must be adjacent.
      assert(left_->last_index_ == right_->first_index_);
      left_->parent_ = base;
      right_->parent_ = base;
    }

    std::unique_ptr<reduced_type> value_;
    std::unique_ptr<node_t> left_;
    std::unique_ptr<node_t> right_;
  };

  struct node_t {
    // Copies one node with out relations or childs.
    node_t(const node_t& other)
        : first_index_(other.first_index_),
          last_index_(other.last_index_),
          data_(other.data_) {}

    // Creates leaf.
    node_t(std::unique_ptr<value_type> value, size_t first_index,
           size_t last_index)
        : first_index_(first_index),
          last_index_(last_index),
          data_(std::in_place_type_t<leaf_data_t>(), std::move(value)) {
      // Leaf can not be empty.
      assert(first_index_ < last_index_);
    }

    // Creates regular node.
    node_t(std::unique_ptr<node_t> left, std::unique_ptr<node_t> right,
           const Reducer& reducer, const Mapper& mapper)
        : first_index_(left->first_index_),
          last_index_(right->last_index_),
          data_(std::in_place_type_t<regular_data_t>(),
                std::make_unique<reduced_type>(
                    reducer(left->value(mapper), right->value(mapper))),
                std::move(left), std::move(right), this) {
      assert(first_index_ < last_index_);
    }

    size_t first_index() const { return first_index_; }
    size_t last_index() const { return last_index_; }
    node_t* parent() const { return parent_; }
    size_t size() const { return last_index_ - first_index_; }

    bool is_leaf() const { return std::holds_alternative<leaf_data_t>(data_); }

    bool is_part_of(size_t first, size_t last) const {
      assert(first <= last);
      return first <= first_index_ && last_index_ <= last;
    }

    bool contains(size_t index) const {
      return first_index_ <= index && index < last_index_;
    }

    decltype(auto) value(const Mapper& mapper) const {
      return std::visit(
          details::overloaded{[&](const leaf_data_t& arg) -> decltype(auto) {
                                return mapper(*arg.value_);
                              },
                              [](const regular_data_t& arg) -> decltype(auto) {
                                return *arg.value_;
                              }},
          data_);
    }

    size_t first_index_;
    size_t last_index_;
    node_t* parent_ = nullptr;
    std::variant<leaf_data_t, regular_data_t> data_;
  };

  // Creates segment tree nodes, time complexity - O(n).
  template <typename InputIt>
  void build_tree(InputIt first, InputIt last) {
    std::vector<std::unique_ptr<node_t>> line;
    const size_t size = std::distance(first, last);
    line.reserve(size);

    size_t i = 0;
    for (auto it = first; it != last; it = std::next(it), ++i) {
      line.emplace_back(std::make_unique<node_t>(
          std::make_unique<value_type>(*it), i, i + 1));
    }
    assert(line.size() == size);

    if (!line.empty()) {
      front_ = line.front().get();
      back_ = line.back().get();
    }

    while (line.size() > 1) {
      std::vector<std::unique_ptr<node_t>> new_line;
      const size_t new_size = line.size() / 2 + bool(line.size() % 2);
      new_line.reserve(new_size);
      for (size_t i = 0; i < line.size(); i += 2) {
        if (i + 1 < line.size()) {
          new_line.emplace_back(std::make_unique<node_t>(
              std::move(line[i]), std::move(line[i + 1]), reducer_, mapper_));
        } else {
          new_line.emplace_back(std::move(line[i]));
        }
      }
      assert(new_line.size() == new_size);
      line = std::move(new_line);
    }

    if (!line.empty()) {
      assert(line.size() == 1);
      head_ = std::move(line.front());
    }
  }

  // Time complexity - O(k) where k is count of nodes.
  void copy_tree(const std::unique_ptr<node_t>& other_head) {
    std::stack<std::pair<node_t*, const node_t*>> st;
    if (other_head) {
      head_ = std::make_unique<node_t>(*other_head);
      st.emplace(head_.get(), other_head.get());
    }

    node_t* prev_leaf = nullptr;

    while (!st.empty()) {
      auto [node, other_node] = st.top();
      st.pop();

      std::visit(
          details::overloaded{
              [this, &prev_leaf, node = node](leaf_data_t& leaf,
                                              const leaf_data_t& other_leaf) {
                if (!prev_leaf) {
                  front_ = node;
                }
                if (prev_leaf) {
                  std::get<leaf_data_t>(prev_leaf->data_).next_ = node;
                  leaf.prev_ = prev_leaf;
                }
                prev_leaf = node;
              },
              [&st, node = node](regular_data_t& regular,
                                 const regular_data_t& other_regular) {
                regular.left_ = std::make_unique<node_t>(*other_regular.left_);
                regular.right_ =
                    std::make_unique<node_t>(*other_regular.right_);
                regular.left_->parent_ = node;
                regular.right_->parent_ = node;
                st.emplace(regular.right_.get(), other_regular.right_.get());
                st.emplace(regular.left_.get(), other_regular.left_.get());
              },
              [](auto&&, auto&&) { assert(false); }},
          node->data_, other_node->data_);
    }
    back_ = prev_leaf;
  }

  // Time complexity - O(log n). O(1) in best case.
  const value_type* get(size_t index) const {
    const value_type* result = nullptr;
    auto node = head_.get();
    while (node) {
      std::visit(details::overloaded{
        [index, &node, &result](const leaf_data_t& arg){
          if (node->contains(index)) {
            result = arg.value_.get();
          }
          node = nullptr;
        },
        [index, &node](const regular_data_t& arg){
          if (arg.left_->contains(index)) {
            node = arg.left_.get();
          } else {
            node = arg.right_.get();
          }
        }
      }, node->data_);
    }
    return result;
  }

#if 0
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
#endif
 public:
  segment_tree() = default;

  explicit segment_tree(const Allocator& allocator) : allocator_(allocator) {}

  explicit segment_tree(Reducer reducer, Mapper mapper = {},
                        const Allocator& allocator = {})
      : reducer_(std::move(reducer)),
        mapper_(std::move(mapper)),
        allocator_(allocator) {}

  // Time complexity - O(n).
  template <typename InputIt, typename = details::require_input_iter<InputIt>>
  segment_tree(InputIt first, InputIt last, Reducer reducer = {},
               Mapper mapper = {}, const Allocator& allocator = {})
      : reducer_(std::move(reducer)),
        mapper_(std::move(mapper)),
        allocator_(allocator) {
    build_tree(first, last);
  }

  // Time complexity - O(n).
  template <typename InputIt, typename = details::require_input_iter<InputIt>>
  segment_tree(InputIt first, InputIt last, const Allocator& allocator)
      : allocator_(allocator) {
    build_tree(first, last);
  }

  // Time complexity - O(k) where k is count of nodes.
  segment_tree(const segment_tree& other, const Allocator& allocator = {})
      : reducer_(other.reducer_),
        mapper_(other.mapper_),
        allocator_(allocator) {
    copy_tree(other.head_);
  }

  segment_tree(segment_tree&& other, const Allocator& allocator = {}) noexcept
      : reducer_(std::move(other.reducer_)),
        mapper_(std::move(other.mapper_)),
        allocator_(allocator),
        head_(std::move(other.head_)),
        front_(other.front_),
        back_(other.back_) {}

  // Time complexity - O(n).
  segment_tree(std::initializer_list<T> init_list, Reducer reducer = {},
               Mapper mapper = {}, const Allocator& allocator = {})
      : reducer_(std::move(reducer)),
        mapper_(std::move(mapper)),
        allocator_(allocator) {
    build_tree(init_list.begin(), init_list.end());
  }

  // Time complexity - O(n).
  segment_tree(std::initializer_list<T> init_list, const Allocator& allocator)
      : allocator_(allocator) {
    build_tree(init_list.begin(), init_list.end());
  }

  // Time complexity - O(n).
  segment_tree& operator=(const segment_tree& other) {
    copy_tree(other.head_);
    return *this;
  }

  segment_tree& operator=(segment_tree&& other) noexcept {
    reducer_ = std::move(other.reducer_);
    mapper_ = std::move(other.mapper_);
    head_ = std::move(other.head_);
    front_ = std::move(other.front_);
    back_ = std::move(other.back_);
    return *this;
  }

  // Time complexity - O(n).
  segment_tree& operator=(std::initializer_list<T> init_list) {
    build_tree(init_list.begin(), init_list.end());
    return *this;
  }

  // Time complexity - O(1).
  void assign(size_type count, const T& value) {
    head_ =
        std::make_unique<node_t>(std::make_unique<value_type>(value), 0, count);
    front_ = head_.get();
    back_ = head_.get();
  }

  // Time complexity - O(n).
  template <class InputIt, typename = details::require_input_iter<InputIt>>
  void assign(InputIt first, InputIt last) {
    build_tree(first, last);
  }

  // Time complexity - O(n).
  void assign(std::initializer_list<T> init_list) { operator=(init_list); }

  // Time complexity - O(1).
  [[nodiscard]] allocator_type get_allocator() const noexcept {
    return allocator_;
  }

  // Time complexity - O(log n). O(1) in best case.
  [[nodiscard]] const_reference at(size_type pos) const {
    if (auto ptr = get(pos)) {
      return *ptr;
    }
    throw std::out_of_range("out_of_range at segment_tree");
  }

  // Time complexity - O(log n). O(1) in best case.
  [[nodiscard]] const_reference operator[](size_type pos) const {
    assert(pos < size());
    return *get(pos);
  }

  // Time complexity - O(1).
  [[nodiscard]] const_reference front() const {
    assert(front_);
    return *std::get<leaf_data_t>(front_->data_).value_;
  }

  // Time complexity - O(1).
  [[nodiscard]] const_reference back() const {
    assert(back_);
    return *std::get<leaf_data_t>(back_->data_).value_;
  }
#if 0

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
#endif
  // Time complexity - O(1).
  [[nodiscard]] bool empty() const noexcept {
    if (!head_) {
      assert(!front_);
      assert(!back_);
    }
    return !head_;
  }

  // Time complexity - O(1).
  [[nodiscard]] size_type size() const noexcept {
    assert(front_);
    assert(back_);
    return back_->last_index_ - front_->first_index_;
  }

  // Time complexity - O(1).
  [[nodiscard]] size_type max_size() const noexcept {
    return std::numeric_limits<size_t>::max();
  }

  // Time complexity - O(n).
  void clear() noexcept {
    head_.reset();
    front_ = nullptr;
    back_ = nullptr;
    assert(empty());
  }

#if 0
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
  void swap(segment_tree& other) noexcept {
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
  bool operator==(const segment_tree<T, R, M, A>& other) const {
    return data_ == other.data_;
  }

  template <typename R, typename M, typename A>
  bool operator!=(const segment_tree<T, R, M, A>& other) const {
    return data_ != other.data_;
  }

  template <typename R, typename M, typename A>
  bool operator<(const segment_tree<T, R, M, A>& other) const {
    return data_ < other.data_;
  }

  template <typename R, typename M, typename A>
  bool operator<=(const segment_tree<T, R, M, A>& other) const {
    return data_ <= other.data_;
  }

  template <typename R, typename M, typename A>
  bool operator>(const segment_tree<T, R, M, A>& other) const {
    return data_ > other.data_;
  }

  template <typename R, typename M, typename A>
  bool operator>=(const segment_tree<T, R, M, A>& other) const {
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
#endif
 private:
  Reducer reducer_;
  Mapper mapper_;
  Allocator allocator_;

  std::unique_ptr<node_t> head_;
  node_t* front_ = nullptr;
  node_t* back_ = nullptr;
};

}  // namespace manavrion::segment_tree
