//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <benchmark/benchmark.h>

#include <numeric>
#include <vector>

#include "manavrion/segment_tree/mapped_segment_tree.h"
#include "manavrion/segment_tree/naive_segment_tree.h"
#include "manavrion/segment_tree/segment_tree.h"

using namespace manavrion::segment_tree;

static auto get_numbers(size_t n) {
  std::vector<int> res(n);
  std::iota(res.begin(), res.end(), 0);
  return res;
}

static void BM_Build_Combined_Simple(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  segment_tree<int, std::plus<int>> st1;
  segment_tree<int, std::multiplies<int>> st2;
  st1.reserve(numbers.size());
  st2.reserve(numbers.size());
  for (auto _ : state) {
    st1.assign(numbers.begin(), numbers.end());
    st2.assign(numbers.begin(), numbers.end());
  }
}

BENCHMARK(BM_Build_Combined_Simple)->Range(2, 1 << 24);

static void BM_Build_Combined_Mapped(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));

  auto reducer = [](auto lhs, auto rhs) {
    return std::make_pair(lhs.first + rhs.first, lhs.second * rhs.second);
  };
  auto mapper = [](auto arg) { return std::make_pair(arg, arg); };
  mapped_segment_tree<int, decltype(reducer), decltype(mapper)> st(reducer,
                                                                   mapper);
  st.reserve(numbers.size());
  for (auto _ : state) {
    st.assign(numbers.begin(), numbers.end());
  }
}

BENCHMARK(BM_Build_Combined_Mapped)->Range(2, 1 << 24);

static void BM_Build_Combined_Naive(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  naive_segment_tree<int, std::plus<int>> st1;
  naive_segment_tree<int, std::multiplies<int>> st2;
  st1.reserve(numbers.size());
  st2.reserve(numbers.size());
  for (auto _ : state) {
    st1.assign(numbers.begin(), numbers.end());
    st2.assign(numbers.begin(), numbers.end());
  }
}

BENCHMARK(BM_Build_Combined_Naive)->Range(2, 1 << 24);
