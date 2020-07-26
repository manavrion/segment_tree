//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#pragma once
#include <algorithm>

namespace manavrion::details {

struct default_reducer {
  template <typename T>
  auto operator()(const T& lhs, const T& rhs) const noexcept {
    return std::min(lhs, rhs);
  }
};

template <typename, typename, typename E = void>
struct deduce_mapper {};

template <typename T, typename Reducer>
struct deduce_mapper<T, Reducer,
                     std::enable_if_t<std::is_constructible_v<
                         T, std::invoke_result_t<Reducer, T, T>>>>
    : std::true_type {
  template <typename V>
  decltype(auto) operator()(V&& v) const noexcept {
    return std::forward<V>(v);
  }
};

}  // namespace manavrion::details
