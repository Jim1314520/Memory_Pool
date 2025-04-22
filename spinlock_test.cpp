#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>

constexpr int LOOP_TIMES = 100000;
int counter = 0;

// ---------- 自旋锁：死转 ----------
std::atomic_flag spin_lock_flag = ATOMIC_FLAG_INIT;

void spinlock_noyield_worker() {
    for (int i = 0; i < LOOP_TIMES; ++i) {
        while (spin_lock_flag.test_and_set(std::memory_order_acquire)) {
            // ❌ 死转，不让出CPU
        }
        for (volatile int j = 0; j < 1000; ++j);  // 模拟点工作量
        ++counter;


        spin_lock_flag.clear(std::memory_order_release);
    }
}

// ---------- 自旋锁：yield ----------
std::atomic_flag spin_lock_yield = ATOMIC_FLAG_INIT;

void spinlock_yield_worker() {
    for (int i = 0; i < LOOP_TIMES; ++i) {
        while (spin_lock_yield.test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield(); // ✅ 主动让出CPU
        }
        for (volatile int j = 0; j < 1000; ++j);  // 模拟点工作量
        ++counter;
        spin_lock_yield.clear(std::memory_order_release);
    }
}

// ---------- 互斥锁 ----------
std::mutex mtx;

void mutex_worker() {
    for (int i = 0; i < LOOP_TIMES; ++i) {
        std::lock_guard<std::mutex> lock(mtx); // ✅ 会阻塞等待，不浪费CPU
        for (volatile int j = 0; j < 1000; ++j);  // 模拟点工作量
        ++counter;
    }
}

// ---------- 运行测试 ----------
void run_test(const std::string& name, void(*worker)()) {
    counter = 0; // 重置计数器
    auto start = std::chrono::high_resolution_clock::now();

    std::thread t1(worker);
    std::thread t2(worker);
    t1.join();
    t2.join();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    std::cout << "[" << name << "] 耗时: " << diff.count() << " 秒, counter = " << counter << std::endl;
}

int main() {
    std::cout << "🔁 实验开始...\n";

    run_test("❌ 死转自旋锁", spinlock_noyield_worker);
    run_test("✅ Yield自旋锁", spinlock_yield_worker);
    run_test("✅✅ std::mutex", mutex_worker);

    return 0;
}
