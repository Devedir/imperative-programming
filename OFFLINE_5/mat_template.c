#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define SIZE 40

void read_vector(double x[], int n) {
    for (int i = 0; i < n; ++i) {
        scanf("%lf", x++);
    }
}

void print_vector(double x[], int n) {
    for (int i = 0; i < n; ++i) {
        printf("%.4f ", x[i]);
    }
    printf("\n");
}

void read_mat(double A[][SIZE], int m, int n) {
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            scanf("%lf", &A[i][j]);
        }
    }
}

void print_mat(double A[][SIZE], int m, int n) {
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%.4f ", A[i][j]);
        }
        printf("\n");
    }
}

void print_mat_ext(const double A[][SIZE], const int m, const int n, const int indices[]) {
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j)
            printf("%.4f ", A[indices[i]][j]);
        printf("\n");
    }
    printf("---\n");
}

// 1. Calculate matrix product, AB = A X B
// A[m][p], B[p][n], AB[m][n]
void mat_product(double A[][SIZE], double B[][SIZE], double AB[][SIZE], int m, int p, int n) {
    for (int col = 0; col < n; col++)
        for (int row = 0; row < m; row++) {
            AB[row][col] = 0;
            for (int i = 0; i < p; i++)
                AB[row][col] += A[row][i] * B[i][col];
        }
}


// 2. Matrix triangulation and determinant calculation - simplified version
// (no rows' swaps). If A[i][i] == 0, function returns NAN.
// Function may change A matrix elements.
double gauss_simplified(double A[][SIZE], int n) {
    double diag, multiplier;
    double determinant = 1.0;
    for (int col = 0; col < n; col++) {
        diag = A[col][col];
        if (diag == 0.0) return NAN;
        determinant *= diag;
        for (int row = col + 1; row < n; row++) {
            multiplier = (-1) * A[row][col] / diag;
            for (int i = 0; i < n; i++)
                A[row][i] += multiplier * A[col][i];
        }
    }
    return determinant;
}

void swap_biggest(const double A[][SIZE], int indices[], const int n, const int col, double* determinant) {
    double max_val = A[indices[col]][col];
    int max_row = col;
    for (int row = col + 1; row < n; row++)
        if (fabs(A[indices[row]][col]) > fabs(max_val)) {
            max_val = A[indices[row]][col];
            max_row = row;
        }
    if (max_row != col) {
        int tmp = indices[max_row];
        indices[max_row] = indices[col];
        indices[col] = tmp;
        *determinant *= -1.0;
    }
}

// 3. Matrix triangulation, determinant calculation, and Ax = b solving - extended version
// (Swap the rows so that the row with the largest, leftmost nonzero entry is on top. While
// swapping the rows use index vector - do not copy entire rows.)
// If max A[i][i] < eps, function returns 0.
// If det != 0 && b != NULL && x != NULL then vector x should contain solution of Ax = b.
double gauss(double A[][SIZE], const double b[], double x[], const int n, const double eps) {
    double diag, multiplier;
    double determinant = 1.0;
    int indices[SIZE];
    for (int i = 0; i < n; i++) indices[i] = i; // {0, 1, 2, ..., n-1}
    double new_b[SIZE];
    for (int i = 0; i < n; i++) new_b[i] = b[i]; // Zwykłe kopiowanie
    for (int col = 0; col < n; col++) {
        swap_biggest(A, indices, n, col, &determinant);
        diag = A[indices[col]][col];
        if (fabs(diag) < eps) {
            determinant = 0.0;
            break;
        }
        determinant *= diag;
        for (int row = col + 1; row < n; row++) {
            multiplier = (-1) * A[indices[row]][col] / diag;
            for (int i = 0; i < n; i++)
                A[indices[row]][i] += multiplier * A[indices[col]][i];
            new_b[indices[row]] += multiplier * new_b[indices[col]];
        }
    }
    if (determinant != 0.0 && b != NULL && x != NULL) {
        double right_sum;
        for (int i = n - 1; i >= 0; i--) {
            right_sum = 0.0;
            for (int j = n - 1; j > i; j--)
                right_sum += A[indices[i]][j] * x[j];
            x[i] = (new_b[indices[i]] - right_sum) / A[indices[i]][i];
        }
    }
    return determinant;
}

void insert_sort_rows(double B[][SIZE], const int indices[], const int n) {
    double new[SIZE][SIZE];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            new[i][j] = B[indices[i]][j];
    for (int x = 0; x < n; x++)
        for (int j = 0; j < n; j++)
            B[x][j] = new[x][j];
}

// 4. Returns the determinant; B contains the inverse of A (if det(A) != 0)
// If max A[i][i] < eps, function returns 0.
double matrix_inv(double A[][SIZE], double B[][SIZE], const int n, const double eps) {
    double diag, multiplier;
    double determinant = 1.0;
    int indices[SIZE];
    for (int i = 0; i < n; i++) indices[i] = i; // {0, 1, 2, ..., n-1}
    for (int row = 0; row < n; row++)
        for (int col = 0; col < n; col++) {
            if (row == col) B[row][col] = 1;
            else B[row][col] = 0;
        }
    for (int col = 0; col < n; col++) {
        swap_biggest(A, indices, n, col, &determinant);
        diag = A[indices[col]][col];
        if (fabs(diag) < eps) {
            determinant = 0.0;
            break;
        }
        determinant *= diag;
        for (int row = col + 1; row < n; row++) {
            multiplier = (-1) * A[indices[row]][col] / diag;
            for (int i = 0; i < n; i++) {
                A[indices[row]][i] += multiplier * A[indices[col]][i];
                B[indices[row]][i] += multiplier * B[indices[col]][i];
            }
        }
    }
    for (int col = n - 1; col >= 0; col--) {
        diag = A[indices[col]][col];
        for (int i = 0; i < n; i++) {
            A[indices[col]][i] /= diag;
            B[indices[col]][i] /= diag;
        }
        for (int row = col - 1; row >= 0; row--) {
            multiplier = (-1) * A[indices[row]][col];
            for (int i = 0; i < n; i++) {
                A[indices[row]][i] += multiplier * A[indices[col]][i];
                B[indices[row]][i] += multiplier * B[indices[col]][i];
            }
        }
    }
    insert_sort_rows(B, indices, n);
    return determinant;
}

int main(void) {

    double A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
    double b[SIZE], x[SIZE], det, eps = 1.e-13;

    int to_do;
    int m, n, p;

    scanf("%d", &to_do);

    switch (to_do) {
        case 1:
            scanf("%d %d %d", &m, &p, &n);
            read_mat(A, m, p);
            read_mat(B, p, n);
            mat_product(A, B, C, m, p, n);
            print_mat(C, m, n);
            break;
        case 2:
            scanf("%d", &n);
            read_mat(A, n, n);
            printf("%.4f\n", gauss_simplified(A, n));
            break;
        case 3:
            scanf("%d", &n);
            read_mat(A, n, n);
            read_vector(b, n);
            det = gauss(A, b, x, n, eps);
            printf("%.4f\n", det);
            if (det) print_vector(x, n);
            break;
        case 4:
            scanf("%d", &n);
            read_mat(A, n, n);
            det = matrix_inv(A, B, n, eps);
            printf("%.4f\n", det);
            if (det) print_mat(B, n, n);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
            break;
    }
    return 0;
}

