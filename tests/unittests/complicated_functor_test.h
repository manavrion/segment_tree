//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#pragma once
#include "test_functors.h"

template <template <typename, typename, typename> typename SegmentTree>
void ComplicatedFunctorTest() {
  const std::vector numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  SegmentTree<int, test_reducer<int>, test_mapper<int>> st(numbers.begin(),
                                                           numbers.end());
  auto r1 = st.query(0, 10);
  EXPECT_EQ(r1.min, 1);
  EXPECT_EQ(r1.max, 10);
  EXPECT_EQ(r1.sum, 55);
  EXPECT_EQ(r1.mul, 3'628'800);

  auto r2 = st.query(3, 6);
  EXPECT_EQ(r2.min, 4);
  EXPECT_EQ(r2.max, 6);
  EXPECT_EQ(r2.sum, 15);
  EXPECT_EQ(r2.mul, 120);

  auto r3 = st.query(1, 10);
  EXPECT_EQ(r3.min, 2);
  EXPECT_EQ(r3.max, 10);
  EXPECT_EQ(r3.sum, 54);
  EXPECT_EQ(r1.mul, 3'628'800);

  st.update(4, 1);
  st.update(2, 5);
  st.update(3, 2);
  st.update(0, 11);
  st.update(1, 7);

  // actual state {11, 7, 5, 2, 1, 6, 7, 8, 9, 10}

  auto r4 = st.query(0, 10);
  EXPECT_EQ(r4.min, 1);
  EXPECT_EQ(r4.max, 11);
  EXPECT_EQ(r4.sum, 66);
  EXPECT_EQ(r4.mul, 23'284'800);

  auto r5 = st.query(3, 6);
  EXPECT_EQ(r5.min, 1);
  EXPECT_EQ(r5.max, 6);
  EXPECT_EQ(r5.sum, 9);
  EXPECT_EQ(r5.mul, 12);

  auto r6 = st.query(1, 10);
  EXPECT_EQ(r6.min, 1);
  EXPECT_EQ(r6.max, 10);
  EXPECT_EQ(r6.sum, 55);
  EXPECT_EQ(r6.mul, 2'116'800);

  //    0, 1, 2, 3, 4, 5, 6, 7
  st = {2, 3, 3, 2, 4, 0, 0};

  auto r7 = st.query(0, 7);
  EXPECT_EQ(r7.min, 0);
  EXPECT_EQ(r7.max, 4);
  EXPECT_EQ(r7.sum, 14);
  EXPECT_EQ(r7.mul, 0);

  auto r8 = st.query(0, 6);
  EXPECT_EQ(r8.min, 0);
  EXPECT_EQ(r8.max, 4);
  EXPECT_EQ(r8.sum, 14);
  EXPECT_EQ(r8.mul, 0);

  auto r9 = st.query(0, 5);
  EXPECT_EQ(r9.min, 2);
  EXPECT_EQ(r9.max, 4);
  EXPECT_EQ(r9.sum, 14);
  EXPECT_EQ(r9.mul, 144);
}
