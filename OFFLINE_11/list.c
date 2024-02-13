#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

struct List;

typedef void (* DataFp)(void*);

typedef void (* ConstDataFp)(const void*);

typedef int (* CompareDataFp)(const void*, const void*);

typedef struct ListElement {
    struct ListElement* next;
    void* data;
} ListElement;

typedef struct {
    ListElement* head;
    ListElement* tail;
    ConstDataFp dump_data;
    DataFp free_data;
    CompareDataFp compare_data;
    DataFp modify_data;
} List;

void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr) return ptr;
    printf("malloc error\n");
    exit(EXIT_FAILURE);
}

void* safe_strdup(const char* string) {
    void* ptr = strdup(string);
    if (ptr) return ptr;
    printf("strdup error\n");
    exit(EXIT_FAILURE);
}

// --- generic functions --- for any data type

void init_list(List* p_list, ConstDataFp dump_data, DataFp free_data,
               CompareDataFp compare_data, DataFp modify_data) {
    p_list->head = NULL;
    p_list->tail = NULL;
    p_list->dump_data = dump_data;
    p_list->free_data = free_data;
    p_list->compare_data = compare_data;
    p_list->modify_data = modify_data;
}

// Print elements of the list
void dump_list(const List* p_list) {
    for (ListElement* ptr = p_list->head; ptr != NULL; ptr = ptr->next)
        p_list->dump_data(ptr->data);
    printf("\n");
}

// Print elements of the list if comparable to data
void dump_list_if(List* p_list, void* data) {
    for (ListElement* ptr = p_list->head; ptr != NULL; ptr = ptr->next)
        if (!p_list->compare_data(ptr->data, data))
            p_list->dump_data(ptr->data);
    printf("\n");
}

// Free element pointed by data using free_data() function
void free_element(DataFp free_data, ListElement* to_delete) {
    if (free_data != NULL)
        free_data(to_delete->data);
    free(to_delete);
}

// Free all elements of the list
void free_list(List* p_list) {
    ListElement* next_ptr;
    for (ListElement* ptr = p_list->head; ptr != NULL; ptr = next_ptr) {
        next_ptr = ptr->next;
        free_element(p_list->free_data, ptr);
    }
    p_list->head = NULL;
    p_list->tail = NULL;
}

bool already_there(List* p_list, void* data) {
    for (ListElement* ptr = p_list->head; ptr != NULL; ptr = ptr->next)
        if (!p_list->compare_data(ptr->data, data)) {
            if (p_list->modify_data != NULL)
                p_list->modify_data(ptr->data);
            return true;
        }
    return false;
}

// Push element at the beginning of the list
void push_front(List* p_list, void* data) {
    //if (already_there(p_list, data)) return;
    ListElement* new = safe_malloc(sizeof(ListElement));
    new->next = p_list->head;
    new->data = data;
    if (p_list->head == NULL)
        p_list->tail = new;
    p_list->head = new;
}

// Push element at the end of the list
void push_back(List* p_list, void* data) {
    //if (already_there(p_list, data)) return;
    ListElement* new = safe_malloc(sizeof(ListElement));
    new->next = NULL;
    new->data = data;
    if (p_list->head == NULL)
        p_list->head = new;
    else
        p_list->tail->next = new;
    p_list->tail = new;
}

// Remove the first element
void pop_front(List* p_list) {
    free_element(p_list->free_data, p_list->head);
    p_list->head = p_list->head->next;
}

// Reverse the list
void reverse(List* p_list) {
    ListElement* ptr = p_list->head;
    ListElement* first = ptr->next;
    ListElement* second = ptr->next;
    ptr->next = NULL;
    while (first != NULL) {
        second = second->next;
        first->next = ptr;
        ptr = first;
        first = second;
    }
    ListElement* tmp = p_list->head;
    p_list->head = p_list->tail;
    p_list->tail = tmp;
}

// find element in sorted list after which to insert given element
ListElement* find_insertion_point(const List* p_list, ListElement* p_element) {
    if (p_list->head == NULL) return NULL; // Pusta lista
    if (p_list->compare_data(p_list->head->data, p_element->data) > 0) return NULL; // Na początek
    for (ListElement* ptr = p_list->head; ptr != p_list->tail; ptr = ptr->next)
        if (p_list->compare_data(ptr->next->data, p_element->data) > 0)
            return ptr; // Gdzieś w środku
    return p_list->tail; // Na koniec
}

// Insert element after 'previous'
void push_after(List* p_list, void* data, ListElement* previous) {
    if (previous == NULL) push_front(p_list, data);
    else {
        ListElement* new = safe_malloc(sizeof(ListElement));
        new->next = previous->next;
        new->data = data;
        previous->next = new;
    }
}

