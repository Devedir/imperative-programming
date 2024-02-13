#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define TAB_SIZE  1000
#define BUF_SIZE  1000

int get(int cols, int row, int col, const int* A) {
    return A[row * cols + col];
}

void set(int cols, int row, int col, int* A, int value) {
    A[row * cols + col] = value;
}

void prod_mat(int rowsA, int colsA, int colsB, int* A, int* B, int* AB) {
    for (int col = 0; col < colsB; col++)
        for (int row = 0; row < rowsA; row++) {
            set(colsB, row, col, AB, 0);
            for (int i = 0; i < colsA; i++) {
                int value = get(colsB, row, col, AB) + (get(colsA, row, i, A) * get(colsB, i, col, B));
                set(colsB, row, col, AB, value);
            }
        }
}

void read_mat(int rows, int cols, int* t) {
    int n = rows * cols;
    for (int i = 0; i < n; i++)
        scanf("%d", t + i);
}

void print_mat(int rows, int cols, int* t) {
    int i = 0;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            printf("%d ", t[i]);
            i++;
        }
        printf("\n");
    }
}

int read_char_lines(char* array[]) {
    int line, length;
    char buff[BUF_SIZE];
    for (line = 0; fgets(buff, BUF_SIZE, stdin) != NULL; line++) {
        for (length = 0; buff[length] != '\n'; length++) {}
        buff[length] = '\0';
        array[line] = malloc(sizeof(char) * (length + 1));
        strcpy(array[line], buff);
    }
    array[line] = NULL; // guardian
    return line - 1;
}

void write_char_line(char* array[], int n) {
    printf("%s\n", array[n]);
}

void delete_lines(char* array[]) {
    for (int i = 0; array[i] != NULL; i++)
        free(array[i]);
}

int read_int_lines_cont(int* ptr_array[]) {
    char buff[BUF_SIZE];
    int line;
    int* ptr = ptr_array[0];
    for (line = 1; fgets(buff, BUF_SIZE, stdin) != NULL; line++) {
        char* token = strtok(buff, " ");
        for (int i = 0; token != NULL; i++) {
            *ptr = atoi(token);
            ptr++;
            token = strtok(NULL, " ");
        }
        ptr_array[line] = ptr;
    }
    return line - 1;
}

void write_int_line_cont(int* ptr_array[], int n) {
    for (int* ptr = ptr_array[n]; ptr < ptr_array[n + 1]; ptr++)
        printf("%d ", *ptr);
    printf("\n");
}

typedef struct {
    int* values;
    int len;
    double average;
} line_type;

int read_int_lines(line_type lines_array[]) {
    char buff[BUF_SIZE];
    int line;
    for (line = 0; fgets(buff, BUF_SIZE, stdin) != NULL; line++) {
        lines_array[line].len = 1;
        for (int i = 0; buff[i] != '\n'; i++)
            if (buff[i] == ' ') lines_array[line].len++;
        lines_array[line].values = malloc(sizeof(int) * lines_array[line].len);
        lines_array[line].average = 0.0;
        char* token = strtok(buff, " ");
        for (int i = 0; token != NULL; i++) {
            int num = atoi(token);
            lines_array[line].average += num;
            lines_array[line].values[i] = num;
            token = strtok(NULL, " ");
        }
        lines_array[line].average /= lines_array[line].len;
    }
    return line - 1;
}

void write_int_line(line_type lines_array[], int n) {
    for (int i = 0; i < lines_array[n].len; i++)
        printf("%d ", lines_array[n].values[i]);
    printf("\n");
}

void delete_int_lines(line_type array[], int line_count) {
    for (int i = 0; i < line_count; i++)
        free(array[i].values);
}

int cmp(const void* a, const void* b) {
    const line_type* first = a;
    const line_type* second = b;
    if (first->average == second->average) return 0;
    return first->average - second->average > 0 ? 1 : -1;
}

void sort_by_average(line_type lines_array[], int line_count) {
    qsort(lines_array, line_count, sizeof(line_type), cmp);
}

typedef struct {
    int r, c, v;
} triplet;

