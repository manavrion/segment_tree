//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#pragma once
#include "manavrion/segment_tree/functors.h"

template <template <typename, typename, typename> typename SegmentTree>
void SimpleFunctorTest() {
  const std::array numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto min = [](const auto& lhs, const auto& rhs) {
    return std::min(lhs, rhs);
  };

  SegmentTree<int, decltype(min),
              manavrion::segment_tree::functors::default_mapper>
      st(numbers.begin(), numbers.end(), min);

  EXPECT_EQ(st.query(0, 10), 1);
  EXPECT_EQ(st.query(3, 6), 4);
  EXPECT_EQ(st.query(1, 10), 2);

  st.update(4, 1);
  st.update(2, 5);
  st.update(3, 2);
  st.update(0, 11);
  st.update(1, 7);

  SegmentTree<int, decltype(min),
              manavrion::segment_tree::functors::default_mapper>
      st2({11, 7, 5, 2, 1, 6, 7, 8, 9, 10}, min);
  EXPECT_EQ(st, st2);

  EXPECT_EQ(st.query(0, 10), 1);
  EXPECT_EQ(st.query(3, 6), 1);
  EXPECT_EQ(st.query(1, 10), 1);
}
