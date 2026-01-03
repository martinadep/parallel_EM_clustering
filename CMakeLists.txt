cmake_minimum_required(VERSION 3.13)
project(parallel_em_clustering)

set(CMAKE_C_STANDARD 11)

find_package(MPI REQUIRED)

set(COMMON_SOURCES
    src/main.c
    src/utils.c
    src/matrix/matrix_inverse_lu.c
    src/matrix/matrix_utils.c
    src/multiv_gaussian.c
    src/em_algorithm.c
    src/log_likelihood.c
)

add_compile_definitions(TOTAL_TIMING)

add_executable(em_clustering ${COMMON_SOURCES})

# MPI links and includes
target_include_directories(em_clustering PRIVATE ${PROJECT_SOURCE_DIR}/include ${MPI_INCLUDE_PATH})
target_link_libraries(em_clustering ${MPI_C_LIBRARIES})

