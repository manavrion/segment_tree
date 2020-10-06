//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#pragma once
#include <algorithm>
#include <numeric>
#include <vector>

inline std::vector<int> get_numbers(size_t n) {
  std::vector<int> res(n);
  std::iota(res.begin(), res.end(), 0);
  return res;
}

struct comb {
  int sum;
  int mul;
};

struct comb_reducer {
  comb operator()(const comb& lhs, const comb& rhs) const {
    return comb{lhs.sum + rhs.sum, lhs.mul + rhs.mul};
  }
};

struct comb_mapper {
  comb operator()(int arg) const { return comb{arg, arg}; }
};

struct quad {
  int sum;
  int mul;
  int min;
  int max;
};

struct quad_reducer {
  quad operator()(const quad& lhs, const quad& rhs) const {
    return quad{lhs.sum + rhs.sum, lhs.mul + rhs.mul,
                std::min(lhs.min, rhs.min), std::max(lhs.max, rhs.max)};
  }
};

struct quad_mapper {
  quad operator()(int arg) const { return quad{arg, arg, arg, arg}; }
};
