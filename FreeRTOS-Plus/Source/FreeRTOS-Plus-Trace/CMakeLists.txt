message(STATUS "CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")
include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/Include
  ${CMAKE_CURRENT_LIST_DIR}/config
)

if (COVERAGE_TEST EQUAL 0)
  file(GLOB freertos_plus_trace_src ${CMAKE_CURRENT_LIST_DIR}/*.c)
  set(posix_sim_src ${posix_sim_src} ${freertos_plus_trace_src})
endif()

include(${CMAKE_CURRENT_LIST_DIR}/streamports/CMakeLists.txt)