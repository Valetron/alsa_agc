cmake_minimum_required(VERSION 3.10)
project(alsa_agc LANGUAGES CXX)

find_package(Boost REQUIRED COMPONENTS program_options)
include(FindALSA)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.hpp")

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 14)

# target_compile_options(${PROJECT_NAME} PUBLIC -fno-rtti)

target_link_libraries(${PROJECT_NAME} PUBLIC Boost::program_options ${ALSA_LIBRARIES})
