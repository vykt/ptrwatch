cmake_minimum_required(VERSION 3.13)

#define build tools
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

#define project
project(ptrwatch VERSION 0.0.1 DESCRIPTION "watch pointer chains in real time" LANGUAGES C CXX)

#add sources
set (SOURCES_PTRWATCH
    ${CMAKE_CURRENT_SOURCE_DIR}/args.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cfg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scrn.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    )

set (HEADERS_PTRWATCH
    ${CMAKE_CURRENT_SOURCE_DIR}/args.h
    ${CMAKE_CURRENT_SOURCE_DIR}/cfg.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scrn.h
    )

#define targets
add_executable(watch ${SOURCES_PTRWATCH} ${HEADERS_PTRWATCH})
add_executable(watch_debug ${SOURCES_PTRWATCH} ${HEADERS_PTRWATCH})

#find dependencies
find_package(Curses REQUIRED)
find_library(PWU_LIB pwu)
#find_library(NCURSES_LIB ncurses)

#ptrscan options
target_compile_features(watch PUBLIC cxx_std_17)
target_compile_options(watch PUBLIC -O2 -Wall)
target_link_libraries(watch PUBLIC "${PWU_LIB}" PUBLIC "${CURSES_LIBRARIES}")

#ptrscan debug options
target_compile_features(watch_debug PUBLIC cxx_std_17)
target_compile_options(watch_debug PUBLIC -ggdb -O0 -Wall)
target_link_libraries(watch_debug PUBLIC "${PWU_LIB}" PUBLIC "${CURSES_LIBRARIES}")
