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
## STL Container Member Functions Reference

> Below is a function-by-function reference for every standard container, plus container adaptors and `std::string`. `n` = container size, `k` = key length, `m` = inserted range size. "Amortized" applies because of doubling-style growth.

---

### `std::vector<T>` — dynamic contiguous array

| Function | Signature (simplified) | Use | Time Complexity |
|---|---|---|---|
| `vector()` / `vector(n)` / `vector(n, val)` / `vector(it, it)` / `vector({...})` | constructors | Create empty / sized / filled / range / init-list | O(n) |
| `operator=` | `vector& = const vector&` / `vector&&` | Copy/move assign | O(n) / O(1) |
| `assign` | `assign(n, val)` / `assign(it, it)` | Replace contents | O(n) |
| `at` | `T& at(size_t i)` | Bounds-checked access (throws) | O(1) |
| `operator[]` | `T& operator[](size_t i)` | Unchecked access | O(1) |
| `front` / `back` | `T& front()` / `T& back()` | First / last element | O(1) |
| `data` | `T* data()` | Raw pointer to buffer | O(1) |
| `begin` / `end` / `rbegin` / `rend` / `cbegin` / `cend` | iterators | Iteration | O(1) |
| `empty` | `bool empty()` | Check if empty | O(1) |
| `size` | `size_t size()` | Element count | O(1) |
| `max_size` | `size_t max_size()` | Theoretical max | O(1) |
| `capacity` | `size_t capacity()` | Allocated slots | O(1) |
| `reserve` | `reserve(size_t n)` | Pre-allocate capacity | O(n) if grows |
| `shrink_to_fit` | `shrink_to_fit()` | Release unused capacity | O(n) |
| `clear` | `clear()` | Destroy all elements | O(n) |
| `insert` | `insert(pos, val)` / `(pos, n, val)` / `(pos, it, it)` | Insert at position | O(n) |
| `emplace` | `emplace(pos, args...)` | In-place construct at pos | O(n) |
| `erase` | `erase(pos)` / `erase(first, last)` | Remove element(s) | O(n) |
| `push_back` | `push_back(const T&)` / `(T&&)` | Append element | Amortized O(1) |
| `emplace_back` | `emplace_back(args...)` | In-place append | Amortized O(1) |
| `pop_back` | `pop_back()` | Remove last | O(1) |
| `resize` | `resize(n)` / `resize(n, val)` | Change size | O(\|new−old\|) |
| `swap` | `swap(vector&)` | Swap with another | O(1) |

---

### `std::deque<T>` — double-ended queue (segmented array)

| Function | Use | Complexity |
|---|---|---|
| `operator[]`, `at`, `front`, `back` | Random access | O(1) |
| `push_back`, `emplace_back` | Append | Amortized O(1) |
| `push_front`, `emplace_front` | Prepend | Amortized O(1) |
| `pop_back`, `pop_front` | Remove end | O(1) |
| `insert`, `emplace`, `erase` | Middle insert/erase | O(n) |
| `size`, `empty`, `clear`, `resize`, `shrink_to_fit`, `swap`, `assign` | as in vector | O(1) / O(n) |
| `begin`, `end`, `rbegin`, `rend` | Iterators (random access) | O(1) |

> No `capacity()` / `reserve()` — deque grows in fixed-size chunks.

---

### `std::list<T>` — doubly-linked list

| Function | Use | Complexity |
|---|---|---|
| `front`, `back` | Access ends | O(1) |
| `push_back`, `push_front`, `emplace_back`, `emplace_front` | Insert at ends | O(1) |
| `pop_back`, `pop_front` | Remove ends | O(1) |
| `insert(it, val)`, `emplace(it, args...)` | Insert at iterator | O(1) |
| `erase(it)` / `erase(first, last)` | Remove | O(1) / O(m) |
| `splice(pos, other)` | Move all nodes from another list | O(1) |
| `splice(pos, other, it)` | Move one node | O(1) |
| `merge(other)` | Merge two sorted lists | O(n) |
| `remove(val)` / `remove_if(pred)` | Remove matching | O(n) |
| `unique()` / `unique(pred)` | Remove adjacent duplicates | O(n) |
| `reverse()` | Reverse in place | O(n) |
| `sort()` / `sort(cmp)` | Sort (merge-sort) | O(n log n) |
| `size`, `empty`, `clear`, `resize`, `swap`, `assign` | standard | O(1) / O(n) |

---

### `std::forward_list<T>` — singly-linked list

