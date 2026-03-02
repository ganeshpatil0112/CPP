# Modern C++ Keywords - Interview Notes

## Table of Contents
1. [noexcept](#noexcept)
2. [override](#override)
3. [explicit](#explicit)
4. [= default](#default)
5. [= delete](#delete)
6. [constexpr](#constexpr)
7. [mutable](#mutable)

---

## noexcept

### **Q: What is `noexcept` and Why Use It?**

**Answer:**
`noexcept` specifies that a function will **not throw exceptions**. If exception occurs, program calls `std::terminate()`. Enables **compiler optimizations** and is **critical for STL containers**.

### **Syntax:**
```cpp
void function() noexcept;              // Won't throw
void function() noexcept(true);        // Same
void function() noexcept(false);       // May throw (default)
```

### **Basic Example:**

```cpp
int add(int a, int b) noexcept {
    return a + b;  // Simple, won't throw
}

int divide(int a, int b) {  // May throw
    if(b == 0) throw runtime_error("Division by zero");
    return a / b;
}
```

###**Performance:** Vector Resize Optimization

**Theory:** STL containers provide **strong exception guarantee** - if operation fails, container unchanged. Achieves this by choosing copy (safe) vs move (fast) based on `noexcept`.

```cpp
class Element {
    int* data;
public:
    // Move WITHOUT noexcept
    Element(Element&& other) {
        data = other.data;
        other.data = nullptr;
    }
    // Problem: Vector uses COPY (slow) for safety
};

class OptimizedElement {
    int* data;
public:
    // Move WITH noexcept
    Element(Element&& other) noexcept {
        data = other.data;
        other.data = nullptr;
    }
    // Benefit: Vector uses MOVE (fast)
};
```

**Why This Matters:**

Without `noexcept`: Vector must use copy during resize (expensive)
```cpp
// Vector internals (simplified)
if constexpr (is_nothrow_move_constructible<T>) {
    // Use MOVE - fast
    ::new(&newMemory[i]) T(std::move(oldMemory[i]));
} else {
    // Use COPY - safe but slow
    ::new(&newMemory[i]) T(oldMemory[i]);
}
```

### **Critical Use Cases:**

#### **1. Move Constructors/Assignment:**
```cpp
class MyClass {
public:
    MyClass(MyClass&& other) noexcept {  // MUST be noexcept
        // Move logic
    }
    
    MyClass& operator=(MyClass&& other) noexcept {
        // Move assignment
        return *this;
    }
};
```

#### **2. Swap Functions:**
```cpp
class MyClass {
public:
    void swap(MyClass& other) noexcept {
        using std::swap;
        swap(data, other.data);
    }
};
```

#### **3. Destructors (Implicit noexcept):**
```cpp
class MyClass {
public:
    ~MyClass() noexcept {  // Implicit, but can be explicit
        // Cleanup - MUST NOT throw
    }
};
```

### **Interview Points:**
- Enables compiler optimizations (no exception handling code)
- Required for STL container move optimization
- Destructors are implicitly `noexcept` (C++11+)
- Zero runtime cost - compile-time feature

---

## override

### **Q: What is `override` Keyword?**

**Answer:**
`override` explicitly indicates a function overrides a base class virtual function. It's a **compile-time safety check** that catches errors like typos, signature mismatches, and missing `virtual` in base.

### **The Problem Without `override`:**

```cpp
class Base {
public:
    virtual void display() const {
        cout << "Base\n";
    }
};

class Derived : public Base {
public:
    // Typo: "displya" - creates NEW function, doesn't override!
    void displya() const {
        cout << "Derived\n";
    }
};

int main() {
    Base* ptr = new Derived();
    ptr->display();  // Calls Base::display (NOT what you wanted!)
}
```

### **The Solution With `override`:**

```cpp
class Derived : public Base {
public:
    // ❌ Compile ERROR: no function named 'displya' to override
    // void displya() const override {}
    
    // ✅ Correct
    void display() const override {
        cout << "Derived\n";
    }
};
```

### **Errors `override` Catches:**

#### **1. Typo in Function Name:**
```cpp
class Derived : public Base {
    void proces() override {}  // ERROR: 'proces' doesn't override
    // Fixed:
    void process() override {}  //  Works
};
```

#### **2. Wrong Parameter Types:**
```cpp
class Derived : public Base {
    void setValue(double v) override {}  // ERROR: parameter mismatch
    // Fixed:
    void setValue(int v) override {}     // ✅ Works
};
```

#### **3. Missing `const`:**
```cpp
class Derived : public Base {
    int getValue() override {}        // ERROR: missing const
    // Fixed:
    int getValue() const override {}  // ✅ Works
};
```

### **Complete Example:**

```cpp
#include <iostream>
using namespace std;

class Shape {
public:
    virtual void draw() const = 0;
    virtual double area() const = 0;
    virtual ~Shape() = default;
};

class Circle : public Shape {
    double radius;
public:
    Circle(double r) : radius(r) {}
    
    void draw() const override {
        cout << "Drawing Circle\n";
    }
    
    double area() const override {
        return 3.14159 * radius * radius;
    }
};

class Rectangle : public Shape {
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    void draw() const override {
        cout << "Drawing Rectangle\n";
    }
    
    double area() const override {
        return width * height;
    }
};
```

### **`override` vs `virtual`:**

| Keyword | Where | Purpose |
|---------|-------|---------|
| **`virtual`** | Base class | Enables overriding |
| **`override`** | Derived class | Confirms overriding |

```cpp
class Base {
    virtual void func();  // 'virtual' enables overriding
};

class Derived : public Base {
    void func() override;  // 'override' confirms it
    // No need for 'virtual' here
};
```

### **Interview Points:**
- Compile-time safety check for correct overriding
- Catches typos, signature mismatches, const-correctness
- Makes refactoring safer
- Zero runtime cost
- Always use when overriding virtual functions

---

## explicit

### **Q: What is `explicit` Keyword?**

**Answer:**
`explicit` prevents **implicit type conversions** and **copy-initialization**. Used with constructors and conversion operators to avoid unexpected conversions.

### **The Problem Without `explicit`:**

```cpp
class MyString {
    char* data;
public:
    MyString(int size) {  // Implicit conversion constructor
        data = new char[size];
    }
};

void process(MyString str) {
    // ...
}

int main() {
    process(100);  // ⚠️ Implicitly converts 100 to MyString!
                   // Probably not what you wanted
}
```

### **The Solution With `explicit`:**

```cpp
class MyString {
    char* data;
public:
    explicit MyString(int size) {  // Prevents implicit conversion
        data = new char[size];
    }
};

void process(MyString str) {}

int main() {
    // process(100);  // ❌ Compile ERROR
    process(MyString(100));  // ✅ Explicit construction required
}
```

### **Complete Example:**

```cpp
#include <iostream>
using namespace std;

class SmartPointer {
    int* ptr;
public:
    // Explicit constructor
    explicit SmartPointer(int* p) : ptr(p) {
        cout << "SmartPointer created\n";
    }
    
    ~SmartPointer() {
        delete ptr;
    }
};

void usePointer(SmartPointer sp) {
    cout << "Using smart pointer\n";
}

int main() {
    int* raw = new int(42);
    
    // ❌ Won't compile - explicit prevents implicit conversion
    // usePointer(raw);
    
    // ✅ Must explicitly construct
    usePointer(SmartPointer(raw));
    
    return 0;
}
```

### **When to Use `explicit`:**

```cpp
class Array {
public:
    explicit Array(int size);        // ✅ Prevent Array arr = 10;
    Array(std::initializer_list<int>);  // ❌ Allow Array arr = {1,2,3};
};

class Complex {
public:
    explicit Complex(double real);   // ✅ Prevent Complex c = 3.14;
    Complex(double real, double imag);  // OK without explicit
};
```

### **Interview Points:**
- Prevents unexpected implicit conversions
- Use for single-parameter constructors
- Makes code intent explicit and safer
- Zero runtime cost - compile-time check

---

## = default

### **Q: What Does `= default` Mean?**

**Answer:**
`= default` explicitly requests compiler to generate the **default implementation** of a special member function. Used when you've defined other constructors but still want the default one.

### **The Problem:**

```cpp
class MyClass {
    int value;
public:
    MyClass(int v) : value(v) {}
    
    // Problem: Default constructor NO LONGER auto-generated!
};

int main() {
    MyClass obj1(42);  // ✅ OK
    MyClass obj2;      // ❌ ERROR - no default constructor
}
```

### **The Solution:**

```cpp
class MyClass {
    int value;
public:
    MyClass(int v) : value(v) {}
    
    MyClass() = default;  // Explicitly request default constructor
};

int main() {
    MyClass obj1(42);  // ✅ OK
    MyClass obj2;      // ✅ OK - uses default constructor
}
```

### **Common Uses:**

```cpp
class MyClass {
public:
    // Default constructor
    MyClass() = default;
    
    // Destructor (especially virtual)
    virtual ~MyClass() = default;
    
    // Copy constructor
    MyClass(const MyClass&) = default;
    
    // Copy assignment
    MyClass& operator=(const MyClass&) = default;
    
    // Move constructor
    MyClass(MyClass&&) = default;
    
    // Move assignment
    MyClass& operator=(MyClass&&) = default;
};
```

### **Virtual Destructor Example:**

```cpp
class Base {
public:
    virtual ~Base() = default;  // Virtual + default implementation
};

class Derived : public Base {
    unique_ptr<int> data;
public:
    ~Derived() = default;  // Smart pointer cleans up automatically
};
```

### **Interview Points:**
- Explicitly requests compiler-generated implementation
- Use when other constructors defined but want default too
- Better than empty implementation (compiler can optimize better)
- Makes intent clear

---

## = delete

### **Q: What Does `= delete` Mean?**

**Answer:**
`= delete` explicitly **disables** a function, making it impossible to call. Generates **compile-time error** if used. Commonly used to prevent copying or specific type conversions.

### **Preventing Copying:**

```cpp
class Singleton {
    static Singleton* instance;
    
    Singleton() {}  // Private constructor
    
public:
    // Delete copy operations
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    
    static Singleton* getInstance();
};

int main() {
    Singleton* s1 = Singleton::getInstance();
    
    // ❌ Won't compile
    // Singleton s2 = *s1;  // ERROR: copy constructor deleted
}
```

### **Preventing Type Conversions:**

```cpp
class MyClass {
public:
    void process(int value) {
        cout << "Processing int: " << value << "\n";
    }
    
    // Delete other overloads to prevent implicit conversions
    void process(double) = delete;
    void process(float) = delete;
    void process(char) = delete;
};

int main() {
    MyClass obj;
    obj.process(42);      // ✅ OK
    // obj.process(3.14);   // ❌ ERROR: double version deleted
    // obj.process(3.14f);  // ❌ ERROR: float version deleted
}
```

### **Unique Ptr Implementation:**

```cpp
template<typename T>
class UniquePtr {
    T* ptr;
public:
    explicit UniquePtr(T* p) : ptr(p) {}
    ~UniquePtr() { delete ptr; }
    
    // Delete copy to enforce unique ownership
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;
    
    // Allow move (transfer ownership)
    UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }
};
```

### **`= delete` vs Private:**

| Approach | Error Type | Can Call from Member/Friend? |
|----------|------------|------------------------------|
| **Private** | Link error | ✅ Yes (bad!) |
| **`= delete`** | Compile error | ❌ No (good!) |

### **Interview Points:**
- Explicitly disables functions
- Compile-time error (better than linker error)
- Use to prevent copying, conversion, or specific overloads
- Part of modern C++ (C++11+)

---

## constexpr

### **Q: What is `constexpr`?**

**Answer:**
`constexpr` indicates that a value/function **can be evaluated at compile-time**. Enables compile-time computation, improving performance and enabling template metaprogramming.

### **constexpr Variables:**

```cpp
constexpr int square(int x) {
    return x * x;
}

int main() {
    constexpr int result = square(5);  // Computed at compile-time
    int arr[result];  // Can use in array size (compile-time constant)
}
```

### **constexpr Functions:**

```cpp
constexpr int factorial(int n) {
    return (n <= 1) ? 1 : n * factorial(n - 1);
}

int main() {
    constexpr int fact5 = factorial(5);  // Computed at compile-time: 120
    
    int n;
    cin >> n;
    int factN = factorial(n);  // Computed at runtime (n not constexpr)
}
```

### **constexpr vs const:**

| Feature | `const` | `constexpr` |
|---------|---------|-------------|
| **Meaning** | Value doesn't change | Evaluated at compile-time |
| **Initialization** | Runtime or compile-time | Must be compile-time |
| **Functions** | Not applicable | Can be constexpr |

```cpp
const int a = getValue();      // Runtime initialization OK
constexpr int b = getValue();  // ERROR if getValue() not constexpr
```

### **Interview Points:**
- Enables compile-time computation
- Can reduce runtime overhead
- Required for template non-type parameters
- More flexible than `const`

---

## mutable

### **Q: What is `mutable` Keyword?**

**Answer:**
`mutable` allows a member variable to be **modified even in const member functions**. Used for implementation details that don't affect logical constness (e.g., caching, mutexes).

### **Basic Example:**

```cpp
class Counter {
    mutable int accessCount;  // Can modify in const functions
    int value;
    
public:
    Counter(int v) : value(v), accessCount(0) {}
    
    int getValue() const {
        accessCount++;  // ✅ OK - mutable member
        return value;
    }
    
    int getAccessCount() const {
        return accessCount;
    }
};

int main() {
    const Counter c(42);
    cout << c.getValue() << "\n";  // Modifies accessCount
    cout << "Accessed: " << c.getAccessCount() << " times\n";
}
```

### **Caching Example:**

```cpp
class ExpensiveCalculation {
    int value;
    mutable bool cached;
    mutable double cachedResult;
    
public:
    double compute() const {
        if (!cached) {
            // Expensive computation
            cachedResult = /* ... */;
            cached = true;
        }
        return cachedResult;
    }
};
```

### **Mutex Example:**

```cpp
class ThreadSafe {
    mutable std::mutex mtx;  // Must be mutable for const functions
   int data;
    
public:
    int getData() const {
        std::lock_guard<std::mutex> lock(mtx);  // Locks mutex in const function
        return data;
    }
};
```

### **Interview Points:**
- Allows modification in const member functions
- Used for implementation details (caching, logging, locking)
- Doesn't affect logical constness
- Common with mutexes in thread-safe classes

---

**Summary - Quick Reference:**

|Keyword | Purpose | When to Use |
|--------|---------|-------------|
| **`noexcept`** | No exceptions | Move constructors, swap, destructors |
| **`override`** | Confirm override | All virtual function overrides |
| **`explicit`** | Prevent implicit conversion | Single-param constructors |
| **`= default`** | Use compiler default | When others defined, want default too |
| **`= delete`** | Disable function | Prevent copying, conversions |
| **`constexpr`** | Compile-time evaluation | Constants, template params |
| **`mutable`** | Modify in const | Caching, mutexes, counters |

---

**Next Files:**
- [Smart Pointers Deep Dive](4_Smart_Pointers_Deep_Dive.md)
- [STL Internals](5_STL_Internals.md)
