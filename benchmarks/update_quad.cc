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

static void BM_Update_Quad_Simple(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  segment_tree<int, std::plus<int>> st1;
  segment_tree<int, std::multiplies<int>> st2;
  segment_tree<int, minimum<int>> st3;
  segment_tree<int, maximum<int>> st4;
  st1.assign(numbers.begin(), numbers.end());
  st2.assign(numbers.begin(), numbers.end());
  st3.assign(numbers.begin(), numbers.end());
  st4.assign(numbers.begin(), numbers.end());
  size_t r = 0;
  for (auto _ : state) {
    size_t i = r % numbers.size();
    st1.update(i, r);
    st2.update(i, r);
    st3.update(i, r);
    st4.update(i, r);
  }
}

BENCHMARK(BM_Update_Quad_Simple)->Range(2, 1 << 24);

static void BM_Update_Quad_Mapped(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  mapped_segment_tree<int, quad_reducer, quad_mapper> st;
  st.assign(numbers.begin(), numbers.end());
  size_t r = 0;
  for (auto _ : state) {
    size_t i = r % numbers.size();
    st.update(i, r);
  }
}

BENCHMARK(BM_Update_Quad_Mapped)->Range(2, 1 << 24);

static void BM_Update_Quad_Naive(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  naive_segment_tree<int, std::plus<int>> st1;
  naive_segment_tree<int, std::multiplies<int>> st2;
  naive_segment_tree<int, minimum<int>> st3;
  naive_segment_tree<int, maximum<int>> st4;
  st1.assign(numbers.begin(), numbers.end());
  st2.assign(numbers.begin(), numbers.end());
  st3.assign(numbers.begin(), numbers.end());
  st4.assign(numbers.begin(), numbers.end());
  size_t r = 0;
  for (auto _ : state) {
    size_t i = r % numbers.size();
    st1.update(i, r);
    st2.update(i, r);
    st3.update(i, r);
    st4.update(i, r);
  }
}

BENCHMARK(BM_Update_Quad_Naive)->Range(2, 1 << 24);
