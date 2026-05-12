#include <vector>
#include <chrono>
#include <iostream> 
#include <thread>


// Structure to operate on
struct pt{
    float x; 
    float y; 
};

// perform u.x = v.x + u.x; u.y = v.y + u.y
// using T threads, with interleaving data accesses 
void sq_1(std::vector<struct pt> &u, std::vector<struct pt> &v, int T){
    auto f = [&u, &v, &T](int i){
        for(int n = i; n < v.size(); n+=T){
            u[n].x = v[n].x + u[n].x;
            u[n].y = v[n].y + u[n].y; 
        };
    };
    
    auto threads = std::vector<std::thread>(T); 
    for(int i = 0; i < T; i++){
        threads[i] = std::thread(f, i); 
    } 
    //Wait for all threads to complete 
    for(auto &t : threads){
        t.join(); 
    }
}

// perform u.x = v.x + u.x; u.y = v.y + u.y
// using T threads. 
// Interleave data accesses, but operate on 8 element blocks
void sq_2(std::vector<struct pt> &u, std::vector<struct pt> &v, int T){
    const int BLOCK = 8; 
    auto f = [&u, &v, &T](int i){
        for(int b = i; b < (v.size() / BLOCK) + 1; b+=T){
            for(int n = 0; (n < BLOCK); n++){
                int idx = BLOCK * b + n; 
                if(idx >= v.size()) break; 
                u[idx].x = v[idx].x + u[idx].x;
                u[idx].y = v[idx].y + u[idx].y; 
            };
        }
        
    };
    
    auto threads = std::vector<std::thread>(T); 
    for(int i = 0; i < T; i++){
        threads[i] = std::thread(f, i); 
    } 
    //Wait for all threads to complete 
    for(auto &t : threads){
        t.join(); 
    }
}

// perform u.x = v.x + u.x; u.y = v.y + u.y
// using T threads. 
// Interleave data accesses, but operate on 16 element blocks
void sq_3(std::vector<struct pt> &u, std::vector<struct pt> &v, int T){
    const int BLOCK = 16; 
    auto f = [&u, &v, &T](int i){
        for(int b = i; b < (v.size() / BLOCK) + 1; b+=T){
            for(int n = 0; (n < BLOCK); n++){
                int idx = BLOCK * b + n; 
                if(idx >= v.size()) break; 
                u[idx].x = v[idx].x + u[idx].x;
                u[idx].y = v[idx].y + u[idx].y; 
            };
        }
        
    };
    
    auto threads = std::vector<std::thread>(T); 
    for(int i = 0; i < T; i++){
        threads[i] = std::thread(f, i); 
    } 
    //Wait for all threads to complete 
    for(auto &t : threads){
        t.join(); 
    }
}

// perform u.x = v.x + u.x; u.y = v.y + u.y
// using T threads. 
// Interleave data accesses, but operate on 32 element blocks
void sq_4(std::vector<struct pt> &u, std::vector<struct pt> &v, int T){
    const int BLOCK = 32; 
    auto f = [&u, &v, &T](int i){
        for(int b = i; b < (v.size() / BLOCK) + 1; b+=T){
            for(int n = 0; (n < BLOCK); n++){
                int idx = BLOCK * b + n; 
                if(idx >= v.size()) break; 
                u[idx].x = v[idx].x + u[idx].x;
                u[idx].y = v[idx].y + u[idx].y; 
            };
        }
        
    };
    
    auto threads = std::vector<std::thread>(T); 
    for(int i = 0; i < T; i++){
        threads[i] = std::thread(f, i); 
    } 
    //Wait for all threads to complete 
    for(auto &t : threads){
        t.join(); 
    }
}

