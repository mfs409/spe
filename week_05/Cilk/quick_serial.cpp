#include <cilk/cilk.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <functional>

// Sort the range between begin and end.
// "end" is one past the final element in the range.
// This is pure C++ code before Cilk++ conversion.
void sample_qsort(int * begin, int * end)
{
    if (begin != end) {
        --end; // Exclude last element (pivot)
        int * middle = std::partition(begin, end,
                    std::bind2nd(std::less<int>(),*end));
        std::swap(*end, *middle); // pivot to middle
        sample_qsort(begin, middle);
        sample_qsort(++middle, ++end); // Exclude pivot
    }
}

// A simple test harness
int qmain(int n)
{
    int *a = new int[n];
    for (int i = 0; i < n; ++i) 
        a[i] = i;
    std::random_shuffle(a, a + n);
    std::cout << "Sorting " << n << " integers"
            << std::endl;
    sample_qsort(a, a + n);
    // Confirm that a is sorted and that each element
    // contains the index.
    for (int i = 0; i < n-1; ++i) {
        if ( a[i] == a[i+1] || a[i] != i ) {
            std::cout << "Sort failed at location i=" << i << " a[i] = "
                    << a[i] << " a[i+1] = " << a[i+1] << std::endl;
            delete[] a;
            return 1;
        }
    }
    std::cout << "Sort succeeded." << std::endl;
    delete[] a;
    return 0;
}

int main(int argc, char* argv[])
{
    int n = 100*1000*1000;
    if (argc == 2)
        n = std::atoi(argv[2]);
    return qmain(n); 
}
