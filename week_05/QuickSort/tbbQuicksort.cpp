#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <tbb/parallel_invoke.h>
#include <tbb/parallel_sort.h>

using std::swap;
using std::vector;

int partition(vector<int>& vec, int low, int high) {
  int pivot = vec[high];
  int i = low - 1;
  for (int j = low; j < high; j++) {
    if (vec[j] <= pivot) {
      i++;
      swap(vec[i], vec[j]);
    }
  }
  swap(vec[i + 1], vec[high]);
  return i + 1;
}

void quickSort(vector<int>& vec, int low, int high) {
  if (low < high) {
    int pivot = partition(vec, low, high);
    quickSort(vec, low, pivot - 1);
    quickSort(vec, pivot + 1, high);
  }
}

const int cutoff = 50000;

void quickSortTBB(vector<int>& vec, int low, int high) {
  if (low < high) {
    int pivot = partition(vec, low, high);
    int size = high - low + 1;
    if (size > cutoff) {
      tbb::parallel_invoke(
        [&] { quickSortTBB(vec, low, pivot - 1); },
        [&] { quickSortTBB(vec, pivot + 1, high); }
      );
    } else {
      quickSort(vec, low, pivot - 1);
      quickSort(vec, pivot + 1, high);
    }
  }
}

void quickSortParallel(vector<int>& vec, int low, int high) {
  if (low < high) {
    int pivot = partition(vec, low, high);
    int size = high - low + 1;
    if (size > cutoff) {
      std::thread t_low([&vec, low, pivot]() { quickSortParallel(vec, low, pivot - 1); });
      std::thread t_high([&vec, high, pivot]() { quickSortParallel(vec, pivot + 1, high); });
      t_low.join();
      t_high.join();
    } else {
      quickSort(vec, low, pivot - 1);
      quickSort(vec, pivot + 1, high);
    }
  }
}

int main(int argc, char* argv[]) {
  int n = 1000000;
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--num" && i + 1 < argc) {
      n = std::stoi(argv[i + 1]);
      break;
    }
  }

  const int iterations = 10;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dis(0, 1000000);

  vector<int> data(n);
  for (int i = 0; i < n; i++) data[i] = dis(gen);

  double sum_seq = 0;
  for (int run = 0; run < iterations; run++) {
    vector<int> copy = data;
    auto t1 = std::chrono::high_resolution_clock::now();
    quickSort(copy, 0, n - 1);
    auto t2 = std::chrono::high_resolution_clock::now();
    sum_seq += std::chrono::duration<double, std::milli>(t2 - t1).count();
    if (!std::is_sorted(copy.begin(), copy.end())) {
      std::cout << "Sort failed.\n";
      return 1;
    }
  }
  double avg_seq = sum_seq / iterations;

  double sum_thread = 0;
  for (int run = 0; run < iterations; run++) {
    vector<int> copy = data;
    auto t1 = std::chrono::high_resolution_clock::now();
    quickSortParallel(copy, 0, n - 1);
    auto t2 = std::chrono::high_resolution_clock::now();
    sum_thread += std::chrono::duration<double, std::milli>(t2 - t1).count();
    if (!std::is_sorted(copy.begin(), copy.end())) {
      std::cout << "Sort failed.\n";
      return 1;
    }
  }
  double avg_thread = sum_thread / iterations;

  double sum_tbb = 0;
  for (int run = 0; run < iterations; run++) {
    vector<int> copy = data;
    auto t1 = std::chrono::high_resolution_clock::now();
    quickSortTBB(copy, 0, n - 1);
    auto t2 = std::chrono::high_resolution_clock::now();
    sum_tbb += std::chrono::duration<double, std::milli>(t2 - t1).count();
    if (!std::is_sorted(copy.begin(), copy.end())) {
      std::cout << "Sort failed.\n";
      return 1;
    }
  }
  double avg_tbb = sum_tbb / iterations;

  double sum_tbb_sort = 0;
  for (int run = 0; run < iterations; run++) {
    vector<int> copy = data;
    auto t1 = std::chrono::high_resolution_clock::now();
    tbb::parallel_sort(copy.begin(), copy.end());
    auto t2 = std::chrono::high_resolution_clock::now();
    sum_tbb_sort += std::chrono::duration<double, std::milli>(t2 - t1).count();
    if (!std::is_sorted(copy.begin(), copy.end())) {
      std::cout << "Sort failed.\n";
      return 1;
    }
  }
  double avg_tbb_sort = sum_tbb_sort / iterations;

  std::cout << "Sequential:        " << avg_seq << " ms\n";
  std::cout << "Thread parallel:   " << avg_thread << " ms\n";
  std::cout << "TBB quicksort:     " << avg_tbb << " ms\n";
  std::cout << "TBB parallel_sort: " << avg_tbb_sort << " ms\n";
  return 0;
}
