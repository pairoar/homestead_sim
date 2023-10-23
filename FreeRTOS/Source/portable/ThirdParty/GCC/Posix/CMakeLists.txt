file(GLOB freertos_portable_thirdparty_gcc_posix_src ${CMAKE_CURRENT_LIST_DIR}/port.c)
set(posix_sim_src ${posix_sim_src} ${freertos_portable_thirdparty_gcc_posix_src})

#add_library(freertos_lib SHARED
#    ${CMAKE_CURRENT_LIST_DIR}/port.c
#)

include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/utils/CMakeLists.txt)

