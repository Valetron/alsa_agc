cmake_minimum_required(VERSION 3.8)
project(agc LANGUAGES CXX)

file(GLOB SOURCES "sources/*.cpp")
file(GLOB HEADERS "headers/*.hpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD ${PROJECT_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ${PROJECT_CXX_STANDARD_REQUIRED})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PUBLIC ${RELEASE_COMPILE_OPTS})
    target_link_options(${PROJECT_NAME} PUBLIC ${RELEASE_LINK_OPTS})
else()
    target_compile_options(${PROJECT_NAME} PUBLIC ${DEBUG_COMPILE_OPTS})
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC ${COMPILE_DEFINES})
target_include_directories(${PROJECT_NAME} PUBLIC "headers")
target_link_libraries(${PROJECT_NAME} PUBLIC ${COMMON_LINK_LIBS})
