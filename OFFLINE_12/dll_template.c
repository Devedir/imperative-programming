#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// list node
typedef struct Node {
    int* data;
    size_t array_size;
    struct Node* next;
    struct Node* prev;
} Node;

// doubly linked list
typedef struct List {
    Node* head;
    Node* tail;
} List;

// iterator
typedef struct iterator {
    struct Node* node_ptr;
    size_t position;
} iterator;

// forward initialization
iterator begin(Node* head) {
    iterator it = {head, 0};
    return it;
}

// backward initialization;
// points to the element following the last one
iterator end(Node* tail) {
    iterator it = {tail, tail->array_size};
    return it;
}

void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr) return ptr;
    exit(EXIT_FAILURE);
}

void* safe_realloc(void* old_ptr, size_t size) {
    void* ptr = realloc(old_ptr, size);
    if (ptr) return ptr;
    free(old_ptr);
    exit(EXIT_FAILURE);
}

Node* create_node(int* data, size_t array_size, Node* next, Node* prev) {
    Node* node = safe_malloc(sizeof(Node));
    node->data = data;
    node->array_size = array_size;
    node->next = next;
    node->prev = prev;
    return node;
}

// initialize list
// creates the front and back sentinels
void init(List* list) {
    list->head = create_node(NULL, 0, NULL, NULL);
    list->tail = create_node(NULL, 0, NULL, list->head);
    list->head->next = list->tail;
}

// to implement ...

// append node to the list
void push_back(List* list, int* data, size_t array_size) {
    Node* new = create_node(data, array_size, list->tail, list->tail->prev);
    list->tail->prev->next = new;
    list->tail->prev = new;
}

// set iterator to move n elements forward from its current position
void skip_forward(iterator* itr, size_t n) {
    size_t remaining = n;
    while (remaining > itr->node_ptr->array_size) {
        remaining -= itr->node_ptr->array_size;
        itr->node_ptr = itr->node_ptr->next;
    }
    itr->position = remaining;
}

// forward iteration - get n-th element in the list
int get_forward(List* list, size_t n) {
    iterator itr = begin(list->head);
    skip_forward(&itr, n);
    return itr.node_ptr->data[itr.position - 1];
}

// set iterator to move n elements backward from its current position
void skip_backward(iterator* itr, size_t n) {
    size_t remaining = n;
    while (remaining > itr->node_ptr->array_size) {
        remaining -= itr->node_ptr->array_size;
        itr->node_ptr = itr->node_ptr->prev;
    }
    itr->position = itr->node_ptr->array_size - remaining + 1;
}

// backward iteration - get n-th element from the end of the list
int get_backward(List* list, size_t n) {
    iterator itr = end(list->tail);
    skip_backward(&itr, n);
    return itr.node_ptr->data[itr.position - 1];
}

void remove_node(Node* node_ptr) {
    node_ptr->next->prev = node_ptr->prev;
    node_ptr->prev->next = node_ptr->next;
    free(node_ptr->data);
    free(node_ptr);
}

// remove n-th element; if array empty remove node
void remove_at(List* list, size_t n) {
    iterator itr = begin(list->head);
    skip_forward(&itr, n);
    if (itr.node_ptr->array_size == 1) {
        remove_node(itr.node_ptr);
    } else {
        memmove(&itr.node_ptr->data[itr.position - 1], &itr.node_ptr->data[itr.position],
                sizeof(int) * (itr.node_ptr->array_size - itr.position));
        itr.node_ptr->array_size -= 1;
    }
}

// return the number of digits of number n
size_t digits(int n) {
    if (n == 0) return 1;
    size_t result = 0;
    while (n != 0) {
        n /= 10;
        result += 1;
    }
    return result;
}

void insert_new(Node* ptr, int value) {
    int* array = safe_malloc(sizeof(int));
    *array = value;
    Node* new = create_node(array, 1, ptr, ptr->prev);
    ptr->prev->next = new;
    ptr->prev = new;
}

// inserts 'value' to the node with the same digits' count
// otherwise insert new node
void put_in_order(List* list, int value) {
    size_t val_digits = digits(value);
    Node* ptr = list->head->next;
    if (ptr == list->tail) {
        insert_new(ptr, value);
        return;
    }
    while (ptr != list->tail && digits(ptr->data[0]) < val_digits)
        ptr = ptr->next;
    if (ptr != list->tail && digits(ptr->data[0]) == val_digits) {
        ptr->data = safe_realloc(ptr->data, sizeof(int) * (ptr->array_size + 1));
        int idx = 0;
        while (idx < ptr->array_size && ptr->data[idx] < value)
            idx++;
        if (idx == ptr->array_size) ptr->data[idx] = value;
        else {
            memmove(&ptr->data[idx + 1], &ptr->data[idx], sizeof(int) * (ptr->array_size - idx));
            ptr->data[idx] = value;
        }
        ptr->array_size += 1;
    } else insert_new(ptr, value);
    printf("");
}

// -------------------------------------------------------------
// helper functions

// print list
void dumpList(const List* list) {
    for (Node* node = list->head->next; node != list->tail; node = node->next) {
        printf("-> ");
        for (int k = 0; k < node->array_size; k++) {
            printf("%d ", node->data[k]);
        }
        printf("\n");
    }
}

// free list
void freeList(List* list) {
    Node* to_delete = list->head->next, * next;
    while (to_delete != list->tail) {
        next = to_delete->next;
        remove_node(to_delete);
        to_delete = next;
    }
}

// read int vector
void read_vector(int tab[], size_t n) {
    for (size_t i = 0; i < n; ++i) {
        scanf("%d", tab + i);
    }
}

// initialize the list and push data
void read_list(List* list) {
    int n;
    size_t size;
    scanf("%d", &n); // number of nodes
    for (int i = 0; i < n; i++) {
        scanf("%zu", &size); // length of the array in i-th node
        int* tab = (int*) safe_malloc(size * sizeof(int));
        read_vector(tab, size);
        push_back(list, tab, size);
    }
}

int main() {
    int to_do, value;
    size_t size, m;
    List list;
    init(&list);

    scanf("%d", &to_do);
    switch (to_do) {
        case 1:
            read_list(&list);
            dumpList(&list);
            break;
        case 2:
            read_list(&list);
            scanf("%zu", &size);
            for (int i = 0; i < size; i++) {
                scanf("%zu", &m);
                printf("%d ", get_forward(&list, m));
            }
            printf("\n");
            break;
        case 3:
            read_list(&list);
            scanf("%zu", &size);
            for (int i = 0; i < size; i++) {
                scanf("%zu", &m);
                printf("%d ", get_backward(&list, m));
            }
            printf("\n");
            break;
        case 4:
            read_list(&list);
            scanf("%zu", &size);
            for (int i = 0; i < size; i++) {
                scanf("%zu", &m);
                remove_at(&list, m);
            }
            dumpList(&list);
            break;
        case 5:
            scanf("%zu", &size);
            for (int i = 0; i < size; i++) {
                scanf("%d", &value);
                put_in_order(&list, value);
            }
            dumpList(&list);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
            break;
    }
    freeList(&list);

    return 0;
}
