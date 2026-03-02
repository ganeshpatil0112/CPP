# STL Internals - Interview Notes

## Table of Contents
1. [Vector Internals](#vector-internals)
2. [String Internals](#string-internals)
3. [Map and Unordered Map](#map-and-unordered-map)
4. [Iterator Implementation](#iterator-implementation)
5. [Allocators](#allocators)

---

## Vector Internals

### **Q: How Does std::vector Work Internally?**

**Answer:**
`std::vector` maintains **3 pointers**: `begin`, `end`, and `capacity`. When full, it **reallocates** with larger capacity (typically 2x growth), **moves/copies** elements, and **deletes old memory**.

### **Internal Structure:**

```cpp
template<typename T>
class Vector {
    T* m_data;      // Pointer to array
    size_t m_size;  // Number of elements
    size_t m_capacity;  // Allocated space
    
public:
    Vector() : m_data(nullptr), m_size(0), m_capacity(0) {}
    
    ~Vector() {
        clear();
        ::operator delete(m_data);
    }
    
    size_t size() const { return m_size; }
    size_t capacity() const { return m_capacity; }
    
    T& operator[](size_t index) {
        return m_data[index];
    }
    
    void push_back(const T& value) {
        if (m_size >= m_capacity) {
            // Reallocate
            size_t newCapacity = (m_capacity == 0) ? 1 : m_capacity * 2;
            reserve(newCapacity);
        }
        m_data[m_size++] = value;
    }
    
    void reserve(size_t newCapacity) {
        if (newCapacity <= m_capacity) return;
        
        // Allocate new memory
        T* newData = (T*)::operator new(newCapacity * sizeof(T));
        
        // Move/copy elements
        for (size_t i = 0; i < m_size; i++) {
            new(&newData[i]) T(std::move(m_data[i]));  // Placement new + move
        }
        
        // Destroy old elements
        for (size_t i = 0; i < m_size; i++) {
            m_data[i].~T();
        }
        
        // Free old memory
        ::operator delete(m_data);
        
        // Update pointers
        m_data = newData;
        m_capacity = newCapacity;
    }
    
    void clear() {
        for (size_t i = 0; i < m_size; i++) {
            m_data[i].~T();
        }
        m_size = 0;
    }
};
```

### **Growth Strategy:**

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v;
    
    for (int i = 0; i < 10; i++) {
        v.push_back(i);
        cout << "Size: " << v.size() 
             << ", Capacity: " << v.capacity() << "\n";
    }
    
    return 0;
}
/* Typical Output:
Size: 1, Capacity: 1
Size: 2, Capacity: 2
Size: 3, Capacity: 4
Size: 4, Capacity: 4
Size: 5, Capacity: 8
Size: 6, Capacity: 8
Size: 7, Capacity: 8
Size: 8, Capacity: 8
Size: 9, Capacity: 16
Size: 10, Capacity: 16
*/
```

**Growth Factor:**
- Most implementations: **2x growth** (MSVC, GCC)
- Some use **1.5x growth** for memory efficiency
- Growth factor > 1 ensures **amortized O(1)** push_back

### **Memory Layout:**

```
Stack:                  Heap:
┌─────────────┐        ┌───┬───┬───┬───┬───┬───┬───┬───┐
│  m_data────────────>│ 1 │ 2 │ 3 │ 4 │ 5 │   │   │   │
│  m_size = 5 │        └───┴───┴───┴───┴───┴───┴───┴───┘
│ m_capacity=8│         ↑                   ↑           ↑
└─────────────┘         begin               end     capacity
```

### **Reallocation Cost:**

```cpp
class Heavy {
    int data[1000];
public:
    Heavy() { cout << "Construct\n"; }
    Heavy(const Heavy&) { cout << "Copy\n"; }
    Heavy(Heavy&&) noexcept { cout << "Move\n"; }
};

int main() {
    vector<Heavy> v;
    v.reserve(3);  // Pre-allocate to avoid reallocation
    
    v.push_back(Heavy());  // Construct + Move
    v.push_back(Heavy());  // Construct + Move
    v.push_back(Heavy());  // Construct + Move
    // Without reserve: would also copy/move during reallocation!
}
```

### **Interview Points:**
- 3 pointers: data, size, capacity
- Growth strategy: typically 2x (amortized O(1) push_back)
- Reallocation invalidates iterators/pointers
- reserve() prevents reallocation
- Uses move if available and noexcept

---

## String Internals

### **Q: How Does std::string Optimize for Small Strings?**

**Answer:**
`std::string` uses **Small String Optimization (SSO)**: stores small strings (typically ≤15 chars) **directly in the object** instead of heap allocation. Larger strings use heap.

### **SSO Implementation:**

```cpp
class String {
    static constexpr size_t SSO_SIZE = 15;
    
    union {
        char sso_buffer[SSO_SIZE + 1];  // Small string storage
        struct {
            char* data;      // Pointer to heap
            size_t capacity;
        } heap;
    };
    
    size_t m_size;
    
    bool isSSO() const {
        return m_size <= SSO_SIZE;
    }
    
public:
    String(const char* str) {
        m_size = strlen(str);
        
        if (isSSO()) {
            // Use SSO buffer
            strcpy(sso_buffer, str);
        } else {
            // Allocate on heap
            heap.capacity = m_size + 1;
            heap.data = new char[heap.capacity];
            strcpy(heap.data, str);
        }
    }
    
    ~String() {
        if (!isSSO()) {
            delete[] heap.data;
        }
    }
    
    const char* c_str() const {
        return isSSO() ? sso_buffer : heap.data;
    }
};
```

### **SSO in Action:**

```cpp
#include <iostream>
#include <string>
using namespace std;

void printStringInfo(const string& s) {
    cout << "String: \"" << s << "\"\n";
    cout << "Size: " << s.size() << "\n";
    cout << "Capacity: " << s.capacity() << "\n";
    cout << "Data address: " << (void*)s.data() << "\n";
    cout << "Object address: " << (void*)&s << "\n\n";
}

int main() {
    string small = "Hi";         // SSO: data address == object address
    string medium = "Hello World Test";  // SSO or heap (depending on implementation)
    string large = "This is a very long string that won't fit in SSO";  // Heap
    
    printStringInfo(small);
    printStringInfo(medium);
    printStringInfo(large);
    
    return 0;
}
```

### **Memory Layout:**

```
Small String (SSO):
┌────────────────────────────┐
│ sso_buffer: "Hello\0"      │  ← Data stored in object
│ m_size: 5                  │
│ (no heap allocation)       │
└────────────────────────────┘

Large String (Heap):
┌────────────────────────────┐       Heap:
│ heap.data ────────────────────────> "Very long string..."
│ heap.capacity: 32          │
│ m_size: 25                 │
└────────────────────────────┘
```

### **Interview Points:**
- SSO: small strings (≤15 chars) stored in object
- No heap allocation for small strings
- Faster construction/destruction for small strings
- Copy-on-write in some implementations

---

## Map and Unordered Map

### **Q: What is the Difference Between map and unordered_map Internally?**

**Answer:**
- **map**: Implemented as **Red-Black Tree** (balanced BST). O(log n) operations. **Ordered** by key.
- **unordered_map**: Implemented as **Hash Table**. O(1) average operations. **Unordered**.

### **map (Red-Black Tree):**

```cpp
// Simplified map node
template<typename Key, typename Value>
struct MapNode {
    pair<const Key, Value> data;
    MapNode* left;
    MapNode* right;
    MapNode* parent;
    bool isRed;  // Red-Black tree color
};

template<typename Key, typename Value>
class Map {
    MapNode<Key, Value>* root;
    size_t m_size;
    
public:
    // Insert: O(log n)
    void insert(const Key& key, const Value& value) {
        // Binary search tree insertion
        // Rebalance if needed (Red-Black tree rules)
    }
    
    // Find: O(log n)
    Value* find(const Key& key) {
        MapNode<Key, Value>* node = root;
        while (node) {
            if (key < node->data.first)
                node = node->left;
            else if (key > node->data.first)
                node = node->right;
            else
                return &node->data.second;
        }
        return nullptr;
    }
};
```

### **unordered_map (Hash Table):**

```cpp
// Simplified unordered_map
template<typename Key, typename Value>
class UnorderedMap {
    struct Entry {
        pair<const Key, Value> data;
        Entry* next;  // For collision handling (chaining)
    };
    
    vector<Entry*> buckets;  // Array of linked lists
    size_t m_size;
    
    size_t hash(const Key& key) const {
        return std::hash<Key>{}(key) % buckets.size();
    }
    
public:
    UnorderedMap() : buckets(16, nullptr), m_size(0) {}
    
    // Insert: O(1) average
    void insert(const Key& key, const Value& value) {
        size_t index = hash(key);
        
        // Insert at head of linked list
        Entry* newEntry = new Entry{{key, value}, buckets[index]};
        buckets[index] = newEntry;
        m_size++;
        
        // Rehash if load factor too high
        if ((float)m_size / buckets.size() > 0.75) {
            rehash();
        }
    }
    
    // Find: O(1) average
    Value* find(const Key& key) {
        size_t index = hash(key);
        Entry* entry = buckets[index];
        
        // Traverse linked list
        while (entry) {
            if (entry->data.first == key)
                return &entry->data.second;
            entry = entry->next;
        }
        return nullptr;
    }
    
    void rehash() {
        // Double bucket count and reinsert all elements
    }
};
```

### **Memory Layout:**

```
map (Red-Black Tree):
         ┌───┐
         │ 5 │
         └─┬─┘
      ┌────┴────┐
    ┌─┴─┐     ┌─┴─┐
    │ 3 │     │ 8 │
    └─┬─┘     └─┬─┘
   ┌──┴──┐   ┌──┴──┐
 ┌─┴─┐ ┌─┴─┐│   ┌─┴─┐
 │ 1 │ │ 4 ││   │ 10│
 └───┘ └───┘└───┘───┘

unordered_map (Hash Table):
Buckets:
[0] → null
[1] → [key:10, val:X] → [key:21, val:Y] → null
[2] → [key:22, val:Z] → null
[3] → null
[4] → [key:15, val:W] → null
...
```

### **Performance Comparison:**

| Operation | map (RB-Tree) | unordered_map (Hash) |
|-----------|---------------|----------------------|
| **Insert** | O(log n) | O(1) average, O(n) worst |
| **Find** | O(log n) | O(1) average, O(n) worst |
| **Delete** | O(log n) | O(1) average, O(n) worst |
| **Ordered** | Yes | No |
| **Memory** | Less | More (buckets + chains) |

```cpp
#include <iostream>
#include <map>
#include <unordered_map>
using namespace std;

int main() {
    // map: ordered iteration
    map<int, string> m;
    m[5] = "five";
    m[2] = "two";
    m[8] = "eight";
    
    cout << "map (ordered):\n";
    for (auto& [k, v] : m) {
        cout << k << " ";  // Output: 2 5 8 (sorted!)
    }
    cout << "\n";
    
    // unordered_map: unordered iteration
    unordered_map<int, string> um;
    um[5] = "five";
    um[2] = "two";
    um[8] = "eight";
    
    cout << "unordered_map (unordered):\n";
    for (auto& [k, v] : um) {
        cout << k << " ";  // Output: unpredictable order
    }
    cout << "\n";
    
    return 0;
}
```

### **Interview Points:**
- map: Red-Black Tree, O(log n), ordered
- unordered_map: Hash table, O(1) average, unordered
- Choose map for ordered iteration, unordered_map for speed
- Collision handling: chaining (linked lists) or open addressing

---

## Iterator Implementation

### **Q: How Do Iterators Work Internally?**

**Answer:**
Iterators are **objects that act like pointers**, overloading operators like `*`, `->`, `++`, `--`. Different containers implement different iterator types.

### **Vector Iterator (Random Access):**

```cpp
template<typename T>
class VectorIterator {
    T* ptr;
    
public:
    VectorIterator(T* p) : ptr(p) {}
    
    // Dereference
    T& operator*() { return *ptr; }
    T* operator->() { return ptr; }
    
    // Increment/Decrement
    VectorIterator& operator++() {
        ptr++;
        return *this;
    }
    
    VectorIterator operator++(int) {  // Post-increment
        VectorIterator temp = *this;
        ptr++;
        return temp;
    }
    
    VectorIterator& operator--() {
        ptr--;
        return *this;
    }
    
    // Random access
    VectorIterator operator+(int n) const {
        return VectorIterator(ptr + n);
    }
    
    VectorIterator operator-(int n) const {
        return VectorIterator(ptr - n);
    }
    
    int operator-(const VectorIterator& other) const {
        return ptr - other.ptr;
    }
    
    T& operator[](int index) {
        return ptr[index];
    }
    
    // Comparison
    bool operator==(const VectorIterator& other) const {
        return ptr == other.ptr;
    }
    
    bool operator!=(const VectorIterator& other) const {
        return ptr != other.ptr;
    }
    
    bool operator<(const VectorIterator& other) const {
        return ptr < other.ptr;
    }
};
```

### **List Iterator (Bidirectional):**

```cpp
template<typename T>
struct ListNode {
    T data;
    ListNode* next;
    ListNode* prev;
};

template<typename T>
class ListIterator {
    ListNode<T>* node;
    
public:
    ListIterator(ListNode<T>* n) : node(n) {}
    
    T& operator*() { return node->data; }
    T* operator->() { return &node->data; }
    
    ListIterator& operator++() {
        node = node->next;
        return *this;
    }
    
    ListIterator& operator--() {
        node = node->prev;
        return *this;
    }
    
    bool operator==(const ListIterator& other) const {
        return node == other.node;
    }
    
    bool operator!=(const ListIterator& other) const {
        return node != other.node;
    }
    
    // Note: No operator+, operator-, or operator[]
    // (not random access)
};
```

### **Iterator Categories:**

| Category | Operations | Examples |
|----------|------------|----------|
| **Input** | `++`, `*`, `==` | istream_iterator |
| **Output** | `++`, `*` | ostream_iterator |
| **Forward** | Input + multi-pass | forward_list |
| **Bidirectional** | Forward + `--` | list, map, set |
| **Random Access** | Bidirectional + `+`, `-`, `[]` | vector, deque, array |

### **Using Iterators:**

```cpp
#include <iostream>
#include <vector>
#include <list>
using namespace std;

int main() {
    vector<int> v = {1, 2, 3, 4, 5};
    list<int> l = {1, 2, 3, 4, 5};
    
    // Vector iterator (random access)
    auto vit = v.begin();
    vit += 3;  // ✅ Jump to index 3
    cout << *vit << "\n";  // 4
    cout << vit[1] << "\n";  // 5
    
    // List iterator (bidirectional)
    auto lit = l.begin();
    // lit += 3;  // ❌ Won't compile (not random access)
    advance(lit, 3);  // ✅ Use advance()
    cout << *lit << "\n";  // 4
    
    return 0;
}
```

### **Interview Points:**
- Iterators abstract pointer operations
- Different categories: Input, Output, Forward, Bidirectional, Random Access
- Container chooses appropriate iterator type
- Algorithms work with any compatible iterator

---

## Allocators

### **Q: What Are Allocators in STL?**

**Answer:**
Allocators are **objects that encapsulate memory allocation/deallocation** for STL containers. Allow **custom memory management** strategies (pools, arenas, debug allocators).

### **Default Allocator:**

```cpp
template<typename T>
class Allocator {
public:
    // Allocate memory (doesn't construct)
    T* allocate(size_t n) {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }
    
    // Deallocate memory (doesn't destroy)
    void deallocate(T* ptr, size_t n) {
        ::operator delete(ptr);
    }
    
    // Construct object at address
    void construct(T* ptr, const T& value) {
        new(ptr) T(value);  // Placement new
    }
    
    // Destroy object
    void destroy(T* ptr) {
        ptr->~T();
    }
};
```

### **Custom Allocator Example:**

```cpp
#include <iostream>
#include <vector>
using namespace std;

template<typename T>
class LoggingAllocator {
public:
    using value_type = T;
    
    LoggingAllocator() = default;
    
    template<typename U>
    LoggingAllocator(const LoggingAllocator<U>&) {}
    
    T* allocate(size_t n) {
        cout << "Allocating " << n << " objects\n";
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }
    
    void deallocate(T* ptr, size_t n) {
        cout << "Deallocating " << n << " objects\n";
        ::operator delete(ptr);
    }
};

template<typename T, typename U>
bool operator==(const LoggingAllocator<T>&, const LoggingAllocator<U>&) {
    return true;
}

template<typename T, typename U>
bool operator!=(const LoggingAllocator<T>&, const LoggingAllocator<U>&) {
    return false;
}

int main() {
    vector<int, LoggingAllocator<int>> v;
    
    v.push_back(1);  // Allocating...
    v.push_back(2);  // Deallocating... Allocating...
    v.push_back(3);  // Deallocating... Allocating...
    
    // Deallocating... (destructor)
    return 0;
}
```

### **Pool Allocator (Advanced):**

```cpp
template<typename T>
class PoolAllocator {
    struct Block {
        char data[sizeof(T)];
        Block* next;
    };
    
    Block* freeList;
    static constexpr size_t POOL_SIZE = 1024;
    
public:
    PoolAllocator() {
        // Pre-allocate pool
        Block* pool = new Block[POOL_SIZE];
        freeList = pool;
        
        for (size_t i = 0; i < POOL_SIZE - 1; i++) {
            pool[i].next = &pool[i + 1];
        }
        pool[POOL_SIZE - 1].next = nullptr;
    }
    
    T* allocate(size_t n) {
        if (n != 1 || !freeList) {
            // Fall back to normal allocation
            return static_cast<T*>(::operator new(n * sizeof(T)));
        }
        
        Block* block = freeList;
        freeList = freeList->next;
        return reinterpret_cast<T*>(block);
    }
    
    void deallocate(T* ptr, size_t n) {
        if (n != 1) {
            ::operator delete(ptr);
            return;
        }
        
        Block* block = reinterpret_cast<Block*>(ptr);
        block->next = freeList;
        freeList = block;
    }
};
```

### **Using Custom Allocators:**

```cpp
// With vector
vector<int, LoggingAllocator<int>> v;

// With map
map<string, int, less<string>, LoggingAllocator<pair<const string, int>>> m;

// With string
using LogString = basic_string<char, char_traits<char>, LoggingAllocator<char>>;
```

### **Interview Points:**
- Allocators separate allocation from construction
- Allow custom memory management strategies
- Pool allocators for performance
- Debug allocators for leak detection
- Most code uses default allocator

---

**Summary - STL Container Internals:**

| Container | Internal Structure | Complexity | Use When |
|-----------|-------------------|------------|----------|
| **vector** | Dynamic array | O(1) access, O(n) insert | Random access, cache-friendly |
| **list** | Doubly-linked list | O(1) insert/delete | Frequent insertion/deletion |
| **map** | Red-Black Tree | O(log n) all ops | Ordered data |
| **unordered_map** | Hash table | O(1) average | Fast lookup, no order |
| **set** | Red-Black Tree | O(log n) all ops | Unique, ordered |
| **unordered_set** | Hash table | O(1) average | Unique, fast lookup |

**Key Takeaways:**
1. **Vector**: contiguous memory, 2x growth, SSO analogy in string
2. **String**: SSO for small strings (≤15 chars)
3. **map**: Red-Black Tree (ordered), O(log n)
4. **unordered_map**: Hash table (unordered), O(1) average
5. **Iterators**: Abstract pointer operations, different categories
6. **Allocators**: Customizable memory management

---

**Complete Interview Notes Index:**
1. [OOP Fundamentals](1_OOP_Fundamentals.md) - Abstract classes, virtual functions, vtable, polymorphism
2. [Memory Management](2_Memory_Management.md) - Copy constructors, Rule of 0/3/5, delete operator
3. [Modern C++ Keywords](3_Modern_Cpp_Keywords.md) - noexcept, override, explicit, constexpr, mutable
4. [Smart Pointers Deep Dive](4_Smart_Pointers_Deep_Dive.md) - unique_ptr, shared_ptr, weak_ptr internals
5. [STL Internals](5_STL_Internals.md) - Container implementations, iterators, allocators

---

**Good luck with your interviews! 🚀**