| Function | Use | Complexity |
|---|---|---|
| `front` | First element | O(1) |
| `push_front`, `emplace_front`, `pop_front` | Front operations | O(1) |
| `insert_after(pos, val)` / `emplace_after` | Insert after pos | O(1) |
| `erase_after(pos)` / `erase_after(first, last)` | Erase after pos | O(1) / O(m) |
| `before_begin()` | Iterator before first | O(1) |
| `splice_after`, `merge`, `remove`, `remove_if`, `unique`, `reverse`, `sort` | as list, "_after" variants | O(1) / O(n) / O(n log n) |
| `resize`, `clear`, `swap`, `assign` | standard | O(n) |

> No `size()` — would be O(n); designed to be minimal-overhead.

---

### `std::array<T, N>` — fixed-size array wrapper

| Function | Use | Complexity |
|---|---|---|
| `operator[]`, `at`, `front`, `back`, `data` | Access | O(1) |
| `size()`, `max_size()`, `empty()` | All return compile-time `N` | O(1) |
| `fill(val)` | Fill all elements | O(n) |
| `swap(other)` | Element-wise swap | O(n) |
| `begin`, `end`, `rbegin`, `rend` | Iterators | O(1) |

---

### `std::set<T>` / `std::multiset<T>` — Red-Black Tree (ordered)

| Function | Signature | Use | Complexity |
|---|---|---|---|
| `insert(val)` / `insert(it, val)` / `insert(range)` | Insert | unique vs multiset | O(log n) |
| `emplace(args...)` | In-place insert | | O(log n) |
| `erase(val)` / `erase(it)` / `erase(first, last)` | Remove | | O(log n) |
| `find(key)` | Locate element | | O(log n) |
| `count(key)` | Count matches (0/1 for set) | | O(log n) |
| `contains(key)` (C++20) | Membership test | | O(log n) |
| `lower_bound(key)` / `upper_bound(key)` | Range bounds | | O(log n) |
| `equal_range(key)` | Pair of bounds | | O(log n) |
| `extract(it)` / `extract(key)` (C++17) | Detach node handle | | O(log n) |
| `merge(other)` (C++17) | Splice nodes from another set | | O(n log n) |
| `key_comp()`, `value_comp()` | Comparator access | | O(1) |
| `size`, `empty`, `clear`, `swap`, `begin`, `end`, `rbegin`, `rend` | standard | bidirectional iters | O(1) / O(n) |

---

### `std::map<K,V>` / `std::multimap<K,V>` — Red-Black Tree (ordered)

| Function | Use | Complexity |
|---|---|---|
| `operator[key]` (map only) | Access/insert default | O(log n) |
| `at(key)` | Access with bounds check (throws) | O(log n) |
| `insert({k,v})` / `insert(it, {k,v})` / `insert_or_assign(k, v)` (C++17) | Insert | O(log n) |
| `emplace(args...)` / `emplace_hint(it, args...)` | In-place | O(log n) |
| `try_emplace(k, args...)` (C++17) | Insert only if absent | O(log n) |
| `erase(key)` / `erase(it)` | Remove | O(log n) |
| `find`, `count`, `contains`, `lower_bound`, `upper_bound`, `equal_range` | as set | O(log n) |
| `extract`, `merge` | Node handle ops | O(log n) / O(n log n) |
| `key_comp`, `value_comp` | Comparators | O(1) |

---

### `std::unordered_set<T>` / `std::unordered_multiset<T>` — hash table

| Function | Use | Complexity (avg / worst) |
|---|---|---|
| `insert`, `emplace` | Insert | O(1) / O(n) |
| `erase(key)` / `erase(it)` | Remove | O(1) / O(n) |
| `find(key)`, `count(key)`, `contains(key)` | Lookup | O(1) / O(n) |
| `equal_range(key)` | Range of matches | O(1) / O(n) |
| `bucket_count()`, `max_bucket_count()` | Bucket info | O(1) |
| `bucket(key)` | Which bucket key maps to | O(1) |
| `bucket_size(idx)` | Elements in bucket | O(1) |
| `load_factor()`, `max_load_factor()` | Load info | O(1) |
| `rehash(n)` | Set min bucket count | O(n) |
| `reserve(n)` | Pre-size for n elements | O(n) |
| `hash_function()`, `key_eq()` | Hash & equality access | O(1) |
| `size`, `empty`, `clear`, `swap`, `begin`, `end` | standard | forward iters only |

---

### `std::unordered_map<K,V>` / `std::unordered_multimap<K,V>` — hash table

