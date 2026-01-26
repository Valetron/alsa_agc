cmake_minimum_required(VERSION 3.8)
project(agc_pipe LANGUAGES CXX)

add_executable(${PROJECT_NAME} main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD ${PROJECT_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ${PROJECT_CXX_STANDARD_REQUIRED})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PUBLIC ${RELEASE_COMPILE_OPTS})
    target_link_options(${PROJECT_NAME} PUBLIC ${RELEASE_LINK_OPTS})
else()
    target_compile_options(${PROJECT_NAME} PUBLIC ${DEBUG_COMPILE_OPTS})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "${AGC_HEADERS}")
target_link_libraries(${PROJECT_NAME} PRIVATE agc Boost::program_options)
