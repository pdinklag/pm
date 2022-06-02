# pm &ndash; Performance Measuring for C++

This library provides an extensible high-level API for measuring performance data of C++20 applications.

## Documentation

The governing concept of pm is *phase-based* measurements using *meters*. The programmer defines phases of their application during which a selection of meters measure performance data. Consider the following simple example, where we measure the memory allocations and the required time of a rather simple computation:

```cpp
#include <pm.hpp>

// ...
pm::MemoryTimePhase compute_phase("Example");
{
    int sum = 0;
    compute_phase.start();

    size_t const bufsize = 1'000'000;
    char* buffer = new char[bufsize];
    for(size_t i = 0; i < bufsize; i++) buffer[i] = (char)i;

    compute_phase.pause();
    std::cout << "initialization done!" << std::endl;
    compute_phase.resume();

    for(size_t i = 0; i < bufsize; i++) sum += buffer[i];
    delete[] buffer;

    compute_phase.stop();
    compute_phase.data()["sum"] = sum;
}

std::cout << compute_phase.gather_data().dump(4) << std::endl;
```

In the example, we allocate one million bytes, initialize them with the truncated iota function and finally sum up the result. Around this, we wrap a `pm::MemoryTimePhase` that will measure our performance data. We *start* it right before the computation begins and *stop* it once it is done. Between the initialization and summing loops, we output a brief progress report to the standard output, which we don't want to measure. We achieve that by *pausing* the measurement right before the output and *resuming* it after. In the end, we enter the computed sum as *auxiliary data* to our phase.

We then convert the measured data to pm's JSON format using `gather_data` and print it. The output will look about like this:

```json
{
    "data": {
        "sum": -497952
    },
    "metrics": {
        "memory": {
            "alloc_bytes": 1000000,
            "alloc_num": 1,
            "closing": 0,
            "free_bytes": 1000000,
            "free_num": 1,
            "peak": 1000000
        },
        "time": 4.162696
    },
    "name": "Example"
}
```

The computation named "Example" took 4.16 milliseconds, during which there was one allocation of one million bytes that were freed again by a single free operation. The peak memory usage was one million bytes, the closing memory usage is 0, indicating no leaks. We have the computed sum as auxiliary data.

### Meters

In the example, we used two meters, which are hidden away somewhat by the convenience type `pm::MemoryTimePhase`. It is defined as:

```cpp
using MemoryTimePhase = Phase<MallocCounter, Stopwatch>;
```

We see that `MemoryTimePhase` is actually a generic `Phase` with two meters: `MallocCounter` and `Stopwatch`.

A `Phase` can therefore be considered a set of meters that also handles JSON data storage. It can have any number of meters attached; the class accepts a template parameter pack of any number of types that satisfy the `Meter` concept for JSON data storages (see `pm/concepts.hpp`). Writing and using custom meters is therefore a rather simple endeavor.

Meters are activated in the order of their position in the template parameter pack, and deactivated in reverse order. For the `MemoryTimePhase`, this means that time measurement begins last and stops first. It has been implemented this way to minimize measuring any overhead.

Meters can be used standalone, it is not necessary to attach them to a phase for them to work.

#### Stopwatch

The stopwatch is pretty much just that: it measures the timestamp at the beginning of the measurement and at the end. It uses the system's highest resolution clock, which typically delivers nanosecond precision.

#### MallocCounter

The memory allocation counter tracks memory allocations and frees reported by pm's `malloc` overrides.

**Your application must be built with memory allocation tracking enabled in order for this to work at all!**

