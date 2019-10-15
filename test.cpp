#include "include/resource.h"

#include <iostream>
#include <thread>

void inc(size_t limit, shared_resource<int> & shared) {
    for (auto i = 0ULL; i < limit; ++i) {
        auto res = grant_access(shared);
        ++(*res);
        std::cout << *res << '\n';
    }
}

int main() {
    using namespace std;
    auto res = shared_resource<int>(0);
    thread threads[10];
    for (auto & t : threads) {
        t = thread(inc, 10, std::ref(res));
    }

    for (auto & t: threads) {
        t.join();
    }
}
