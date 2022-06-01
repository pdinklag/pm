# create library pm-malloc with malloc overrides
add_library(pm-malloc malloc_callback.cpp malloc_override.cpp)
target_compile_definitions(pm-malloc PUBLIC PM_MALLOC)
target_link_libraries(pm-malloc PUBLIC pm)
