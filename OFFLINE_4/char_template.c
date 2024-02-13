#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

// consider chars from [FIRST_CHAR, LAST_CHAR)
#define FIRST_CHAR 33
#define LAST_CHAR 127
#define MAX_CHARS (LAST_CHAR - FIRST_CHAR)
#define MAX_BIGRAMS ((LAST_CHAR - FIRST_CHAR) * (LAST_CHAR - FIRST_CHAR))

#define MAX_LINE 128

#define NEWLINE '\n'
#define IN_WORD 1

#define IN_LINE_COMMENT 1
#define IN_BLOCK_COMMENT 2

int count[MAX_BIGRAMS] = {0};

// sort indices according to their respective counts.
// sort alphabetically if counts equal
int cmp(const void* a, const void* b) {
    int va = *(int*) a;
    int vb = *(int*) b;
    if (count[va] == count[vb]) return va - vb;
    return count[vb] - count[va];
}

// sort indices according to their respective counts.
// sort alphabetically if counts equal
int cmp_di(const void* a, const void* b) {
    int va = *(int*) a;
    int vb = *(int*) b;
    // sort according to second char if counts and the first char equal
    if (count[va] == count[vb] && va / MAX_CHARS == vb / MAX_CHARS) return va % MAX_CHARS - vb % MAX_CHARS;
    // sort according to first char if counts equal
    if (count[va] == count[vb]) return va / MAX_CHARS - vb / MAX_CHARS;
    return count[vb] - count[va];
}

bool is_white(char c) {
    return c == ' ' || c == '\t' || c == NEWLINE;
}

bool is_good(char c) {
    return c >= FIRST_CHAR && c < LAST_CHAR;
}

// count lines, words & chars in a given text file
void wc(int* nl, int* nw, int* nc) {
    *nl = 0;
    *nw = 0;
    *nc = 0;
    char line[MAX_LINE];
    int i;
    while (fgets(line, MAX_LINE, stdin) != NULL) {
        for (i = 0; line[i] != NEWLINE; i++)
            if (i != 0 && is_white(line[i]) && !is_white(line[i - 1]))
                *nw += 1;
        *nw += 1;
        *nl += 1;
        *nc += i + 1;
    }
}

void char_count(int char_no, int* n_char, int* cnt) {
    char line[MAX_LINE];
    while (fgets(line, MAX_LINE, stdin) != NULL)
        for (int i = 0; line[i] != NEWLINE; i++)
            if (is_good(line[i]))
                count[line[i] - FIRST_CHAR]++;
    int indices[MAX_CHARS];
    for (int i = 0; i < MAX_CHARS; i++)
        indices[i] = i;
    qsort(indices, MAX_CHARS, sizeof(int), cmp);
    *n_char = indices[char_no - 1] + FIRST_CHAR;
    *cnt = count[indices[char_no - 1]];
}

void bigram_count(int bigram_no, int bigram[]) {
    char line[MAX_LINE];
    while (fgets(line, MAX_LINE, stdin) != NULL) {
        for (int i = 0; line[i + 1] != NEWLINE; i++)
            if (is_good(line[i]) && is_good(line[i + 1]))
                count[MAX_CHARS * (line[i] - FIRST_CHAR) + line[i + 1] - FIRST_CHAR]++;
    }
    int indices[MAX_BIGRAMS];
    for (int i = 0; i < MAX_BIGRAMS; i++)
        indices[i] = i;
    qsort(indices, MAX_BIGRAMS, sizeof(int), cmp_di);
    bigram[0] = indices[bigram_no - 1] / MAX_CHARS + FIRST_CHAR;
    bigram[1] = indices[bigram_no - 1] % MAX_CHARS + FIRST_CHAR;
    bigram[2] = count[indices[bigram_no - 1]];
}

void find_comments(int* line_comment_counter, int* block_comment_counter) {
    char line[MAX_LINE];
    bool inside_block = false;
    bool inside_oneliner = false;
    *line_comment_counter = 0;
    *block_comment_counter = 0;
    while (fgets(line, MAX_LINE, stdin) != NULL) {
        for (int i = 0; line[i + 1] != NEWLINE; i++) {
            if (line[i] == NEWLINE) break;
            if (!inside_block && !inside_oneliner && line[i] == '/') {
                switch (line[i + 1]) {
                    case '/':
                        inside_oneliner = true;
                        *line_comment_counter += 1;
                        break;
                    case '*':
                        inside_block = true;
                        *block_comment_counter += 1;
                        break;
                }
                i++;
            } else if (inside_block && line[i] == '*' && line[i + 1] == '/') {
                inside_block = false;
                i++;
            }
        }
        inside_oneliner = false;
    }
}

int read_int() {
    char line[MAX_LINE];
    fgets(line, MAX_LINE, stdin); // to get the whole line
    return (int) strtol(line, NULL, 10);
}

int main(void) {
    int to_do;
    int nl, nw, nc, char_no, n_char, cnt;
    int line_comment_counter, block_comment_counter;
    int bigram[3];

    to_do = read_int();
    switch (to_do) {
        case 1: // wc()
            wc(&nl, &nw, &nc);
            printf("%d %d %d\n", nl, nw, nc);
            break;
        case 2: // char_count()
            char_no = read_int();
            char_count(char_no, &n_char, &cnt);
            printf("%c %d\n", n_char, cnt);
            break;
        case 3: // bigram_count()
            char_no = read_int();
            bigram_count(char_no, bigram);
            printf("%c%c %d\n", bigram[0], bigram[1], bigram[2]);
            break;
        case 4:
            find_comments(&line_comment_counter, &block_comment_counter);
            printf("%d %d\n", block_comment_counter, line_comment_counter);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
            break;
    }
    return 0;
}

