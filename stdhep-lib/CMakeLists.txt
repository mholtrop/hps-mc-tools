#
# Specify the minimum CMake version required.  If the cmake version
# minimum is not met, cmake will fail.
cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
cmake_policy(SET CMP0005 NEW)
cmake_policy(SET CMP0042 NEW)

# Set the name of the project
project(hps-mc-tools)

add_subdirectory(src/mcfio)
add_subdirectory(src/src)

