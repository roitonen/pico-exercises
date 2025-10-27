cmake_minimum_required(VERSION 3.13)

# Check if PICO_SDK_PATH is set
if(NOT DEFINED ENV{PICO_SDK_PATH})
    message(FATAL_ERROR "PICO_SDK_PATH not set. Please set it to your pico-sdk path.")
endif()

# Include SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Project name
project(pico_exercises C CXX ASM)

# SDK initialization
pico_sdk_init()

# Add subdirectory for the first exercise
add_subdirectory(exercise1)
add_subdirectory(exercise2)
