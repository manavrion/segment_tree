//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#pragma once
#include <numeric>
#include <vector>

inline auto get_numbers(size_t n) {
  std::vector<int> res(n);
  std::iota(res.begin(), res.end(), 0);
  return res;
}
