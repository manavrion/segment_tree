#include <gtest/gtest.h>

#include "segment_tree/node_based_segment_tree.h"
#include "segment_tree/segment_tree.h"

template <typename T>
struct default_minmax {
  auto operator()(const T& lhs, const T& rhs) {
    return std::pair<const T&, const T&>(std::min(lhs, rhs),
                                         std::max(lhs, rhs));
  }
};

template <typename T>
struct default_minmax_reducer {
  std::pair<const T&, const T&> operator()(
      const std::pair<const T&, const T&>& lhs,
      const std::pair<const T&, const T&>& rhs) {
    return std::pair<const T&, const T&>(std::min(lhs.first, rhs.first),
                                         std::max(lhs.second, rhs.second));
  }
};

using namespace manavrion;

TEST(SimpleTest, Test) {
  segment_tree<int> st1;
  node_based_segment_tree<int> st2;
}
