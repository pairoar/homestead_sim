message(STATUS "CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")
file(GLOB freertos_portable_thirdparty_gcc_posix_utils_src ${CMAKE_CURRENT_LIST_DIR}/wait_for_event.c)
set(posix_sim_src ${posix_sim_src} ${freertos_portable_thirdparty_gcc_posix_utils_src})

#add_library(freertos_lib SHARED
#    ${CMAKE_CURRENT_LIST_DIR}/wait_for_event.c
#)

include_directories(${CMAKE_CURRENT_LIST_DIR})
