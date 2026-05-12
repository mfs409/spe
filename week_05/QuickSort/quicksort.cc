#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

using std::swap;
using std::vector;


int partition(vector<int>& vec, int low, int high) {
  int pivot=vec[high];
  int i = low-1;

  for (int j=low; j<high; j++) {
    if (vec[j]<=pivot) {
      i++;
      swap(vec[i], vec[j]);
    }
  }
  swap(vec[i+1], vec[high]);
  return i+1;
}


void quickSort(vector<int>& vec, int low, int high) {
  if (low<high) {
    int pivot = partition(vec, low, high);
    quickSort(vec, low, pivot-1);
    quickSort(vec, pivot+1, high);
  }
}


int cutoff=50000;

void quickSortParallel(vector<int>& vec, int low, int high) {
  if (low<high) {
    int pivot = partition(vec, low, high);

    if (high - low + 1 > cutoff) {
      std::thread t_low([&vec, low, pivot]() { quickSortParallel(vec, low, pivot-1); });
      std::thread t_high([&vec, high, pivot]() { quickSortParallel(vec, pivot+1, high); });
      t_low.join();
      t_high.join();
    } 
    else {
      quickSort(vec, low, pivot-1);
      quickSort(vec, pivot+1, high);
    }
  }
}

int main(int argc, char* argv[]) {
  int n = 1000000;  
  // if --num and number is provided, then set n to the number
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--num" && i+1 < argc) {
      n = std::stoi(argv[i+1]);
      break;
    }
  }


  int iterations=10;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dis(0, 1000000);

  vector<int> data(n);
  for (int i=0; i < n; i++){
    data[i] = dis(gen);
  }

  //running traditional quicksort for 10 iterations and taking the average time
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

  double avg_seq = sum_seq/iterations;

  //running parallel quicksort for 10 iterations and taking the average time
  double sum_par = 0;
  for (int run = 0; run < iterations; run++) {
    vector<int> copy = data;
    auto t1 = std::chrono::high_resolution_clock::now();
    quickSortParallel(copy, 0, n - 1);
    auto t2 = std::chrono::high_resolution_clock::now();
    sum_par += std::chrono::duration<double, std::milli>(t2 - t1).count();
    if (!std::is_sorted(copy.begin(), copy.end())) {
      std::cout << "Sort failed.\n";
      return 1;
    }
  }

  double avg_par = sum_par/iterations;

  std::cout << "Sequential Quicksort avg : " << avg_seq << " ms\n";
  std::cout << "Parallel Quicksort avg :   " << avg_par << " ms\n";
  return 0;
}
