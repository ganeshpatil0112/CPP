
                    C++ INTERVIEW PREPARATION GUIDE
                          (2-3 Years Experience)


## TABLE OF CONTENTS
1. [Core C++ Concepts](#1-core-c-concepts)
2. [Object-Oriented Programming (OOP)](#2-object-oriented-programming-oop)
3. [Memory Management](#3-memory-management)
4. [Modern C++ (C++11/14/17/20)](#4-modern-c-c11141720)
5. [STL (Standard Template Library)](#5-stl-standard-template-library)
6. [Templates](#6-templates)
7. [Multithreading & Concurrency](#7-multithreading--concurrency)
8. [Design Patterns](#8-design-patterns)
9. [SOLID Principles](#9-solid-principles)
10. [Type Casting](#10-type-casting)
11. [Advanced Topics](#11-advanced-topics)
12. [Coding Problems & Interview Questions](#12-coding-problems--interview-questions)
13. [Company-Specific Questions](#13-company-specific-interview-questions)

================================================================================

## 1. CORE C++ CONCEPTS

### Functions & Keywords
- [ ] **Inline functions** - why use it? significance?
- [ ] **Difference between inline and macros**
- [ ] **const functions** - how they work
- [ ] **static functions**
- [ ] Can static function use non-static member of class?
- [ ] **Mutable keyword**
- [ ] **constexpr** (C++11)
- [ ] **override keyword** (C++11)

### Pointers & References
- [ ] **this pointer**
- [ ] **lvalue & rvalue**
- [ ] **rvalue reference** (C++11)
- [ ] **Function pointers vs std::function**

### Memory Operations
- [ ] **new vs malloc**
- [ ] **delete vs free**
- [ ] How new/delete works internally
- [ ] How to avoid memory segmentation and memory leaks

### Data Structures Selection
- [ ] Correctly selecting the right data structure
- [ ] Selecting the best optimum algorithm
- [ ] Time complexity analysis

================================================================================

## 2. OBJECT-ORIENTED PROGRAMMING (OOP)

### Four Main Pillars
- [ ] **Encapsulation**
- [ ] **Abstraction**
- [ ] **Inheritance**
- [ ] **Polymorphism**

### Polymorphism Deep Dive
- [ ] **Compile-time vs Run-time Polymorphism**
- [ ] **Function Overloading**
- [ ] **Operator Overloading**
- [ ] **Virtual Functions** and their role in run-time polymorphism

### Constructors & Destructors
- [ ] **All types of constructors**
  - Default constructor
  - Parameterized constructor
  - Copy constructor
  - Move constructor (C++11)
  - Delegating constructors
- [ ] **Copy constructor** - when to implement user-defined vs default
- [ ] **Move constructor** (C++11)
- [ ] Can we overload the destructor? (Answer: No)
- [ ] **Virtual destructor** - usage and why needed
- [ ] **Virtual constructor** concept

### Special Member Functions Rules
- [ ] **Rule of Zero**
- [ ] **Rule of Three** (Destructor, Copy Constructor, Copy Assignment)
- [ ] **Rule of Five** (+ Move Constructor, Move Assignment)
- [ ] **3-5-0 rules**

### Class Members
- [ ] **Member initializer list**
- [ ] **Static members**
- [ ] **const member functions**

### Virtual Functions
- [ ] **Virtual keyword** and how it works internally
- [ ] **Pure virtual functions**
- [ ] **Abstract classes**
- [ ] **Interface implementation**
- [ ] **vtable and vptr mechanism**
- [ ] Which function is called if base class constructor calls some function?
- [ ] Impact of overriding when using smart pointers

================================================================================

## 3. MEMORY MANAGEMENT

### Copy Semantics
- [ ] **Shallow copy vs deep copy**
- [ ] **Copy constructor** implementation
- [ ] **Copy assignment operator**

### Move Semantics (C++11)
- [ ] **Move constructor**
- [ ] **Move assignment**
- [ ] **Move semantics** advantages
- [ ] **std::move**

### Smart Pointers (C++11)
- [ ] **unique_ptr**
  - `make_unique`
  - How unique_ptr restricts copying (internal implementation)
  - When to use
- [ ] **shared_ptr**
  - Own implementation (interview question)
  - `use_count()`
  - How it works internally
  - When to use
- [ ] **weak_ptr**
  - Use cases
  - Breaking circular references
- [ ] Pseudo code for smart pointers

### Advanced Memory Techniques
- [ ] **Memory pool** - how to create
- [ ] Memory pool in multithreading
- [ ] **Lazy initialization** in C++

================================================================================

## 4. MODERN C++ (C++11/14/17/20)

### C++11 Features
- [ ] **Auto type deduction**
- [ ] **Range-based for loops**
- [ ] **Lambda expressions** (unnamed functions)
- [ ] **rvalue references**
- [ ] **Move semantics**
- [ ] **Smart pointers**
- [ ] **std::thread**
- [ ] **Variadic templates**
- [ ] **constexpr**
- [ ] **override and final** keywords
- [ ] **nullptr**

### C++14 Features
- [ ] Generic lambdas
- [ ] auto return type deduction
- [ ] Binary literals

### C++17 Features
- [ ] Structured bindings
- [ ] if/switch with initializers
- [ ] `std::optional`
- [ ] `std::variant`
- [ ] `std::any`
- [ ] Fold expressions

### C++20 Features
- [ ] Concepts
- [ ] Ranges
- [ ] Coroutines
- [ ] Modules
- [ ] `std::span`

### Function Objects
- [ ] **Lambda functions**
- [ ] **Functors** (function objects)
- [ ] **std::function**
  - Type safety
  - Flexibility (supports lambdas, member functions, etc.)
- [ ] **Callbacks** in C++

================================================================================

## 5. STL (STANDARD TEMPLATE LIBRARY)

### Containers

#### Vector
- [ ] `std::vector<TypeX> vector1;`
- [ ] `std::vector<TypeX>* vector2 = new vector<TypeX>();`
- [ ] `std::vector<TypeX*> vector3;`
- [ ] **What exactly is the difference between them?**
- [ ] Impact in C++11 (move semantics with temporaries)

#### Map
- [ ] Different types of map
- [ ] Time complexity
- [ ] Custom class objects as keys

#### Set
- [ ] Custom class objects
- [ ] Time complexity

#### Other Containers
- [ ] `std::list`
- [ ] `std::multimap`

#### Unordered Containers (C++11)
- [ ] `std::unordered_map`
- [ ] `std::unordered_set`
- [ ] Hash-related topics
- [ ] Additional containers related to hash-map

### Iterators
- [ ] Types of iterators
- [ ] Iterator invalidation

### Algorithms
- [ ] `sort`, `find`, `transform`
- [ ] `for_each`
- [ ] Other STL algorithms

### Internal Working
- [ ] How STL containers work internally
- [ ] STL wrapper class implementation

================================================================================

## 6. TEMPLATES

### Template Basics
- [ ] **Function templates**
- [ ] **Class templates**
- [ ] **Template instantiation** - when does it expand?
- [ ] Template return values (template as return type)

### Advanced Templates
- [ ] **Variadic templates** (C++11)
- [ ] Template specialization
- [ ] SFINAE

================================================================================

## 7. MULTITHREADING & CONCURRENCY

### Threading Basics
- [ ] **std::thread**
- [ ] Thread objects
- [ ] Thread synchronization
- [ ] **Thread pool**

### Synchronization Primitives
- [ ] **std::mutex**
- [ ] **lock_guard** syntax
- [ ] `std::lock_guard`
- [ ] `std::unique_lock`
- [ ] **condition_variable**

### Concurrency Utilities
- [ ] **std::async**
- [ ] **std::future**
- [ ] **std::promise**

### Common Problems
- [ ] **Race conditions**
- [ ] **Deadlocks**
- [ ] **Producer-consumer problem** with thread synchronization
- [ ] Thread safety

### Advanced Concepts
- [ ] **Shared memory**
- [ ] **Semaphore**
- [ ] Singleton pattern with multithreading

================================================================================

## 8. DESIGN PATTERNS

### What is a Design Pattern?
- [ ] Introduction to Design Patterns
- [ ] When and why to use them

### Categories
- [ ] **Creational Patterns**
- [ ] **Structural Patterns**
- [ ] **Behavioral Patterns**

### Creational Patterns
- [ ] **Singleton Design Pattern** (with multithreading)
- [ ] **Factory Method**
- [ ] **Abstract Factory**
- [ ] **Builder Pattern** (optional)
- [ ] **Prototype Pattern** (optional)

### Behavioral Patterns
- [ ] **Observer Pattern**
- [ ] **Strategy Pattern**

### Interview Preparation
- [ ] **"Which design pattern are you using in your current project?"**

================================================================================

## 9. SOLID PRINCIPLES

- [ ] **S - Single Responsibility Principle (SRP)**
- [ ] **O - Open/Closed Principle (OCP)**
- [ ] **L - Liskov Substitution Principle (LSP)**
- [ ] **I - Interface Segregation Principle (ISP)**
- [ ] **D - Dependency Inversion Principle (DIP)**

================================================================================

## 10. TYPE CASTING

### C++ Casting Operators
- [ ] **static_cast** - when to use
- [ ] **dynamic_cast** - when to use
- [ ] **const_cast** - when to use
- [ ] **reinterpret_cast** - when to use
- [ ] Where to use which casting

================================================================================

## 11. ADVANCED TOPICS

### Low-Latency Techniques (HFT)
- [ ] **Lock-free programming**
- [ ] **Bitwise operations**
- [ ] **Memory pooling**
- [ ] **Cache-friendly code**
- [ ] **Zero-copy techniques**
- [ ] **Fast logging**

### Protocols
- [ ] **FIX protocol** (for trading systems)

### Interview Edge Cases
- [ ] Classic C++ pitfalls
- [ ] Undefined behavior scenarios

================================================================================

## 12. CODING PROBLEMS & INTERVIEW QUESTIONS

### String Manipulation
- [ ] **Reverse a string** program
- [ ] String examples using constructors

### Data Structures
- [ ] **Linked list** - detailed study with coding practice
- [ ] **Delete a node in linked list without head pointer**
  - Given: memory address of node to be deleted
  - [TCS Interview Question]

### Custom Class Implementations
- [ ] `std::set` for custom class objects
- [ ] `std::map` for custom class objects

================================================================================

## 13. COMPANY-SPECIFIC INTERVIEW QUESTIONS

### HSBC
1. Own `shared_ptr` implementation
2. Polymorphism
3. Which function is called if base class constructor calls some function?
4. Map and different types of map and its time complexity
5. Type casting
6. Quant-related questions on bonds

---

### BMW TechWord (C++ ADAS OSS, ECLLIpse-R)
1. Inline function - why use it? significance?
2. Smart pointers
3. Casting in C++
4. Shared memory, semaphore
5. Thread, lambda function, mutex
6. Template - when does it expand? instantiation?
7. `unique_ptr`, `make_unique`
8. Rule of Zero, Three, and Five
9. Design Patterns in C++
10. Shallow copy vs deep copy
11. Move constructor and copy constructor
12. Interface class
13. Virtual function, pure virtual function
14. Difference between inline and macros
15. Virtual destructor usage
16. How to avoid memory segmentation and memory leaks
17. const function, static function
18. Can static function use non-static member of class?
19. How const function works
20. Move semantics

---

### General Interview Questions
1. Different smart pointers and where to use which type
2. Use of `weak_ptr` - where can we use?
3. Features of C++11, 14, and 17
4. Multithreading and thread pool
5. Race condition, deadlock, producer-consumer problem
6. Callback in C++
7. Inner implementation of `unique_ptr` wrapper class
8. STL wrapper class
9. Memory pool - how to create, in multithreading
10. Why implement user-defined copy constructor over default?
11. Use of templates - what if return value is also template?
12. Can we overload destructor?
13. Why derive destructor as virtual?
14. `lock_guard` syntax
15. How `unique_ptr` restricts copying - internal implementation?

---

### Finastra C++ questions:
Based on OOP  
1) what are the OOP concepts/pillars of OPP  
2)what is Polymorphism  
3)what are pointers initialisation in polymorphism  
4)what is virtual table and virtual pointers  
5) Write the code for pointers initialisation for polymorphism  
6)what I can do so that I can't use the same function in inherited class  
7) Difference between object creation using (new keyword ) vs normal keyword  
8)Copy constructor  
9)static keyword  
10)virtual functions as I overriding the function 

================================================================================

## INTERVIEW PREPARATION CHECKLIST

### Core Concepts
- [ ] OOPS concepts (all four pillars)
- [ ] Copy constructor
- [ ] vtable and vptr working
- [ ] this pointer
- [ ] Member initializer list

### Memory Management
- [ ] Smart pointers (unique, shared, weak)
- [ ] new/delete vs malloc/free
- [ ] Shallow vs deep copy
- [ ] Move semantics

### STL
- [ ] Internal working of STL containers
- [ ] Vector, Map, Set, List
- [ ] STL algorithms

### Modern C++
- [ ] Lambda functions
- [ ] Auto type deduction
- [ ] Range-based loops
- [ ] rvalue references

### Multithreading
- [ ] `std::thread`
- [ ] Mutex, `lock_guard`
- [ ] Race conditions, deadlocks
- [ ] Thread synchronization

### Design Patterns
- [ ] Singleton, Factory, Abstract Factory
- [ ] Observer, Strategy
- [ ] Real project examples

### SOLID Principles
- [ ] All five principles with examples

### Coding Practice
- [ ] Reverse string
- [ ] Linked list operations
- [ ] Custom class in STL containers

================================================================================

## TIPS FOR SUCCESS

✓ **Focus on fundamentals first**, then advanced topics
✓ **Practice coding problems hands-on**
✓ **Prepare real examples** from your current/past projects
✓ **Understand WHY, not just WHAT**
✓ **Be ready to write code** during interviews
✓ **Review time complexity** for STL operations
✓ **Practice explaining concepts** clearly and concisely
✓ **Don't be demotivated** - persistence is key!

================================================================================

## STUDY SCHEDULE RECOMMENDATION

### Week 1-2: Core C++ & OOP
- Focus on all OOP concepts
- Practice constructors/destructors
- Virtual functions and vtable

### Week 3-4: Memory Management & Modern C++
- Smart pointers implementation
- Move semantics
- C++11/14/17 features

### Week 5-6: STL & Templates
- All STL containers
- Template programming
- Custom classes in STL

### Week 7-8: Multithreading & Design Patterns
- Threading concepts
- Synchronization primitives
- All design patterns

### Week 9: SOLID Principles & Advanced Topics
- SOLID principles
- HFT techniques (if relevant)
- Type casting

### Week 10: Mock Interviews & Coding Practice
- Company-specific questions
- Coding problems
- Mock interviews

================================================================================
