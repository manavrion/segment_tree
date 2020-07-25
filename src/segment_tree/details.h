//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#pragma once
#include <algorithm>

namespace manavrion::details {

template <typename T>
struct default_min {
  decltype(auto) operator()(const T& lhs, const T& rhs) const noexcept {
    return std::min(lhs, rhs);
  }
};

}  // namespace manavrion::details
