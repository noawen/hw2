cmake_minimum_required(VERSION 3.7)
project(shuster_hw2)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        challenge.c
        challenge.h
        challenge_room_system_fields.h
        challenge_system.c
        challenge_system.h
        challenge_system_banch_tests.c
        constants.h
        system_additional_types.h
        visitor_room.c
        visitor_room.h)

SET(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic-errors -Werror -DNDEBUG")

SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

add_executable(shuster_hw2 ${SOURCE_FILES})