message(STATUS "CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")

file(GLOB homestead_src ${CMAKE_CURRENT_LIST_DIR}/*.c)
set(posix_sim_src ${posix_sim_src} ${homestead_src})