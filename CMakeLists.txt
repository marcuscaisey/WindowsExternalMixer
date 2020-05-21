set(CMAKE_TOOLCHAIN_FILE cmake/Arduino-toolchain.cmake)

cmake_minimum_required(VERSION 3.16.5)
project(WindowsExternalMixer
        VERSION 0.0.1
        HOMEPAGE_URL https://github.com/marcuscaisey/WindowsExternalMixer
        LANGUAGES CXX)

set(SRC_DIR arduino/src)

set(SOURCES
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/rotary.cpp)

add_executable(main ${SOURCES})

target_link_arduino_libraries(main PRIVATE core)

target_enable_arduino_upload(main)
