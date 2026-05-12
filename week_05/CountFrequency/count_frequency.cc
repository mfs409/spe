#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

using std::vector;

const int NUM_THREADS = 16;


void count_sequential(const vector<int>& data, vector<int>& count) {
  for (int x : data) {
    count[x]++;
  }
}


void count_naive_parallel(const vector<int>& data, vector<int>& count) {
  int n = data.size();
  vector<std::thread> threads;
  for (int t=0; t < NUM_THREADS; t++) {
    //splitting the data into NUM_THREADS parts
    int start = (t*n) / NUM_THREADS;
    int end = ((t+1) * n) / NUM_THREADS;

    //creating a thread for each part of the data and counting the frequency of each value in the part  
    threads.push_back(std::thread([&, start, end]() {
      for (int i=start; i<end; i++) {
        count[data[i]]++;  
      }
    }));
  }
  //joining all the threads
  for (int t = 0; t < NUM_THREADS; t++) {
    threads[t].join();
  }
}



void count_atomic(const vector<int>& data, vector<std::atomic<int>>& count) {
  int n=data.size();
  vector<std::thread> threads;
  for (int t = 0; t < NUM_THREADS; t++) {
    //splitting the data into NUM_THREADS parts
    int start = (t*n) / NUM_THREADS;
    int end = ((t+1) * n) / NUM_THREADS;

    //creating a thread for each part of the data and counting the frequency of each value in the part  
    threads.push_back(std::thread([&, start, end]() {
      for (int i = start; i<end; i++) {
        count[data[i]]++; 
      }
    }));
  }

  //joining all the threads
  for (int t = 0; t < NUM_THREADS; t++) {
    threads[t].join();
  }
}


void count_thread_local(const vector<int>& data, vector<int>& count) {
  int n = data.size();
  int k = count.size();
  //creating a vector of vectors to store the frequency of each value in each thread
  vector<vector<int>> local(NUM_THREADS, vector<int>(k, 0));
  vector<std::thread> threads;

  for (int t=0; t < NUM_THREADS; t++) {
    //splitting the data into NUM_THREADS parts
    int start = (t*n) / NUM_THREADS;
    int end = ((t+1) * n) / NUM_THREADS;

    //creating a thread for each part of the data and counting the frequency of each value in the part  
    threads.push_back(std::thread([&, t, start, end]() {
      for (int i = start; i < end; i++) {
        local[t][data[i]]++;
      }
    }));
  }

  //joining all the threads
  for (int t = 0; t < NUM_THREADS; t++) {
    threads[t].join();
  }
  // merge thread-local counts into final count
  for (int b = 0; b < k; b++) {
    count[b] = 0;
    for (int t = 0; t < NUM_THREADS; t++) {
      count[b] += local[t][b];
    }
  }
}

int main() {
  const int n = 10000000;
  const int k = 128;


  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dis(0, k - 1);
  vector<int> data(n);
  for (int i = 0; i < n; i++) {
    data[i] = dis(gen);
  }

  vector<int> count(k, 0);
  auto t1 = std::chrono::high_resolution_clock::now();
  count_sequential(data, count);
  auto t2 = std::chrono::high_resolution_clock::now();
  double ms_seq = std::chrono::duration<double, std::milli>(t2 - t1).count();

  vector<std::atomic<int>> count_at(k);
  for (int b = 0; b < k; b++) {
    count_at[b].store(0);
  }

  t1 = std::chrono::high_resolution_clock::now();
  count_atomic(data, count_at);
  t2 = std::chrono::high_resolution_clock::now();
  double ms_atomic = std::chrono::duration<double, std::milli>(t2 - t1).count();

  if (!std::equal(count_at.begin(), count_at.end(), count.begin())) {
    std::cout << "failed (result != sequential).\n";
    return 1;
  }

  vector<int> count_tl(k, 0);
  t1 = std::chrono::high_resolution_clock::now();
  count_thread_local(data, count_tl);
  t2 = std::chrono::high_resolution_clock::now();
  double ms_tl = std::chrono::duration<double, std::milli>(t2 - t1).count();
  if (count_tl != count) {
    std::cout << "failed (result != sequential).\n";
    return 1;
  }

  std::cout << "Sequential:    " << ms_seq << " ms\n";
  std::cout << "Atomic:        " << ms_atomic << " ms\n";
  std::cout << "Thread-local:  " << ms_tl << " ms\n";
  return 0;
}
