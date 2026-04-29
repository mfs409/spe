# Optimizing For Specific Workflows and Profile Guided Optimization


## Part One: Optimizing for Specific Workflows


So far in this course, we have discussed the fundamentals of many different optimization techniques. In this section on optimizing for specific workflows, we will examine ways in which we can apply these techniques to specific workflows.


### What is a Workflow?


In order to optimize for a specific workflow, we must first understand what we mean when we refer to a workflow. Generally speaking, a workflow is a repetitive computational task, often (but not always) automated. Examples of workflows include reading content from a CSV and writing it to another storage location, performing a string of calculations on input data, or categorizing and storing data.


### Suggestions for Optimizing Workflows
<details>
<summary>How might optimizing for a specific workflow be different from general optimizations?</summary>
<br>
When we view optimizations through a general lens, we can only make general suggestions based on the most universal good programming practices. When we optimize for a specific workflow, we can be guided by the specific concerns of our program; for instance, we might be most concerned about optimal space usage in one program, and runtime in another. Additionally, when we optimize for a specific workflow, we will likely have some information on the architecture that the program will run on, giving us more information on how best to take advantage of our architecture. Having these ideas allows us to make more extreme optimizations; in many cases, we decide not to optimize for one case, because it makes another case worse, but when we are able to make assumptions, we can be more ambitious in our optimizations.
</details>
<br>
<details>
<summary>How do we take advantage of knowing our architecture?</summary>
<br>
Knowing the architecture that your workflows will be running on can influence how you optimize for specific workflows. This is due to different systems processing operations and data in different ways, and these differences can be exploited to take full advantage of the hardware. For instance, knowing the SIMD extensions of your architecture can open the door to massive hardware-level parallelism and understanding GPU support can massively increase your throughput. Additionally, leveraging low-level allocator properties can help ensure that data is aligned and doesn't cross memory boundaries (which can be costly).
</details>
<br>
<details>
<summary>How do we take advantage of knowing time and space requirements?</summary>
<br>
Understanding the time and space requirements of your workflow can be a critical area of optimization because it can allow you to best utilize the resources at your disposal. This is particularly relevant if you understand your constraints, since if you know you have a lot of memory but strict time constraints, you can use memoization or pre-allocation to gain faster execution. If accuracy is not an issue, you can greatly optimize your workflow by using faster, but less precise methods (i.e. TCP vs. UDP).
</details>
<br>
<details>
<summary>How do we take advantage of having predictions about our input?</summary>
<br>
Ordinarily when we are programming, we cannot make any assumptions regarding how the program will be used, the intent of the users, or the format of the input. However, when you are working on a workflow, you usually have a good idea of the specifications of that workflow. For instance, we may have a good idea of the scale of the input, the size of values, the distribution of the input, the necessary precision for the input, or the priority of inputs. Knowing these things, we can make informed optimization decisions. To examine a few cases, we might consider parallelism or concurrency. Depending on the exact scale of the input, doing something like multithreading might add to the runtime by causing threads to wait for each other, in the case of a relatively small input, however, for a larger input, that overhead may be outweighed by the benefit of performing multiple tasks at the same time. As another example, knowing the priority of input values opens up a few opportunities to potentially optimize, from implementing a prioritized data structure, to scheduling either individual tasks in our workflow, or the workflow itself. Similarly, knowing the distribution of your data may also allow you to make a more efficient data structure choice.
</details>


### Practical Examples


