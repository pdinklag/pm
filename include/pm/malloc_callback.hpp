#ifndef _PM_MALLOC_CALLBACK_HPP
#define _PM_MALLOC_CALLBACK_HPP

#include <algorithm>
#include <vector>

namespace pm {

/**
 * \brief Abstract base for memory allocation callback handlers
 * 
 * Instances will receive callbacks for memory allocations (via \ref on_alloc ) and frees (via \ref on_free ) as long as they are registered.
 * Registration is done by calling \ref register_callback from the implementing class and has to be done manually.
 * Upon destruction, \ref unregister_callback is called automatically.
 * The registration process is \em not thread-safe.
 * 
 * The static notifiers, \ref notify_malloc and \ref notify_free , are hooked into tudocomp's `malloc` overrides.
 * Therefore, automatic memory allocation tracking only functions if said overrides are enabled.
 */
class MallocCallback {
private:
    #ifdef PM_MALLOC
    static void* install_;
    static std::vector<MallocCallback*> registry_;
    static bool critical_;
    #endif

    bool registered_;

protected:
    inline void register_callback() {
        #ifdef PM_MALLOC
        if(!registered_) {
            critical_ = true;
            registry_.push_back(this);
            registered_ = true;
            critical_ = false;
        }
        #endif
    }

    inline void unregister_callback() {
        #ifdef PM_MALLOC
        critical_ = true;
        auto it = std::find(registry_.begin(), registry_.end(), this);
        if(it != registry_.end()) {
            registry_.erase(it);
        }
        registered_ = false;
        critical_ = false;
        #endif
    }

    /**
     * \brief Called when a memory allocation is tracked
     * 
     * \param bytes the number of allocated bytes
     */
    virtual void on_alloc(size_t bytes) = 0;

    /**
     * \brief Called when a memory free is tracked
     * 
     * \param bytes the number of released bytes
     */
    virtual void on_free(size_t bytes) = 0;

public:
    /**
     * \brief Tracks a memory allocation
     * 
     * \param bytes the number of allocated bytes
     */
    inline static void notify_malloc(size_t bytes) {
        #ifdef PM_MALLOC
        if(critical_) return;
        for(auto cb : registry_) cb->on_alloc(bytes);
        #endif
    }

    /**
     * \brief Tracks a memory release
     * 
     * \param bytes the number of release bytes
     */
    inline static void notify_free(size_t bytes) {
        #ifdef PM_MALLOC
        if(critical_) return;
        for(auto cb : registry_) cb->on_free(bytes);
        #endif
    }

    inline MallocCallback() : registered_(false) {
    }

    inline ~MallocCallback() {
        unregister_callback();
    }

    MallocCallback(MallocCallback const& other) = delete;
    MallocCallback(MallocCallback&& other) = delete;
    MallocCallback& operator=(MallocCallback const& other) = delete;
    MallocCallback& operator=(MallocCallback&& other) = delete;
};

}

#endif
