//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#pragma once

#include <array>
#include <iterator>

constexpr auto numbers = {0, 1, 2};

using manavrion::segment_tree::functors::default_mapper;
using manavrion::segment_tree::functors::default_reducer;

template <typename SegmentTree>
void ConstructorTest() {
  SegmentTree default_ctor;
  SegmentTree alloc_ctor(std::allocator<int>{});

  SegmentTree func_ctor0(default_reducer{}, default_mapper{},
                         std::allocator<int>{});
  SegmentTree func_ctor1(default_reducer{}, default_mapper{});
  SegmentTree func_ctor2(default_reducer{});

  SegmentTree range_ctor0(numbers.begin(), numbers.end(), default_reducer{},
                          default_mapper{}, std::allocator<int>{});
  SegmentTree range_ctor1(numbers.begin(), numbers.end(), default_reducer{},
                          default_mapper{});
  SegmentTree range_ctor2(numbers.begin(), numbers.end(), default_reducer{});
  SegmentTree range_ctor3(numbers.begin(), numbers.end());

  SegmentTree range_alloc_ctor(numbers.begin(), numbers.end(),
                               std::allocator<int>{});

  SegmentTree copy_ctor0(default_ctor, std::allocator<int>{});
  SegmentTree copy_ctor1(default_ctor);

  SegmentTree move_ctor0(SegmentTree{});
  SegmentTree move_ctor1(SegmentTree{}, std::allocator<int>{});

  SegmentTree init_ctor0(numbers, default_reducer{}, default_mapper{},
                         std::allocator<int>{});
  SegmentTree init_ctor1(numbers, default_reducer{}, default_mapper{});
  SegmentTree init_ctor2(numbers, default_reducer{});
  SegmentTree init_ctor3(numbers);
  SegmentTree init_ctor_ = numbers;

  SegmentTree init_alloc_ctor(numbers, std::allocator<int>{});
}

template <typename SegmentTree>
void OperatorAssignTest() {
  SegmentTree other_tree(numbers.begin(), numbers.end());
  SegmentTree segment_tree;

  segment_tree = other_tree;
  segment_tree = std::move(other_tree);
  segment_tree = numbers;
}

template <typename SegmentTree>
void MethodAssignTest() {
  SegmentTree segment_tree = numbers;

  segment_tree.assign(10, 123);
  segment_tree.assign(numbers.begin(), numbers.end());
  segment_tree.assign(numbers);
}

template <typename SegmentTree>
void MethodGetAllocatorTest() {
  SegmentTree segment_tree;
  EXPECT_EQ(segment_tree.get_allocator(), std::allocator<int>());
}

template <typename SegmentTree>
void MethodElementAccess() {
  SegmentTree segment_tree = numbers;
  const auto& const_segment_tree = segment_tree;
  EXPECT_EQ(segment_tree.at(0), 0);
  EXPECT_EQ(const_segment_tree.at(2), 2);

  EXPECT_EQ(segment_tree[0], 0);
  EXPECT_EQ(const_segment_tree[2], 2);

  EXPECT_EQ(segment_tree.front(), 0);
  EXPECT_EQ(const_segment_tree.front(), 0);

  EXPECT_EQ(segment_tree.back(), 2);
  EXPECT_EQ(const_segment_tree.back(), 2);

  EXPECT_EQ(segment_tree.data()[0], 0);
  EXPECT_EQ(segment_tree.data()[2], 2);
}

template <typename SegmentTree>
void MethodIterators() {
  SegmentTree segment_tree = numbers;
  const auto& const_segment_tree = segment_tree;

  EXPECT_EQ(*segment_tree.begin(), 0);
  EXPECT_EQ(*const_segment_tree.begin(), 0);
  EXPECT_EQ(*segment_tree.cbegin(), 0);

  EXPECT_EQ(*std::prev(segment_tree.end()), 2);
  EXPECT_EQ(*std::prev(const_segment_tree.end()), 2);
  EXPECT_EQ(*std::prev(segment_tree.cend()), 2);

  EXPECT_EQ(*segment_tree.rbegin(), 2);
  EXPECT_EQ(*const_segment_tree.rbegin(), 2);
  EXPECT_EQ(*segment_tree.crbegin(), 2);

  EXPECT_EQ(*std::prev(segment_tree.rend()), 0);
  EXPECT_EQ(*std::prev(const_segment_tree.rend()), 0);
  EXPECT_EQ(*std::prev(segment_tree.crend()), 0);
}

template <typename SegmentTree>
void MethodCapacity() {
  SegmentTree segment_tree = numbers;
  EXPECT_EQ(segment_tree.max_size(), std::vector<int>{}.max_size());

  EXPECT_FALSE(segment_tree.empty());
  EXPECT_EQ(segment_tree.size(), 3);
  segment_tree.clear();
  EXPECT_TRUE(segment_tree.empty());
  EXPECT_EQ(segment_tree.size(), 0);
}

template <typename SegmentTree>
void MethodModifiers() {
  SegmentTree segment_tree = numbers;

  segment_tree.clear();
  EXPECT_TRUE(segment_tree.empty());
}