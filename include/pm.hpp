#ifndef _PM_HPP
#define _PM_HPP

#include <pm/phase.hpp>
#include <pm/malloc_counter.hpp>
#include <pm/noop_phase.hpp>
#include <pm/stopwatch.hpp>

namespace pm {

/**
 * \brief Predefined configuration for \ref Phase "phases" that measure nothing, but can still hold application-defined measurement data
 */
using DataPhase = Phase<>;

/**
 * \brief Predefined configuration for \ref Phase "phases" that measure only running time
 */
using TimePhase = Phase<Stopwatch>;

/**
 * \brief Predefined configuration for \ref Phase "phases" that measure memory allocations and running time
 * 
 * Note that the \ref Stopwatch indicator is intentionally listed last.
 * Because when starting or stopping phases, the indicators are processed in the order in which they were defined,
 * this will cause time measurements to start at the latest necessary point and stop them earliest.
 */
using MemoryTimePhase = Phase<MallocCounter, Stopwatch>;

}

#endif
