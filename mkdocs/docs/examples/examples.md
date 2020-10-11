# Default functor

```C++
  // std::plus functor will be used at segment_tree by default.
  segment_tree<int> st = {0, 1, 2, 3, 4};

  // Finds a minimum value in [2, 5) range.
  // Time complexity - O(log n) where "n" is st.size().
  std::cout << st.query(2, 5) << std::endl;
  // Prints: 9

  // Rewrites a value located at index 2 with value 5.
  // Time complexity - O(log n) where "n" is st.size().
  st.update(2, 5);
  std::cout << st.query(2, 5) << std::endl;
  // Prints: 12
```

# Custom functor

```C++
  // You can use custom functors.
  segment_tree<int, std::multiplies<int>> st({0, 1, 2, 3, 4});
  std::cout << st.query(2, 5) << std::endl;
  // Prints: 24
```

# Combined functor

```C++
  // You can combine operations.
  // To achieve it, you have to define reducer and mapper.

  auto reducer = [](auto lhs, auto rhs) {
    return std::make_pair(lhs.first + rhs.first, lhs.second * rhs.second);
  };
  auto mapper = [](auto arg) { return std::make_pair(arg, arg); };

  mapped_segment_tree<int, decltype(reducer), decltype(mapper)> st(
      {0, 1, 2, 3, 4}, reducer, mapper);
  auto result = st.query(2, 5);
  std::cout << "sum: " << result.first << std::endl;
  // Prints: "sum: 9"
  std::cout << "mul: " << result.second << std::endl;
  // Prints: "mul: 24"
```
