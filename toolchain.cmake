
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

set(CMAKE_C_FLAGS "-Wall"
  CACHE STRING "my cmake c flags")
set(CMAKE_CXX_FLAGS "-Wall"
  CACHE STRING "my cmake cxx flags")

set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -gdb3 -DDEBUG"
  CACHE STRING "my cmake c flags")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -gdb3 -DDEBUG"
  CACHE STRING "my cmake c flags")

set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG"
  CACHE STRING "my release cmake cxx flags")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG"
  CACHE STRING "my release cmake cxx flags")