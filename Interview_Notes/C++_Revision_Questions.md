# C++ Interview Questions — Curated Revision Set

A focused, curated list of interview questions for quick revision.
For detailed answers, refer to the corresponding note files in [Interview_Notes/](.).

---

## Table of Contents
1. [OOP Fundamentals](#1-oop-fundamentals)
2. [Memory Management](#2-memory-management)
3. [Modern C++ Keywords](#3-modern-c-keywords)
4. [Smart Pointers Deep Dive](#4-smart-pointers-deep-dive)
5. [Multithreading & Concurrency](#5-multithreading--concurrency)
6. [Design Patterns](#6-design-patterns)
7. [STL Library](#7-stl-library)
8. [SOLID Principles](#8-solid-principles)

---

## 1. OOP Fundamentals

**General**
- Explain all OOP concepts (the four pillars).

### Abstract Classes & Interfaces
- What is an Abstract Class? How is it different from a regular class?
- What is a Pure Virtual Function? Syntax?
- Can an abstract class have a constructor? A destructor?

### Virtual Functions
- What is a virtual function? Why do we need it?
- What happens if you call a virtual function from a constructor or destructor? Why?
- Can a static function be virtual? Why not?
- Can a constructor be virtual? What is the "virtual constructor" idiom?

### VTable and VPtr
- What is a vtable? What is a vptr?
- Where is the vptr stored in the object? Where is the vtable stored?
- When is the vtable created — compile time or runtime?

### Polymorphism - Pointer Initialization
- What are the pointer initializations in polymorphism?
- What is the slicing problem? Show an example.

### Virtual Destructor
- Why do we need a virtual destructor?
- What happens if the base class destructor is NOT virtual and you `delete` via a base pointer?
- Can we overload a destructor? Why / why not?

---

## 2. Memory Management

### Copy Constructor & Assignment
- What is a copy constructor? Default vs user-defined.
- Why implement a user-defined copy constructor over the default?
- How does the copy constructor behave for `std::vector`, raw array (`int*`), `shared_ptr`, and `unique_ptr`?

### Rule of 0 / 3 / 5
- Explain Rule of Zero, Rule of Three, and Rule of Five.
- What is the Rule of Five in C++11 (+ move constructor, move assignment)?

### Move Semantics
- What is move semantics? Why was it introduced in C++11?
- What is an rvalue reference? Difference between lvalue and rvalue.
- What is RAII? Give examples.

---

## 3. Modern C++ Keywords

### noexcept
- What is `noexcept`? Why does it matter?

### override / final
- What is the `override` keyword? What error does it catch?
- What is the `final` keyword? Its effect on a virtual function?

### explicit
- What does `explicit` do on a constructor?
- What does `explicit` do on a conversion operator (C++11)?
- When should single-argument constructors be marked `explicit`?

### = default and = delete
- What are the `= default` and `= delete` keywords? When to use each?

### constexpr
- What is `constexpr`? Difference from `const`.
- Difference between `constexpr`, `consteval`, and `constinit`.

### mutable
- What is the `mutable` keyword? When/why use it?

---

## 4. Smart Pointers Deep Dive

### General
- What are smart pointers? Why were they introduced?
- What are the different smart pointers and where to use which type?

### unique_ptr
- What is `unique_ptr`? When to use it?
- How does `unique_ptr` restrict copying — internal implementation?

### shared_ptr
- How does `shared_ptr` work internally?
- Implement your own `shared_ptr`.
- What is `use_count()`?

### weak_ptr
- What is `weak_ptr`? Use cases.
- How does `weak_ptr` break circular references? Show with code.

### Internals & Comparison
- Internal implementation of `unique_ptr`, `shared_ptr`, and `weak_ptr`.
- Difference between `shared_ptr` and `unique_ptr` in terms of copy/move semantics.

---

## 5. Multithreading & Concurrency

### Basics
- What is `std::thread`? How to create and join a thread?
- What is a thread pool? How would you implement one?

### Synchronization
- What is `std::mutex`?
- `std::lock_guard` syntax and use.
- Difference between `std::lock_guard` and `std::unique_lock`.

### Common Problems
- What is a race condition? How to prevent it?
- What is a deadlock? How to avoid it (lock ordering, `std::scoped_lock`)?
- Producer-consumer problem with thread synchronization — write the code.

### Patterns
- Singleton with multithreading — thread-safe implementation.

---

## 6. Design Patterns

### Introduction
- What is a design pattern? Why use them?
- Categories: Creational, Structural, Behavioral — give an example of each.

### Creational Patterns
- **Singleton** — implementation, thread-safety, why it's considered an anti-pattern.
- **Factory Method** — code example.
- **Abstract Factory** — code example, difference from Factory Method.
- **Builder Pattern** — when to use.
- **Prototype Pattern** — when to use.

### Behavioral Patterns
- **Observer Pattern** — implement (consider `weak_ptr` for safety).
- **Strategy Pattern** — implement.

---

## 7. STL Library


---

## 8. SOLID Principles


---
