//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <iostream>

#include "manavrion/segment_tree/mapped_segment_tree.h"

using manavrion::segment_tree::mapped_segment_tree;

int main() {
  // You can combine operations.
  // To achieve it, you have to define reducer and mapper.

  auto reducer = [](auto lhs, auto rhs) {
    return std::make_pair(lhs.first + rhs.first, lhs.second * rhs.second);
  };
  auto mapper = [](auto arg) { return std::make_pair(arg, arg); };

  mapped_segment_tree<int, decltype(reducer), decltype(mapper)> st(
      {0, 1, 2, 3, 4}, reducer, mapper);
  auto result = st.query(2, 5);
  std::cout << "sum: " << result.first << std::endl;
  // Prints: "sum: 9"
  std::cout << "mul: " << result.second << std::endl;
  // Prints: "mul: 24"
}
