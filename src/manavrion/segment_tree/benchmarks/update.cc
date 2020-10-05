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

static void BM_Update_Simple(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  segment_tree<int> st;
  st.assign(numbers.begin(), numbers.end());
  size_t r = 0;
  for (auto _ : state) {
    size_t i = r % st.size();
    st.update(i, r);
  }
}

BENCHMARK(BM_Update_Simple)->Range(2, 1 << 24);

static void BM_Update_Mapped(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  mapped_segment_tree<int> st;
  st.assign(numbers.begin(), numbers.end());
  size_t r = 0;
  for (auto _ : state) {
    size_t i = r % st.size();
    st.update(i, r);
  }
}

BENCHMARK(BM_Update_Mapped)->Range(2, 1 << 24);

static void BM_Update_Naive(benchmark::State& state) {
  auto numbers = get_numbers(state.range(0));
  naive_segment_tree<int> st;
  st.assign(numbers.begin(), numbers.end());
  size_t r = 0;
  for (auto _ : state) {
    size_t i = r % st.size();
    st.update(i, r);
  }
}

BENCHMARK(BM_Update_Naive)->Range(2, 1 << 24);
