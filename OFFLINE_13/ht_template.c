#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define BUFFER_SIZE 1024
#define MAX_RATE 4
#define MEMORY_ALLOCATION_ERROR (-1)

typedef union {
    int int_data;
    char char_data;
    // ... other primitive types used
    void* ptr_data;
} data_union;

typedef struct ht_element {
    struct ht_element* next;
    data_union data;
} ht_element;

typedef void (* DataFp)(data_union);

typedef void (* DataPFp)(data_union*);

typedef int (* CompareDataFp)(data_union, data_union);

typedef size_t (* HashFp)(data_union, size_t);

typedef data_union (* CreateDataFp)(void*);

typedef struct {
    size_t size;
    size_t no_elements;
    ht_element* ht;
    DataFp dump_data;
    CreateDataFp create_data;
    DataFp free_data;
    CompareDataFp compare_data;
    HashFp hash_function;
    DataPFp modify_data;
} hash_table;

// ---------------------- functions to implement

void* smalloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr) return ptr;
    exit(MEMORY_ALLOCATION_ERROR);
}

void* scalloc(size_t nmemb, size_t size) {
    void* ptr = calloc(nmemb, size);
    if (ptr) return ptr;
    exit(MEMORY_ALLOCATION_ERROR);
}

// initialize table fields
void init_ht(hash_table* p_table, size_t size, DataFp dump_data, CreateDataFp create_data,
             DataFp free_data, CompareDataFp compare_data, HashFp hash_function, DataPFp modify_data) {
    p_table->ht = scalloc(size, sizeof(ht_element));
    p_table->size = size;
    p_table->no_elements = 0;
    p_table->dump_data = dump_data;
    p_table->create_data = create_data;
    p_table->free_data = free_data;
    p_table->compare_data = compare_data;
    p_table->hash_function = hash_function;
    p_table->modify_data = modify_data;
}

// print elements of the list with hash n
void dump_list(const hash_table* p_table, size_t n) {
    for (ht_element* ptr = p_table->ht[n].next; ptr != NULL; ptr = ptr->next)
        p_table->dump_data(ptr->data);
}

// Free element pointed by data_union using free_data() function
void free_element(DataFp free_data, ht_element* to_delete) {
    if (free_data != NULL) free_data(to_delete->data);
    free(to_delete);
}

// free all elements from the table (and the table itself)
void free_table(hash_table* p_table) {
    for (int i = 0; i < p_table->size; i++) {
        ht_element* next;
        for (ht_element* ptr = p_table->ht[i].next; ptr != NULL; ptr = next) {
            next = ptr->next;
            free_element(p_table->free_data, ptr);
        }
    }
    free(p_table->ht);
}

// calculate hash function for integer k
size_t hash_base(int k, size_t size) {
    static const double c = 0.618033988; // (sqrt(5.) – 1) / 2.;
    double tmp = k * c;
    return (size_t) floor((double) size * (tmp - floor(tmp)));
}

void rehash(hash_table* p_table) {
    size_t new_size = p_table->size * 2;
    ht_element* new = scalloc(new_size, sizeof(ht_element));
    for (size_t i = 0; i < p_table->size; i++) {
        ht_element* ptr = &p_table->ht[i];
        while (ptr->next != NULL) {
            size_t n = p_table->hash_function(ptr->next->data, new_size);
            ht_element* tmp = ptr->next->next;
            ptr->next->next = new[n].next;
            new[n].next = ptr->next;
            ptr->next = tmp;
        }
    }
    p_table->ht = new;
    p_table->size = new_size;
}

// find element; return pointer to previous
ht_element* find_previous(hash_table* p_table, data_union data) {
    size_t n = p_table->hash_function(data, p_table->size);
    for (ht_element* ptr = &p_table->ht[n]; ptr->next != NULL; ptr = ptr->next)
        if (!p_table->compare_data(ptr->next->data, data))
            return ptr;
    return NULL;
}

// return pointer to element with given value
ht_element* get_element(hash_table* p_table, data_union* data) {
    size_t n = p_table->hash_function(*data, p_table->size);
    for (ht_element* ptr = p_table->ht[n].next; ptr != NULL; ptr = ptr->next)
        if (!p_table->compare_data(ptr->data, *data))
            return ptr;
    return NULL;
}

// insert element
void insert_element(hash_table* p_table, data_union* data) {
    size_t n = p_table->hash_function(*data, p_table->size);
    ht_element* ptr = &p_table->ht[n];
    ht_element* new = smalloc(sizeof(ht_element));
    new->data = *data;
    new->next = ptr->next;
    ptr->next = new;
    p_table->no_elements++;
    if (p_table->no_elements / p_table->size > MAX_RATE)
        rehash(p_table);
}

// remove element
void remove_element(hash_table* p_table, data_union data) {
    ht_element* prev = find_previous(p_table, data);
    if (prev == NULL) return;
    ht_element* to_delete = prev->next;
    prev->next = prev->next->next;
    free_element(p_table->free_data, to_delete);
    p_table->no_elements--;
}

