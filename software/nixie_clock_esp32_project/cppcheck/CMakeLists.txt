cmake_minimum_required(VERSION 3.16)
project(nixie_clock_misra C)

include_directories(
    ../main
    ../components
)

file(GLOB_RECURSE PROJECT_SOURCES
    "../main/*.c"
    "../components/**/*.c"
)

add_executable(misra_dummy ${PROJECT_SOURCES})
