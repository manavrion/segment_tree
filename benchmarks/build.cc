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

static void BM_Build_Simple(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  segment_tree<int> st;
  st.reserve(numbers.size());
  for (auto _ : state) {
    st.assign(numbers.begin(), numbers.end());
  }
}

BENCHMARK(BM_Build_Simple)->Range(2, 1 << 24);

static void BM_Build_Mapped(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  mapped_segment_tree<int> st;
  st.reserve(numbers.size());
  for (auto _ : state) {
    st.assign(numbers.begin(), numbers.end());
  }
}

BENCHMARK(BM_Build_Mapped)->Range(2, 1 << 24);

static void BM_Build_Naive(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  naive_segment_tree<int> st;
  st.reserve(numbers.size());
  for (auto _ : state) {
    st.assign(numbers.begin(), numbers.end());
  }
}

BENCHMARK(BM_Build_Naive)->Range(2, 1 << 24);
