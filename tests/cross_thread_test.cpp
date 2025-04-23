#include "../include/MemoryPool.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

using namespace Kama_memoryPool;

int main() {
    constexpr int N = 100000;
    std::vector<std::pair<void*, size_t>> blocks;
    blocks.reserve(N);

    /* ───────── 1) 线程 A 分配 ───────── */
    std::thread tA([&]{
        for (int i = 0; i < N; ++i) {
            size_t sz = (i % 32 + 1) * 8;           // 8 – 256 B
            void* p   = MemoryPool::allocate(sz);
            assert(p);
            std::memset(p, 0xAA, sz);               // 写入哨兵字节
            blocks.emplace_back(p, sz);
        }
    });

    /* ───────── 2) 线程 B 释放 ───────── */
    std::thread tB([&]{
        /* 等待 A 填充完 vector */
        tA.join();
        for (auto& [ptr, sz] : blocks) {
            /* 读回哨兵验证数据没损坏 */
            for (size_t j = 0; j < sz; ++j)
                assert(static_cast<unsigned char*>(ptr)[j] == 0xAA);
            MemoryPool::deallocate(ptr, sz);        // 跨线程释放
        }
    });

    tB.join();
    std::cout << "Cross‑thread deallocation test passed!\n";
    return 0;
}
