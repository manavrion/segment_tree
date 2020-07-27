//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#pragma once
#include <iterator>
#include <type_traits>

namespace manavrion::segment_tree::details {

template <typename Functor>
struct scoped {
  scoped(Functor functor) : functor_(std::move(functor)) {}
  ~scoped() { functor_(); }

 private:
  Functor functor_;
};

template <typename Functor>
scoped(Functor) -> scoped<Functor>;

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template <typename InputIt>
using require_input_iter = std::enable_if_t<std::is_convertible_v<
    typename std::iterator_traits<InputIt>::iterator_category,
    std::input_iterator_tag>>;

}  // namespace manavrion::segment_tree::details
