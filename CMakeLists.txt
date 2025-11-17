cmake_minimum_required(VERSION 3.13)
project(parallel_em_clustering)

set(CMAKE_C_STANDARD 11)

# OpenMP
# find_package(OpenMP REQUIRED)
# if(OpenMP_FOUND)
#     message(STATUS "OpenMP found")
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
# endif()

set(COMMON_SOURCES
    src/main.c
    )

add_compile_definitions(
    # BANDWIDTH=9.0
    # KERNEL="epanechnikov"
    # DIM=3
)


add_executable(em_clustering ${COMMON_SOURCES})
target_include_directories(em_clustering PRIVATE ${PROJECT_SOURCE_DIR}/include)

