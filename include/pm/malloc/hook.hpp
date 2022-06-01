#ifndef _PM_MALLOC_HOOK_HPP
#define _PM_MALLOC_HOOK_HPP

#include <cstddef>

namespace pm::malloc_hook {

/**
 * \brief Called by the `malloc` overrides when memory is allocated
 * 
 * \param bytes the number of allocated bytes
 */
void on_malloc(size_t bytes);

/**
 * \brief Called by `malloc` overrides when memory is freed
 * 
 * \param bytes the number of freed bytes
 */
void on_free(size_t bytes);

}

#endif
