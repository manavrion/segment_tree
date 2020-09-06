//
// Copyright (C) 2020 Ruslan Manaev (manavrion@gmail.com)
// This file is part of the segment_tree header-only library.
//

#pragma once
#include <random>

template <typename Test, typename Canonical>
void RandomTestImpl(const std::vector<int> as) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(-5, 5);

  Test test(as.begin(), as.end());
  Canonical canonical(as.begin(), as.end());

  auto make_all_query = [&]() {
    // EXPECT_EQ(test, canonical);
    for (size_t first_index = 0; first_index <= as.size(); ++first_index) {
      for (size_t last_index = first_index; last_index <= as.size();
           ++last_index) {
        auto test_res = test.query(first_index, last_index);
        auto canonical_res = canonical.query(first_index, last_index);
        EXPECT_EQ(test_res.min, canonical_res.min);
        EXPECT_EQ(test_res.max, canonical_res.max);
        EXPECT_EQ(test_res.sum, canonical_res.sum);
        EXPECT_EQ(test_res.mul, canonical_res.mul);
      }
    }
  };
  make_all_query();

  if (!as.empty()) {
    std::uniform_int_distribution<> dist_indexes(0, as.size() - 1);
    for (size_t update_count = 0; update_count < 100; ++update_count) {
      const size_t index = dist_indexes(gen);
      int value = dist(gen);
      test.update(index, value);
      canonical.update(index, value);
    }
  }
  make_all_query();

  for (size_t first_index = 0; first_index <= as.size(); ++first_index) {
    for (size_t last_index = first_index; last_index <= as.size();
         ++last_index) {
      int value = dist(gen);
      std::fill(test.begin() + first_index, test.begin() + last_index, value);
      test.update_range(test.begin() + first_index, test.begin() + last_index);

      std::fill(canonical.begin() + first_index, canonical.begin() + last_index,
                value);
      canonical.update_range(canonical.begin() + first_index,
                             canonical.begin() + last_index);
    }
  }
  make_all_query();
}

template <typename Test, typename Canonical>
void RandomTest() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(-5, 5);

  // size *
  for (size_t size = 0; size < 20; ++size) {
    for (size_t repeat = 0; repeat < 10; ++repeat) {
      std::vector<int> as(size);
      for (auto& a : as) {
        a = dist(gen);
      }
      RandomTestImpl<Test, Canonical>(std::move(as));
      if (size == 0) break;
    }
  }
}
