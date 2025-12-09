#include <stdio.h>
#include "include/commons.h"
#include "matrix/matrix.h"

int main() {
    printf("Hello, EM Clustering!\n");

    float **example_matrix = alloc_matrix(3);
    example_matrix[0][0] = 4; example_matrix[0][1] = 7; example_matrix[0][2] = 1;
    example_matrix[1][0] = 2; example_matrix[1][1] = 6; example_matrix[1][2] = 3;
    example_matrix[2][0] = 5; example_matrix[2][1] = 8; example_matrix[2][2] = 9;

    print_matrix(example_matrix, 3);

    float **inverse_matrix = alloc_matrix(3);
    if (invert_matrix(example_matrix, 3, inverse_matrix) == 0) {
        printf("Inverse matrix:\n");
        print_matrix(inverse_matrix, 3);
    } else {
        printf("Matrix is singular and cannot be inverted.\n");
    }

    return 0;
}