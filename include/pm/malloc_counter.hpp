#pragma once

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
     * \brief Does nothing
     * 
     * It is expected that parent malloc counters live at least as long as their children and thus receive the same allocation and free callbacks.
     * 
     * \param parent the parent counter
     */
    inline void propagate(MallocCounter& parent) {
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
