set(CMAKE_TOOLCHAIN_FILE cmake/Arduino-toolchain.cmake)
set(ARDUINO_BOARD_OPTIONS_FILE cmake/BoardOptions.cmake)

cmake_minimum_required(VERSION 3.16.5)
project(WindowsExternalMixer
        VERSION 0.0.1
        HOMEPAGE_URL https://github.com/marcuscaisey/WindowsExternalMixer
        LANGUAGES CXX)

set(SRC_DIR src)
set(LIBS_DIR libs)

set(SOURCES
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/rotary.cpp)

set(LIBS
        ${LIBS_DIR}/EnableInterrupt)

add_executable(main ${SOURCES})

target_include_directories(main PRIVATE ${LIBS})

target_link_arduino_libraries(main PRIVATE core)

target_enable_arduino_upload(main)
