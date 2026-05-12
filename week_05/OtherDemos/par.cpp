// Code based on execution policy page on cppreference.com
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <execution>

void measure(auto policy, std::vector<std::uint64_t> v){
    const auto start = std::chrono::steady_clock::now();
    std::sort(policy, v.begin(), v.end());
    const auto finish = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start) << "\n"; 
}
// Measure execution time of std::sort with different execution policies
// Program is called with number of elements to sort
int main(int argc, char ** argv){
    if(argc != 2){
        std::cout << "Incorrect Number of Arguments\n"; 
        return -1; 
    } 
    int N = 0; 
    try{
        N = std::stoi(argv[1]); 
    }catch(...){
        std::cout << "Enter a Valid Number\n"; 
        return -1; 
    }

    auto v = std::vector<uint64_t>(N);  
    auto gen = std::mt19937(std::random_device{}()); 
    std::ranges::generate(v, gen); 
    
    std::cout << "Seq     : "; 
    measure(std::execution::seq, v); 

    std::cout << "Unseq   : "; 
    measure(std::execution::unseq, v); 

    std::cout << "Parunseq: "; 
    measure(std::execution::par_unseq, v); 

    std::cout << "Par     : "; 
    measure(std::execution::par, v); 


    return 0;
}
