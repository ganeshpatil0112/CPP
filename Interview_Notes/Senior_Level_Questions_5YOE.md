# C++ Interview Questions — 5 Years Experience (2025-2026)

A curated list of questions currently being asked for **C++ developers with ~5 years of experience**.
Organized by topic and frequency. Goes a level deeper than the 2-3 YOE list in
[../CPP_Interview_Preparation.md](../CPP_Interview_Preparation.md).

---

## Table of Contents
1. [Core Language & Modern C++](#1-core-language--modern-c)
2. [Memory & Smart Pointers](#2-memory--smart-pointers)
3. [OOP / Polymorphism Deep Dive](#3-oop--polymorphism-deep-dive)
4. [Templates & Metaprogramming](#4-templates--metaprogramming)
5. [STL Internals](#5-stl-internals)
6. [Concurrency & Multithreading](#6-concurrency--multithreading)
7. [Design Patterns & Architecture](#7-design-patterns--architecture)
8. [SOLID & Design Discussion](#8-solid--design-discussion)
9. [Performance / Low-Latency](#9-performance--low-latency)
10. [Debugging & Production Issues](#10-debugging--production-issues)
11. [System Design / Scenario Questions](#11-system-design--scenario-questions)
12. [Behavioral / Project Questions](#12-behavioral--project-questions)
13. [Recent Hot Topics (2025-2026)](#13-recent-hot-topics-2025-2026)

---

## 1. Core Language & Modern C++
*(High Frequency)*

1. Difference between `std::move` and `std::forward` — when does perfect forwarding break?
2. What is reference collapsing? Explain `T&&` in templates (universal / forwarding references).
3. Explain `noexcept` — why does it matter for move constructors and `std::vector` growth?
4. Difference between `constexpr`, `consteval` (C++20), and `constinit` (C++20).
5. What is copy elision / RVO / NRVO? When is it mandatory (C++17)?
6. Why is `std::string` SSO (Small String Optimization) important? How does it affect move performance?
7. Difference between `auto`, `decltype`, and `decltype(auto)`.
8. What does `explicit` do on constructors and conversion operators? Why use `explicit` on single-arg constructors?
9. Lambda internals — what does the compiler generate? Capture by value vs reference vs move-capture (`[x = std::move(v)]`).
10. Difference between `std::function` and a raw function pointer — overhead of `std::function`?
11. CTAD (Class Template Argument Deduction) — C++17.
12. Structured bindings — how do they work under the hood?
13. `std::optional` vs pointer vs sentinel value — when to use each?
14. `std::variant` vs `union` vs inheritance-based polymorphism.
15. What are C++20 Concepts? Replace SFINAE with a concept example.
16. C++20 Ranges — `views` vs `algorithms`; lazy evaluation.
17. Three-way comparison operator `<=>` (spaceship) — C++20.
18. Coroutines basics — `co_await`, `co_yield`, `co_return` (high-level understanding expected).
19. Modules (C++20) — advantages over headers.

---

## 2. Memory & Smart Pointers
*(Very High Frequency)*

20. Implement `shared_ptr` from scratch (control block, atomic refcount, `weak_ptr` interaction).
21. Implement `unique_ptr` — how is copy disabled? (deleted copy ctor/assign).
22. Why prefer `make_shared` over `new` + `shared_ptr`? When does it backfire? (delayed memory release when `weak_ptr` outlives).
23. How does `enable_shared_from_this` work internally?
24. Aliasing constructor of `shared_ptr` — use case?
25. Custom deleter in `unique_ptr` vs `shared_ptr` — size difference?
26. Circular reference scenario — fix with `weak_ptr`.
27. What is a memory pool / arena allocator? Implement one. When is it useful (HFT, game engines)?
28. Placement `new` — use cases.
29. `new` vs `malloc` — alignment, constructor invocation, exceptions.
30. What is `std::pmr` (polymorphic memory resources, C++17)?
31. Memory alignment — `alignas`, `alignof`, false sharing, cache line padding.
32. How would you detect a memory leak in production?

---

## 3. OOP / Polymorphism Deep Dive

33. Layout of an object with virtual functions — vptr location, vtable structure.
34. Multiple inheritance — diamond problem, virtual inheritance, vtable layout.
35. Cost of a virtual call vs direct call vs `std::function` call.
36. Why must a base class destructor be virtual? What if it's not?
37. Calling virtual functions from constructor/destructor — what happens and why?
38. CRTP (Curiously Recurring Template Pattern) — static polymorphism. Implement and explain.
39. Type erasure — how does `std::function` / `std::any` achieve it? Implement a simple `AnyCallable`.
40. Pimpl idiom — pros/cons, impact on compile time and ABI.
41. Rule of 0/3/5 — when do you write each? Why "Rule of Zero" is preferred?
42. Difference between `override`, `final`, and `virtual` — runtime/compile-time effects.
43. Slicing problem — example and how to prevent.

---

## 4. Templates & Metaprogramming

44. SFINAE vs Concepts vs `if constexpr` — when to use which.
45. Variadic templates — fold expressions (C++17), parameter packs.
46. Explicit specialization vs partial specialization (only for classes).
47. Implement `std::is_same`, `std::enable_if`, `std::conditional`.
48. Template instantiation — when does it happen? Two-phase name lookup.
49. CRTP for compile-time polymorphism — example.
50. Tag dispatch vs `if constexpr` vs concepts.

---

## 5. STL Internals
*(Frequently Asked at Senior Level)*

51. `std::vector` growth strategy — why 1.5x or 2x? Amortized O(1) `push_back` proof.
52. Iterator invalidation rules for `vector`, `deque`, `map`, `unordered_map`.
53. `std::map` (RB-tree) vs `std::unordered_map` (hash table) — internals, complexities, when to use.
54. How does `std::unordered_map` handle collisions? Load factor, rehashing.
55. Custom hash and equality for user-defined types in `unordered_map`.
56. Custom comparator for `std::set` / `std::map` — strict weak ordering requirement.
57. `std::deque` internal structure — chunked array.
58. `emplace` vs `insert` vs `push_back` — when does emplace actually win?
59. `std::array` vs C-array vs `std::vector` on the stack.
60. `std::string_view` and `std::span` — lifetime pitfalls.
61. Allocator-aware containers — when to write a custom allocator.

---

## 6. Concurrency & Multithreading
*(Senior-Level Focus)*

62. C++ memory model — `memory_order_relaxed`, `acquire`, `release`, `acq_rel`, `seq_cst`. Explain with example.
63. Implement a thread-safe Singleton (Meyers' singleton, why it works after C++11).
64. Implement a thread pool from scratch.
65. Implement a producer-consumer queue using `std::mutex` + `condition_variable`.
66. Lock-free queue (SPSC) using `std::atomic` — explain ABA problem.
67. `std::mutex` vs `std::shared_mutex` vs `std::recursive_mutex`.
68. `std::lock_guard` vs `std::unique_lock` vs `std::scoped_lock` (C++17).
69. `std::async` launch policies (`async` vs `deferred`) — pitfalls.
70. `std::promise` / `std::future` / `std::packaged_task` — when to use each.
71. `std::jthread` and `std::stop_token` (C++20).
72. Deadlock avoidance — lock ordering, `std::lock`, `std::scoped_lock`.
73. False sharing — what it is and how to prevent (`alignas(std::hardware_destructive_interference_size)`).
74. Double-checked locking — why broken pre-C++11, fixed via `std::atomic` or `std::call_once`.
75. Difference between `std::atomic<T>` and `volatile T` — why `volatile` is NOT for threading.

---

## 7. Design Patterns & Architecture
*(5 YOE expects real examples from your projects)*

76. Singleton — thread-safe variants, why it's often considered an anti-pattern.
77. Factory vs Abstract Factory — concrete code.
78. Observer pattern — implement with `weak_ptr` to avoid dangling.
79. Strategy vs Template Method.
80. Decorator vs Adapter vs Proxy.
81. RAII as a design pattern — examples beyond locks/files.
82. Dependency Injection in C++ (no framework — pure idioms).
83. "Which patterns have you used in your project?" — be ready with 2-3 concrete stories.

---

## 8. SOLID & Design Discussion

84. SRP, OCP, LSP, ISP, DIP — give a C++ code example violating and fixing each.
85. LSP violation in inheritance — Rectangle/Square classic example.
86. How does DIP relate to interfaces (abstract base classes) in C++?

---

## 9. Performance / Low-Latency
*(Asked for HFT, Trading, Systems Roles)*

87. Cache-friendly data layout — AoS vs SoA.
88. Branch prediction — `[[likely]]` / `[[unlikely]]` (C++20).
89. Avoiding heap allocation in hot path — stack, pool, ring buffer.
90. `inline` vs LTO — does `inline` actually inline?
91. Profiling tools you've used (perf, VTune, gprof, callgrind).
92. Lock-free vs wait-free — difference.
93. Zero-copy techniques.
94. Why is `std::endl` slower than `'\n'`?
95. Empty Base Optimization (EBO).

---

## 10. Debugging & Production Issues
*(Very Common at 5 YOE)*

96. How do you debug a crash with only a core dump?
97. Tools: gdb, valgrind, AddressSanitizer, ThreadSanitizer, UBSan — what catches what?
98. Common undefined behavior cases (signed overflow, strict aliasing, dangling reference).
99. How to debug a deadlock in production?
100. ABI compatibility — what breaks ABI? (changing vtable layout, member order, etc.)

---

## 11. System Design / Scenario Questions

101. Design a logger (thread-safe, async, lock-free queue).
102. Design an LRU cache (with `std::list` + `std::unordered_map`, O(1) ops).
103. Design a message dispatcher / event bus.
104. Design a thread pool with task priorities.
105. Design a memory allocator for fixed-size objects.
106. How would you replace a `std::map` in a hot path? (flat_map, sorted vector, hash map).

---

## 12. Behavioral / Project Questions
*(Expected at 5 YOE)*

107. "Walk me through the most complex bug you fixed."
108. "Where did you use move semantics to improve performance? Quantify the improvement."
109. "Describe a design decision you'd reverse now."
110. "How do you do code reviews?"
111. "How do you onboard a junior dev?"
112. "Tell me about a time you disagreed with a senior engineer on design."
113. "How do you stay updated with new C++ standards?"

---

## 13. Recent Hot Topics (2025-2026)

- **C++23 features**: `std::expected`, `std::print`, `if consteval`, deducing `this` (explicit object parameter).
- **`std::flat_map` / `std::flat_set`** (C++23) — cache-friendly alternatives to `std::map` / `std::set`.
- **Senders / Receivers** — executor proposal preview (P2300).
- **Safer C++** — bounds checking, profiles, comparisons with Rust (memory safety questions).
- **`std::mdspan`** (C++23) — multi-dimensional non-owning view.
- **`std::generator`** (C++23) — coroutine-based range.

---

## Preparation Tips for 5 YOE

- **Depth over breadth**: Interviewers expect you to explain *internals*, not just usage.
- **Bring project stories**: Every concept (move semantics, smart pointers, threading) should map to something you've actually built.
- **Whiteboard practice**: Implement `shared_ptr`, thread pool, LRU cache, and a simple memory pool until you can do them under 20 minutes.
- **Know your tooling**: sanitizers, profilers, debuggers — expect "how would you find this bug?" questions.
- **System design**: At 5 YOE, expect at least one design round (logger, cache, dispatcher).
- **Behavioral readiness**: Have 3-4 STAR-format stories ready (conflict, failure, leadership, complex bug).

---
