cmake_minimum_required(VERSION 3.8)
project(alsa_agc LANGUAGES CXX)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.hpp")

add_library(${PROJECT_NAME} SHARED  ${SOURCES} ${HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD ${PROJECT_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ${PROJECT_CXX_STANDARD_REQUIRED}
    PREFIX "libasound_module_pcm_"
    POSITION_INDEPENDENT_CODE TRUE)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE ${RELEASE_COMPILE_OPTS})
    target_link_options(${PROJECT_NAME} PRIVATE ${RELEASE_LINK_OPTS})
else()
    target_compile_options(${PROJECT_NAME} PRIVATE ${DEBUG_COMPILE_OPTS})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "${AGC_HEADERS}")
target_link_libraries(${PROJECT_NAME} PRIVATE agc ALSA::ALSA)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION "/usr/lib/alsa-lib")
