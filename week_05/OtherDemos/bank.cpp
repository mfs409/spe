#include <thread>
#include <vector>
#include <iostream>

int cash = 0;   //Shared global variable

void customer() {
    for (int i = 0; i < 100000; ++i) {
        cash++;
        cash--;
    }
}

int main() {
    const int N = 32;

    std::vector<std::thread> threads;

    for (int i = 0; i < N; ++i) {
        threads.emplace_back(customer);
    }

    for (auto &t : threads) {
        t.join();
    }

    std::cout << "Total: " << cash << std::endl;

    return 0;
}

oneapi::tbb::parallel_for(
    oneapi::tbb::blocked_range<int>(0, n),
    [&](const oneapi::tbb::blocked_range<int>& r) {
        for (int i = r.begin(); i != r.end(); ++i)
            y[i] += a * x[i];
    }
);