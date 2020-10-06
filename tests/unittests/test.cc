//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <gtest/gtest.h>

#include <memory>

#include "complicated_functor_test.h"
#include "manavrion/segment_tree/mapped_segment_tree.h"
#include "manavrion/segment_tree/naive_segment_tree.h"
#include "manavrion/segment_tree/segment_tree.h"
#include "method_test.h"
#include "random_test.h"
#include "simple_functor_test.h"
#include "test_functors.h"
#include "test_segment_tree.h"

using namespace manavrion::segment_tree;

template <typename T, typename Reducer, typename Mapper>
using TestSegmentTree =
    test_segment_tree<T, Reducer, Mapper, std::allocator<T>>;

template <typename T, typename Reducer, typename Mapper>
using NaiveSegmentTree = naive_segment_tree<T, Reducer, std::allocator<T>>;

template <typename T, typename Reducer, typename Mapper>
using MappedSegmentTree =
    mapped_segment_tree<T, Reducer, Mapper, std::allocator<T>>;

template <typename T, typename Reducer, typename Mapper>
using SegmentTree = segment_tree<T, Reducer, std::allocator<T>>;

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

// TEST(TestSegmentTree, Test) { TestSimpleSegmentTree<TestSegmentTree>(); }

// TEST(NaiveSegmentTree, Test) { TestSimpleSegmentTree<NaiveSegmentTree>(); }

// TEST(MappedSegmentTree, Test) { TestSimpleSegmentTree<MappedSegmentTree>(); }

// TEST(SegmentTree, Test) { TestSimpleSegmentTree<SegmentTree>(); }

TEST(MappedSegmentTree_Vs_Naive, RandomTest) {
  RandomTest<mapped_segment_tree<int>, naive_segment_tree<int>>();
}

#if 0
TEST(MappedSegmentTree_Vs_SegmentTree, RandomTest) {
  RandomTest<MappedSegmentTree<int, test_reducer<int>, test_mapper<int>>,
             SegmentTree<int, test_reducer<int>, test_mapper<int>>>();
}
#endif

#if 0
TEST(TestSegmentTree_Vs_Naive, RandomTest) {
  RandomTest<TestSegmentTree<int, test_reducer<int>, test_mapper<int>>,
             NaiveSegmentTree<int, test_reducer<int>, test_mapper<int>>>();
}
#endif