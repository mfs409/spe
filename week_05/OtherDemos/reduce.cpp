#include <tbb/parallel_reduce.h>
#include <tbb/blocked_range.h>
#include <vector>
#include <iostream>

int main() {
    std::vector<int> data(1000000, 3);

    int sum = tbb::parallel_reduce(
        tbb::blocked_range<size_t>(0, data.size()),
        0,  // identity value
        [&](const tbb::blocked_range<size_t>& r, int local_sum) {
            for (size_t i = r.begin(); i != r.end(); ++i)
                local_sum += data[i];
            return local_sum;
        },
        std::plus<>()
    );

    std::cout << "Sum = " << sum << "\n";
}