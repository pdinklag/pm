/**
 * malloc_callback.cpp
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
