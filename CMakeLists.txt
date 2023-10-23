cmake_minimum_required(VERSION 3.11)
PROJECT(homestead_sim C CXX)
set(CMAKE_CXX_STANDARD 17)

# set cmake verbose mode when debugging
#set(CMAKE_VERBOSE_MAKEFILE ON)

# Sources
set(posix_sim_src "" CACHE INTERNAL "FreeRTOS Posix Simulator Source Lists")
set(freertos_src "" CACHE INTERNAL "FreeRTOS Posix Simulator Source Lists")
set(freertos_portable_memmang_src "" CACHE INTERNAL "FreeRTOS Posix Simulator Source Lists")
set(freertos_portable_thirdparty_gcc_posix_src "" CACHE INTERNAL "FreeRTOS Posix Simulator Source Lists")
set(freertos_portable_thirdparty_gcc_posix_utils_src "" CACHE INTERNAL "FreeRTOS Posix Simulator Source Lists")
set(freertos_demo_common_minimal_src "" CACHE INTERNAL "FreeRTOS Posix Simulator Source Lists")
set(freertos_demo_posix_gcc_src "" CACHE INTERNAL "FreeRTOS Posix Simulator Source Lists")
set(freertos_plus_trace_src "" CACHE INTERNAL "FreeRTOS Posix Simulator Source Lists")
set(freertos_plus_trace_streamports_src "" CACHE INTERNAL "FreeRTOS Posix Simulator Source Lists")
set(homestead_src "" CACHE INTERNAL "Homestead Source Lists")

# toolchain
include(${CMAKE_CURRENT_SOURCE_DIR}/toolchain.cmake)

# options
include(${CMAKE_CURRENT_SOURCE_DIR}/options.cmake)

#include subdirectory
include(${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Plus/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/homestead/CMakeLists.txt)


# set definitions
ADD_DEFINITIONS("-D_WINDOWS_")

if (TRACE_ON_ENTER EQUAL 1)
  ADD_DEFINITIONS("-DTRACE_ON_ENTER=1")
endif()

if (COVERAGE_TEST EQUAL 1)
  ADD_DEFINITIONS("-DprojCOVERAGE_TEST=1")
else()
  ADD_DEFINITIONS("-DprojCOVERAGE_TEST=0")
endif()

if (PROFILE EQUAL 1)
  add_compile_options("-pg -O0")
  link_libraries("-pg -O0")
else()
  add_compile_options("-O3")
  link_libraries("-O3")
endif()

if (SANITIZE_ADDRESS EQUAL 1)
  add_compile_options("-fsanitize=address -fsanitize=alignment")
  link_libraries("-fsanitize=address -fsanitize=alignment")
endif()

if (SANITIZE_LEAK EQUAL 1)
  add_compile_options("-fsanitize=leak")
  link_libraries("-fsanitize=leak")
endif()


if (${USER_DEMO} STREQUAL "FULL_DEMO")
  ADD_DEFINITIONS("-DUSER_DEMO=1")
else()
  ADD_DEFINITIONS("-DUSER_DEMO=0")
endif()

#message(STATUS, "${posix_sim_src}")
message(STATUS, "homestead_src = ${homestead_src}")

# libs
#set(FREERTOS_LIBS ${freertos_lib})
set(FREERTOS_LIBS "")
set(POSIX_SIM_COMMON_LIBS pthread)
set(POSIX_SIM_LIBS ${POSIX_SIM_COMMON_LIBS} ${FREERTOS_LIBS})
include(FindPkgConfig)
pkg_check_modules(posix_sim_pkgs REQUIRED ${POSIX_SIMLIBS})
include_directories(${posix_sim_pkgs_INCLUDE_DIR})

# executable
add_executable(${PROJECT_NAME} ${posix_sim_src})
target_link_libraries(${PROJECT_NAME} ${posix_sim_pkgs_LDFLAGS} ${POSIX_SIM_LIBS_FLAGS})


#add_subdirectory(homestead)