// Insert element preserving order
void insert_in_order(List* p_list, void* p_data) {
    if (already_there(p_list, p_data)) return;
    ListElement* new = safe_malloc(sizeof(ListElement));
    new->next = NULL;
    new->data = p_data;
    push_after(p_list, p_data, find_insertion_point(p_list, new));
}

// -----------------------------------------------------------
// --- type-specific definitions

// int element

void dump_int(const void* d) {
    printf("%d ", *((int*) d));
}

void free_int(void* d) {
    free(d);
}

int cmp_int(const void* a, const void* b) {
    const int* first = a;
    const int* second = b;
    return *first - *second;
}

int* create_data_int(int v) {
    int* ptr = safe_malloc(sizeof(int));
    *ptr = v;
    return ptr;
}

// Word element

typedef struct DataWord {
    char* word;
    int counter;
} DataWord;

void dump_word(const void* d) {
    printf("%s ", ((DataWord*) d)->word);
}

void dump_word_lowercase(const void* d) {
    const char* word = ((DataWord*) d)->word;
    for (int i = 0; word[i] != '\0'; i++)
        printf("%c", tolower(word[i]));
    printf(" ");
}

void free_word(void* d) {
    free(((DataWord*) d)->word);
    free(d);
}

int cmp_word_alphabet(const void* a, const void* b) {
    return strcasecmp(((DataWord*) a)->word, ((DataWord*) b)->word);
}

int cmp_word_counter(const void* a, const void* b) {
    return cmp_int(&((DataWord*) a)->counter, &((DataWord*) b)->counter);
}

void modify_word(void* p) {
    ((DataWord*) p)->counter++;
}

void* create_data_word(const char* string, int counter) {
    DataWord* ptr = safe_malloc(sizeof(DataWord));
    ptr->word = safe_strdup(string);
    ptr->counter = counter; // Po co ten parametr? Nie można zawsze 1?
    return ptr;
}

// read text, parse it to words, and insert those words to the list.
// Order of insertions is given by the last parameter of type CompareDataFp.
// (comparator function address). If this address is not NULL the element is
// inserted according to the comparator. Otherwise, read order is preserved.
void stream_to_list(List* p_list, FILE* stream, CompareDataFp cmp) {
    char buff[BUFFER_SIZE] = {0};
    char delim[] = " \n\t\r\v\f.,?!:;-";
    p_list->compare_data = cmp;
    while (fgets(buff, BUFFER_SIZE, stream) != NULL) {
        for (char* str = strtok(buff, delim); str != NULL; str = strtok(NULL, delim)) {
            if (strlen(str) == 0) continue;
            DataWord* data = create_data_word(str, 1);
            if (cmp == NULL) push_back(p_list, data);
            else insert_in_order(p_list, data);
        }
    }
}

// test integer list
void list_test(List* p_list, int n) {
    char op;
    int v;
    for (int i = 0; i < n; ++i) {
        scanf(" %c", &op);
        switch (op) {
            case 'f':
                scanf("%d", &v);
                push_front(p_list, create_data_int(v));
                break;
            case 'b':
                scanf("%d", &v);
                push_back(p_list, create_data_int(v));
                break;
            case 'd':
                pop_front(p_list);
                break;
            case 'r':
                reverse(p_list);
                break;
            case 'i':
                scanf("%d", &v);
                insert_in_order(p_list, create_data_int(v));
                break;
            default:
                printf("No such operation: %c\n", op);
                break;
        }
    }
}

int main(void) {
    int to_do, n;
    List list;

    scanf("%d", &to_do);
    switch (to_do) {
        case 1: // test integer list
            scanf("%d", &n);
            init_list(&list, dump_int, free_int, cmp_int, NULL);
            list_test(&list, n);
            dump_list(&list);
            free_list(&list);
            break;
        case 2: // read words from text, insert into list, and print
            init_list(&list, dump_word, free_word, NULL, NULL);
            stream_to_list(&list, stdin, NULL);
            dump_list(&list);
            free_list(&list);
            break;
        case 3: // read words, insert into list alphabetically, print words encountered n times
            scanf("%d", &n);
            init_list(&list, dump_word_lowercase, free_word, NULL, modify_word);
            stream_to_list(&list, stdin, cmp_word_alphabet);
            list.compare_data = cmp_word_counter;
            DataWord data = {NULL, n};
            dump_list_if(&list, &data);
            free_list(&list);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
            break;
    }
    return 0;
}

