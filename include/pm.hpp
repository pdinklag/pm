/**
 * pm.hpp
 * part of pdinklag/pm
 * 
 * MIT License
 * 
 * Patrick Dinklage
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _PM_HPP
#define _PM_HPP

#include <pm/phase.hpp>
#include <pm/malloc_counter.hpp>
#include <pm/noop_phase.hpp>
#include <pm/noop_result.hpp>
#include <pm/result.hpp>
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
