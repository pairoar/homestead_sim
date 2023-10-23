message(STATUS "CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")

file(GLOB freertos_demo_common_minimal_src 
    ${CMAKE_CURRENT_LIST_DIR}/AbortDelay.c
    ${CMAKE_CURRENT_LIST_DIR}/BlockQ.c
    ${CMAKE_CURRENT_LIST_DIR}/blocktim.c
    ${CMAKE_CURRENT_LIST_DIR}/countsem.c
    ${CMAKE_CURRENT_LIST_DIR}/death.c
    ${CMAKE_CURRENT_LIST_DIR}/dynamic.c
    ${CMAKE_CURRENT_LIST_DIR}/EventGroupsDemo.c
    ${CMAKE_CURRENT_LIST_DIR}/flop.c
    ${CMAKE_CURRENT_LIST_DIR}/GenQTest.c
    ${CMAKE_CURRENT_LIST_DIR}/integer.c
    ${CMAKE_CURRENT_LIST_DIR}/IntSemTest.c
    ${CMAKE_CURRENT_LIST_DIR}/MessageBufferAMP.c
    ${CMAKE_CURRENT_LIST_DIR}/MessageBufferDemo.c
    ${CMAKE_CURRENT_LIST_DIR}/PollQ.c
    ${CMAKE_CURRENT_LIST_DIR}/QPeek.c
    ${CMAKE_CURRENT_LIST_DIR}/QueueOverwrite.c
    ${CMAKE_CURRENT_LIST_DIR}/QueueSet.c
    ${CMAKE_CURRENT_LIST_DIR}/QueueSetPolling.c
    ${CMAKE_CURRENT_LIST_DIR}/recmutex.c
    ${CMAKE_CURRENT_LIST_DIR}/semtest.c
    ${CMAKE_CURRENT_LIST_DIR}/StaticAllocation.c
    ${CMAKE_CURRENT_LIST_DIR}/StreamBufferDemo.c
    ${CMAKE_CURRENT_LIST_DIR}/StreamBufferInterrupt.c
    ${CMAKE_CURRENT_LIST_DIR}/TaskNotify.c
    ${CMAKE_CURRENT_LIST_DIR}/TimerDemo.c
)

set(posix_sim_src ${posix_sim_src} ${freertos_demo_common_minimal_src})
