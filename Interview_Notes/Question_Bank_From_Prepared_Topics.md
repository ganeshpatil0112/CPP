# C++ Interview Question Bank — From Topics You've Prepared

A consolidated list of interview questions derived **strictly from the topics covered in your existing notes** ([1_OOP_Fundamentals.md](1_OOP_Fundamentals.md) – [6_Performance_Optimization.md](6_Performance_Optimization.md)) and your prep files ([../CPP_Interview_Preparation.md](../CPP_Interview_Preparation.md), `../.txt`).

Use this as a self-test checklist before interviews.

---

## Table of Contents
1. [OOP Fundamentals](#1-oop-fundamentals)
2. [Memory Management](#2-memory-management)
3. [Modern C++ Keywords](#3-modern-c-keywords)
4. [Smart Pointers Deep Dive](#4-smart-pointers-deep-dive)
5. [STL Internals](#5-stl-internals)
6. [Performance Optimization](#6-performance-optimization)
7. [Templates](#7-templates)
8. [Multithreading & Concurrency](#8-multithreading--concurrency)
9. [Design Patterns](#9-design-patterns)
10. [SOLID Principles](#10-solid-principles)
11. [Type Casting](#11-type-casting)
12. [Coding Problems](#12-coding-problems)
13. [Company-Specific Asked Questions](#13-company-specific-asked-questions)

---

## 1. OOP Fundamentals
*Source: [1_OOP_Fundamentals.md](1_OOP_Fundamentals.md)*

### Four Pillars
1. What are the four pillars/concepts of OOP? Explain each with an example.
2. Difference between Encapsulation and Abstraction.
3. What is Inheritance? Types of inheritance in C++.
4. What is Polymorphism? Compile-time vs Run-time polymorphism.

### Abstract Classes & Interfaces
5. What is an Abstract Class? How is it different from a regular class?
6. What is a Pure Virtual Function? Syntax?
7. Can an abstract class have a constructor? A destructor?
8. Can an abstract class have non-virtual member functions?
9. How do you implement an "interface" in C++ (no `interface` keyword)?
10. What happens if a derived class does not override all pure virtual functions?

### Virtual Functions
11. What is a virtual function? Why do we need it?
12. How does the `virtual` keyword work internally?
13. Difference between virtual function and pure virtual function.
14. What happens if you call a virtual function from a constructor or destructor? Why?
15. Which function is called if a base class constructor calls some function? *(HSBC)*
16. Can a static function be virtual? Why not?
17. Can a constructor be virtual? What is the "virtual constructor" idiom?
18. What does the `override` keyword guarantee? What does `final` do?
19. Override with a different return type — is it possible? (covariant return types)
20. What can I do so that a function cannot be overridden in a derived class? *(Finastra)*

### VTable and VPtr
21. What is a vtable? What is a vptr?
22. Where is the vptr stored in the object? Where is the vtable stored?
23. When is the vtable created — compile time or runtime?
24. What is the size impact of adding a virtual function to a class?
25. Draw the vtable layout for a base + derived class with overridden virtual functions.
26. How does a virtual call get dispatched at runtime? (Step by step)
27. Impact of overriding when using smart pointers.

### Polymorphism - Pointer Initialization *(Finastra)*
28. What are the pointer initializations in polymorphism?
29. Write the code for pointer initialization for polymorphism (base pointer to derived object).
30. Difference between `Base* b = new Derived();` and `Derived d; Base* b = &d;`.
31. What is the slicing problem? Show an example.

### This Pointer
32. What is the `this` pointer? Where is it stored?
33. Is `this` available in static member functions? Why not?
34. Can you delete `this` inside a member function? When is it safe?
35. Returning `*this` vs `this` — use cases (method chaining).

### Virtual Destructor
36. Why do we need a virtual destructor?
37. What happens if the base class destructor is NOT virtual and you `delete` via base pointer?
38. Can a destructor be pure virtual? If yes, must it have a body? Why?
39. Can we overload a destructor? *(Answer: No — why?)*

### Constructors
40. List all types of constructors with examples.
41. What is a delegating constructor (C++11)?
42. What is a converting constructor? Why use `explicit`?
43. Default constructor — when is it auto-generated, when is it suppressed?

### Operator Overloading & Function Overloading
44. What is function overloading? Rules?
45. What is operator overloading? Which operators cannot be overloaded?
46. Why overload `operator=`? Self-assignment check importance.

### Static & Const Members
47. What is a static member? Static member function?
48. Can a static function use non-static members of a class? *(BMW)*
49. What is a `const` member function? How does it work internally?
50. How does the `const` keyword propagate through `this`?

---

## 2. Memory Management
*Source: [2_Memory_Management.md](2_Memory_Management.md)*

### Copy Constructor & Assignment
51. What is a copy constructor? Default vs user-defined.
52. Why implement a user-defined copy constructor over the default?
53. Shallow copy vs deep copy — show with code.
54. When is the copy constructor called? List all scenarios.
55. How does the copy constructor behave for `std::vector`, raw array (`int*`), `shared_ptr`, and `unique_ptr`?
56. What does `memcpy` do? Does it work for user-defined objects? Why is it dangerous for non-POD?

### Rule of 0/3/5
57. Explain Rule of Zero, Rule of Three, Rule of Five.
58. When do you write each? Why is "Rule of Zero" preferred?
59. What is the Rule of Five in C++11 (+ move ctor, move assignment)?

### Move Semantics
60. What is move semantics? Why was it introduced in C++11?
61. What is an rvalue reference? Difference between lvalue and rvalue.
62. What does `std::move` actually do? (Spoiler: just a cast.)
63. As a C++ developer, how did you increase execution speed? *(AI interview — answer: move semantics)*
64. With move semantics, how do you ensure deallocation to avoid dangling pointers?
65. Write a move constructor and move assignment operator for a class managing a heap resource.

### Member Initializer List
66. What is a member initializer list? Why use it?
67. Difference between initialization in initializer list vs constructor body.
68. Which members MUST be initialized via the initializer list? (const, references, no-default-ctor base)
69. Order of initialization — declaration order vs initializer list order.

### new / delete / malloc / free
70. Difference between `new` and `malloc`.
71. Difference between `delete` and `free`.
72. How `new`/`delete` work internally (operator new + constructor).
73. Difference between object creation using `new` vs the normal (stack) way. *(Finastra)*
74. What is placement `new`?
75. Difference between `delete p;` and `delete[] p;` — what goes wrong if mixed?

### Dangling Pointers & Leaks
76. What is a dangling pointer? How to avoid?
77. How to avoid memory segmentation faults and memory leaks? *(BMW)*
78. What is RAII? Give examples.

---

## 3. Modern C++ Keywords
*Source: [3_Modern_Cpp_Keywords.md](3_Modern_Cpp_Keywords.md)*

### noexcept
79. What is `noexcept`? Why does it matter?
80. Why does `noexcept` on a move constructor matter for `std::vector` growth?
81. What happens if a `noexcept` function throws?

### override / final
82. What does `override` do? What error does it catch?
83. What does `final` do on a class? On a virtual function?

### explicit
84. What does `explicit` do on a constructor?
85. What does `explicit` do on a conversion operator (C++11)?
86. When should single-argument constructors be marked `explicit`?

### = default and = delete
87. What does it mean to make a constructor/destructor `= default`?
88. What does `= delete` do? Use cases (disable copy, disable specific overloads).

### constexpr
89. What is `constexpr`? Difference from `const`.
90. Difference between `constexpr`, `consteval`, and `constinit`.

### mutable
91. What is the `mutable` keyword? When/why use it?
92. Can `mutable` be applied to a reference? To a static member?

---

## 4. Smart Pointers Deep Dive
*Source: [4_Smart_Pointers_Deep_Dive.md](4_Smart_Pointers_Deep_Dive.md)*

### General
93. What are smart pointers? Why were they introduced?
94. What are the different smart pointers and where to use which type?
95. Write pseudo code for a smart pointer.

### unique_ptr
96. What is `unique_ptr`? When to use it?
97. How does `unique_ptr` restrict copying — internal implementation? *(BMW / General)*
98. What is `make_unique`? Why prefer it over `new`?
99. Can `unique_ptr` be moved? Show ownership transfer.
100. Custom deleter in `unique_ptr` — size impact?

### shared_ptr
101. How does `shared_ptr` work internally? (control block, atomic refcount)
102. Implement your own `shared_ptr`. *(HSBC)*
103. What is `use_count()`?
104. For `shared_ptr`, does the copy constructor copy the same memory location? How is the resource shared?
105. Why prefer `make_shared` over `new` + `shared_ptr`? When does it backfire?
106. What is the aliasing constructor of `shared_ptr`?

### weak_ptr
107. What is `weak_ptr`? Use cases.
108. How does `weak_ptr` break circular references? Show with code.
109. How is `weak_ptr` implemented? (weak count in control block)

### Internal Implementation
110. Internal implementation of `unique_ptr`, `shared_ptr`, and `weak_ptr`.
111. Difference between `shared_ptr` and `unique_ptr` in terms of copy/move semantics.

---

## 5. STL Internals
*Source: [5_STL_Internals.md](5_STL_Internals.md)*

### Vector
112. How does `std::vector` work internally? (capacity vs size, growth strategy)
113. Difference between these three:
    - `std::vector<TypeX> v1;`
    - `std::vector<TypeX>* v2 = new std::vector<TypeX>();`
    - `std::vector<TypeX*> v3;`
114. Iterator invalidation rules for `vector` on `push_back`, `insert`, `erase`.
115. Impact of C++11 move semantics on `std::vector` operations with temporaries.

### String
116. How is `std::string` implemented? What is SSO (Small String Optimization)?
117. `std::string` vs `std::string_view` — when to use which?

### Map / Set / Unordered Variants
118. Different types of map (`map`, `multimap`, `unordered_map`, `unordered_multimap`). *(HSBC)*
119. Time complexity comparison between `std::map` and `std::unordered_map`.
120. How does `std::map` work internally? (Red-Black Tree)
121. How does `std::unordered_map` handle collisions? Load factor, rehashing.
122. How to use a custom class as a key in `std::map`? *(strict weak ordering — `operator<`)*
123. How to use a custom class as a key in `std::unordered_map`? *(hash + equality)*
124. How to use a custom class in `std::set`?
125. Equals and hashcode — equivalent concepts in C++ for hash containers.

### Iterators & Algorithms
126. Types of iterators in STL.
127. What is iterator invalidation? Examples per container.
128. Explain `sort`, `find`, `transform`, `for_each`.

### Internal Working / Wrapper Class
129. Internal working of STL containers (overview).
130. STL wrapper class implementation — write a templated container/wrapper.

---

## 6. Performance Optimization
*Source: [6_Performance_Optimization.md](6_Performance_Optimization.md)*

131. As a C++ developer, how did you increase execution speed of the system?
132. With move semantics, how do you ensure memory is deallocated to avoid dangling pointers?
133. What compiler optimizations are available? (-O2, -O3, LTO, inlining)
134. What is inline function? Why use it? Significance? *(BMW)*
135. Difference between `inline` and macros. *(BMW)*
136. Cache-friendly code — what does it mean?
137. How do you select the right data structure for a problem?
138. How do you select the right/optimal algorithm? Time complexity analysis.

### HFT / Low-Latency
139. Lock-free programming basics.
140. Bitwise operations for optimization.
141. Memory pooling — how to create a memory pool? In multithreading?
142. Zero-copy techniques.
143. Fast logging design.
144. What is the FIX protocol? *(Trading systems)*

---

## 7. Templates
*Source: [../CPP_Interview_Preparation.md](../CPP_Interview_Preparation.md) §6*

145. Difference between function template and class template.
146. When does a template get expanded? What is template instantiation? *(BMW)*
147. What if the return value is also a template? Show usage.
148. What are variadic templates? Use cases.
149. What is template specialization? Full vs partial.
150. What is SFINAE? Replace with Concepts (C++20).

---

## 8. Multithreading & Concurrency
*Source: [../CPP_Interview_Preparation.md](../CPP_Interview_Preparation.md) §7*

### Basics
151. What is `std::thread`? How to create and join a thread?
152. What is a thread pool? How would you implement one?

### Synchronization
153. What is `std::mutex`?
154. `std::lock_guard` syntax and use.
155. Difference between `std::lock_guard` and `std::unique_lock`.
156. What is `std::condition_variable`? Producer-consumer example.

### Async Utilities
157. What is `std::async`? Launch policies.
158. `std::future` vs `std::promise` — when to use which.

### Problems
159. What is a race condition? How to prevent?
160. What is a deadlock? How to avoid (lock ordering, `std::scoped_lock`)?
161. Producer-consumer problem with thread synchronization — write the code.

### Advanced
162. What is shared memory? What is a semaphore? *(BMW)*
163. Singleton with multithreading — thread-safe implementation.
164. Lambda function in multithreading context.

---

## 9. Design Patterns
*Source: [../CPP_Interview_Preparation.md](../CPP_Interview_Preparation.md) §8, [../DEV/Design Patterns/Design_Patterns_Notes.md](../DEV/Design%20Patterns/Design_Patterns_Notes.md)*

165. What is a design pattern? Why use them?
166. Categories: Creational, Structural, Behavioral — give an example of each.

### Creational
167. Singleton — implementation, thread-safety, why considered an anti-pattern.
168. Factory Method — code example.
169. Abstract Factory — code example, difference from Factory Method.
170. Builder Pattern — when to use.
171. Prototype Pattern — when to use.

### Behavioral
172. Observer Pattern — implement (consider `weak_ptr` for safety).
173. Strategy Pattern — implement.

### Interview Question
174. **"Which design pattern are you using in your current project?"** — have 2-3 concrete stories ready.

---

## 10. SOLID Principles
*Source: [../CPP_Interview_Preparation.md](../CPP_Interview_Preparation.md) §9, [../DEV/SOLID_Principles.md](../DEV/SOLID_Principles.md)*

175. **S** — Single Responsibility Principle. C++ example.
176. **O** — Open/Closed Principle. C++ example.
177. **L** — Liskov Substitution Principle. Classic violation example.
178. **I** — Interface Segregation Principle. C++ example using abstract classes.
179. **D** — Dependency Inversion Principle. How it relates to abstract base classes.

---

## 11. Type Casting
*Source: [../CPP_Interview_Preparation.md](../CPP_Interview_Preparation.md) §10*

180. When to use `static_cast`? Example.
181. When to use `dynamic_cast`? Why does it require RTTI?
182. When to use `const_cast`? Is modifying a truly-const object UB?
183. When to use `reinterpret_cast`? Dangers?
184. C-style cast vs C++ casts — why C++ casts are preferred.

---

## 12. Coding Problems
*Source: [../CPP_Interview_Preparation.md](../CPP_Interview_Preparation.md) §12*

185. **Reverse a string** — implement in place.
186. String construction examples (different `std::string` constructors).
187. Linked List — implement, insert, delete, reverse.
188. **Delete a node in linked list without the head pointer**, given only the node's address. *(TCS)*
189. Use a custom class object as a key in `std::set` / `std::map` — write the code.
190. Implement your own `shared_ptr`.
191. Implement your own `unique_ptr`.
192. Implement a simple memory pool.

---

## 13. Company-Specific Asked Questions
*Source: [../CPP_Interview_Preparation.md](../CPP_Interview_Preparation.md) §13, `../.txt`*

### HSBC
1. Own `shared_ptr` implementation.
2. Polymorphism.
3. Which function is called if base class constructor calls a virtual function?
4. Map and its variants — time complexity of each.
5. Type casting in C++.
6. Quant-related questions on bonds.

### BMW TechWord (ADAS / Eclipse-R)
1. Inline function — why use it? Significance.
2. Smart pointers.
3. Casting in C++.
4. Shared memory, semaphore.
5. Thread, lambda function, mutex.
6. Template — when does it expand? Instantiation?
7. `unique_ptr`, `make_unique`.
8. Rule of Zero, Three, and Five.
9. Design Patterns in C++.
10. Shallow copy vs deep copy.
11. Move constructor and copy constructor.
12. Interface class.
13. Virtual function, pure virtual function.
14. Difference between inline and macros.
15. Virtual destructor usage.
16. How to avoid memory segmentation and memory leaks.
17. const function, static function.
18. Can a static function use non-static members of a class?
19. How does a const function work?
20. Move semantics.

### Finastra (OOP-focused)
1. OOP concepts/pillars.
2. What is Polymorphism?
3. Pointer initialization in polymorphism.
4. What are vtable and vptrs?
5. Write the code for pointer initialization for polymorphism.
6. How to prevent a function from being overridden in a derived class.
7. Difference between object creation with `new` vs the normal way.
8. Copy constructor.
9. `static` keyword.
10. Virtual functions and overriding.

### TCS
1. Delete a node in a linked list without head pointer — given only the node's memory address. Write a short snippet.

### General / Multi-Company
1. Different smart pointers — where to use which.
2. Use of `weak_ptr` — where can we use it?
3. Features of C++11, 14, and 17.
4. Multithreading and thread pool.
5. Race condition, deadlock, producer-consumer problem.
6. Callback in C++.
7. Inner implementation of `unique_ptr` wrapper class.
8. STL wrapper class implementation.
9. Memory pool — how to create, in multithreading.
10. Why implement user-defined copy constructor over default?
11. Use of templates — what if the return value is also a template?
12. Can we overload destructors?
13. Why derive destructor as virtual?
14. `lock_guard` syntax.
15. How does `unique_ptr` restrict copying — internal implementation?

### AI Interview Questions
1. As a C++ developer, how did you increase the execution speed of the system? *(Move semantics)*
2. With move semantics, how do you ensure memory deallocation to avoid dangling pointers?

---

## How to Use This Document

- Go through each question and **try to answer out loud** as if explaining to an interviewer.
- For any question you can't answer cleanly, jump to the corresponding note file:
  - OOP → [1_OOP_Fundamentals.md](1_OOP_Fundamentals.md)
  - Memory → [2_Memory_Management.md](2_Memory_Management.md)
  - Keywords → [3_Modern_Cpp_Keywords.md](3_Modern_Cpp_Keywords.md)
  - Smart Pointers → [4_Smart_Pointers_Deep_Dive.md](4_Smart_Pointers_Deep_Dive.md)
  - STL → [5_STL_Internals.md](5_STL_Internals.md)
  - Performance → [6_Performance_Optimization.md](6_Performance_Optimization.md)
- For senior-level depth (5 YOE), cross-reference [7_Senior_Level_Questions_5YOE.md](7_Senior_Level_Questions_5YOE.md).

---
