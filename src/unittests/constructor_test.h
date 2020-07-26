//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#pragma once

#include <array>

template <template <typename, typename, typename> typename S>
void ConstructorTest() {
  using namespace manavrion::segment_tree::functors;
  using SegmentTree = S<int, default_reducer, default_mapper>;
  const std::array numbers = {0, 1, 2};
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

template <template <typename, typename, typename> typename S>
void OperatorAssignTest() {
  using namespace manavrion::segment_tree::functors;
  using SegmentTree = S<int, default_reducer, default_mapper>;
  const std::array numbers = {0, 1, 2};
  SegmentTree other_tree(numbers.begin(), numbers.end());
  SegmentTree segment_tree;

  segment_tree = other_tree;
  segment_tree = std::move(other_tree);
  segment_tree = {0, 1, 2};
}

template <template <typename, typename, typename> typename S>
void AssignTest() {
  using namespace manavrion::segment_tree::functors;
  using SegmentTree = S<int, default_reducer, default_mapper>;
  const std::array numbers = {0, 1, 2};
  SegmentTree segment_tree = {0, 1, 2};

  segment_tree.assign(10, 123);
  segment_tree.assign(numbers.begin(), numbers.end());
  segment_tree.assign({0, 1, 2});
}
