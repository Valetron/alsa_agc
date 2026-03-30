set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build/conan_toolchain.cmake")
set(QT_CREATOR_SKIP_CONAN_SETUP ON)

cmake_minimum_required(VERSION 3.8)
project(AGC LANGUAGES CXX)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(DEBUG "ccache found")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PROGRAM})
endif()

find_package(ALSA REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost REQUIRED)

set(PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(RELEASE_COMPILE_OPTS -Os -ffunction-sections -fdata-sections -fmerge-all-constants -fno-threadsafe-statics) # TODO: -Wno-psabi
set(RELEASE_LINK_OPTS -Wl,--exclude-libs,ALL -flto -Wl,--strip-all)
set(DEBUG_COMPILE_OPTS -O0 -g -fno-omit-frame-pointer)
set(COMPILE_DEFINES "-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG")
set(COMMON_LINK_LIBS spdlog::spdlog -static-libgcc -static-libstdc++ atomic)
set(PROJECT_CXX_STANDARD 20)
set(PROJECT_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libagc)

set(AGC_HEADERS "${PROJECT_ROOT_DIR}/libagc/headers")

add_subdirectory(utils)
add_subdirectory(alsa_module)