// type-specific definitions

// int element

size_t hash_int(data_union data, size_t size) {
    return hash_base(data.int_data, size);
}

void dump_int(data_union data) {
    printf("%d ", data.int_data);
}

int cmp_int(data_union a, data_union b) {
    return a.int_data - b.int_data;
}

data_union create_int(void* value) {
    int val;
    scanf(" %d", &val);
    if (value != NULL) {
        data_union* new = (data_union*) value;
        new->int_data = val;
        return *new;
    }
    return (data_union) {.int_data = val};
}

// char element

size_t hash_char(data_union data, size_t size) {
    return hash_base((int) data.char_data, size);
}

void dump_char(data_union data) {
    printf("%c ", data.char_data);
}

int cmp_char(data_union a, data_union b) {
    return a.char_data - b.char_data;
}

data_union create_char(void* value) {
    char val;
    scanf(" %c", &val);
    if (value != NULL) {
        data_union* new = (data_union*) value;
        new->char_data = val;
        return *new;
    }
    return (data_union) {.char_data = val};
}

// Word element

typedef struct DataWord {
    char* word;
    int counter;
} DataWord;

void dump_word(data_union data) {
    DataWord* ptr = data.ptr_data;
    printf("%s %d\n", ptr->word, ptr->counter);
}

void free_word(data_union data) {
    free(((DataWord*) data.ptr_data)->word);
    free(data.ptr_data);
}

int cmp_word(data_union a, data_union b) {
    return strcmp(((DataWord*) a.ptr_data)->word, ((DataWord*) b.ptr_data)->word);
}

size_t hash_word(data_union data, size_t size) {
    int s = 0;
    DataWord* dw = (DataWord*) data.ptr_data;
    char* p = dw->word;
    while (*p) {
        s += *p++;
    }
    return hash_base(s, size);
}

void modify_word(data_union* data) {
    ((DataWord*) data->ptr_data)->counter++;
}

data_union create_data_word(void* value) {
    DataWord* ptr = smalloc(sizeof(DataWord));
    char* str = value;
    ptr->word = smalloc(strlen(str) + sizeof(char));
    ptr->word = strcpy(ptr->word, str);
    ptr->counter = 1;
    return (data_union) {.ptr_data = ptr};
}

// read text, parse it to words, and insert these words to the hashtable
void stream_to_ht(hash_table* p_table, FILE* stream) {
    char buff[BUFFER_SIZE] = {0};
    char delim[] = " \n\t\r\v\f.,?!:;-";
    while (fgets(buff, BUFFER_SIZE, stream) != NULL) {
        for (char* str = strtok(buff, delim); str != NULL; str = strtok(NULL, delim)) {
            size_t len = strlen(str);
            if (len == 0) continue;
            for (int i = 0; i < len; i++) str[i] = (char) tolower(str[i]);
            data_union data = create_data_word(str);
            ht_element* found = get_element(p_table, &data);
            if (found) p_table->modify_data(&found->data);
            else insert_element(p_table, &data);
        }
    }
}

// test primitive type list
void test_ht(hash_table* p_table, int n) {
    char op;
    data_union data;
    for (int i = 0; i < n; ++i) {
        scanf(" %c", &op);
//		p_table->create_data(&data);
        data = p_table->create_data(NULL); // should give the same result as the line above
        switch (op) {
            case 'r':
                remove_element(p_table, data);
                break;
            case 'i':
                insert_element(p_table, &data);
                break;
            default:
                printf("No such operation: %c\n", op);
                break;
        }
    }
}

int main(void) {
    int to_do, n;
    size_t index;
    hash_table table;
    char buffer[BUFFER_SIZE];
    data_union data;

    scanf("%d", &to_do);
    switch (to_do) {
        case 1: // test integer hash table
            scanf("%d %zu", &n, &index);
            init_ht(&table, 4, dump_int, create_int, NULL, cmp_int, hash_int, NULL);
            test_ht(&table, n);
            printf("%zu\n", table.size);
            dump_list(&table, index);
            break;
        case 2: // test char hash table
            scanf("%d %zu", &n, &index);
            init_ht(&table, 4, dump_char, create_char, NULL, cmp_char, hash_char, NULL);
            test_ht(&table, n);
            printf("%zu\n", table.size);
            dump_list(&table, index);
            break;
        case 3: // read words from text, insert into hash table, and print
            scanf("%s", buffer);
            init_ht(&table, 8, dump_word, create_data_word, free_word, cmp_word, hash_word, modify_word);
            stream_to_ht(&table, stdin);
            printf("%zu\n", table.size);
            data = table.create_data(buffer);
            ht_element* e = get_element(&table, &data);
            if (e) table.dump_data(e->data);
            if (table.free_data) table.free_data(data);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
            break;
    }
    free_table(&table);

    return 0;
}

