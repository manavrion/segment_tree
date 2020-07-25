//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <functional>
#include <type_traits>
#include <vector>

#include "details.h"

namespace manavrion {

template <typename T, typename Mapper = details::default_mapper<T>,
          typename Reducer = details::default_reducer<T>>
class segment_tree {
  static_assert(std::is_invocable_v<Mapper, T>);
  using mapper_result = std::decay_t<std::invoke_result_t<Mapper, T>>;
  static_assert(std::is_invocable_v<Reducer, mapper_result, mapper_result>);
  static_assert(std::is_convertible_v<
                std::invoke_result_t<Reducer, mapper_result, mapper_result>,
                mapper_result>);

 public:
  using value_type = T;
  using node_value_type = mapper_result;

 public:
  segment_tree() {}

 private:
  Mapper mapper_;
  Reducer reducer_;
  std::vector<value_type> data_;
  std::vector<node_value_type> nodes_;
};

}  // namespace manavrion
