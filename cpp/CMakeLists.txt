set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build/conan_toolchain.cmake")
set(QT_CREATOR_SKIP_CONAN_SETUP ON)

cmake_minimum_required(VERSION 3.8)
project(alsa_agc LANGUAGES CXX)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(DEBUG "ccache found")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PROGRAM})
endif()

find_package(ALSA REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost REQUIRED)

file(GLOB SOURCES "sources/*.cpp")
file(GLOB HEADERS "headers/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)

# add_compile_definitions(SPDLOG_COMPILED_LIB)

target_compile_definitions(${PROJECT_NAME} PUBLIC -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
target_compile_options(${PROJECT_NAME} PRIVATE -Os -ffunction-sections -fdata-sections -fmerge-all-constants -fno-threadsafe-statics) # TODO: -Wno-psabi
target_link_options(${PROJECT_NAME} PRIVATE -Wl,--exclude-libs,ALL -flto -Wl,--strip-all)
target_include_directories(${PROJECT_NAME} PUBLIC "headers")
target_link_libraries(${PROJECT_NAME} PUBLIC ALSA::ALSA spdlog::spdlog Boost::program_options -static-libgcc -static-libstdc++ atomic)
