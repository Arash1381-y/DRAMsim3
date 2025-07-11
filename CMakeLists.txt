cmake_minimum_required(VERSION 3.0.0)
project(dramsim3)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Renamed inih to dramsim3_inih to prevent name collisions
add_library(dramsim3_inih INTERFACE)
target_include_directories(dramsim3_inih INTERFACE ext/headers)

# Renamed format to dramsim3_format to prevent name collisions (e.g., with fmtlib)
add_library(dramsim3_format INTERFACE)
target_include_directories(dramsim3_format INTERFACE ext/fmt/include)
target_compile_definitions(dramsim3_format INTERFACE FMT_HEADER_ONLY=1)

# Renamed args to dramsim3_args to prevent name collisions
# argparsing library, only used in main program not the library
add_library(dramsim3_args INTERFACE)
target_include_directories(dramsim3_args INTERFACE ext/headers)

# Renamed json to dramsim3_json to prevent name collisions
add_library(dramsim3_json INTERFACE)
target_include_directories(dramsim3_json INTERFACE ext/headers)

# Main DRAMSim Lib
add_library(dramsim3 SHARED
    src/bankstate.cc
    src/channel_state.cc
    src/command_queue.cc
    src/common.cc
    src/configuration.cc
    src/controller.cc
    src/dram_system.cc
    src/hmc.cc
    src/refresh.cc
    src/simple_stats.cc
    src/timing.cc
    src/memory_system.cc
        src/flexisim_stats.cc
        src/flexisim_stats.h
)

if (THERMAL)
    # dependency check
    # sudo apt-get install libatlas-base-dev on ubuntu
    find_package(BLAS REQUIRED)
    find_package(OpenMP REQUIRED)
    # YOU need to build superlu on your own. Do the following:
    # git submodule update --init
    # cd ext/SuperLU_MT_3.1 && make lib
    find_library(SUPERLU
        NAME superlu_mt_OPENMP libsuperlu_mt_OPENMP
        HINTS ${PROJECT_SOURCE_DIR}/ext/SuperLU_MT_3.1/lib/
    )

    target_link_libraries(dramsim3
        PRIVATE ${SUPERLU} f77blas atlas m ${OpenMP_C_FLAGS}
    )
    target_sources(dramsim3
        PRIVATE src/thermal.cc src/sp_ienv.c src/thermal_solver.c
    )
    target_compile_options(dramsim3 PRIVATE -DTHERMAL -D_LONGINT -DAdd_ ${OpenMP_C_FLAGS})

    add_executable(thermalreplay src/thermal_replay.cc)
    # Link to the renamed inih target
    target_link_libraries(thermalreplay dramsim3 dramsim3_inih)
    target_compile_options(thermalreplay PRIVATE -DTHERMAL -D_LONGINT -DAdd_ ${OpenMP_C_FLAGS})
endif (THERMAL)

if (CMD_TRACE)
    target_compile_options(dramsim3 PRIVATE -DCMD_TRACE)
endif (CMD_TRACE)

if (ADDR_TRACE)
    target_compile_options(dramsim3 PRIVATE -DADDR_TRACE)
endif (ADDR_TRACE)

# Propagate src and ext/headers as interface include directories
# This ensures that consuming projects automatically find INIReader.h
target_include_directories(dramsim3 INTERFACE src ext/headers)
target_compile_options(dramsim3 PRIVATE -Wall)
# Link to the renamed inih and format targets
target_link_libraries(dramsim3 PRIVATE dramsim3_inih dramsim3_format)
set_target_properties(dramsim3 PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# trace CPU, .etc
add_executable(dramsim3main src/main.cc src/cpu.cc)
# Link to the renamed args target
target_link_libraries(dramsim3main PRIVATE dramsim3 dramsim3_args)
target_compile_options(dramsim3main PRIVATE)
set_target_properties(dramsim3main PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Renamed Catch to dramsim3_Catch to prevent name collisions
add_library(dramsim3_Catch INTERFACE)
target_include_directories(dramsim3_Catch INTERFACE ext/headers) # This path likely points to Catch2 includes

add_executable(dramsim3test EXCLUDE_FROM_ALL
    tests/test_config.cc
    tests/test_dramsys.cc
    tests/test_hmcsys.cc # IDK somehow this can literally crush your computer
)
# Link to the renamed Catch target
target_link_libraries(dramsim3test dramsim3_Catch dramsim3)
target_include_directories(dramsim3test PRIVATE src/)

# We have to use this custome command because there's a bug in cmake
# that if you do `make test` it doesn't build your updated test files
# so we're stucking with `make dramsim3test` for now
add_custom_command(
    TARGET dramsim3test POST_BUILD
    COMMAND dramsim3test
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    DEPENDS dramsim3test dramsim3
)

