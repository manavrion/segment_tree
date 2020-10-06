//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <iostream>

#include "manavrion/segment_tree/segment_tree.h"

using manavrion::segment_tree::segment_tree;

int main() {
  // You can use custom functors.
  segment_tree<int, std::multiplies<int>> st({0, 1, 2, 3, 4});
  std::cout << st.query(2, 5) << std::endl;
  // Prints: 24
}