<ol>
<li> <b>Real-Time Graphics and Game Physics</b>
<br>
Imagine you are developing a physics engine for a video game.
You need to calculate the physical interactions and collisions for 10,000 in-game particles while maintaining a good fps. Performing a brute force check for every possible pair of particles will kill your performance and would be too slow for a good user experience. How can you design a system that ensures that the game has a working physics engine without sacrificing the UX?
<br><br>
<details>
<summary>With this question, start by considering . . .</summary>
<ul>
<li>What level of precision/accuracy in the game physics can we accept before it harms the user experience?</li>
<li>Taking advantage of spatial partitioning methods like the Barnes-Hut algorithm and octrees</li>
<li>Try using focusing on what is in the field-of-view of the user (culling)</li>
<li>Do all particles need complex physics? Can you get away with some bugs (like tunneling)?</li>
</ul>
</details>
</li>
<br>
<li> <b>Data Processing and Analysis</b>
<br>
Let’s say you are responsible for the computing in a life sciences lab. You have to design a system with the following knowledge:
Every day, your team produces thousands of data points, which will need to be written to a database, and included in a previously performed sequence of calculations. Given that the data will need to be fully entered, and all calculations updated prior to more data being collected, how might you design this system? What things would you need to think about?
<br><br>
<details>
<summary>With this question, start by considering . . .</summary>
<ul>
<li>Volume of database reads and writes</li>
<li>Available space compared to size of computations (can we cache some amount of our results?)</li>
<li>Efficiency of prefetching and caching</li>
<li>Buffering writes to maximize write efficiency</li>
<li>Importance of data integrity compared to necessary speed of computation</li>
</ul>
</details>
</li>
<br>
<li><b>Responsive User App</b>
<br>
You are developing a mobile photo app.
When a user takes a photo, several things need to happen, like uploading to the cloud, syncing metadata, creating ML tags, compression, etc.
However, these all take time, and the user will also want to be able to see the photo in their gallery immediately.
How can you design this system so that the user can see their photo immediately, while also accomplishing all other tasks in a timely manner?
<br><br>
<details>
<summary>With this question, start by considering . . .</summary>
<ul>
<li>How should we schedule tasks? What is more important for the user experience?</li>
<li>Can multithreading help us out? Are there any dependencies between these tasks?</li>
<li>How much does the OS allow us to bundle into the overall event?</li>
<li>Phone hardware constraints</li>
</ul>
</details>
</li>
</ol>


### Intersection of Human and Compiler Optimizations

The compiler makes many optimizations on its own, without any direction needed by a person. However, it won't go any farther than low-level optimizations, as the compiler cannot risk changing the behavior of your program. Even then, the compiler may not always be able to optimize its low-level specifics to the fullest extent. **This is where the developer can come in and help the compiler out by giving it hints as to how to optimize:**

- **Branch hinting:** The compiler can perform branch prediction to precompute likely branches and optimize code layout, but it can only guess so well. The developer can let the compiler know that a certain branch is very likely to be taken most of the time, so it can optimize around that. 

    Say you have an if-else on a hot path, and you know it is going to go down the same branch 99% of the time. Tell the compiler to expect that branch:
    ```cpp
    if (__builtin_expect(conditional, 1)) {
        //likely branch
    }
    else {
        //unlikely branch
    }
    ```
    *Note that this example is specifically for GCC/Clang*

- **Caching likely-to-be-used data:** You may know that certain data is likely going to be used before it is actually referenced. Prefetch this data ahead of time so it can be retrieved in the background and can already be ready by the time it is needed.

    ```cpp
    for (int i = 0; i < n; i++) {
        //prefetch 8 elements ahead
        __builtin_prefetch(&data[i + 8], 0, 1);
        process(data[i]);
    }
    ```
    *Note that this example is specifically for GCC/Clang*

    Similarly, you can mark certain data as not likely to be used, telling the compiler NOT to waste time and space caching it.

- ***And plenty more!***
    - Inlining functions
    - Loop unrolling
    - SIMD/Vectorization
    - Data alignment hints
    - Marking hot vs cold code
    - Array-of-structs vs struct-of-arrays

**Note that the optimizations you can make and how you make them depend on your compiler and CPU architecture. It is also important to note that in many cases, a lot of these optimizations will very likely already be handled automatically by your compiler.**

## Part Two: Profile-Guided Optimization

