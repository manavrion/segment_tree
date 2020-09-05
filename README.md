# Segment tree
A generic segment tree C++ header-only library.

## Examples

### Default functor

```C++
// "Min" functor will be used at segment_tree by default.
segment_tree<int> st = {0, 1, 2, 3, 4};

// Finds a minimum value in [2, 5) range.
// Time complexity - O(log n) where "n" is st.size().
EXPECT_EQ(st.query(2, 5), 2);

// Rewrites a value located at index 2 with value 5.
// Time complexity - O(log n) where "n" is st.size().
st.update(2, 5);
EXPECT_EQ(st, segment_tree<int>({0, 1, 5, 3, 4}));
EXPECT_EQ(st.query(2, 5), 3);
```

### Custom functor

```C++
auto plus = [](auto lhs, auto rhs) { return lhs + rhs; };

// You can use custom functors.
segment_tree<int, decltype(plus)> st({0, 1, 2, 3, 4}, plus);
EXPECT_EQ(st.query(2, 5), 9);
```

### Combined functor

```C++
// You can combine operations.
// To achieve it, you have to define reducer and mapper.

auto reducer = [](auto lhs, auto rhs) {
  return std::make_pair(lhs.first + rhs.first, lhs.second * rhs.second);
};
auto mapper = [](auto arg) { return std::make_pair(arg, arg); };

segment_tree<int, decltype(reducer), decltype(mapper)> st({0, 1, 2, 3, 4},
                                                            reducer, mapper);
auto result = st.query(2, 5);
EXPECT_EQ(result.first, 9);
EXPECT_EQ(result.second, 24);
```

# How to build

1. git clone git@github.com:manavrion/segment_tree.git
2. ./build.sh

## Clang compiler

1. sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
2. update-alternatives --install /usr/bin/clang clang /usr/bin/clang-10 10
3. update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-10 10

# How to test

1. ./test.sh
