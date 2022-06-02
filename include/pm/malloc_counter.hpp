/**
 * pm/malloc_counter.hpp
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

#ifndef _PM_MALLOC_COUNTER_HPP
#define _PM_MALLOC_COUNTER_HPP

#include <algorithm>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <pm/malloc_callback.hpp>

namespace pm {

/**
 * \brief Measures memory allocations and frees
 * 
 * As an implementation of \ref MallocCallback , it receives memory allocation and free callbacks if tudocomp's `malloc` overrides are enabled.
 * 
 * This class satisfies the \ref pm::Meter "Meter" concept for gathering data in JSON data storages.
 */
class MallocCounter : public MallocCallback {
private:
    bool active_;

    intmax_t current_;
    uintmax_t peak_;

    uintmax_t alloc_num_;
    uintmax_t alloc_bytes_;
    uintmax_t free_num_;
    uintmax_t free_bytes_;

protected:
    inline void on_alloc(size_t bytes) override {
        current_ += bytes;
        if(current_ > 0) peak_ = std::max(peak_, (uintmax_t)current_);

        ++alloc_num_;
        alloc_bytes_ += bytes;
    }

    inline void on_free(size_t bytes) override {
        current_ -= bytes;

        ++free_num_;
        free_bytes_ += bytes;
    }

    inline void reset() {
        current_ = 0;
        peak_ = 0;
        alloc_num_ = 0;
        alloc_bytes_ = 0;
        free_num_ = 0;
        free_bytes_ = 0;
    }

public:
    inline MallocCounter() : MallocCallback(), active_(false) {
        reset();
    }

    MallocCounter(MallocCounter const& other) = delete;
    MallocCounter& operator=(MallocCounter const& other) = delete;

    inline MallocCounter(MallocCounter&& other) {
        *this = std::move(other);
    }

    inline MallocCounter& operator=(MallocCounter&& other) {
        current_ = other.current_;
        peak_ = other.peak_;
        alloc_num_ = other.alloc_num_;
        alloc_bytes_ = other.alloc_bytes_;
        free_num_ = other.free_num_;
        free_bytes_ = other.free_bytes_;
        active_ = other.active_;

        if(active_) {
            // nobody should be moving an active malloc counter, but who knows...
            other.unregister_callback();
            other.active_ = false;
            register_callback();
        }

        return *this;
    }

    /**
     * \brief Starts allocation tracking.
     */
    inline void start() {
        reset();
        resume();
    }
    
    /**
     * \brief Pauses allocation tracking.
     */
    inline void pause() {
        if(active_) {
            unregister_callback();
            active_ = false;
        }
    }

    /**
     * \brief Resumes allocation tracking.
     */
    inline void resume() {
        if(!active_) {
            register_callback();
            active_ = true;
        }
    }

    /**
     * \brief Ends allocation tracking.
     */
    inline void stop() {
        pause();
    }

    /**
     * \brief The current number of allocated bytes
     * 
     * Note that this may be negative if the counter has seen frees, but not the corresponding allocations.
     * 
     * \return the current number of allocated bytes 
     */
    intmax_t count() const { return current_; }

    /**
     * \brief The peak number of allocated bytes
     * 
     * \return the peak number of allocated bytes 
     */
    uintmax_t peak() const { return peak_; }

    /**
     * \brief The number of tracked memory allocations
     * 
     * \return the number of tracked memory allocations
     */
    uintmax_t alloc_num() const { return alloc_num_; }

    /**
     * \brief The number of bytes allocated by tracked memory allocations
     * 
     * \return the number of bytes allocated by tracked memory allocations
     */
    uintmax_t alloc_bytes() const { return alloc_bytes_; }

    /**
     * \brief The number of tracked memory releases
     * 
     * \return the number of tracked memory releases
     */
    uintmax_t free_num() const { return free_num_; }

    /**
     * \brief The number of bytes freed by tracked memory releases
     * 
     * \return the number of bytes freed by tracked memory releases
     */
    uintmax_t free_bytes() const { return free_bytes_; }

    /**
     * \brief The key for identifying this measurement in a data storage
     * 
     * \return the key for identifying this measurement in a data storage
     */
    std::string key() const { return "memory"; }

    /**
     * \brief Gathers data in a JSON data storage
     * 
     * \param data the JSON data storage
     */
    nlohmann::json gather_metrics() const {
        nlohmann::json obj;
        obj["peak"] = peak_;
        obj["closing"] = current_;
        obj["alloc_num"] = alloc_num_;
        obj["alloc_bytes"] = alloc_bytes_;
        obj["free_num"] = free_num_;
        obj["free_bytes"] = free_bytes_;
        return obj;
    }
};

}

#endif
