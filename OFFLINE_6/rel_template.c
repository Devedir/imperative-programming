#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_REL_SIZE 100
#define MAX_RANGE 100

typedef struct {
    int first;
    int second;
} pair;

// Add pair to existing relation if not already there
int add_relation(pair*, int, pair);

// Case 1:

// The relation R is reflexive if xRx for every x in X
int is_reflexive(const pair*, int);

int is_reflexive(const pair* relation, int n) {
    bool first_reflected, second_reflected;
    for (int i = 0; i < n; i++) {
        first_reflected = false;
        second_reflected = false;
        for (int j = 0; j < n; j++) {
            if (relation[i].first == relation[j].first && relation[i].first == relation[j].second)
                first_reflected = true;
            if (relation[i].second == relation[j].first && relation[i].second == relation[j].second)
                second_reflected = true;
        }
        if (!first_reflected || !second_reflected) return 0;
    }
    return 1;
}

// The relation R on the set X is called irreflexive
// if xRx is false for every x in X
int is_irreflexive(const pair*, int);

int is_irreflexive(const pair* relation, int n) {
    bool first_reflected, second_reflected;
    for (int i = 0; i < n; i++) {
        first_reflected = false;
        second_reflected = false;
        for (int j = 0; j < n; j++) {
            if (relation[i].first == relation[j].first && relation[i].first == relation[j].second)
                first_reflected = true;
            if (relation[i].second == relation[j].first && relation[i].second == relation[j].second)
                second_reflected = true;
        }
        if (first_reflected || second_reflected) return 0;
    }
    return 1;
}

// A binary relation R over a set X is symmetric if:
// for all x, y in X xRy <=> yRx
int is_symmetric(const pair* relation, int n) {
    bool found;
    for (int i = 0; i < n; i++) {
        found = false;
        for (int j = 0; j < n; j++)
            if (relation[i].first == relation[j].second && relation[i].second == relation[j].first) {
                found = true;
                break;
            }
        if (!found) return 0;
    }
    return 1;
}

// A binary relation R over a set X is antisymmetric if:
// for all x,y in X if xRy and yRx then x=y
int is_antisymmetric(const pair* relation, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (relation[i].first == relation[j].second && relation[i].second == relation[j].first
                && relation[i].first != relation[i].second)
                return 0;
    return 1;
}

// A binary relation R over a set X is asymmetric if:
// for all x,y in X, if xRy then not yRx
int is_asymmetric(const pair* relation, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (relation[i].first == relation[j].second && relation[i].second == relation[j].first)
                return 0;
    return 1;
}

// A homogeneous relation R on the set X is a transitive relation if:
// for all x, y, z in X, if xRy and yRz, then xRz
int is_transitive(const pair* relation, int n) {
    bool found;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (relation[j].first == relation[i].second) {
                found = false;
                for (int k = 0; k < n; k++)
                    if (relation[i].first == relation[k].first && relation[j].second == relation[k].second)
                        found = true;
                if (!found) return 0;
            }
    return 1;
}

// Case 2:

// A partial order relation is a homogeneous relation that is
// reflexive, transitive, and antisymmetric
int is_partial_order(const pair* relation, int n) {
    return is_reflexive(relation, n) && is_transitive(relation, n) && is_antisymmetric(relation, n);
}

int is_connected(const pair*, int);

// A total order relation is a partial order relation that is connected
int is_total_order(const pair* relation, int n) {
    return is_partial_order(relation, n) && is_connected(relation, n);
}

bool search(const pair* relation, const int n, const int x, const int y) {
    bool found = false;
    for (int k = 0; k < n; k++)
        if ((relation[k].first == x && relation[k].second == y) || (relation[k].first == y && relation[k].second == x))
            found = true;
    return found;
}

// Relation R is connected if for each x, y in X:
// xRy or yRx (or both)
int is_connected(const pair* relation, int n) {
    int x, y;
    for (int i = 0; i < n; i++) {
        x = relation[i].first;
        for (int j = 0; j < n; j++) {
            y = relation[j].first;
            if (!search(relation, n, x, y)) return 0;
            y = relation[j].second;
            if (!search(relation, n, x, y)) return 0;
        }
        x = relation[i].second;
        for (int j = 0; j < n; j++) {
            y = relation[j].first;
            if (!search(relation, n, x, y)) return 0;
            y = relation[j].second;
            if (!search(relation, n, x, y)) return 0;
        }
    }
    return 1;
}

bool in_array(const int array[], const int n, const int x) {
    for (int i = 0; i < n; i++)
        if (array[i] == x) return true;
    return false;
}