There are many ways you can use your knowledge of your project's purpose and context to better optimize it. Sometimes that may be larger design or algorithmic changes, and other times it may be low-level optimizations.

However, optimization isn't limited to the developer's explicit decisions. As noted before, compilers make many optimizations automatically, and modern compilers can be *very* good at optimizing on their own, not needing any advice from the developer.

With that said, these optimizations are often based purely on guesses, without any knowledge of your program's actual purpose. While those static, default heuristics it uses can be very good, **they must be safe guesses**, and the compiler can only take it so far without actually knowing anything about your program.

**But what if the compiler could *observe* your program, and use that to optimize even better?**

Enter **Profile-Guided Optimization (PGO)**. With PGO, you can tell your compiler to observe your program actually running, learn about how the program is used, and optimize it even more effectively than before.

### What is profiling?
To profile a program means to observe and measure runtime behavior, collecting empirical data while the program is actually running. This allows us (and the compiler) to determine performance bottlenecks and usage experimentally, rather than by just guessing. By using this data **(a profile)**, the compiler can actually see the decisions and hot/cold paths the program took when actually being used.

*"A profiler is a tool used to help you analyze the performance of your applications to improve poorly performing code." (Microsoft, 2025)*

A profiler (the tool being used to create the profile) will observe what happens in the program and collect data about things like:
- CPU time, memory usage, cache behavior, I/O
- It will track things like function calls, branch mispredictions, heap allocation, execution time, cache hits/misses, network calls, disk reads/writes

**So how does it do this?**

Different profilers use different methods to create a profile:

- Some profilers may periodically record snapshots of your program’s state, like its call stack at some given moment. Say, perhaps, every 1ms. These are called **sampling profilers**
    - These profilers introduce minimal overhead to your program, making them very safe. However, as a result, they can't always create a thorough enough profile to optimize as much as is possible.
- Others may insert extra code into your program to measure it. For example, counters and flags to keep track of how many times certain code blocks are executed. These are called **instrumentation profilers**
    - A downside is that this can add overhead to your program when creating the profile. While this is often negligible, and the overhead is only needed when creating the profile (not after re-optimizing with the profile), in some performance-critical applications, this minor performance decrease could alter your program's behavior, if it relies on very specific timing. However, this is unlikely in the majority of cases.
    - On the flip side, instrumentation creates a more thorough profile, often leading to even better performance improvements.
- Hybrid profilers also exist

**Once the program finishes, the profiler will record its findings into a file, being the profile**

### The Steps to PGO
<ol>
<li><h4>Choosing a Profiling Tool and Performance Interests</h4>
<p>Many languages have their own, built in performance profilers, so your initial choice will be based on your language. However, for languages with many profiler options, you may be able to choose a profiler geared towards a specific performance interest (for instance, some may care a little more about time efficiency, while others are more focused on memory usage).</p>
</li>
<li><h4>Running the Profiler</h4>
<p>When you run your code with a profiler, you will want to be sure that the program is running in a manner as close to its real use case as possible. The profiler will highlight things to optimize based on the given run of the program, so if you have a branch that will be used 99 times out of 100 in production, but you run your profiler with a 1 in 100 case, where a different branch is taken, your profile will not be providing accurate usage information to your compiler, and compiling with this profile may not improve, and may even hurt your performance.</p>
</li>
<li><h4>Merging and Formatting the Profiles</h4>
<p>For many profiling tools, you can run the profiler many times before compiling with the profile. You will likely want to do this, in order to have your PGO account for any use case, or just to have the most detailed information going into the compiler. Some profiling tools will append to the same profile file for every program run, but others will generate a new profile file each run, and will require you to either attach each one individually before compiling, or reformat the profiles together before compiling.</p>
</li>
<li><h4>Compiling with the Profile</h4>
<p>Once you are ready with profile files, in most cases, you can pass these to your compiler when you compile your program, and this will guide your compiler in making optimization decisions.</p>
</li>
</ol>