Please see [Usage with Memory Allocation Tracking](#with-memory-allocation-tracking) for details.

### NoopPhase

The `NoopPhase` does not measure anything. In fact, all operations are implemented as no-ops.

Its single purpose is compile-time optimization for production. In the example above, let us rewrite the declaration of the phase like this:

```cpp
using Phase = std::conditional_t<is_production, pm::NoopPhase, pm::MemoryTimePhase>;
Phase compute_phase("Example");
```

Now, if you define a constant expression `is_production` that resolves to `true`, all measurements will be replaced by no-ops and should be completely optimized away by the compiler if given the corresponding optimization flags (e.g., `-O3`).

This allows you to leave all the measurement code in your source and not worry about any performance overhead in production builds.

### Phase Hierarchies

A phase can be declared the *child* of another phase, resulting in a hierarchy of phases. To avoid the need for hierarchy management, pm is designed in a way where the hierarchal relationship is defined only on the *data* of phases, representing it only in JSON. This results in a use pattern where the hierarchy is defined completely outside any computational code, and particularly *after* termination of a child phase.

Let us add sub phases to the initial example:

```cpp
pm::MemoryTimePhase compute_phase("Example");
{
    int sum = 0;
    
	pm::TimePhase iota_phase("Iota");
	pm::TimePhase sum_phase("Sum");
    
    compute_phase.start();
	size_t const bufsize = 1'000'000;
    char* buffer = new char[bufsize];
    
    iota_phase.start();
    for(size_t i = 0; i < bufsize; i++) buffer[i] = (char)i;
    iota_phase.stop();

    compute_phase.pause();
    std::cout << "initialization done!" << std::endl;
    compute_phase.resume();

    sum_phase.start();
    for(size_t i = 0; i < bufsize; i++) sum += buffer[i];
    sum_phase.stop();
    
    delete[] buffer;
    compute_phase.stop();
    compute_phase.append_child(iota_phase);
    compute_phase.append_child(sum_phase);
    compute_phase.data()["sum"] = sum;
}

std::cout << compute_phase.gather_data().dump(4) << std::endl;
```

We create the child phases for the two loops as only `TimePhase`, because they do not include any memory allocations that would interest us. Note that we create them *before* starting the computation phase to avoid their creation skewing the measurement of the computation. 

Similarly, we only define the hierarchy of the end of the computation, where we use `append_child` to enter the child phase data into the computation phase data. The resulting JSON looks as follows:

```json
{
    "children": [
        {
            "metrics": {
                "time": 2.008603
            },
            "name": "Iota"
        },
        {
            "metrics": {
                "time": 1.425463
            },
            "name": "Sum"
        }
    ],
    "data": {
        "sum": -497952
    },
    "metrics": {
        "memory": {
            "alloc_bytes": 1000000,
            "alloc_num": 1,
            "closing": 0,
            "free_bytes": 1000000,
            "free_num": 1,
            "peak": 1000000
        },
        "time": 3.434859
    },
    "name": "Example"
}
```

We see that the list `children` has been added to the JSON object, which contains the JSON object of each child phase in the order in which they have been appended.

## License

(tbd)

## Usage

**Brief**: submodule pm and add `pm` or `pm-malloc` to your CMake target dependencies, depending on whether you want to link with or without memory allocation tracking.

---

The recommended build environment is CMake and using pm as a submodule. Simply adding the corresponding targets as dependencies will set up your CMake targets for using pm.

### Without Memory Allocation Tracking

If you do not need memory allocation tracking, pm can be used as a header-only library.

The library defines the interface target `pm`. Adding it as a dependency will automatically add the include directory to your target.

Note that the `MallocCounter` meter can be used, so any code using it will remain compilable. All metrics will simply report zero.

### With Memory Allocation Tracking

If you do need memory allocation tracking, the library must be built with the `PM_MALLOC` macro defined and linked against. The easiest way to do this is to add the `pm-malloc` target to your CMake dependencies.

**Memory allocation tracking adds a slight runtime overhead.** It is recommended to not even include it in any production builds. To deactivate it, simply link against `pm` *instead of* `pm-malloc` (see [Without Memory Allocation Tracking](#without-memory-allocation-tracking)).

The runtime overhead stems from the fact that pm overrides `malloc` and friends, and does some bookkeeping for every allocation or free. While this is kept to a minimum and should not create much of an impact, to a small extent, it always will.

You cannot use pm's memory allocation tracking together with any other library that overrides `malloc`. For example, it is not compatible to [malloc_count](https://github.com/bingmann/malloc_count) (but it provides the same core functionality, anyway). Furthermore, memory allocation tracking will not work if you run your application with `valgrind`.
