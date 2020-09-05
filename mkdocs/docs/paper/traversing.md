# Traversing

## Top-down

```mermaid
graph TD
    0 -->|0 * 2 + 1| 1
    0 -->|0 * 2 + 2| 2
    1 -->|1 * 2 + 1| 3
    1 -->|1 * 2 + 2| 4
    2 -->|2 * 2 + 1| 5
    2 -->|2 * 2 + 2| 6
    3 -->|3 * 2 + 1 - 7| v0[0]:::plain
    3 -->|3 * 2 + 2 - 7| v1[1]:::plain
    4 -->|4 * 2 + 1 - 7| v2[2]:::plain
    4 -->|4 * 2 + 2 - 7| v3[3]:::plain
    5 -->|5 * 2 + 1 - 7| v4[4]:::plain
    5 -->|5 * 2 + 2 - 7| v5[5]:::plain
    6 -->|6 * 2 + 1 - 7| v6[6]:::plain
    6 -->|6 * 2 + 2 - 7| v7[7]:::plain
    classDef plain fill:#fff;
```

### Traversing at tree

left_child = i*2 + 1

right_child = i*2 + 2

### Traversing from tree to the plain data

shift = 3

shift_down = 7

left_child = i*2 + 1 - shift_down

right_child = i*2 + 2 - shift_down


## Down-top

```mermaid
graph TD
    1 -->|"(1 - 1) / 2"| 0
    2 -->|"(2 - 1) / 2"| 0
    3 -->|"(3 - 1) / 2"| 1
    4 -->|"(4 - 1) / 2"| 1
    5 -->|"(5 - 1) / 2"| 2
    6 -->|"(6 - 1) / 2"| 2
    v0[0]:::plain -->|"(0 + 7 - 1) / 2"| 3
    v1[1]:::plain -->|"(1 + 7 - 1) / 2"| 3
    v2[2]:::plain -->|"(2 + 7 - 1) / 2"| 4
    v3[3]:::plain -->|"(3 + 7 - 1) / 2"| 4
    v4[4]:::plain -->|"(4 + 7 - 1) / 2"| 5
    v5[5]:::plain -->|"(5 + 7 - 1) / 2"| 5
    v6[6]:::plain -->|"(6 + 7 - 1) / 2"| 6
    v7[7]:::plain -->|"(7 + 7 - 1) / 2"| 6
    classDef plain fill:#fff;
```

### Traversing at tree

parent = (i - 1) / 2

### Traversing from tree to the plain data

shift = 3

shift_down = 7

parent = (j + shift_down - 1) / 2
