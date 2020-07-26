//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <gtest/gtest.h>

#include "manavrion/segment_tree/node_based_segment_tree.h"
#include "manavrion/segment_tree/segment_tree.h"
#include "unittests/complicated_functor_test.h"
#include "unittests/simple_functor_test.h"

using namespace manavrion::segment_tree;

template <template <typename, typename, typename> typename SegmentTree>
void TestSimpleSegmentTree() {
  SimpleFunctorTest<SegmentTree>();
  ComplicatedFunctorTest<SegmentTree>();
}

TEST(NodeBasedSegmentTree, Test) {
  TestSimpleSegmentTree<node_based_segment_tree>();
}
