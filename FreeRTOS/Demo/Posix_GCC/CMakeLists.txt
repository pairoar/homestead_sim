message(STATUS "CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")
file(GLOB freertos_demo_posix_gcc_src ${CMAKE_CURRENT_LIST_DIR}/*.c)
set(posix_sim_src ${posix_sim_src} ${freertos_demo_posix_gcc_src})

include_directories(${CMAKE_CURRENT_LIST_DIR})
