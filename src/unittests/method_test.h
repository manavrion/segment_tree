//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#pragma once

#include <array>

constexpr std::array numbers = {0, 1, 2};

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

  SegmentTree init_ctor0({0, 1, 2}, default_reducer{}, default_mapper{},
                         std::allocator<int>{});
  SegmentTree init_ctor1({0, 1, 2}, default_reducer{}, default_mapper{});
  SegmentTree init_ctor2({0, 1, 2}, default_reducer{});
  SegmentTree init_ctor3({0, 1, 2});
  SegmentTree init_ctor_ = {0, 1, 2};

  SegmentTree init_alloc_ctor({0, 1, 2}, std::allocator<int>{});
}

template <typename SegmentTree>
void OperatorAssignTest() {
  SegmentTree other_tree(numbers.begin(), numbers.end());
  SegmentTree segment_tree;

  segment_tree = other_tree;
  segment_tree = std::move(other_tree);
  segment_tree = {0, 1, 2};
}

template <typename SegmentTree>
void MethodAssignTest() {
  SegmentTree segment_tree = {0, 1, 2};

  segment_tree.assign(10, 123);
  segment_tree.assign(numbers.begin(), numbers.end());
  segment_tree.assign({0, 1, 2});
}

template <typename SegmentTree>
void MethodGetAllocatorTest() {
  SegmentTree segment_tree;
  EXPECT_EQ(segment_tree.get_allocator(), std::allocator<int>());
}
