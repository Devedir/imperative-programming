#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#define QUEUE_SIZE 52

enum state { OK = 0, UNDERFLOW = -1, OVERFLOW = -2 };

int rand_from_interval(int a, int b) {
    if (a == b)
        return a;
    else if (a > b)
        return INT_MIN;
    else if (b - a > RAND_MAX)
        return INT_MAX;
    else
        return (rand() % (b - a + 1)) + a;
}

void swap (int array[], int i, int k) {
    int tmp = array[i];
    array[i] = array[k];
    array[k] = tmp;
}

void rand_permutation(int n, int array[]) {
    for (int i = 0; i < n; i++)
        array[i] = i;
    int k;
    for (int i = 0; i < n-1; i++) {
        k = rand_from_interval(i, n - 1);
        swap(array, i, k);
    }
}


struct holder {
    int deck[QUEUE_SIZE];
    int out;
    int len;
};


int queue_push(struct holder* who, int val) {
    if (who->len == QUEUE_SIZE)
        return OVERFLOW;
    who->deck[(who->out + who->len) % QUEUE_SIZE] = val;
    who->len++;
    return OK;
}

int queue_pop(struct holder* who) {
    if (who->len == 0)
        return UNDERFLOW;
    who->out++;
    who->out %= QUEUE_SIZE;
    who->len--;
    return who->deck[who->out - 1];
}

int queue_state(struct holder* who) {
    return who->len;
}

void queue_print(struct holder* who) {
    for (int i = who->out; i < who->out + who->len; i++)
        printf("%d ", who->deck[i % QUEUE_SIZE]);
}


void unresolved(int code, struct holder* player_A, struct holder* player_B, struct holder* table_A, struct holder* table_B) {
    printf("%d ", code);
    printf("%d ", queue_state(player_A) + queue_state(table_A));
    printf("%d", queue_state(player_B) + queue_state(table_B));
}


void win_A(int conflicts) {
    printf("2 %d", conflicts);
}


void win_B(struct holder* player_B) {
    printf("3\n");
    queue_print(player_B);
}


int peek(struct holder* who) {
    return who->deck[who->out];
}


void collect(struct holder* who, struct holder* own_table, struct holder* other_table) {
    while (own_table->len != 0)
        queue_push(who, queue_pop(own_table));
    while (other_table->len != 0)
        queue_push(who, queue_pop(other_table));
}


int main(void) {
    int seed, version, conflict_limit;
    scanf("%d %d %d", &seed, &version, &conflict_limit);
    srand((unsigned) seed);

    int talia[QUEUE_SIZE];
    for (int i = 0; i < QUEUE_SIZE; i++)
        talia[i] = i;
    rand_permutation(QUEUE_SIZE, talia);

    struct holder player_A = {{}, 0, 0};
    struct holder player_B = {{}, 0, 0};
    struct holder table_A = {{}, 0, 0};
    struct holder table_B = {{}, 0, 0};

    for (int i = 0; i < QUEUE_SIZE / 2; i++) {
        queue_push(&player_A, talia[i]);
        queue_push(&player_B, talia[i + (QUEUE_SIZE / 2)]);
    }

    int conflict = 0;
    while (conflict < conflict_limit) {
        queue_push(&table_A, queue_pop(&player_A));
        queue_push(&table_B, queue_pop(&player_B));
        if (peek(&table_A) / 4 > peek(&table_B) / 4) { // Konflikt wygrywa gracz A
            collect(&player_A, &table_A, &table_B);
            if (player_B.len == 0) {
                win_A(conflict + 1);
                return 0;
            }
        } else if (peek(&table_A) / 4 < peek(&table_B) / 4) { // Konflikt wygrywa gracz B
            collect(&player_B, &table_B, &table_A);
            if (player_A.len == 0) {
                win_B(&player_B);
                return 0;
            }
        } else { // Wojna
            switch (version) {
                case 1: // Uproszczona
                    queue_push(&player_A, queue_pop(&table_A));
                    queue_push(&player_B, queue_pop(&table_B));
                    break;
                case 0: // Standardowa
                    if (player_A.len == 0 || player_B.len == 0) {
                        unresolved(1, &player_A, &player_B, &table_A, &table_B);
                        return 0;
                    }
                    queue_push(&table_A, queue_pop(&player_A));
                    queue_push(&table_B, queue_pop(&player_B));
                    break;
                default:
                    return -1;
            }
        }
        conflict++;
    }

    // Brak zwyciestwa po maksymalnej liczbie konfliktów
    unresolved(0, &player_A, &player_B, &table_A, &table_B);
    return 0;
}
