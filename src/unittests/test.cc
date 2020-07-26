//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <gtest/gtest.h>

#include <memory>

#include "manavrion/segment_tree/node_based_segment_tree.h"
#include "manavrion/segment_tree/segment_tree.h"
#include "unittests/complicated_functor_test.h"
#include "unittests/method_test.h"
#include "unittests/simple_functor_test.h"

using namespace manavrion::segment_tree;

template <typename T, typename Reducer, typename Mapper>
using NodeBasedSegmentTree =
    node_based_segment_tree<T, Reducer, Mapper, std::allocator<T>>;

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
}

TEST(NodeBasedSegmentTree, Test) {
  TestSimpleSegmentTree<NodeBasedSegmentTree>();
}
