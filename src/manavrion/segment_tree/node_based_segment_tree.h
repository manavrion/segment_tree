//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <cassert>
#include <functional>
#include <memory>
#include <numeric>
#include <queue>
#include <range/v3/view/filter.hpp>
#include <type_traits>
#include <vector>

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
  using iterator = typename container_type::iterator;
  using const_iterator = typename container_type::const_iterator;
  using reverse_iterator = typename container_type::reverse_iterator;
  using const_reverse_iterator =
      typename container_type::const_reverse_iterator;

  // segment_tree specific.
  using reduced_type = mapper_result;
  using mapper_type = Mapper;
  using reducer_type = Reducer;

 private:
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

 public:
  node_based_segment_tree() = default;

  explicit node_based_segment_tree(const Allocator& allocator)
      : data_(allocator) {}

  explicit node_based_segment_tree(Reducer reducer, Mapper mapper = {},
                                   const Allocator& allocator = {})
      : reducer_(std::move(reducer)),
        mapper_(std::move(mapper)),
        data_(allocator) {}

  template <typename InputIt>
  node_based_segment_tree(InputIt first, InputIt last, Reducer reducer = {},
                          Mapper mapper = {}, const Allocator& allocator = {})
      : reducer_(std::move(reducer)),
        mapper_(std::move(mapper)),
        data_(first, last, allocator) {
    build_tree();
  }

  // Time complexity - O(n).
  template <typename InputIt>
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
    build_tree();
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
    clear();
    data_ = init_list;
    build_tree();
    return *this;
  }

  // Time complexity - O(n). Can be reduced to O(1) with dynamic_segment_tree.
  void assign(size_type count, const T& value) {
    clear();
    data_.assign(count, value);
    build_tree();
  }

  // Time complexity - O(n).
  template <class InputIt>
  void assign(InputIt first, InputIt last) {
    clear();
    data_.assign(first, last);
    build_tree();
  }

  // Time complexity - O(n).
  void assign(std::initializer_list<T> init_list) { operator=(init_list); }

  [[nodiscard]] allocator_type get_allocator() const noexcept {
    return data_.get_allocator();
  }

  [[nodiscard]] reference at(size_type pos) { return data_.at(pos); }
  [[nodiscard]] const_reference at(size_type pos) const {
    return data_.at(pos);
  }

  [[nodiscard]] reference operator[](size_type pos) { return data_[pos]; }
  [[nodiscard]] const_reference operator[](size_type pos) const {
    return data_[pos];
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

  // Updates unique element.
  // Time complexity - O(log n).
  void update(size_t index, value_type new_value) {
    data_[index] = std::move(new_value);
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

  [[nodiscard]] bool empty() const {
    if (data_.empty()) {
      assert(!head_);
      assert(tails_.empty());
    }
    return data_.empty();
  }

  void clear() {
    data_.clear();
    head_.reset();
    tails_.clear();
    assert(empty());
  }

 private:
  Reducer reducer_;
  Mapper mapper_;
  std::vector<T, Allocator> data_;

  std::unique_ptr<node_t> head_;
  std::vector<node_t*> tails_;
};

}  // namespace manavrion::segment_tree
