//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <functional>
#include <type_traits>
#include <vector>

#include "details.h"

namespace manavrion {

template <typename T, typename Functor = details::default_min<T>,
          typename Reducer = Functor>
class segment_tree {
  static_assert(std::is_invocable_v<Functor, T, T>);
  using functor_result = std::decay_t<std::invoke_result_t<Functor, T, T>>;
  static_assert(std::is_invocable_v<Reducer, functor_result, functor_result>);
  static_assert(std::is_convertible_v<
                std::invoke_result_t<Reducer, functor_result, functor_result>,
                functor_result>);

 public:
  using value_type = T;
  using node_type = functor_result;

 public:
  segment_tree() {}

 private:
  Functor functor_;
  Reducer reducer_;
  std::vector<value_type> data_;
  std::vector<node_type> nodes_;
};

}  // namespace manavrion