| Function | Use | Complexity (avg / worst) |
|---|---|---|
| `operator[key]` (map only) | Access/insert default | O(1) / O(n) |
| `at(key)` | Checked access (throws) | O(1) / O(n) |
| `insert`, `insert_or_assign`, `emplace`, `emplace_hint`, `try_emplace` | Insert variants | O(1) / O(n) |
| `erase`, `find`, `count`, `contains`, `equal_range` | as unordered_set | O(1) / O(n) |
| `bucket_count`, `bucket`, `bucket_size`, `load_factor`, `max_load_factor`, `rehash`, `reserve` | Bucket/hash tuning | O(1) / O(n) |

---

### `std::stack<T, Container=deque<T>>` — LIFO adaptor

| Function | Use | Complexity |
|---|---|---|
| `push(val)` / `emplace(args...)` | Add on top | O(1) |
| `pop()` | Remove top (no return) | O(1) |
| `top()` | Access top | O(1) |
| `size`, `empty`, `swap` | Standard | O(1) |

> No iterators. Underlying container: `deque` by default; can use `vector` or `list`.

---

### `std::queue<T, Container=deque<T>>` — FIFO adaptor

| Function | Use | Complexity |
|---|---|---|
| `push(val)` / `emplace(args...)` | Enqueue at back | O(1) |
| `pop()` | Dequeue from front | O(1) |
| `front()` / `back()` | Access ends | O(1) |
| `size`, `empty`, `swap` | Standard | O(1) |

---

### `std::priority_queue<T, Container=vector<T>, Compare=less<T>>` — heap adaptor

| Function | Use | Complexity |
|---|---|---|
| `push(val)` / `emplace(args...)` | Insert (sift up) | O(log n) |
| `pop()` | Remove top (sift down) | O(log n) |
| `top()` | Access max (or min if `greater`) | O(1) |
| `size`, `empty`, `swap` | Standard | O(1) |

> Default = max-heap. Use `priority_queue<T, vector<T>, greater<T>>` for min-heap.

---

### `std::string` (≈ `vector<char>` with SSO + text ops)

| Function | Use | Complexity |
|---|---|---|
| `size` / `length`, `empty`, `capacity`, `reserve`, `shrink_to_fit`, `clear`, `resize` | Same as vector | O(1) / O(n) |
| `operator[]`, `at`, `front`, `back`, `data`, `c_str` | Access | O(1) |
| `operator+=`, `append(str)` / `(n, ch)` / `(it, it)` | Append | Amortized O(m) |
| `push_back(ch)`, `pop_back()` | Single char | O(1) |
| `insert(pos, ...)`, `erase(pos, n)`, `replace(pos, n, str)` | Mutate | O(n) |
| `find`, `rfind`, `find_first_of`, `find_last_of`, `find_first_not_of`, `find_last_not_of` | Search | O(n·m) naive |
| `substr(pos, n)` | Extract substring | O(n) |
| `compare(other)` | Lexicographic compare | O(n) |
| `starts_with`, `ends_with`, `contains` (C++20/23) | Predicates | O(m) |
| `stoi`, `stol`, `stof`, `stod`, `to_string` (free fns) | Conversions | O(n) |

---

### `std::span<T>` (C++20) — non-owning view of contiguous range

| Function | Use | Complexity |
|---|---|---|
| `size`, `size_bytes`, `empty` | Info | O(1) |
| `operator[]`, `at` (C++26), `front`, `back`, `data` | Access | O(1) |
| `first(n)`, `last(n)`, `subspan(off, n)` | Sub-view | O(1) |
| `begin`, `end`, `rbegin`, `rend` | Iteration | O(1) |

---

### `std::bitset<N>` — fixed-size bit array

| Function | Use | Complexity |
|---|---|---|
| `operator[]`, `test(i)` | Access bit | O(1) |
| `set()` / `set(i, val)`, `reset()` / `reset(i)`, `flip()` / `flip(i)` | Modify | O(N) / O(1) |
| `count()` | Number of 1 bits | O(N) |
| `size()` | N | O(1) |
| `any()`, `all()`, `none()` | Predicates | O(N) |
| `to_ulong()`, `to_ullong()`, `to_string()` | Conversions | O(N) |
| `operator&`, `\|`, `^`, `~`, `<<`, `>>` | Bitwise ops | O(N) |

---

## Functors (Function Objects)

### **What is a functor?**

A **functor** is any object that overloads `operator()`, making it **callable like a function**. Functors carry state, can be inlined by the compiler (unlike function pointers), and are the foundation of STL algorithms.

