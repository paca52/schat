cmake_minimum_required(VERSION 3.16)
project(chat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

find_package(Threads REQUIRED)

# ---- Auto-collect sources ---------------------------------------------------
# CONFIGURE_DEPENDS makes the build re-check the glob, so new files in src/
# are picked up on the next build without manually re-running cmake.
file(GLOB COMMON_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Files that contain a main() are apps, not part of the shared library
set(APP_SOURCES
  ${CMAKE_SOURCE_DIR}/src/server.cpp
  ${CMAKE_SOURCE_DIR}/src/client.cpp
)
list(REMOVE_ITEM COMMON_SOURCES ${APP_SOURCES})

# ---- Shared code ------------------------------------------------------------
add_library(chat_common STATIC ${COMMON_SOURCES})
target_include_directories(chat_common PUBLIC ${CMAKE_SOURCE_DIR}/h)
target_link_libraries(chat_common PUBLIC Threads::Threads)

# ---- Separate apps (EXCLUDE_FROM_ALL = only built when asked for) -----------
add_executable(server EXCLUDE_FROM_ALL src/server.cpp)
target_link_libraries(server PRIVATE chat_common)

add_executable(client EXCLUDE_FROM_ALL src/client.cpp)
target_link_libraries(client PRIVATE chat_common)

# ---- Warnings ---------------------------------------------------------------
foreach(tgt chat_common server client)
  if(MSVC)
    target_compile_options(${tgt} PRIVATE /W4)
  else()
    target_compile_options(${tgt} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endforeach()
