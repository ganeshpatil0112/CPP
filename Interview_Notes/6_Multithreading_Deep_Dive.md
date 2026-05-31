# C++ Multithreading — Interview Revision Notes
*Target: ~4 Years Experience | C++11 / 14 / 17 / 20*

A consolidated revision guide. Topics are grouped logically (not in the order asked) so related concepts stay together.

---

## Table of Contents
1. [Introduction & Why Multithreading](#1-introduction--why-multithreading)
2. [Process vs Thread](#2-process-vs-thread)
3. [std::thread Basics](#3-stdthread-basics)
4. [Race Condition](#4-race-condition)
5. [std::mutex](#5-stdmutex)
6. [Types of Mutex](#6-types-of-mutex)
7. [Lock Wrappers — `lock_guard` vs `unique_lock` vs `scoped_lock`](#7-lock-wrappers--lock_guard-vs-unique_lock-vs-scoped_lock)
8. [Deadlock & Avoidance](#8-deadlock--avoidance)
9. [`std::condition_variable`](#9-stdcondition_variable)
10. [Producer–Consumer Problem](#10-producerconsumer-problem)
11. [Semaphore](#11-semaphore)
12. [Shared Memory (IPC)](#12-shared-memory-ipc)
13. [Atomics & C++ Memory Model](#13-atomics--c-memory-model)
14. [Lock-Free SPSC Queue & ABA Problem](#14-lock-free-spsc-queue--aba-problem)
15. [Thread-Safe Singleton (Double-Checked Locking + `call_once`)](#15-thread-safe-singleton-double-checked-locking--call_once)
16. [Thread Pool](#16-thread-pool)
17. [`std::async`, `future`, `promise`, `packaged_task`](#17-stdasync-future-promise-packaged_task)
18. [C++17 / C++20 Additions](#18-c17--c20-additions)
19. [Quick Interview Cheat Sheet](#19-quick-interview-cheat-sheet)

---

## 1. Introduction & Why Multithreading

### What is multithreading?
A **thread** is the smallest unit of execution within a process. **Multithreading** means running multiple threads concurrently inside the same process, sharing the same address space (code, heap, globals) while each thread has its **own stack and registers**.

### Why use multithreading?
- **Responsiveness** — UI stays interactive while background work runs.
- **Throughput / Performance** — utilize multiple CPU cores (true parallelism).
- **Resource sharing** — threads share memory, so communication is faster/cheaper than between processes.
- **Latency hiding** — overlap I/O wait with CPU work.

### Real-world use cases
| Domain | Example |
|---|---|
| Web servers | One thread per request / thread pool |
| Game engines | Render thread + physics thread + audio thread |
| GUI apps | Main UI thread + worker threads |
| Trading / HFT | Market-data thread, order-book thread, strategy thread |
| Data processing | Parallel map-reduce, ML training |
| OS / Drivers | Interrupt handlers, kernel workers |

### Benefits vs Risks
**Benefits:** parallel speedup, better responsiveness, modular design.
**Risks:** race conditions, deadlocks, livelock, starvation, harder debugging, non-determinism.

---

## 2. Process vs Thread

| Aspect | Process | Thread |
|---|---|---|
| Address space | Separate | Shared within process |
| Creation cost | High | Low |
| Communication | IPC (pipes, shared memory, sockets) | Direct shared memory |
| Crash impact | Isolated | Brings down the whole process |
| Context switch | Expensive | Cheap |

---

## 3. `std::thread` Basics

Introduced in **C++11**. Header `<thread>`.

```cpp
#include <thread>
#include <iostream>

void worker(int id) {
    std::cout << "Thread " << id << " running\n";
}

int main() {
    std::thread t1(worker, 1);          // create + start
    std::thread t2([] { std::cout << "lambda\n"; });

    t1.join();   // wait for t1 to finish
    t2.join();
    // If you don't join() or detach(), destructor calls std::terminate()
}
```

**Key APIs**
- `join()` — block until thread finishes.
- `detach()` — let thread run independently (daemon-like). Cannot join later.
- `joinable()` — true until `join`/`detach`.
- `std::this_thread::get_id()`, `sleep_for`, `yield`.
- `std::thread::hardware_concurrency()` — number of HW threads (hint).

**Pitfalls**
- Forgetting to `join` → `std::terminate`.
- Passing references → use `std::ref()` or you get a copy.
- Capturing locals by reference in a detached thread → dangling reference UB.

---

## 4. Race Condition

### What is it?
A **race condition** occurs when two or more threads access shared data **concurrently** and at least one of them **writes**, *and* the result depends on the timing/interleaving of those accesses. The outcome is non-deterministic and usually wrong.

### Classic example
```cpp
int counter = 0;
void inc() { for (int i = 0; i < 100000; ++i) ++counter; }   // NOT atomic

int main() {
    std::thread a(inc), b(inc);
    a.join(); b.join();
    std::cout << counter << "\n";   // expected 200000, actually less
}
```
`++counter` is **read → modify → write** — three steps. Two threads can read the same value, both increment, both write back → one update is lost.

### How to prevent
1. **Mutex** to serialize access.
2. **`std::atomic`** for simple counters/flags.
3. **Immutability** — read-only data is race-free.
4. **Thread-local storage** (`thread_local`) — each thread has its own copy.
5. **Message passing** — avoid sharing entirely.

---

## 5. `std::mutex`

A **mutex** (mutual exclusion) is a synchronization primitive that guarantees only **one thread at a time** holds it. Used to protect shared data and create *critical sections*.

### Why we use it
- Eliminate race conditions on shared writable data.
- Protect data structures (containers, files, counters).
- Enforce invariants that span multiple variables.

### Basic usage (manual — discouraged)
```cpp
std::mutex m;
int counter = 0;

void inc() {
    m.lock();
    ++counter;          // critical section
    m.unlock();
}
```
Problem: if an exception is thrown between `lock()` and `unlock()`, the mutex stays locked → **deadlock**. Always use RAII wrappers.

### Fix with RAII
```cpp
void inc() {
    std::lock_guard<std::mutex> lk(m);
    ++counter;
}   // unlock automatic at scope exit
```

### Real-time uses
- Bank account balance updates.
- Logger writing to a shared file.
- Cache (insert/lookup).
- Queue between producer and consumer.

---

## 6. Types of Mutex

| Type | Header | Use Case |
|---|---|---|
| `std::mutex` | `<mutex>` | Default; non-recursive, single owner. |
| `std::recursive_mutex` | `<mutex>` | Same thread can lock multiple times (e.g., recursive function holding a lock). Count must match unlocks. |
| `std::timed_mutex` | `<mutex>` | Adds `try_lock_for(duration)` / `try_lock_until(time)`. |
| `std::recursive_timed_mutex` | `<mutex>` | Combination of the above. |
| `std::shared_mutex` *(C++17)* | `<shared_mutex>` | Reader-writer lock: many readers OR one writer. |
| `std::shared_timed_mutex` *(C++14)* | `<shared_mutex>` | Shared mutex + timed operations. |

### `recursive_mutex` example
```cpp
std::recursive_mutex rm;
void recurse(int n) {
    std::lock_guard<std::recursive_mutex> lk(rm);
    if (n > 0) recurse(n - 1);   // would deadlock with std::mutex
}
```
**Tip:** Needing `recursive_mutex` usually signals bad design — refactor.

### `shared_mutex` (reader-writer) example
```cpp
std::shared_mutex sm;
std::map<int, std::string> cache;

std::string read(int k) {
    std::shared_lock<std::shared_mutex> lk(sm);   // shared (read) lock
    return cache[k];
}
void write(int k, std::string v) {
    std::unique_lock<std::shared_mutex> lk(sm);   // exclusive (write) lock
    cache[k] = std::move(v);
}
```
Use when reads vastly outnumber writes.

---

## 7. Lock Wrappers — `lock_guard` vs `unique_lock` vs `scoped_lock`

| Wrapper | Since | Movable | Manual unlock/relock | Multi-mutex | Use with `condition_variable` |
|---|---|---|---|---|---|
| `std::lock_guard` | C++11 | No | No | No | No |
| `std::unique_lock` | C++11 | Yes | Yes | One mutex | **Yes** |
| `std::scoped_lock` | **C++17** | No | No | **Many (deadlock-free)** | No |

### `lock_guard` — simplest, lightest
```cpp
std::lock_guard<std::mutex> lk(m);   // locks now, unlocks at scope exit
```

### `unique_lock` — flexible
```cpp
std::unique_lock<std::mutex> lk(m);
lk.unlock();
// ... do work without the lock
lk.lock();
// Required by std::condition_variable::wait()
```

### `scoped_lock` (C++17) — multi-mutex, deadlock-free
```cpp
std::mutex m1, m2;
std::scoped_lock lk(m1, m2);   // CTAD; locks both atomically using deadlock-avoidance algorithm
```
Internally uses `std::lock(...)` which applies a try-and-back-off algorithm so the order of arguments doesn't matter.

**Rule of thumb:**
- One mutex, simple scope → `lock_guard`.
- Need to wait on `condition_variable`, or unlock early → `unique_lock`.
- Two or more mutexes → `scoped_lock` (C++17). Pre-C++17, use `std::lock` + adopt locks.

---

## 8. Deadlock & Avoidance

### What is a deadlock?
Two or more threads are blocked **forever**, each waiting for a resource the other holds.

Four Coffman conditions (all must hold):
1. Mutual exclusion
2. Hold and wait
3. No preemption
4. Circular wait

### Classic example
```cpp
std::mutex A, B;
void t1() { std::lock_guard la(A); std::lock_guard lb(B); /* work */ }
void t2() { std::lock_guard lb(B); std::lock_guard la(A); /* work */ }
// t1 holds A waits B; t2 holds B waits A → deadlock
```

### Avoidance strategies
1. **Lock ordering** — always acquire mutexes in a globally fixed order.
2. **`std::lock(m1, m2, ...)`** — locks all atomically, deadlock-free (C++11):
   ```cpp
   std::lock(A, B);
   std::lock_guard la(A, std::adopt_lock);
   std::lock_guard lb(B, std::adopt_lock);
   ```
3. **`std::scoped_lock(m1, m2, ...)`** — same thing, cleaner syntax (C++17).
4. **Try-lock with timeout** — back off if you can't acquire.
5. **Lock-free / hierarchical mutex** — design-level prevention.
6. **Keep critical sections small** — less chance of nested locking.

---

## 9. `std::condition_variable`

A primitive that lets a thread **wait** until some condition becomes true, while atomically releasing a mutex during the wait. Used together with a `std::mutex` and a predicate.

### Why?
Polling (`while (!ready) {}`) burns CPU. A condition variable puts the thread to sleep until another thread `notify`s it.

### API
- `wait(lock, pred)` — releases lock, sleeps; on wake, reacquires lock and checks predicate.
- `notify_one()` — wake one waiting thread.
- `notify_all()` — wake all waiting threads.

**Must use `std::unique_lock<std::mutex>` (not `lock_guard`)** because the CV must unlock/relock.

### Spurious wakeups
A CV can wake a thread even without a notify. **Always wait with a predicate.**

```cpp
cv.wait(lk, []{ return ready; });   // safe even with spurious wakeups
```

### TOPIC: Condition Variable In C++ Threading

IMPORTANT POINT: CV are used for two purpose
 	A. Notify other threads
 	B. Waiting for some conditions

1. Condition Variable allows running threads to wait on some conditions and once those conditions are met the waiting thread
is notified using :
    - a. notify_one();
    - b. notify_all();
2. You need mutex to use condition variable.
3. If some thread want to wait on some condition then it has to do these things:
    - a. Acquire the mutex lock using std :: unique_lock<std :: mutex> lock(m) ; .
    - b. Execute wait, wait for, or wait until. The wait operations atomically release the mutex
        and suspend the execution of the thread.
    - c. When the condition variable is notified, the thread is awakened, and the mutex is atomically reacquired.
        The thread should then check the condition and resume waiting if the wake up was spurious.

NOTE:
1. Condition variables are used to synchronize two or more threads.
2. Best use case of condition variable is Producer/Consumer problem.

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

std::condition variable cv;
std::mutex m;
long balance = 0;

void addMoney(int money) {
	std :: lock_guard<mutex> lg(m) ;
	balance+=money;
	cout << "Amount Added Current Balance: " << balance << endl;
	cv.notify_one();    //This will notify the threads if the threads are wait for the Conditional_Variable notification to check the conditional and proceed. 
}

void withdrowMoney(int money) {
	std::unique lock<mutex> ul(m);
	cv.wait(ul, [] { return (balance != 0) ? true : false; }); //After the mutex lock, thread will wait and in sleep mode here until the notification for the Coditional_Variable comes and that becomes TRUE.
	if(balance>=money) {
	balance -= money;
	cout << "Amount Deducted: " << money << endl;
	}else{
	cout << "Amount Can't Be Deducted, Current Balance Is Less Than " << money << endl;
	}
	cout << "Current Balance Is: " << balance << endl;
}

int main() {
	std :: thread t1(withdrowMoney, 500);
	std :: this_thread: : sleep_for(std : : chrono: : seconds (2) ) ;|
	std :: thread t2(addMoney, 500);
	t1.join();
	t2.join();
	return 0;
}
```

---

## 10. Producer–Consumer Problem with the Mutex and Conditional variable.

### Problem
One or more **producers** put items into a buffer; one or more **consumers** take items out. Must avoid:
- Consumer reading from empty buffer.
- Producer writing to full buffer.
- Data corruption from concurrent access.

- Shared buffer (deque<int>) is accessed by two threads → needs synchronization.
- std::mutex guarantees only one thread touches the buffer at a time.
- std::condition_variable lets a thread sleep until a condition is true, instead of busy-waiting:
    - Producer waits while buffer is full.
    - Consumer waits while buffer is empty.
- unique_lock is used (not lock_guard) because cond.wait() must be able to unlock the mutex while sleeping and re-lock it on wakeup.
- notify_one() wakes the other thread after producing/consuming, so it can re-check its predicate.

### Solution (mutex + condition variable, bounded queue)
```cpp
#include <iostream>
#include <queue>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

// ---------- Shared resources ----------
std::mutex mu;                       // Protects access to the shared buffer
std::condition_variable cond;        // Used to signal/wait between producer & consumer
deque<int> buffer;                   // The shared bounded buffer
const unsigned int maxBufferSize = 50; // Upper bound -> producer must wait if full

// ---------- Producer ----------
void producer(int val) {
    while (val) {
        // unique_lock is REQUIRED here (not lock_guard) because
        // condition_variable::wait() needs to unlock/relock the mutex.
        std::unique_lock<std::mutex> locker(mu);

        // Wait until the buffer has space.
        // wait() atomically: releases the lock + sleeps, then
        // re-acquires the lock when notified AND predicate is true.
        // The predicate also protects against "spurious wakeups".
        cond.wait(locker, [](){ return buffer.size() < maxBufferSize; });

        // Critical section: safe to modify shared buffer
        buffer.push_back(val);
        cout << "Produced: " << val << endl;
        val--;

        // Release lock BEFORE notifying -> consumer won't wake up
        // only to immediately block on the mutex (minor optimization).
        locker.unlock();

        // Wake one waiting consumer (if any) so it can re-check its predicate.
        cond.notify_one();
    }
}

// ---------- Consumer ----------
void consumer() {
    while (true) {
        std::unique_lock<std::mutex> locker(mu);

        // Sleep until producer puts something in the buffer.
        cond.wait(locker, [](){ return buffer.size() > 0; });

        // Critical section: safe to read/remove from buffer
        int val = buffer.back();
        buffer.pop_back();
        cout << "Consumed: " << val << endl;

        locker.unlock();

        // Notify producer in case it was blocked on a full buffer.
        cond.notify_one();
    }
}

int main() {
    std::thread t1(producer, 100);
    std::thread t2(consumer);

    t1.join();   // <-- was missing ';'
    t2.join();
    return 0;
}
```
### Key points to remember for interviews:
- Why unique_lock over lock_guard? condition_variable::wait must unlock the mutex while waiting — only unique_lock supports that.
- Why pass a predicate to wait? Protects against spurious wakeups and makes the code equivalent to a while (!pred) cond.wait(lock); loop.
- notify_one vs notify_all — notify_one is enough here since only one counterpart can make progress per slot.
- Bounded buffer — without maxBufferSize, the producer would grow memory unboundedly if consumer is slow.
- Program never exits cleanly — consumer loops forever; in production you'd add a done flag + notify_all to shut it down.

Two CVs (`not_empty`, `not_full`) avoid unnecessary wakeups.

---

## 11. Semaphore

### Theory
A **semaphore** is a synchronization primitive that maintains a **counter**:
- `acquire()` / `wait()` / `P()` — if counter > 0, decrement; else block.
- `release()` / `signal()` / `V()` — increment counter, wake a waiter.

Types:
- **Binary semaphore** — counter ∈ {0, 1}; similar to a mutex but **ownership-less** (any thread can release).
- **Counting semaphore** — counter ≥ 0; controls access to a pool of N identical resources.

### Mutex vs Semaphore
| | Mutex | Semaphore |
|---|---|---|
| Ownership | Yes (only locker can unlock) | No |
| Counter | Binary, with ownership | Generic counter |
| Use | Critical section | Resource pool / signaling |

### C++20 `std::counting_semaphore` / `std::binary_semaphore`
```cpp
#include <semaphore>
#include <thread>

std::counting_semaphore<10> sem(3);   // max 10, initial 3 permits

void worker() {
    sem.acquire();          // decrement; wait if 0
    // ... use shared resource (max 3 threads here)
    sem.release();          // increment; wake one waiter
}

-----------------------------------------------------------------------------------------------------------------------------
binary_semaphore Example:

#include <chrono>
#include <iostream>
#include <semaphore>
#include <thread>

// global binary semaphore instances
// object counts are set to zero
// objects are in non-signaled state
std::binary_semaphore
    SignalMainToThread{0},
    SignalThreadToMain{0};

void ThreadProc()
{
	// wait for a signal from the main proc
	// by attempting to decrement the semaphore
	SignalMainToThread.acquire();

	// this call blocks until the semaphore's count
	// is increased from the main proc
	std::cout << "[thread] Got the signal\n"; // response message

	using namespace std: :literals;
	std::this_thread: : sleep_for(3s) ;

	std::cout << "[thread] Send the signal\n"; // message

	// signal the main proc back
	SignalThreadToMain. release();
}

int main()
{
	// create some worker th@ad
	std::thread thrWorker(ThreadProc);

	std::cout << "[main] Send the signal\n"; // message

	// signal the worker thread to start working
	// by increasing the semaphore's count
	SignalMainToThread.release();

	// wait until the worker thread is done doing the work
	// by attempting to decrement the semaphore's count
	SignalThreadToMain.acquire();

	std::cout << "[main] Got the signal\n"; // response message
	thrWorker.join();
}

Key concepts to remember:
acquire(): Attempts to decrement the semaphore. If the value is 0, the thread blocks until the value becomes greater than 0 (signaled by another thread).
release(): Increments the semaphore, effectively signaling other threads waiting on it.
Initialization: The speaker emphasizes that initializing with 0 is critical for signaling because it ensures the thread starts in a blocked state.
```

### Producer–Consumer using semaphores
```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

std::binary_semaphore
	signal_to_producer{1}, // 1 - Will not stop the thread at the first time.
	signal_to_consumer{0}; // 0 - Will stop the thread at the first time until CV gets and signal and becomes TRUE.

#define buff_size 5
int buff[buff_size];

void producer() {
	while(1) {
		signal_to_producer.acquire();
		cout << "Produced =";
		for(int i=0; i<buff_size; ++i) {
			buff[i] = i*i; // task of producer
			cout << buff[i] << "" << std :: flush;
			std :: this_thread :: sleep_for(milliseconds (200) );
		}
	cout << endl;
	signal_to_consumer.release();
	}
}

void consumer(){
	while(1) {
		signal_to_consumer.acquire();
		cout << "Consumed =";
		for(int i=buff_size-1; i>=0; -- i) {
			cout << buff[i] << "" << std :: flush;
			buff[i] = 0; // task of consumer
			std :: this_thread :: sleep_for(milliseconds (200) );
		}
		cout << endl; cout << endl;
		signal_to_producer. release();
	}
}

int main()
{
	std::thread producer_thread(producer) ;
	std::thread consumer thread (consumer) ;

	std::cout << "[main] Got the signal\n"; 
	producer_thread.join();
	consumer_thread.join();
	return 0;
}
```

**Pre-C++20:** use POSIX `sem_t` (Linux) or roll your own with mutex + CV.

---

## 12. Shared Memory (IPC)

Not to be confused with threads sharing memory. **Shared memory** here means **inter-process communication**: a memory region mapped into multiple processes' address spaces.

### Use case
Fastest IPC — no copy through the kernel. Used in HFT, video pipelines, databases.

### POSIX example (Linux)
```cpp
#include <sys/mman.h>
#include <fcntl.h>
int fd = shm_open("/myseg", O_CREAT | O_RDWR, 0666);
ftruncate(fd, 4096);
void* p = mmap(nullptr, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
// Both processes can read/write *p
```

### Synchronization across processes
Threads inside one process use `std::mutex`. **Across processes**, use:
- Named POSIX semaphores (`sem_open`).
- Process-shared `pthread_mutex_t` with `PTHREAD_PROCESS_SHARED` attribute.
- Atomic operations on shared memory (lock-free).

Windows: `CreateFileMapping` + `MapViewOfFile`, `CreateMutex`, `CreateSemaphore`.

---

## 13. Atomics & C++ Memory Model

`std::atomic<T>` (C++11) gives indivisible operations without locks for small types.

```cpp
std::atomic<int> counter{0};
void inc() { for (int i=0;i<100000;++i) counter.fetch_add(1, std::memory_order_relaxed); }
```

### Memory orderings (briefly)
| Order | Guarantee |
|---|---|
| `relaxed` | Atomic only; no ordering with other ops. Good for counters. |
| `acquire` | No reads/writes can be reordered before this load. |
| `release` | No reads/writes can be reordered after this store. |
| `acq_rel` | Both, for RMW ops. |
| `seq_cst` *(default)* | Total global order across all atomic ops. Safest, slowest. |

### `volatile` is NOT for threading
`volatile` prevents compiler optimizations for memory-mapped I/O, but provides **no atomicity and no ordering** between threads. Use `std::atomic`.

---

## 14. Lock-Free SPSC Queue & ABA Problem

### Single-Producer Single-Consumer (SPSC) ring buffer
Easiest lock-free queue — one writer thread, one reader thread.

```cpp
template<typename T, size_t N>
class SPSCQueue {
    std::array<T, N> buf;
    std::atomic<size_t> head{0};   // producer writes
    std::atomic<size_t> tail{0};   // consumer reads
public:
    bool push(const T& v) {
        size_t h = head.load(std::memory_order_relaxed);
        size_t next = (h + 1) % N;
        if (next == tail.load(std::memory_order_acquire)) return false; // full
        buf[h] = v;
        head.store(next, std::memory_order_release);
        return true;
    }
    bool pop(T& out) {
        size_t t = tail.load(std::memory_order_relaxed);
        if (t == head.load(std::memory_order_acquire)) return false;    // empty
        out = buf[t];
        tail.store((t + 1) % N, std::memory_order_release);
        return true;
    }
};
```
Producer uses `release` to publish data; consumer uses `acquire` to see it.

### ABA Problem
In lock-free algorithms using **compare-and-swap (CAS)**, a value can change from **A → B → A** between two reads. CAS sees "A" and thinks nothing changed, but the world did (e.g., the node was freed and reallocated).

**Mitigations:**
- **Tagged pointers / version counters** — store `(ptr, counter)`; counter always increments → CAS sees the change.
- **Hazard pointers** — protect pointers from being reclaimed while in use.
- **RCU / Epoch-based reclamation**.
- For SPSC queue above, ABA doesn't arise because only one producer and one consumer, no node reuse via CAS.

---

## 15. Thread-Safe Singleton (Double-Checked Locking + `call_once`)

### The naive (broken) Singleton
```cpp
Singleton* getInstance() {
    if (!instance) instance = new Singleton();   // race!
    return instance;
}
```

### Double-Checked Locking (DCL) — broken pre-C++11
```cpp
Singleton* getInstance() {
    if (!instance) {                         // (1) no lock
        std::lock_guard<std::mutex> lk(m);
        if (!instance) instance = new Singleton();   // (2) under lock
    }
    return instance;
}
```
**Why broken pre-C++11:** without a defined memory model, the compiler/CPU could reorder steps inside `new Singleton()` (allocate → assign-to-`instance` → construct), so another thread at (1) could see a non-null but **half-constructed** object.

### Correct (C++11) — `std::atomic`
```cpp
std::atomic<Singleton*> instance{nullptr};
std::mutex m;
Singleton* getInstance() {
    Singleton* p = instance.load(std::memory_order_acquire);
    if (!p) {
        std::lock_guard<std::mutex> lk(m);
        p = instance.load(std::memory_order_relaxed);
        if (!p) {
            p = new Singleton();
            instance.store(p, std::memory_order_release);
        }
    }
    return p;
}
```

### Correct & cleaner — `std::call_once`
```cpp
std::once_flag flag;
Singleton* instance = nullptr;
Singleton* getInstance() {
    std::call_once(flag, []{ instance = new Singleton(); });
    return instance;
}
```

### Best (C++11) — Meyers' Singleton
```cpp
Singleton& getInstance() {
    static Singleton inst;   // thread-safe init guaranteed by C++11 [stmt.dcl]/4
    return inst;
}
```
Local `static` initialization is thread-safe since C++11. No mutex, no atomics, no leak.

**Why Singleton is often an anti-pattern:** hidden global state, hard to mock/test, lifetime issues, hides dependencies.

---

## 16. Thread Pool

A **thread pool** keeps a fixed number of worker threads alive, pulling tasks from a queue. Avoids the cost of creating/destroying threads per task.

### Minimal implementation
```cpp
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <future>
#include <condition_variable>

class ThreadPool {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex m;
    std::condition_variable cv;
    bool stop = false;
public:
    explicit ThreadPool(size_t n) {
        for (size_t i = 0; i < n; ++i)
            workers.emplace_back([this] {
                for (;;) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lk(m);
                        cv.wait(lk, [this]{ return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front()); tasks.pop();
                    }
                    task();
                }
            });
    }

    template<class F>
    auto submit(F&& f) -> std::future<decltype(f())> {
        using R = decltype(f());
        auto task = std::make_shared<std::packaged_task<R()>>(std::forward<F>(f));
        std::future<R> fut = task->get_future();
        {
            std::lock_guard<std::mutex> lk(m);
            tasks.emplace([task]{ (*task)(); });
        }
        cv.notify_one();
        return fut;
    }

    ~ThreadPool() {
        { std::lock_guard<std::mutex> lk(m); stop = true; }
        cv.notify_all();
        for (auto& t : workers) t.join();
    }
};
```

**Why a pool?**
- Bounds resource use.
- Amortizes thread creation cost.
- Useful for servers, parallel algorithms, async event handling.

---

## 17. `std::async`, `future`, `promise`, `packaged_task`

| Type | Purpose |
|---|---|
| `std::future<T>` | Read-end of an asynchronous result. |
| `std::promise<T>` | Write-end; set value/exception to be observed by future. |
| `std::packaged_task<Sig>` | Wraps a callable; running it sets a future. |
| `std::async` | Convenience launcher; returns a future. |

### `std::async`
```cpp
auto f = std::async(std::launch::async, []{ return 42; });
int x = f.get();   // 42
```
Launch policies:
- `std::launch::async` — new thread.
- `std::launch::deferred` — runs lazily on `get()`/`wait()`.
- Default = either (implementation-defined) — **avoid the default**, be explicit.

### Pitfall: destructor of async future blocks
If you don't store the future from `std::async(std::launch::async, ...)`, its destructor **joins** — surprising serial behavior.

### `promise` / `future` pair
```cpp
std::promise<int> p;
std::future<int> f = p.get_future();
std::thread t([&]{ p.set_value(7); });
std::cout << f.get();   // 7
t.join();
```

### `packaged_task`
Used inside thread pools (see above). Wraps a callable into something whose result is a future.

---

## 18. C++17 / C++20 Additions

### C++17
- **`std::scoped_lock`** — multi-mutex deadlock-free locking (replaces `std::lock` boilerplate).
- **`std::shared_mutex`** — finalized (was `shared_timed_mutex` in C++14).
- **Parallel algorithms** — execution policies in `<algorithm>`:
  ```cpp
  std::sort(std::execution::par, v.begin(), v.end());
  ```

### C++20
- **`std::jthread`** — joining thread; **automatically joins on destruction** and supports cooperative cancellation via `std::stop_token`.
  ```cpp
  #include <thread>
  void task(std::stop_token st) {
      while (!st.stop_requested()) { /* work */ }
  }
  int main() {
      std::jthread jt(task);
      // ... jt.request_stop();  // optional; destructor calls request_stop then join
  }   // auto-joins here
  ```
  Fixes the common bug of forgetting to `join()` a `std::thread`.

- **`std::stop_token` / `std::stop_source` / `std::stop_callback`** — cooperative cancellation API; thread checks `st.stop_requested()`.

- **`std::counting_semaphore<N>` / `std::binary_semaphore`** — see [§11](#11-semaphore).

- **`std::latch`** — single-use countdown barrier; threads `arrive_and_wait()` until count = 0.
  ```cpp
  std::latch start{3};
  // workers: start.arrive_and_wait();   // released when 3 arrive
  ```

- **`std::barrier`** — reusable barrier with optional completion function (run once when all arrive, then resets).

- **`std::atomic_ref<T>`** — atomic operations on existing non-atomic objects.

- **Atomic waits** — `std::atomic<T>::wait/notify_one/notify_all` — efficient futex-like sleeping on an atomic value.

- **Coroutines** — `co_await`, `co_yield`, `co_return` — orthogonal to threads but enable async without threads-per-task.

---

## 19. Quick Interview Cheat Sheet

**"Race condition vs Data race"** — A *data race* is the C++ standard term for concurrent access where at least one is a non-atomic write → **undefined behavior**. *Race condition* is broader (any logic depending on timing).

**Why `lock_guard` over `m.lock()/unlock()`?** Exception safety (RAII).

**Why `std::scoped_lock` over `std::lock` + adopt?** Cleaner, less boilerplate, same deadlock-avoidance.

**Why `unique_lock` needed for `condition_variable::wait`?** CV must unlock and relock; `lock_guard` cannot be unlocked.

**`notify_one` vs `notify_all`** — One: cheaper, wake a single waiter; All: when condition allows multiple waiters to proceed (e.g., shutdown signal).

**`std::atomic<int>` vs `volatile int`** — Atomic gives atomicity + ordering + thread visibility. Volatile gives none of these for threads.

**Why is `std::shared_ptr`'s control block thread-safe but the pointee isn't?** Refcount uses atomics; the object you point to is your responsibility.

**False sharing** — Two threads write to different variables that happen to share a CPU cache line → ping-pong invalidation kills perf. Fix with `alignas(std::hardware_destructive_interference_size)` (C++17).

**Best practice ladder**
1. Prefer immutability / message passing.
2. If sharing, prefer `std::atomic` for simple state.
3. Use `std::mutex` + RAII for complex critical sections.
4. Reach for lock-free only when profiling demands it.
5. Prefer `std::jthread` (C++20) over `std::thread`.

---

### Suggested study order before an interview
1. §1–§4 (concepts & race conditions)
2. §5–§9 (mutex / locks / CV)
3. §10 producer–consumer (code from memory)
4. §11 semaphore + §15 singleton + §16 thread pool (be ready to whiteboard)
5. §13–§14 atomics, ABA (senior depth)
6. §18 C++17/20 features (modernity signal)

---