// Fills an array and returns its size
int find_max_elements(const pair* relation, int n, int* max_elements) {
    int g;
    int no_elements = 0;
    bool is_max;
    for (int i = 0; i < n; i++) {
        g = relation[i].second;
        is_max = true;
        for (int j = 0; j < n; j++)
            if (relation[j].first == g && relation[j].second != g) {
                is_max = false;
                break;
            }
        if (is_max && !in_array(max_elements, no_elements, g)) {
            max_elements[no_elements] = g;
            no_elements++;
        }
    }
    return no_elements;
}

// Fills an array and returns its size
int find_min_elements(const pair* relation, int n, int* min_elements) {
    int m;
    int no_elements = 0;
    bool is_min;
    for (int i = 0; i < n; i++) {
        m = relation[i].first;
        is_min = true;
        for (int j = 0; j < n; j++)
            if (relation[j].second == m && relation[j].first != m) {
                is_min = false;
                break;
            }
        if (is_min && !in_array(min_elements, no_elements, m)) {
            min_elements[no_elements] = m;
            no_elements++;
        }
    }
    return no_elements;
}

void add_to_domain(int domain[], int* no_elements, const int x) {
    for (int i = 0; i < *no_elements; i++)
        if (domain[i] == x) return;
    domain[*no_elements] = x;
    *no_elements += 1;
}

int int_cmp (const void *a, const void *b) {
    const int *ia = (const int *)a; // casting pointer types
    const int *ib = (const int *)b;
    return *ia  - *ib;
}

// Fills an array and returns its size
int get_domain(const pair* relation, int n, int* domain) {
    int no_elements = 0;
    int x;
    for (int i = 0; i < n; i++)
        add_to_domain(domain, &no_elements, relation[i].first);
    for (int i = 0; i < n; i++)
        add_to_domain(domain, &no_elements, relation[i].second);
    qsort(domain, no_elements, sizeof(int), int_cmp);
    return no_elements;
}

// Case 3:

int composition(const pair* relation_1, int n_1, const pair* relation_2, int n_2, pair* comp_relation) {
    int no_elements = 0;
    pair new_pair;
    for (int i = 0; i < n_1; i++)
        for (int j = 0; j < n_2; j++)
            if (relation_2[j].first == relation_1[i].second) {
                new_pair.first = relation_1[i].first;
                new_pair.second = relation_2[j].second;
                no_elements = add_relation(comp_relation, no_elements, new_pair);
            }
    return no_elements;
}

// Add pair to existing relation if not already there
// Return new (or old) length of the array
int add_relation(pair* relation, const int n, const pair new_pair) {
    for (int i = 0; i < n; i++)
        if (relation[i].first == new_pair.first && relation[i].second == new_pair.second)
            return n;
    relation[n] = new_pair;
    return n + 1;
}

// Read number of pairs, n, and then n pairs of ints
// Pair uniqueness guaranteed
int read_relation(pair* relation) {
    int n;
    pair new;
    scanf("%d", &n);
    int size = 0;
    while (size < n) {
        scanf("%d %d", &new.first, &new.second);
        size = add_relation(relation, size, new);
    }
    return n;
}

void print_int_array(const int* array, int n) {
    printf("%d\n", n);
    for (int i = 0; i < n; i++)
        printf("%d ", array[i]);
    printf("\n");
}

int main(void) {
    int to_do;
    pair relation[MAX_REL_SIZE];
    pair relation_2[MAX_REL_SIZE];
    pair comp_relation[MAX_REL_SIZE];
    int domain[MAX_REL_SIZE];
    int max_elements[MAX_REL_SIZE];
    int min_elements[MAX_REL_SIZE];

    scanf("%d", &to_do);
    int size = read_relation(relation);
    int ordered, size_2, n_domain;

    switch (to_do) {
        case 1:
            printf("%d ", is_reflexive(relation, size));
            printf("%d ", is_irreflexive(relation, size));
            printf("%d ", is_symmetric(relation, size));
            printf("%d ", is_antisymmetric(relation, size));
            printf("%d ", is_asymmetric(relation, size));
            printf("%d\n", is_transitive(relation, size));
            break;
        case 2:
            ordered = is_partial_order(relation, size);
            n_domain = get_domain(relation, size, domain);
            printf("%d %d\n", ordered, is_total_order(relation, size));
            print_int_array(domain, n_domain);
            if (!ordered) break;
            int no_max_elements = find_max_elements(relation, size, max_elements);
            int no_min_elements = find_min_elements(relation, size, min_elements);
            print_int_array(max_elements, no_max_elements);
            print_int_array(min_elements, no_min_elements);
            break;
        case 3:
            size_2 = read_relation(relation_2);
            printf("%d\n", composition(relation, size, relation_2, size_2, comp_relation));
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
            break;
    }
    return 0;
}

