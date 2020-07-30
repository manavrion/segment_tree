//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <gtest/gtest.h>

#include <memory>

#include "manavrion/segment_tree/plain_segment_tree.h"
#include "manavrion/segment_tree/segment_tree.h"
#include "manavrion/segment_tree/unittests/complicated_functor_test.h"
#include "manavrion/segment_tree/unittests/method_test.h"
#include "manavrion/segment_tree/unittests/simple_functor_test.h"

using namespace manavrion::segment_tree;

template <typename T, typename Reducer, typename Mapper>
using PlainSegmentTree =
    plain_segment_tree<T, Reducer, Mapper, std::allocator<T>>;

template <typename T, typename Reducer, typename Mapper>
using SegmentTree = segment_tree<T, Reducer, Mapper, std::allocator<T>>;

template <template <typename, typename, typename> typename S>
void TestSimpleSegmentTree() {
  using namespace manavrion::segment_tree::functors;
  using SegmentTree = S<int, default_reducer, default_mapper>;

  SimpleFunctorTest<S>();
  ComplicatedFunctorTest<S>();

  ConstructorTest<SegmentTree>();
  OperatorAssignTest<SegmentTree>();
  MethodAssignTest<SegmentTree>();
  MethodGetAllocatorTest<SegmentTree>();
  MethodElementAccess<SegmentTree>();
  MethodIterators<SegmentTree>();
  MethodCapacity<SegmentTree>();
  MethodModifiers<SegmentTree>();
  Comparison<SegmentTree>();
  Specialized<SegmentTree>();
}

TEST(PlainSegmentTree, Test) { TestSimpleSegmentTree<PlainSegmentTree>(); }

template <template <typename, typename, typename> typename S>
void TestSimpleSegmentTreeSpecial() {
  using namespace manavrion::segment_tree::functors;
  using SegmentTree = S<int, default_reducer, default_mapper>;

  // SimpleFunctorTest<S>();
  // ComplicatedFunctorTest<S>();

  ConstructorTest<SegmentTree>();
  OperatorAssignTest<SegmentTree>();
  // MethodAssignTest<SegmentTree>();
  // MethodGetAllocatorTest<SegmentTree>();
  // MethodElementAccess<SegmentTree>();
  // MethodIterators<SegmentTree>();
  // MethodCapacity<SegmentTree>();
  // MethodModifiers<SegmentTree>();
  // Comparison<SegmentTree>();
  // Specialized<SegmentTree>();
}

TEST(SegmentTree, Test) {
  segment_tree<int> st = {0, 1, 2, 3, 4};
  segment_tree<int> st2 = st;
  TestSimpleSegmentTreeSpecial<SegmentTree>();
}
