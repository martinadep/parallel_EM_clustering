cmake_minimum_required(VERSION 3.13)
project(parallel_em_clustering)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")

# OpenMP
find_package(OpenMP REQUIRED)
if(OpenMP_FOUND)
    message(STATUS "OpenMP found")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(COMMON_SOURCES
    src/main.c
    src/utils.c
    src/matrix/matrix_inverse_lu.c
    src/matrix/matrix_utils.c
    src/multiv_gaussian.c
    src/em_algorithm.c
    src/log_likelihood.c
    # src/matrix/matrix_inverse.c
    )

add_compile_definitions(
    TOTAL_TIMING
    # KERNEL="epanechnikov"
    # DIM=3
)


add_executable(em_clustering ${COMMON_SOURCES})
target_include_directories(em_clustering PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(em_clustering m)

# Parallel OpenMP version
set(PARALLEL_SOURCES
    src/parallel_omp/main.c
    src/parallel_omp/em_algorithm.c
    src/utils.c
    src/matrix/matrix_inverse_lu.c
    src/matrix/matrix_utils.c
    src/multiv_gaussian.c
    src/log_likelihood.c
)

add_executable(em_clustering_omp ${PARALLEL_SOURCES})
target_include_directories(em_clustering_omp PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(em_clustering_omp m OpenMP::OpenMP_C)
