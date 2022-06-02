/**
 * pm/stopwatch.hpp
 * part of pdinklag/pm
 * 
 * MIT License
 * 
 * Copyright (c) 2022 Patrick Dinklage
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

#ifndef _PM_STOPWATCH_HPP
#define _PM_STOPWATCH_HPP

#include <chrono>

#include <nlohmann/json.hpp>

namespace pm {

/**
 * \brief Measures time durations
 * 
 * For measuring the elapsed time, the system's highest resolution clock is used.
 * 
 * This class satisfies the \ref pm::Meter "Meter" concept for gathering data in JSON data storages.
 */
class Stopwatch {
private:
    using Clock = std::chrono::high_resolution_clock;
    using Time = Clock::time_point;
    using Duration = Clock::duration;

    Time start_;
    Duration elapsed_;

public:
    /**
     * \brief Constructs a new stopwatch
     * 
     * Note that this does \em not start the stopwatch; \ref start must be called manually.
     */
    inline Stopwatch() : elapsed_(0) {}

    Stopwatch(Stopwatch const& other) = delete;
    Stopwatch(Stopwatch&& other) = default;
    Stopwatch& operator=(Stopwatch const& other) = delete;
    Stopwatch& operator=(Stopwatch&& other) = default;

    /**
     * \brief Starts the time measurement
     * 
     * This will reset the elapsed time to zero
     */
    inline void start() {
        elapsed_ = Duration(0);
        resume();
    }

    /**
     * \brief Pauses the time measurement
     * 
     */
    inline void pause() {
        elapsed_ += Clock::now() - start_;
    }

    /**
     * \brief Resumes the time measurement
     * 
     */
    inline void resume() {
        start_ = Clock::now();
    }

    /**
     * \brief Stops the time measurement
     * 
     * This is technically equivalent to pausing the stopwatch.
     */
    inline void stop() {
        pause();
    }

    /**
     * \brief Reports the measured elapsed time in nanosecond precision
     * 
     * \return uintmax_t the measured elapsed time in nanosecond precision
     */
    uintmax_t elapsed_time_nanos() const { return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_).count(); }

    /**
     * \brief Reports the measured elapsed time in seconds
     * 
     * \return uintmax_t the measured elapsed time in seconds
     */
    double elapsed_time_millis() const { return std::chrono::duration<double, std::milli>(elapsed_).count(); }

    /**
     * \brief The key for identifying this measurement in a data storage
     * 
     * \return the key for identifying this measurement in a data storage
     */
    std::string key() const { return "time"; }

    /**
     * \brief Gathers data in a JSON data storage
     * 
     * \param data the JSON data storage
     */
    nlohmann::json gather_metrics() const { return elapsed_time_millis(); }
};

}

#endif
