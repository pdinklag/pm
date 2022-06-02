add_executable(test-pm pm.cpp)
target_link_libraries(test-pm PRIVATE pm)
add_test(pm ${CMAKE_CURRENT_BINARY_DIR}/test-pm)

add_executable(test-pm-malloc pm_malloc.cpp)
target_link_libraries(test-pm-malloc PRIVATE pm-malloc)
add_test(pm-malloc ${CMAKE_CURRENT_BINARY_DIR}/test-pm-malloc)

add_executable(test-examples examples.cpp)
target_link_libraries(test-examples PRIVATE pm-malloc)
add_test(examples ${CMAKE_CURRENT_BINARY_DIR}/test-examples)
