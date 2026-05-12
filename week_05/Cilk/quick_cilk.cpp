#include <iostream>
#include <algorithm>
#include <random>
#include <cilk/cilk.h>
#include <cilk/cilkscale.h>

constexpr std::ptrdiff_t BASE_CASE_LENGTH = 32;

template <typename T>
void sample_qsort(T* begin, T* end) {
  if (end - begin < BASE_CASE_LENGTH) {
    std::sort(begin, end);  // base case: serial sort
  } else {
    --end;  // exclude last element (pivot) from partition
    T* middle = std::partition(begin, end,
                               [pivot=*end](T a) { return a < pivot; });
    std::swap(*end, *middle);  // move pivot to middle
    cilk_scope {
      cilk_spawn sample_qsort(begin, middle);
      sample_qsort(++middle, ++end);  // exclude pivot and restore end
    }
  }
}

int main(int argc, char* argv[]) {
  long n = 100 * 1000 * 1000;
  if (argc == 2)
    n = (long) std::atoi(argv[1]);

  std::default_random_engine rng;
  std::uniform_int_distribution<long> dist(0,n);
  std::vector<long> a(n);
  std::generate(a.begin(), a.end(), [&]() { return dist(rng); });

  std::cout << "Sorting " << n << " random integers" << std::endl;
  wsp_t start = wsp_getworkspan();
  sample_qsort(a.data(), a.data() + a.size());
  wsp_t end = wsp_getworkspan(); 
  bool pass = std::is_sorted(a.cbegin(), a.cend());
  std::cout << "Sort " << ((pass) ? "succeeded" : "failed") << "\n";
  wsp_t elapsed = wsp_sub(end, start);
  wsp_dump(elapsed, "qsort_sample"); 
  return (pass) ? 0 : 1;
}
