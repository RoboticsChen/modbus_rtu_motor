cmake_minimum_required(VERSION 3.5.0)
project(spine_motor)

set(CMAKE_BUILD_TYPE Debug)

add_library(spine_motor SHARED src/spine_motor.cpp src/modbus_client.cpp)
target_include_directories(spine_motor PUBLIC include)
target_link_libraries(spine_motor modbus)

add_executable(mmk2_spine example/mmk2_spine.cpp)
target_include_directories(mmk2_spine PUBLIC include)
target_link_libraries(mmk2_spine spine_motor ncurses)