### Benefits of PGO
<ol>
<li><h4>Performance Without Code Changes</h4>
<p>Oftentimes manual optimization comes at the cost of making one's code harder to read, which hurts maintainability. Profile-guided optimization works around this hurdle since the compiler does all the workflow-specific optimizations at compile time. This is particularly the case when it comes to inlining, since doing so manually will make your code less DRY and thereby less maintainable.</p>
</li>
<li><h4>Optimal Inlining</h4>
<p>In a non-PGO context, compiler inlining is often done with general heuristics that may or may not be best for your workflow. They can also lead to over-lining or under-inlining, both of which can hurt performance. With profile-guided optimization, compilers can be much more strategic about its inlining and prioritize the hot functions (even virtual ones that are often skipped).</p>
</li>
<li><h4>Improved Cache Efficiency</h4>
<p>With profiles, compilers have a better understanding of what the hot and cold paths are in your code. It can use this info to pack the hottest instructions together and maximize the cache hits in the CPU's L1 cache and minimize misses.</p>
</li>
<li><h4>Link-Time Optimization</h4>
<p>PGO can become even more effective with link-time optimization (LTO) since the compiler can optimize across different source files. This allows profiles to made based on how the entire application processes your workflow, and the compiler can decide on the best optimizations accordingly. The downside to this is longer compilation times, but for critical software this is an essential step towards higher performance.</p>
</li>
</ol>


### PGO Effectiveness & Statistics
Results of PGO can vary:
- Sometimes speedups can be great, reaching 10%, 20%, or even occasionaly more than that
- Other times, speedups are negligible or even nonexistant, compared to just basic optimization without profiling
- There are even cases where PGO can hurt performance, though this is rare. Also, even if performance isn't hurt, if the workflow is simple, there's a chance PGO might make your binary size much bigger

How effective PGO will be can depend on multiple factors:
- **The nature of your code.** Some programs naturally lend themselves to PGO, while others just aren't made in a way to benefit from it. For example, branch-heavy code tends to work better with PGO.
- **The compiler (and language).** Compilers do PGO in different ways, and some may just be better at certain optimizations based on a profile. Also, of course, the language you are writing in has a large impact on performance.
- **The quality of your profile.** A profile is only data, so that data must be accurate and relevant to see good results. A profile is only good if it accurately reflects the specific usage of your program you want it to. If you run and use your program one in a different way than you want it to be optimized for, the profile's data will not be accurate to what you desire. A bad profile can also just happen by chance, which is why it may often be a good idea to make multiple profiles and combine them.

