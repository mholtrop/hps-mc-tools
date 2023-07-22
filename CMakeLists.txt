#
# Specify the minimum CMake version required.  If the cmake version
# minimum is not met, cmake will fail.
cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
cmake_policy(SET CMP0005 NEW)
cmake_policy(SET CMP0042 NEW)

# Set the name of the project
project(hps-mc-tools)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_options(-DDEBUG=1)
endif()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(LCIO REQUIRED)
include_directories( AFTER "${LCIO_INCLUDE_DIR}")

find_package(ROOT REQUIRED)
##---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
message(STATUS "ROOT_USE_FILE: ${ROOT_USE_FILE}")
#include(${ROOT_USE_FILE})  # This interferes with StdHep -- figure out what later.
include_directories(${ROOT_INCLUDE_DIRS})
link_directories(${ROOT_LIBRARY_DIR})
message(STATUS "ROOT found at: ${ROOT_DIR}")

add_subdirectory(stdhep-lib)
include_directories(stdhep-lib/src/src/inc stdhep-tools/src)

add_executable(stdhep-print src/stdhep_print.cpp stdhep-tools/src/stdhep_util.cpp)
target_link_libraries(stdhep-print stdhepC mcfio)
install(TARGETS stdhep-print)

add_executable(stdhep2root src/stdhep2root.cpp stdhep-tools/src/stdhep_util.cpp)
target_link_libraries(stdhep2root stdhepC mcfio ${ROOT_LIBRARIES})
install(TARGETS stdhep2root)

add_executable(stdhep_split src/stdhep_split.cpp stdhep-tools/src/stdhep_util.cpp)
target_link_libraries(stdhep_split stdhepC mcfio)
install(TARGETS stdhep_split)

add_executable(particle-trainer src/particle-trainer.cpp stdhep-tools/src/stdhep_util.cpp)
target_link_libraries(particle-trainer stdhepC mcfio)
install(TARGETS particle-trainer)

add_executable(mumu_translate src/mumu_translate.cpp stdhep-tools/src/stdhep_util.cpp)
target_link_libraries(mumu_translate stdhepC mcfio)
install(TARGETS mumu_translate)

add_executable(particle-gun src/particle-gun.cpp stdhep-tools/src/stdhep_util.cpp)
target_link_libraries(particle-gun stdhepC mcfio)
install(TARGETS particle-gun)

add_executable(emep-gun src/emep-gun.cpp stdhep-tools/src/stdhep_util.cpp)
target_link_libraries(emep-gun stdhepC mcfio)

add_executable(lcio_info src/lcio_info.cpp)
target_link_libraries(lcio_info ${LCIO_LIBRARIES})
install(TARGETS lcio_info)
