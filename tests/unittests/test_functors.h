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
                                   lhs.sum + rhs.sum, lhs.mul * rhs.mul};
  }
};
