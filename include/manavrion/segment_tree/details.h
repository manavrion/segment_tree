//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#pragma once
#include <iterator>
#include <type_traits>

namespace manavrion::segment_tree::details {

template <typename InputIt>
using require_input_iter = std::enable_if_t<std::is_convertible_v<
    typename std::iterator_traits<InputIt>::iterator_category,
    std::input_iterator_tag>>;

struct default_reducer {
  template <typename T>
  auto operator()(const T& lhs, const T& rhs) const noexcept {
    return std::min(lhs, rhs);
  }
};

struct default_mapper {
  template <typename T>
  decltype(auto) operator()(T&& t) const noexcept {
    return std::forward<T>(t);
  }
};

template <typename, typename, typename E = void>
struct deduce_mapper {};

template <typename T, typename Reducer>
struct deduce_mapper<T, Reducer,
                     std::enable_if_t<std::is_constructible_v<
                         T, std::invoke_result_t<Reducer, T, T>>>>
    : public default_mapper {};

}  // namespace manavrion::segment_tree::details
