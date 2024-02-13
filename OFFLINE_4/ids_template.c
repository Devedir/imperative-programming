#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define IN_LINE_COMMENT 1
#define IN_BLOCK_COMMENT 2
#define IN_STRING 4
#define IN_ID 8

#define MAX_ID_LEN 64
#define MAX_IDS 1024
#define MAX_LINE 126

int index_cmp(const void*, const void*);

int cmp(const void*, const void*);

char identifiers[MAX_IDS][MAX_ID_LEN];

const char* keywords[] = {
        "auto", "break", "case", "char",
        "const", "continue", "default", "do",
        "double", "else", "enum", "extern",
        "float", "for", "goto", "if",
        "int", "long", "register", "return",
        "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union",
        "unsigned", "void", "volatile", "while"
};

bool is_white(int c) {
    return c == ' ' || c == '\t' || c == '\n';
}

bool is_letter(int c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

bool is_number(int c) {
    return c >= '0' && c <= '9';
}

void checks(int* cur, int* prev, int* block_len, bool* ignore_text, bool* ignore_line, bool* ignore_block) {
    if (*cur == '\"') *ignore_text = !*ignore_text;
    bool ignoring = *ignore_text || *ignore_block || *ignore_line;
    if (!ignoring && *prev == '/' && *cur == '/') *ignore_line = true;
    else if (*cur == '\n') {
        *ignore_line = false;
        *ignore_text = false;
    } else if (!ignoring && *prev == '/' && *cur == '*') {
        *ignore_block = true;
        *block_len = 2;
    } else if (*block_len > 2 && *prev == '*' && *cur == '/') {
        *ignore_block = false;
        *block_len = 0;
    }
}

bool my_cmp(char w1[], char w2[]) {
    if (strlen(w1) != strlen(w2)) return false;
    return !strcmp(w1, w2);
}

bool good(char word[], int found) {
    for (int i = 0; i < found; i++)
        if (my_cmp(word, identifiers[i])) return false;
    for (int i = 0; i < 32; i++)
        if (my_cmp(word, keywords[i])) return false;
    return true;
}

int find_idents() {
    int found = 0;
    char word[MAX_ID_LEN] = {'\0'};
    int word_len = 0;
    int c = ' ';
    int prev = ' ';
    int block_len = 0;
    int idx;
    bool ignore_text = false;
    bool ignore_line = false;
    bool ignore_block = false;
    while (c != EOF) {
        do {
            prev = c;
            c = getchar();
            if (c == EOF) return found;
        } while (is_white(c));
        while (c == '\"') {
            do {
                prev = c;
                c = getchar();
                if (c == EOF) return found;
            } while (c != '\"');
            c = getchar();
        }
        if (is_white(c)) continue;
        while (!is_letter(c) || ignore_text || ignore_line || ignore_block) {
            if (c == EOF) return found;
            if (c == '\'') {
                do {
                    prev = c;
                    c = getchar();
                    if (c == EOF) return found;
                } while (c != '\'');
            }
            checks(&c, &prev, &block_len, &ignore_text, &ignore_line, &ignore_block);
            prev = c;
            c = getchar();
            if (block_len == 2 && c != '/') block_len++;
        }

        // Wreszcie potencjalna litera początkowa
        idx = 0;
        while (is_letter(c) || is_number(c)) {
            word[idx] = (char)c;
            word_len++;
            idx++;
            prev = c;
            c = getchar();
            if (c == EOF) return found;
        }
        if (good(word, found)) {
            for (int i = 0; i < word_len; i++)
                identifiers[found][i] = word[i];
            found++;
        }
        for (int i = 0; i < word_len; i++) word[i] = '\0';
        word_len = 0;
        c = prev;
    }
    return found;
}

int cmp(const void* first_arg, const void* second_arg) {
    char* a = *(char**) first_arg;
    char* b = *(char**) second_arg;
    return strcmp(a, b);
}

int index_cmp(const void* first_arg, const void* second_arg) {
    int a = *(int*) first_arg;
    int b = *(int*) second_arg;
    return strcmp(identifiers[a], identifiers[b]);
}

int main(void) {
    printf("%d\n", find_idents());
    return 0;
}

