//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <gtest/gtest.h>

#include "manavrion/segment_tree/mapped_segment_tree.h"
#include "manavrion/segment_tree/naive_segment_tree.h"
#include "manavrion/segment_tree/segment_tree.h"

using namespace manavrion::segment_tree;

namespace {

template <typename SegmentTree>
void LiteTest() {
  SegmentTree st = {0, 1, 2, 3, 4, 5, 6, 7};
  EXPECT_EQ(st.query(0, 0), 0);
  EXPECT_EQ(st.query(0, 1), 0);
  EXPECT_EQ(st.query(0, 2), 1);
  EXPECT_EQ(st.query(2, 5), 9);
  EXPECT_EQ(st.query(6, 7), 6);
  EXPECT_EQ(st.query(3, 6), 12);
}

}  // namespace

TEST(LiteTest, MappedSegmentTree) { LiteTest<mapped_segment_tree<int>>(); }

TEST(LiteTest, NaiveSegmentTree) { LiteTest<naive_segment_tree<int>>(); }

TEST(LiteTest, SimpleSegmentTree) { LiteTest<segment_tree<int>>(); }
