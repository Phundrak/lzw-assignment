cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

project("projet_lzw")

set(TGT "projet_lzw")
set(${TGT}_VERSION_MAJOR 0)
set(${TGT}_VERSION_MINOR 1)

set(CXX_COVERAGE_COMPILE_FLAGS "-pedantic -Wall -Wextra -Wold-style-cast -Woverloaded-virtual -Wfloat-equal -Wwrite-strings -Wpointer-arith -Wcast-qual -Wcast-align -Wconversion -Wsign-conversion -Wshadow -Weffc++ -Wredundant-decls -Wdouble-promotion -Winit-self -Wswitch-default -Wswitch-enum -Wundef -Winline -Wunused -Wnon-virtual-dtor -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "${CXX_COVERAGE_COMPILE_FLAGS} -DDebug -g -pg")
set(CMAKE_CXX_FLAGS_RELEASE "${CXX_COVERAGE_COMPILE_FLAGS} -O3")

include(CheckCXXCompilerFlag)

# Check for standard to use
check_cxx_compiler_flag(-std=c++17 HAVE_FLAG_STD_CXX17)
if(HAVE_FLAG_STD_CXX17)
    set(CMAKE_CXX_STANDARD 17)
else()
    check_cxx_compiler_flag(-std=c++1z HAVE_FLAG_STD_CXX1Z)
    if(HAVE_FLAG_STD_CXX1Z)
        set(CXX_COVERAGE_COMPILE_FLAGS "${CXX_COVERAGE_COMPILE_FLAGS} -std=c++1z")
    else()
        message( FATAL_ERROR "C++17 not supported, CMake will exit." )
    endif()
endif()
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "../debug/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_COVERAGE_COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CXX_COVERAGE_COMPILE_FLAGS}")

# indicate the doc build as an option, ON by default
if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    option(BUILD_DOC "Build documentation" ON)
    find_package(Doxygen
                 REQUIRED dot)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile)
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")
        add_custom_target(docs ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
    else()
        message("Doxygen needs to be installed to generate the doxygen documentation.")
    endif()
endif()

include_directories(includes)
file(GLOB SOURCES "src/*.cc")
add_executable(${TGT} ${SOURCES})
