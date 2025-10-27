cmake_minimum_required(VERSION 3.10)
project(FPGA_pro)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type to Debug by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/_readdata.cpp
    src/graphinit.cpp
)

# Header files (for IDE support)
set(HEADERS
    include/_readdata.h
    include/graphinit.h
)

# Create executable
add_executable(fpga_pro ${SOURCES} ${HEADERS})

# Create data validation executable
add_executable(print_data_structures 
    src/print_data_structures.cpp 
    src/_readdata.cpp
    include/_readdata.h
)

# Set output directory
set_target_properties(fpga_pro PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(print_data_structures PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy test data to build directory (optional)
file(GLOB TEST_DATA "public/case*/*")
file(COPY ${TEST_DATA} DESTINATION ${CMAKE_BINARY_DIR}/public)

# Add custom target to run with specific test case
add_custom_target(run_case01
    COMMAND ${CMAKE_BINARY_DIR}/bin/fpga_pro
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS fpga_pro
    COMMENT "Running FPGA pro with case01 data"
)

add_custom_target(run_case02
    COMMAND ${CMAKE_BINARY_DIR}/bin/fpga_pro
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS fpga_pro
    COMMENT "Running FPGA pro with case02 data"
)

add_custom_target(run_case03
    COMMAND ${CMAKE_BINARY_DIR}/bin/fpga_pro
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS fpga_pro
    COMMENT "Running FPGA pro with case03 data"
)

add_custom_target(run_case04
    COMMAND ${CMAKE_BINARY_DIR}/bin/fpga_pro
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS fpga_pro
    COMMENT "Running FPGA pro with case04 data"
)

# Install target (optional)
install(TARGETS fpga_pro
    RUNTIME DESTINATION bin
)

# Print configuration info
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Source files: ${SOURCES}")
