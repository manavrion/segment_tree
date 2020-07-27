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

  SegmentTree init_test1 = {0};
  EXPECT_EQ(init_test1.size(), 1);
  SegmentTree init_test2 = {0, 1};
  EXPECT_EQ(init_test2.size(), 2);
  SegmentTree init_test3 = {0, 1, 2};
  EXPECT_EQ(init_test3.size(), 3);
  SegmentTree init_test4 = {0, 1, 2, 3};
  EXPECT_EQ(init_test4.size(), 4);
  SegmentTree init_test5 = {0, 1, 2, 3, 4};
  EXPECT_EQ(init_test5.size(), 5);
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

  segment_tree = numbers;

  auto it1 = segment_tree.insert(segment_tree.begin(), -1);
  EXPECT_EQ(segment_tree, SegmentTree({-1, 0, 1, 2}));
  EXPECT_EQ(*it1, -1);

  auto it2 = segment_tree.insert(segment_tree.end(), 3);
  EXPECT_EQ(segment_tree, SegmentTree({-1, 0, 1, 2, 3}));
  EXPECT_EQ(*it2, 3);

  // actual state: {-1, 0, 1, 2, 3}
  auto it3 = segment_tree.insert(segment_tree.begin(), 2, 0);
  EXPECT_EQ(segment_tree, SegmentTree({0, 0, -1, 0, 1, 2, 3}));
  EXPECT_EQ(it3, segment_tree.begin());

  segment_tree = numbers;

  auto it4 = segment_tree.insert(std::next(segment_tree.begin()),
                                 numbers.begin(), numbers.end());
  EXPECT_EQ(segment_tree, SegmentTree({0, 0, 1, 2, 1, 2}));
  EXPECT_EQ(it4, std::next(segment_tree.begin()));

  segment_tree = numbers;

  auto it5 = segment_tree.insert(std::next(segment_tree.begin()), numbers);
  EXPECT_EQ(segment_tree, SegmentTree({0, 0, 1, 2, 1, 2}));
  EXPECT_EQ(it5, std::next(segment_tree.begin()));

  segment_tree = numbers;

  auto it6 = segment_tree.emplace(segment_tree.begin(), 3);
  EXPECT_EQ(segment_tree, SegmentTree({3, 0, 1, 2}));
  EXPECT_EQ(it6, segment_tree.begin());

  segment_tree = numbers;
  auto it7 = segment_tree.erase(std::next(segment_tree.begin()));
  EXPECT_EQ(segment_tree, SegmentTree({0, 2}));
  EXPECT_EQ(it7, std::next(segment_tree.begin()));

  segment_tree = {0, 1, 2, 3, 4, 5, 6};
  auto it8 = segment_tree.erase(std::next(segment_tree.begin(), 3),
                                std::next(segment_tree.begin(), 6));
  EXPECT_EQ(segment_tree, SegmentTree({0, 1, 2, 6}));
  EXPECT_EQ(it8, std::next(segment_tree.begin(), 3));

  segment_tree = {0, 1, 2, 3, 4, 5, 6};
  segment_tree.resize(3);
  EXPECT_EQ(segment_tree, SegmentTree({0, 1, 2}));
  segment_tree.resize(5);
  EXPECT_EQ(segment_tree, SegmentTree({0, 1, 2, 0, 0}));

  segment_tree = numbers;
  SegmentTree other = {9, 8, 7};
  segment_tree.swap(other);
  EXPECT_EQ(segment_tree, SegmentTree({9, 8, 7}));
  EXPECT_EQ(other, SegmentTree(numbers));
}

template <typename SegmentTree>
void Comparison() {
  EXPECT_TRUE(SegmentTree({}) == SegmentTree({}));
  EXPECT_TRUE(SegmentTree({1, 2}) == SegmentTree({1, 2}));
  EXPECT_TRUE(SegmentTree({2}) != SegmentTree({1, 2}));
  EXPECT_TRUE(SegmentTree({1, 2}) < SegmentTree({5}));
  EXPECT_TRUE(SegmentTree({5}) > SegmentTree({1, 2}));
  EXPECT_TRUE(SegmentTree({1, 2}) <= SegmentTree({5}));
  EXPECT_TRUE(SegmentTree({5}) >= SegmentTree({1, 2}));
  EXPECT_TRUE(SegmentTree({5}) <= SegmentTree({5}));
  EXPECT_TRUE(SegmentTree({5}) >= SegmentTree({5}));
}

template <typename SegmentTree>
void Specialized() {
  SegmentTree segment_tree = numbers;
  SegmentTree other = {9, 8, 7};
  std::swap(segment_tree, other);
  EXPECT_EQ(segment_tree, SegmentTree({9, 8, 7}));
  EXPECT_EQ(other, SegmentTree(numbers));
}
