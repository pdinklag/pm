#ifdef PM_MALLOC

#include <pm/malloc/hook.hpp>
#include <pm/malloc_callback.hpp>

using namespace pm;

void* MallocCallback::install_ = malloc(0);

auto init_callback() {
    std::vector<MallocCallback*> cb;
    cb.reserve(32);
    return cb;
}

std::vector<MallocCallback*> MallocCallback::registry_ = init_callback();
bool MallocCallback::critical_ = false;

// implement malloc_hook
void malloc_hook::on_malloc(size_t bytes) { MallocCallback::notify_malloc(bytes); }
void malloc_hook::on_free(size_t bytes)   { MallocCallback::notify_free(bytes); }

#endif