**Some reports from various sources on the effects of PGO:**
- [A 2025 survey](https://arxiv.org/html/2507.16649v1) reports speedups can sometimes reach **5-30%** on real applications with PGO
- [Real-world benchmarks](https://www.emergentmind.com/topics/profile-guided-optimizations-in-gcc) of PGO speedups with GCC on C/C++:
    - **5-10%** with instrumentation
    - **3-7%** with hardware sampling
- [Google reports](https://dl.acm.org/doi/abs/10.1145/2854038.2854044) that its AutoFDO tool that it uses for many of its binaries provides a geomean of **10.5%** speedup.
- [Android dev docs for game developers](https://developer.android.com/games/agde/pgo-overview) give a conservative PGO speedup estimate of **~5%** on key threads.


### Code Demo


To see how profile-guided optimization can improve your program’s performance, let’s take a look at this example:


```cpp
#include <vector>
#include <iostream>

using namespace std;


class Worker {
public:
    virtual long long doWork(int i) = 0;
};

class FastWorker : public Worker {
public:
    long long doWork(int i) override {
        return i + 1;
    }
};

class SlowWorker : public Worker {
public:
    long long doWork(int i) override {
        return i * 2;
    }
};


int main() {
    int N = 100000000;

    vector<Worker*> workers(N);
    FastWorker fast;
    SlowWorker slow;

    for (int i = 0; i < N; i++) {
        if (i % 100 == 0) {
            workers[i] = &fast;
        }
        else {
            workers[i] = &slow;
        }
    }

    long long res = 0;
    for (int i = 0; i < N; i++) {
        res += workers[i]->doWork(i);
    }

    cout << res << endl;
    return 0;
}
```


Virtual functions are treated like black boxes by compilers since they can be overwritten, which prevents the functions from being inlined and requires a vtable lookup at every iteration of the `doWork` for loop. This results in a slower execution time, as can be seen when we compile and run the code as is:


```bash
$ g++ -O3 demo.cpp -o test-demo
$ ./test-demo
9949999951000000

real    0m0.218s
user    0m0.181s
sys     0m0.036s
```


But looking at the code, we can see that the FastWorker is called 99/100 times compared to the slow worker, so the absence of inlining is killing the potential performance gains. This is where PGO can be handy. Using the instrumentation flags available with GCC, we can have the compiler optimize for our static workflow.


```bash
$ g++ -O3 -fprofile-generate demo.cpp -o test-demo
$ ./test-demo
9949999951000000
$ g++ -O3 -fprofile-use demo.cpp -o test-demo
$ time ./test-demo
9949999951000000

real    0m0.180s
user    0m0.142s
sys     0m0.037s
```


We can see that we got around a 20% speedup with no code changes! By looking into the assembly code generated by both:




**Without PGO:**
```asm
...
.L7:
    movq    (%r12,%rbx,8), %rdi   # load the worker object
    movl    %ebx, %esi            
    movq    (%rdi), %rax          # follow the pointer to the vtable
.LEHB1:
    call    *(%rax)               # indirect call to the address
    addq    $1, %rbx              # increment i
    addq    %rax, %rbp
    cmpq    $100000000, %rbx      # check if i exceeds N
    jne    .L7
...
```
As we can see, there is an indirect call to the address in the `rax` register. The CPU can't pipeline this since it can't assume what the address is, which hurts the branch prediction potential since this happens throughout the for loop.


**With PGO:**
```asm
...
.L23:
    addq    %rbx, %rbp              # FastWorker addition is inlined! (i+1)
    addq    $1, %rbx                # increment i                    
    cmpq    $100000001, %rbx        # check if i exceeds N
    je    .L5
.L6
    movq    -8(%r12,%rbx,8), %rdi             # load the worker object
    movq    (%rdi), %rax                      # get the vtable
    movq    (%rax), %rax                      # get the function pointer
    cmpq    $_ZN10FastWorker6doWorkEi, %rax   # check if it is FastWorker
    je    .L23                                # jump to .L23 if so
    call    *%rax                             # indirect call if not
...
```
After profile-guided optimization, the compiler was able to recognize that the FastWorker function is called way more often than the SlowWorker, which led it to inline the function. This will greatly improve performance since the inlined function is way better for the branch predictor.


### Practical Example


<b>Real-Time Graphics and Game Physics</b>
<br>
Let's return to this previously mentioned example, and consider: how might PGO benefit this program?
<br><br>
<details>
<summary>With this question, start by considering . . .</summary>
<ul>
<li>What code will be hot/cold for a game?</li>
<li>What is the purpose of a game engine? Is it specialized, or general-purpose?</li>
<li>How much code are you writing, and how much is already written behind the scenes?</li>
</ul>

In a video game, a lot of the computation usually goes to physics and/or rendering. These calculations tend to be relatively simple logic repeated many times. As a result, PGO can help the compiler realize it should prioritize optimizing this code, like more rigorously unrolling the loops they run in, improving cache efficiency, inlining certain code, etc. On the other hand, it won't focus much on colder code that isn't very intensive or isn't run often, such as menu logic or processing player input. This is called **pessimizing** that code that doesn't need as much optimization.

The reason game engines can benefit so much from PGO (and by extension any sort of external library or third-party code) is because they are made to be very general purpose, so that they can support all kinds of games to be made. As a result, your specific game may not use a lot of the code in the background that it doesn't need. Normally, the compiler may not know how to deal with this, but with PGO, the compiler can understand your code's semantics better and optimize for your game specifically, like by inlining hot functions, not worrying about the library code that isn't used. This way, you don't need to worry as much about or try to modify that library code that you didn't write, just to get a bit more juice out of your program.


</details>


### Limitations of PGO


Now that we have described what PGO is, how to use it, and the potential benefits of it, we should examine cases in which we may not want to use PGO, or cases in which PGO may not supply a significant benefit.


<ol>
<li><h4>Limitations on Potential Changes</h4>
<p>Compilers can only make optimizations that do not fundamentally change the behavior of a program. Techniques that may significantly speed up a program, such as changes in the data structure or layout, the algorithm, or certain ordering optimizations will not be considered the be the same behavior, and thus, cannot be done by a compiler, even if a profile seems to indicate that that change would be helpful. These optimizations must be performed by a human programmer.</p>
</li>
<li><h4>Differing Use Cases</h4>
<p>Because a profiler analyzes actual runs of a program, it will optimize to whatever use cases it sees in the profiles you provide. If we do not adequately cover all the use cases we want to optimize for, the compiler may over fit to a particular use case, hurting performance in other cases. While, as we discussed in part one, it may be okay to fit to a particular use case, if you have a lot of information regarding how your program will be used, that will not always be the case, so we need to carefully evaluate the data we are giving our compiler.</p>
</li>
<li><h4>Static Changes</h4>
<p>PGO should be done on unchanged programs; a profile file from a previous iteration of your program will not generally provide helpful information to the compiler on how to optimize this newer version. Therefore, PGO should ideally be done once, after all human development on a program is complete.</p>
</li>
</ol>


### PGO In the Real World

PGO isn't just an idea; it is used in the real world in many places for better production code performance.

- Many production application binaries are optimized with PGO before release, such as Google Chrome or CPython’s release binaries
- Java's JVM performs PGO during runtime and optimizes the currently running code via its JIT (Just-In-Time) compiler. Microsoft’s .NET platform runtime does the same. This is known sometimes as **Dynamic PGO.**
- V8, the high-performance JavaScript engine Chrome and Node.js use, similarly does PGO-style optimization during runtime
- Several widely-used programming languages used today have support for PGO, such as:
    - C++
    - GoLang
    - Rust
    - *And more!*


### Conclusion


Both optimization for a specific workflow and profile guided optimization can introduce significant speedups and better use of resources. On top of other optimization techniques we have discussed in this class, both these facets of optimization offer improvements really specific to your program and use case; workflow optimization through allowing you to make assumptions, and PGO through analyzing specific program runs and use cases. When optimizing any program, you should consider employing both approaches!


### References


https://dev.to/asyraf/how-to-add-dropdown-in-markdown-o78
<br>
https://learn.microsoft.com/en-us/visualstudio/profiling/what-is-a-profiler?view=visualstudio
<br>
https://www.ibm.com/think/topics/workflow
<br>
https://learn.microsoft.com/en-us/visualstudio/profiling/choose-performance-tool?view=visualstudio
<br>
https://www.geeksforgeeks.org/system-design/types-of-cache/
<br>
https://go.dev/doc/pgo
<br>
https://arxiv.org/html/2507.16649v1
<br>
https://llvm.org/docs/HowToBuildWithPGO.html
<br>
https://developer.android.com/games/agde/pgo-overview
<br>
https://www.emergentmind.com/topics/profile-guided-optimizations-in-gcc
<br>
https://en.wikipedia.org/wiki/Profile-guided_optimization
<br>
https://dl.acm.org/doi/abs/10.1145/2854038.2854044

