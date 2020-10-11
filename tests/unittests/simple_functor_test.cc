//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <gtest/gtest.h>

#include <array>

#include "manavrion/segment_tree/mapped_segment_tree.h"
#include "manavrion/segment_tree/naive_segment_tree.h"
#include "manavrion/segment_tree/segment_tree.h"

using namespace manavrion::segment_tree;

namespace {

struct min_test_reducer {
  int operator()(int lhs, int rhs) const noexcept { return std::min(lhs, rhs); }
};

template <typename SegmentTree>
void SimpleFunctorTest() {
  const std::array<int, 10> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  SegmentTree st(numbers.begin(), numbers.end());

  EXPECT_EQ(st.query(0, 10), 1);
  EXPECT_EQ(st.query(3, 6), 4);
  EXPECT_EQ(st.query(1, 10), 2);

  st.update(4, 1);
  st.update(2, 5);
  st.update(3, 2);
  st.update(0, 11);
  st.update(1, 7);

  SegmentTree st2({11, 7, 5, 2, 1, 6, 7, 8, 9, 10});
  EXPECT_EQ(st, st2);

  EXPECT_EQ(st.query(0, 10), 1);
  EXPECT_EQ(st.query(3, 6), 1);
  EXPECT_EQ(st.query(1, 10), 1);
}

}  // namespace

TEST(SimpleFunctorTest, MappedSegmentTree) {
  SimpleFunctorTest<mapped_segment_tree<int, min_test_reducer>>();
}

TEST(SimpleFunctorTest, NaiveSegmentTree) {
  SimpleFunctorTest<naive_segment_tree<int, min_test_reducer>>();
}

TEST(SimpleFunctorTest, SimpleSegmentTree) {
  SimpleFunctorTest<segment_tree<int, min_test_reducer>>();
}
