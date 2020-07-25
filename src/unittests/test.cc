#include <gtest/gtest.h>

#include "manavrion/segment_tree/node_based_segment_tree.h"
#include "manavrion/segment_tree/segment_tree.h"

using namespace manavrion::segment_tree;

TEST(SimpleTest, Test) {
  segment_tree<int> st1;
  node_based_segment_tree<int> st2;
}

TEST(NodeBasedSegmentTree, BasicTest) {
  const std::vector numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  node_based_segment_tree<int> st(numbers.begin(), numbers.end());
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
