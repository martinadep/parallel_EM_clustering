cmake_minimum_required(VERSION 3.10)
project(Parallel_EM_Clustering C)

# --- General compilation options ---
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O3 -lm")

# Include header directory (common to all versions)
include_directories(${CMAKE_SOURCE_DIR}/src/include)

# ==========================================
# 0. Common Source Files (Helpers and Math)
# ==========================================
# These files are used by ALL versions (Seq, MPI, OpenMP)
set(SOURCES_COMMON
    src/log_likelihood.c
    src/multiv_gaussian.c
    src/utils.c
    src/matrix/matrix_inverse.c
    src/matrix/matrix_determinant.c
)

# ==========================================
# 1. Sequential Version
# ==========================================
# Sources taken from the root of src/
add_executable(em_clustering_seq 
    src/main.c 
    src/em_algorithm.c 
    ${SOURCES_COMMON}
)
target_link_libraries(em_clustering_seq m)

# ==========================================
# 2. MPI Version (Distributed Memory)
# ==========================================
find_package(MPI)

if(MPI_FOUND)
    message(STATUS "MPI found. Building MPI executable.")
    include_directories(${MPI_INCLUDE_PATH})

    # Sources taken from src/mpi/ folder
    add_executable(em_clustering_mpi 
        src/mpi/main.c 
        src/mpi/em_algorithm.c 
        ${SOURCES_COMMON}
    )
    
    target_link_libraries(em_clustering_mpi PRIVATE MPI::MPI_C m)
else()
    message(WARNING "MPI not found. Skipping MPI build.")
endif()

# ==========================================
# 3. OpenMP Version (Shared Memory)
# ==========================================
find_package(OpenMP)

if(OpenMP_C_FOUND)
    message(STATUS "OpenMP found. Building OpenMP executable.")
    
    # Sources taken from src/omp/ folder
    add_executable(em_clustering_omp 
        src/omp/main.c 
        src/omp/em_algorithm.c 
        ${SOURCES_COMMON}
    )
    
    # Add OpenMP specific flags for this target
    target_compile_options(em_clustering_omp PRIVATE ${OpenMP_C_FLAGS})
    target_link_libraries(em_clustering_omp PRIVATE OpenMP::OpenMP_C m)
else()
    message(WARNING "OpenMP not found. Skipping OpenMP build.")
endif()
