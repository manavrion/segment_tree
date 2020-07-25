//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <cassert>
#include <functional>
#include <memory>
#include <numeric>
#include <queue>
#include <type_traits>
#include <vector>

#include "details.h"

namespace manavrion {

template <typename T, typename Mapper = details::default_mapper<T>,
          typename Reducer = details::default_reducer<T>>
class node_based_segment_tree {
  static_assert(std::is_invocable_v<Mapper, T>);
  using mapper_result = std::decay_t<std::invoke_result_t<Mapper, T>>;
  static_assert(std::is_invocable_v<Reducer, mapper_result, mapper_result>);
  static_assert(std::is_convertible_v<
                std::invoke_result_t<Reducer, mapper_result, mapper_result>,
                mapper_result>);

 public:
  using value_type = T;
  using node_value_type = mapper_result;

 private:
  struct node_t {
    node_value_type node_value_;
    size_t first_index_;
    size_t last_index_;
    node_t* parent_ = nullptr;
    std::unique_ptr<node_t> left_;
    std::unique_ptr<node_t> right_;

    node_t(node_value_type node_value, size_t first_index, size_t last_index)
        : node_value_(std::move(node_value)),
          first_index_(first_index),
          last_index_(last_index) {
      assert(first_index_ < last_index_);
    }

    node_t(std::unique_ptr<node_t> left, std::unique_ptr<node_t> right,
           const Reducer& reducer)
        : node_value_(reducer(left->node_value_, right->node_value_)),
          first_index_(left->first_index_),
          last_index_(right->last_index_),
          left_(std::move(left)),
          right_(std::move(right)) {
      // Nodes must be adjacent.
      assert(left_->last_index_ == right_->first_index_);
      assert(first_index_ < last_index_);
      left_->parent_ = this;
      right_->parent_ = this;
    }

    [[nodiscard]] size_t size() const noexcept {
      assert(first_index_ < last_index_);
      return last_index_ - first_index_;
    }

    [[nodiscard]] bool is_leaf() const noexcept {
      bool result = size() <= 2;
      if (result) {
        assert(!left_);
        assert(!right_);
      }
      return result;
    }

    [[nodiscard]] std::array<node_t*, 2> childs() noexcept {
      assert(left_ || right_);
      return {left_.get(), right_.get()};
    }
  };

 public:
  node_based_segment_tree() = default;
  node_based_segment_tree(const node_based_segment_tree&) = default;
  node_based_segment_tree& operator=(const node_based_segment_tree&) = default;
  node_based_segment_tree(node_based_segment_tree&&) = default;
  node_based_segment_tree& operator=(node_based_segment_tree&&) = default;

  // Creation of segment tree nodes,
  // Time complexity - O(n).
  template <typename InputIt>
  node_based_segment_tree(InputIt first, InputIt last, Mapper mapper = {},
                          Reducer reducer = {})
      : mapper_(std::move(mapper)), reducer_(std::move(reducer)) {
    data_.assign(first, last);
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

  // Make a query on [first_index, last_index) segment.
  // Time complexity - O(log n).
  node_value_type query(size_t first_index, size_t last_index) const {
    assert(head_);
    assert(first_index <= last_index);
    assert(last_index <= data_.size());
    //
    const size_t max_size = 4;
    std::vector<node_value_type> results;
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
      if (first_index <= node->first_index_ &&
          node->last_index_ <= last_index) {
        results.emplace_back(node->node_value_);
        continue;
      }
      if (node->is_leaf()) {
        if (first_index == node->first_index_) {
          results.emplace_back(mapper_(data_[first_index]));
        }
        if (first_index == node->first_index_ + 1) {
          assert(first_index < node->last_index_);
          results.emplace_back(mapper_(data_[first_index]));
        }
        continue;
      }
      q.emplace_back(node->left_.get());
      q.emplace_back(node->right_.get());
    }
    assert(!results.empty());
    node_value_type result = std::move(results.back());
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
      node->node_value_ = reducer_(mapper_(data_[node->first_index_]),
                                   mapper_(data_[node->first_index_ + 1]));
    } else {
      node->node_value_ = mapper_(data_[node->first_index_]);
    }
    node = node->parent_;
    while (node) {
      auto childs = node->childs();
      assert(childs.size() != 1);
      node->node_value_ =
          reducer_(childs[0]->node_value_, childs[1]->node_value_);
      node = node->parent_;
    }
  }

  [[nodiscard]] bool empty() const { return data_.empty(); }

  void clear() {
    data_.clear();
    head_.reset();
    tails_.clear();
  }

 private:
  Mapper mapper_;
  Reducer reducer_;
  std::vector<value_type> data_;

  std::unique_ptr<node_t> head_;
  std::vector<node_t*> tails_;
};

}  // namespace manavrion
