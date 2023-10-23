message(STATUS "CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include(${CMAKE_CURRENT_LIST_DIR}/source/CMakeLists.txt)

#target_include_directories(posix_sim PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include" )
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/include)

#target_include_directories(${PROJECT_NAME}
#  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include     
#)