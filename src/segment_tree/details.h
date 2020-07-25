//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#pragma once
#include <algorithm>

namespace manavrion::details {

template <typename T>
struct default_reduce_result_t {
  T min;
  T max;
  T sum;
};

template <typename T>
struct default_mapper {
  auto operator()(const T& t) const noexcept {
    return default_reduce_result_t<T>{t, t, t};
  }
};

template <typename T>
struct default_reducer {
  auto operator()(const default_reduce_result_t<T>& lhs,
                  const default_reduce_result_t<T>& rhs) const noexcept {
    return default_reduce_result_t<T>{std::min(lhs.min, rhs.min),
                                      std::max(lhs.max, rhs.max),
                                      lhs.sum + rhs.sum};
  }
};

}  // namespace manavrion::details