// perform u.x = v.x + u.x; u.y = v.y + u.y
// using T threads. 
// Interleave data accesses, but operate on 7 element blocks
void sq_5(std::vector<struct pt> &u, std::vector<struct pt> &v, int T){
    const int BLOCK = 7; 
    auto f = [&u, &v, &T](int i){
        for(int b = i; b < (v.size() / BLOCK) + 1; b+=T){
            for(int n = 0; (n < BLOCK); n++){
                int idx = BLOCK * b + n; 
                if(idx >= v.size()) break; 
                u[idx].x = v[idx].x + u[idx].x;
                u[idx].y = v[idx].y + u[idx].y; 
            };
        }
        
    };
    
    auto threads = std::vector<std::thread>(T); 
    for(int i = 0; i < T; i++){
        threads[i] = std::thread(f, i); 
    } 
    //Wait for all threads to complete 
    for(auto &t : threads){
        t.join(); 
    }
}

// perform u.x = v.x + u.x; u.y = v.y + u.y
// using T threads. 
// Have each thread operate on a continous section of the data
void sq_6(std::vector<struct pt> &u, std::vector<struct pt> &v, int T){
    auto f = [&u, &v, &T](int s, int e){
        for(int idx = s; idx < e; idx++){
            u[idx].x = v[idx].x + u[idx].x;
            u[idx].y = v[idx].y + u[idx].y; 

        }
    };
    
    int CHUNK_SIZE = v.size() / T; 
    auto threads = std::vector<std::thread>(T); 
    int start; 
    int end;

    for(int i = 0; i < T; i++){
        int start = i * CHUNK_SIZE; 
        int end = start + CHUNK_SIZE;
        threads[i] = std::thread(f, start, end); 
    } 
    if(end < v.size())
        f(end, v.size());  
    //Wait for all threads to complete 
    for(auto &t : threads){
        t.join(); 
    }
}

//Demonstrate impacts of false sharing
// Accepts argument for number of threads to spawn, and test to perform
const int V_SIZE = 1000000; 
int main(int argc, char ** argv){
    int T = 1; // Number of threads
    int test = 0; 
    if(argc != 3){ 
        std::cout << "Incorrect Number of Arguments\n"; 
        return -1; 
    } 
    try{
        T = std::stoi(argv[1]); 
        test = std::stoi(argv[2]);
    }catch(...){
        std::cout << "Enter a Valid Number\n"; 
        return -1; 
    }

    auto u = std::vector<struct pt>(V_SIZE);
    auto v = std::vector<struct pt>(V_SIZE); 
    // Populate data 
    for(int i = 0; i < V_SIZE; i++){
        u[i].x = i * 1.1; 
        u[i].y = i * 1.1; 
        v[i].x = i * 0.9; 
        v[i].y = i * 0.9; 
    }
    
    // Perform specified benchmark
    const int RUNS = 5;
    switch(test){
        case 1:{
            const auto start = std::chrono::steady_clock::now();
            for(int i = 0; i < RUNS; i++){sq_1(u, v, T);}
            const auto finish = std::chrono::steady_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start) << "\n"; 
            break;
        }
        case 2:{
            const auto start = std::chrono::steady_clock::now();
            for(int i = 0; i < RUNS; i++){sq_2(u, v, T);} 
            const auto finish = std::chrono::steady_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start) << "\n"; 
            break;
        }
        case 3:{
            const auto start = std::chrono::steady_clock::now();
            for(int i = 0; i < RUNS; i++){sq_3(u, v, T);} 
            const auto finish = std::chrono::steady_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start) << "\n"; 
            break;
        }
        case 4:{
            const auto start = std::chrono::steady_clock::now();
            for(int i = 0; i < RUNS; i++){sq_4(u, v, T);} 
            const auto finish = std::chrono::steady_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start) << "\n"; 
            break;
        }
        case 5:{
            const auto start = std::chrono::steady_clock::now();
            for(int i = 0; i < RUNS; i++){sq_5(u, v, T);} 
            const auto finish = std::chrono::steady_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start) << "\n"; 
            break;
        }
        case 6:{
            const auto start = std::chrono::steady_clock::now();
            for(int i = 0; i < RUNS; i++){sq_6(u, v, T);} 
            const auto finish = std::chrono::steady_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start) << "\n"; 
            break;
        }

    }




    // Sum elements of the vector. 
    // Divide work across provided number of threads
}
