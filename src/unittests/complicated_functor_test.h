//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#pragma once

template <typename T>
struct test_reduce_result_t {
  T min;
  T max;
  T sum;
  T mul;
};

template <typename T>
struct test_mapper {
  auto operator()(const T& t) const noexcept {
    return test_reduce_result_t<T>{t, t, t, t};
  }
};

template <typename T>
struct test_reducer {
  auto operator()(const test_reduce_result_t<T>& lhs,
                  const test_reduce_result_t<T>& rhs) const noexcept {
    return test_reduce_result_t<T>{std::min(lhs.min, rhs.min),
                                   std::max(lhs.max, rhs.max),
                                   lhs.sum + rhs.sum, lhs.sum * rhs.sum};
  }
};

template <template <typename, typename, typename> typename SegmentTree>
void ComplicatedFunctorTest() {
  const std::vector numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  SegmentTree<int, test_reducer<int>, test_mapper<int>> st(numbers.begin(),
                                                           numbers.end());
  auto r1 = st.query(0, 10);
  EXPECT_EQ(r1.min, 1);
  EXPECT_EQ(r1.max, 10);
  EXPECT_EQ(r1.sum, 55);

  auto r2 = st.query(3, 6);
  EXPECT_EQ(r2.min, 4);
  EXPECT_EQ(r2.max, 6);
  EXPECT_EQ(r2.sum, 15);

  auto r3 = st.query(1, 10);
  EXPECT_EQ(r3.min, 2);
  EXPECT_EQ(r3.max, 10);
  EXPECT_EQ(r3.sum, 54);

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

  auto r5 = st.query(3, 6);
  EXPECT_EQ(r5.min, 1);
  EXPECT_EQ(r5.max, 6);
  EXPECT_EQ(r5.sum, 9);

  auto r6 = st.query(1, 10);
  EXPECT_EQ(r6.min, 1);
  EXPECT_EQ(r6.max, 10);
  EXPECT_EQ(r6.sum, 55);
}
