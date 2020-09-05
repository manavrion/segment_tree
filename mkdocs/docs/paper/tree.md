# Tree

## Classic segment tree structure

```mermaid
graph TD
    0 --> 1
    0 --> 2
    1 --> 3
    1 --> 4
    2 --> 5
    2 --> 6
    3 --> 7
    3 --> 8
    4 --> 9
    4 --> 10
    5 --> 11
    5 --> 12
    6 --> 13
    6 --> 14
```

## Used format

Tree:
```mermaid
graph TD
    0 --> 1
    0 --> 2
    1 --> 3
    1 --> 4
    2 --> 5
    2 --> 6
```

And plain data:
```mermaid
graph TD
    0:::plain
    1:::plain
    2:::plain
    3:::plain
    4:::plain
    5:::plain
    6:::plain
    7:::plain
    classDef plain fill:#fff;
```

Let's define 'shift' value as an index of the left-bottom node of a given tree.
For this case, 'shift' equals to 3.

Also, we can define 'shift_up' and 'shift_down' operations.

shift_up = shift / 2

shift_down = shift * 2 + 1
