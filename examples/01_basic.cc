//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <iostream>

#include "manavrion/segment_tree/segment_tree.h"

using manavrion::segment_tree::segment_tree;

int main() {
  // std::plus functor will be used at segment_tree by default.
  segment_tree<int> st = {0, 1, 2, 3, 4};

  // Finds a minimum value in [2, 5) range.
  // Time complexity - O(log n) where "n" is st.size().
  std::cout << st.query(2, 5) << std::endl;
  // Prints: 9

  // Rewrites a value located at index 2 with value 5.
  // Time complexity - O(log n) where "n" is st.size().
  st.update(2, 5);
  std::cout << st.query(2, 5) << std::endl;
  // Prints: 12
}