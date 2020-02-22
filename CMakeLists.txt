cmake_minimum_required(VERSION 3.8)
project(cilisp)

add_executable(cilisp)
set_property(TARGET cilisp PROPERTY C_STANDARD 11)
set_property(TARGET cilisp PROPERTY C_STANDARD_REQUIRED ON)

#Turning on all warnings
target_compile_options(cilisp PRIVATE -Wall -Wextra -Wpedantic -Weverything)

#Turning off false positive warnings
target_compile_options(cilisp PRIVATE -Wno-unused-macros -Wno-unreachable-code-break)
target_compile_options(cilisp PRIVATE -Wno-padded -Wno-missing-variable-declarations)
target_compile_options(cilisp PRIVATE -Wno-reserved-id-macro -Wno-extra-semi-stmt)
target_compile_options(cilisp PRIVATE -Wno-float-equal -Wno-missing-prototypes)
target_compile_options(cilisp PRIVATE -Wno-unreachable-code -Wno-covered-switch-default)
target_compile_options(cilisp PRIVATE -Wno-switch-enum -Wno-float-conversion)

target_include_directories(cilisp PRIVATE src)
target_include_directories(cilisp PRIVATE src/flex)
target_include_directories(cilisp PRIVATE src/bison)

find_package(FLEX)
find_package(BISON)

FLEX_TARGET(ciLispScanner src/ciLisp.l ${CMAKE_SOURCE_DIR}/src/flex/ciLispScanner.c COMPILE_FLAGS)
BISON_TARGET(ciLispParser src/ciLisp.y ${CMAKE_SOURCE_DIR}/src/bison/ciLispParser.c VERBOSE COMPILE_FLAGS "-Wall")
ADD_FLEX_BISON_DEPENDENCY(ciLispScanner ciLispParser)

target_sources(cilisp PRIVATE src/ciLisp.c src/ciLisp.h)
target_sources(cilisp PRIVATE ${BISON_ciLispParser_OUTPUTS})
target_sources(cilisp PRIVATE ${FLEX_ciLispScanner_OUTPUTS})

target_link_libraries(cilisp m)