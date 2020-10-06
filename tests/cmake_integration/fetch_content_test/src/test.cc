//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <iostream>

#include "manavrion/segment_tree/segment_tree.h"

using manavrion::segment_tree::segment_tree;

int main() {
  segment_tree<int> st = {0, 1, 2, 3, 4};

  std::cout << st.query(2, 5) << std::endl;

  st.update(2, 5);
  std::cout << st.query(2, 5) << std::endl;
}
