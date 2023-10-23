message(STATUS "CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")
file(GLOB freertos_portable_memmang_src ${CMAKE_CURRENT_LIST_DIR}/heap_3.c)
set(posix_sim_src ${posix_sim_src} ${freertos_portable_memmang_src})

#add_library(freertos_lib SHARED
#    ${CMAKE_CURRENT_LIST_DIR}/heap_3.c
#)