int read_sparse(triplet* triplet_array, int n_triplets) {
    for (int i = 0; i < n_triplets; i++)
        scanf("%d %d %d", &triplet_array[i].r, &triplet_array[i].c, &triplet_array[i].v);
    return n_triplets;
}

int cmp_triplets(const void* t1, const void* t2) {
    const triplet* first = t1;
    const triplet* second = t2;
    int result = first->r - second->r;
    if (result != 0) return result;
    return first->c - second->c;
}

void make_CSR(triplet* triplet_array, int n_triplets, int rows, int* V, int* C, int* R) {
    qsort(triplet_array, n_triplets, sizeof(triplet), cmp_triplets);
    for (int i = 0; i < n_triplets; i++) {
        V[i] = triplet_array[i].v;
        C[i] = triplet_array[i].c;
    }
    int idx = 0;
    R[0] = 0;
    for (int row = 1; row < rows + 1; row++) {
        R[row] = R[row - 1];
        while (triplet_array[idx].r == row - 1) {
            R[row]++;
            idx++;
        }
    }
}

void multiply_by_vector(int rows, const int* V, const int* C, const int* R, const int* x, int* y) {
    for (int i = 0; i < rows; i++) {
        y[i] = 0;
        for (int j = R[i]; j < R[i + 1]; j++)
            y[i] += V[j] * x[C[j]];
    }
}

void read_vector(int* v, int n) {
    for (int i = 0; i < n; i++)
        scanf("%d", v + i);
}

void write_vector(int* v, int n) {
    for (int i = 0; i < n; i++)
        printf("%d ", v[i]);
    printf("\n");
}

int read_int() {
    char c_buf[BUF_SIZE];
    fgets(c_buf, BUF_SIZE, stdin);
    return (int) strtol(c_buf, NULL, 10);
}

int main(void) {
    int to_do = read_int();

    int A[TAB_SIZE], B[TAB_SIZE], AB[TAB_SIZE];
    int n, lines_counter, rowsA, colsA, rowsB, colsB;
    int rows, cols, n_triplets;
    char* char_lines_array[TAB_SIZE] = {NULL};
    int continuous_array[TAB_SIZE];
    int* ptr_array[TAB_SIZE];
    triplet triplet_array[TAB_SIZE];
    int V[TAB_SIZE], C[TAB_SIZE], R[TAB_SIZE];
    int x[TAB_SIZE], y[TAB_SIZE];
    line_type int_lines_array[TAB_SIZE];

    switch (to_do) {
        case 1:
            scanf("%d %d", &rowsA, &colsA);
            read_mat(rowsA, colsA, A);
            scanf("%d %d", &rowsB, &colsB);
            read_mat(rowsB, colsB, B);
            prod_mat(rowsA, colsA, colsB, A, B, AB);
            print_mat(rowsA, colsB, AB);
            break;
        case 2:
            n = read_int() - 1; // we count from 1 :)
            ptr_array[0] = continuous_array;
            read_int_lines_cont(ptr_array);
            write_int_line_cont(ptr_array, n);
            break;
        case 3:
            n = read_int() - 1;
            read_char_lines(char_lines_array);
            write_char_line(char_lines_array, n);
            delete_lines(char_lines_array);
            break;
        case 4:
            n = read_int() - 1;
            lines_counter = read_int_lines(int_lines_array);
            sort_by_average(int_lines_array, lines_counter);
            write_int_line(int_lines_array, n);
            printf("%.2f", int_lines_array[n].average); // Brakło tego w schemacie a było w dokumentacji...
            delete_int_lines(int_lines_array, lines_counter);
            break;
        case 5:
            scanf("%d %d %d", &rows, &cols, &n_triplets);
            n_triplets = read_sparse(triplet_array, n_triplets);
            read_vector(x, cols);
            make_CSR(triplet_array, n_triplets, rows, V, C, R);
            multiply_by_vector(rows, V, C, R, x, y);
            write_vector(V, n_triplets);
            write_vector(C, n_triplets);
            write_vector(R, rows + 1);
            write_vector(y, rows);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
            break;
    }
    return 0;
}