```cpp
struct Multiply {
    int factor;
    Multiply(int f) : factor(f) {}
    int operator()(int x) const { return x * factor; }   // makes it callable
};

Multiply times3(3);
cout << times3(10);   // 30  — looks like a function call
```

### **Why functors over function pointers?**

1. **Stateful** — can hold data between calls.
2. **Inlined** — compiler knows the exact type, can inline the body.
3. **Type-safe** — each functor type is unique.
4. **Composable** — can be passed to / returned from templates cleanly.

### **Built-in STL functors (`<functional>`)**

| Category | Functor | Effect |
|---|---|---|
| **Arithmetic** | `plus<T>`, `minus<T>`, `multiplies<T>`, `divides<T>`, `modulus<T>`, `negate<T>` | `a+b`, `a-b`, … |
| **Comparison** | `equal_to<T>`, `not_equal_to<T>`, `greater<T>`, `less<T>`, `greater_equal<T>`, `less_equal<T>` | Predicates |
| **Logical** | `logical_and<T>`, `logical_or<T>`, `logical_not<T>` | Boolean |
| **Bitwise** | `bit_and<T>`, `bit_or<T>`, `bit_xor<T>`, `bit_not<T>` | Bit ops |

Transparent versions (C++14): use `<>` and let types deduce: `less<>{}`, `greater<>{}`.

### **Common uses in STL**

```cpp
// 1. Sort descending
sort(v.begin(), v.end(), greater<int>{});

// 2. Min-heap priority_queue
priority_queue<int, vector<int>, greater<int>> minHeap;

// 3. Custom comparator for set / map
set<string, greater<>> s;

// 4. Algorithms with predicates
auto cnt = count_if(v.begin(), v.end(), [](int x){ return x > 10; });

// 5. transform / accumulate
transform(v.begin(), v.end(), v.begin(), negate<int>{});
int sum = accumulate(v.begin(), v.end(), 0, plus<int>{});
```

### **Custom functor with state**

```cpp
struct Counter {
    int count = 0;
    void operator()(int) { ++count; }
};

Counter c = for_each(v.begin(), v.end(), Counter{});
cout << c.count;   // number of elements visited
```

### **Lambdas are functors**

Every lambda is compiled into an **anonymous functor class** with `operator()`:

```cpp
auto add = [x = 10](int y) { return x + y; };
// Roughly equivalent to:
struct __lambda { int x = 10; int operator()(int y) const { return x + y; } };
```

### **`std::function` — type-erased wrapper**

Holds any callable (functor, lambda, function pointer, bind expression) with matching signature. Carries small runtime cost (heap alloc + virtual dispatch) — prefer templates / `auto` when performance matters.

```cpp
std::function<int(int,int)> f = plus<int>{};
f = [](int a, int b){ return a * b; };          // also works
```

### **Function adaptors**

| Adaptor | Use |
|---|---|
| `std::bind(fn, args...)` | Partial application (mostly superseded by lambdas) |
| `std::ref` / `std::cref` | Pass by reference into bind/thread |
| `std::not_fn(pred)` (C++17) | Negate a predicate |
| `std::mem_fn(&Class::method)` | Treat member function as callable |
| `std::invoke(fn, args...)` (C++17) | Uniform invocation for all callable forms |

### **Interview points**
- Functor = class with `operator()`.
- Faster than function pointers because the type is known at compile time → inlining.
- Lambdas are anonymous functors generated by the compiler.
- Built-in functors in `<functional>` cover arithmetic, comparison, logical, bitwise.
- `std::function` erases the type but adds overhead.
- Functors are the backbone of STL algorithms (`sort`, `for_each`, `accumulate`, `transform`, …).

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
2. [Memory Management](2_Memory_Management.md) - Copy constructors, Rule of 0/3/5, delete operator, dangling pointers, RAII
3. [Modern C++ Keywords](3_Modern_Cpp_Keywords.md) - noexcept, override, explicit, constexpr, mutable
4. [Smart Pointers Deep Dive](4_Smart_Pointers_Deep_Dive.md) - unique_ptr, shared_ptr, weak_ptr internals
5. [STL Internals](5_STL_Internals.md) - Container implementations, iterators, allocators
6. [Multithreading Deep Dive](6_Multithreading_Deep_Dive.md) - Threads, mutexes, atomics, condition variables
7. [Performance Optimization](7_Performance_Optimization.md) - Move semantics, compiler optimizations, cache efficiency

---

**Good luck with your interviews! 🚀**
