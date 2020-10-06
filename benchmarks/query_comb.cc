//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#include <benchmark/benchmark.h>

#include "benchmark_helpers.h"
#include "manavrion/segment_tree/mapped_segment_tree.h"
#include "manavrion/segment_tree/naive_segment_tree.h"
#include "manavrion/segment_tree/segment_tree.h"

using namespace manavrion::segment_tree;

static void BM_Query_Combined_Simple(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  segment_tree<int, std::plus<int>> st1;
  segment_tree<int, std::multiplies<int>> st2;
  st1.assign(numbers.begin(), numbers.end());
  st2.assign(numbers.begin(), numbers.end());
  size_t r = 0;
  for (auto _ : state) {
    size_t start = r % numbers.size();
    if (start + numbers.size() / 2 >= numbers.size()) {
      r = 0;
      start = 0;
    }
    ++r;
    auto res1 = st1.query(start, start + st1.size() / 2);
    auto res2 = st2.query(start, start + st1.size() / 2);
    benchmark::DoNotOptimize(res1 + res2);
  }
}

BENCHMARK(BM_Query_Combined_Simple)->Range(2, 1 << 24);

static void BM_Query_Combined_Mapped(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  mapped_segment_tree<int, comb_reducer,comb_mapper> st;
  st.assign(numbers.begin(), numbers.end());
  size_t r = 0;
  for (auto _ : state) {
    size_t start = r % numbers.size();
    if (start + numbers.size() / 2 >= numbers.size()) {
      r = 0;
      start = 0;
    }
    ++r;
    auto res = st.query(start, start + st.size() / 2);
    benchmark::DoNotOptimize(res.sum + res.mul);
  }
}

BENCHMARK(BM_Query_Combined_Mapped)->Range(2, 1 << 24);

static void BM_Query_Combined_Naive(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  naive_segment_tree<int, std::plus<int>> st1;
  naive_segment_tree<int, std::multiplies<int>> st2;
  st1.assign(numbers.begin(), numbers.end());
  st2.assign(numbers.begin(), numbers.end());
  size_t r = 0;
  for (auto _ : state) {
    size_t start = r % numbers.size();
    if (start + numbers.size() / 2 >= numbers.size()) {
      r = 0;
      start = 0;
    }
    ++r;
    auto res1 = st1.query(start, start + st1.size() / 2);
    auto res2 = st2.query(start, start + st1.size() / 2);
    benchmark::DoNotOptimize(res1 + res2);
  }
}

BENCHMARK(BM_Query_Combined_Naive)->Range(2, 1 << 24);
