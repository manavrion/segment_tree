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

static void BM_Query_Simple(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  segment_tree<int> st;
  st.assign(numbers.begin(), numbers.end());
  size_t r = 0;
  for (auto _ : state) {
    size_t start = r % st.size();
    if (start + st.size() / 2 >= st.size()) {
      r = 0;
      start = 0;
    }
    ++r;
    benchmark::DoNotOptimize(st.query(start, start + st.size() / 2));
  }
}

BENCHMARK(BM_Query_Simple)->Range(2, 1 << 24);

static void BM_Query_Mapped(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  mapped_segment_tree<int> st;
  st.assign(numbers.begin(), numbers.end());
  size_t r = 0;
  for (auto _ : state) {
    size_t start = r % st.size();
    if (start + st.size() / 2 >= st.size()) {
      r = 0;
      start = 0;
    }
    ++r;
    benchmark::DoNotOptimize(st.query(start, start + st.size() / 2));
  }
}

BENCHMARK(BM_Query_Mapped)->Range(2, 1 << 24);

static void BM_Query_Naive(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  naive_segment_tree<int> st;
  st.assign(numbers.begin(), numbers.end());
  size_t r = 0;
  for (auto _ : state) {
    size_t start = r % st.size();
    if (start + st.size() / 2 >= st.size()) {
      r = 0;
      start = 0;
    }
    ++r;
    benchmark::DoNotOptimize(st.query(start, start + st.size() / 2));
  }
}

BENCHMARK(BM_Query_Naive)->Range(2, 1 << 24);
