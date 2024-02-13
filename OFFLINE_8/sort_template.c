#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int day;
    int month;
    int year;
} Date;

// 1 bsearch2

#define FOOD_MAX  30   // max. number of goods
#define ART_MAX   15   // max. length of name's name + 1
#define RECORD_MAX 40  // max. line length

typedef struct {
    char name[20];
    float price;
    int amount;
    Date valid_date;
} Food;

typedef int (* ComparFp)(const void*, const void*);

// cmp functions return positive ints if the first argument is "bigger", 0 if "equal" and negative if "smaller" than the second

// compare dates
int cmp_date(const void* d1, const void* d2) {
    const Date* first = (Date*) d1;
    const Date* second = (Date*) d2;
    int result = first->year - second->year;
    if (result != 0) return result;
    result = first->month - second->month;
    if (result != 0) return result;
    return first->day - second->day;
}

// compare foods
int cmp(const void* a, const void* b) {
    const Food* first = (Food*) a;
    const Food* second = (Food*) b;
    int result = strcmp(first->name, second->name);
    if (result != 0) return result;
    float f_result = first->price - second->price;
    if (first->price != second->price) {
        if (f_result > 0.0) return 1;
        else return -1;
    }
    return cmp_date(&first->valid_date, &second->valid_date);
}

// bsearch returning address where to insert new element
void* bsearch2(const void* key, const void* base, size_t nmemb, size_t size, ComparFp compar, char* result) {
    if (nmemb == 0) {
        *result = 0;
        return base;
    }
    int cmp_result;
    if (nmemb == 1) {
        cmp_result = compar(key, base);
        if (cmp_result == 0) {
            *result = 1;
            return base;
        }
        *result = 0;
        if (cmp_result < 0)
            return base;
        return base + size;
    }
    const void* half = base + (size * (nmemb / 2));
    cmp_result = compar(key, half);
    if (cmp_result == 0) {
        *result = 1;
        return half;
    } else if (cmp_result < 0)
        return bsearch2(key, base, nmemb / 2, size, compar, result);
    else
        return bsearch2(key, half + size, nmemb - (nmemb / 2) - 1, size, compar, result);
}

// print goods of given name
void print_art(Food* p, int n, char* art) {
    for (int i = 0; i < n; i++)
        if (!strcmp(p[i].name, art))
            printf("%.2f %d %02d.%02d.%04d\n", p[i].price, p[i].amount,
                   p[i].valid_date.day, p[i].valid_date.month, p[i].valid_date.year);
}

// add record to table if absent
Food* add_record(Food* tab, int* np, ComparFp compar, Food* new) {
    char found;
    Food* position = bsearch2(new, tab, *np, sizeof(Food), compar, &found);
    if (found) {
        position->amount += new->amount;
        return position;
    }
    for (int i = *np + 1; &tab[i] != position; i--)
        tab[i] = tab[i - 1];
    *position = *new;
    *np += 1;
    return position;
}

// read no lines of data
// calls add_record if sorted = 1 and just adds element if sorted = 0
int read_goods(Food* tab, int no, FILE* stream, int sorted) {
    int n = 0;
    for (int i = 0; i < no; i++) {
        Food new;
        fscanf(stream, "%s %f %d %d.%d.%d", new.name, &new.price, &new.amount,
               &new.valid_date.day, &new.valid_date.month, &new.valid_date.year);
        if (sorted) add_record(tab, &n, cmp, &new);
        else {
            tab[n] = new;
            n++;
        }
    }
    return n;
}

int cmp_qs(const void* a, const void* b) {
    Food* fa = (Food*) a, * fb = (Food*) b;
    return cmp_date(&fa->valid_date, &fb->valid_date);
}

int cmp_bs(const void* a, const void* b) {
    Date* pd = (Date*) a;
    Food* fb = (Food*) b;
    return cmp_date(pd, &fb->valid_date);
}

void add_days(struct tm* date, int days) {
    time_t timestamp = mktime(date);
    timestamp += days * 24 * 60 * 60;
    *date = *localtime(&timestamp);
}

// finds the value of goods due on 'curr_date'
float value(Food* food_tab, size_t n, Date curr_date, int days) {
    size_t food_size = sizeof(Food);
    qsort(food_tab, n, food_size, cmp_qs);
    struct tm t = {.tm_mday=curr_date.day, .tm_mon=curr_date.month - 1, .tm_year=curr_date.year - 1900};
    add_days(&t, days);
    Date expiry_date = {.day=t.tm_mday, .month=t.tm_mon + 1, .year=t.tm_year + 1900};
    Food* found_pos = bsearch(&expiry_date, food_tab, n, food_size, cmp_bs);
    float sum = found_pos->price * (float) found_pos->amount;
    for (Food* pos = found_pos - 1; !cmp_date(&pos->valid_date, &expiry_date); pos--)
        sum += pos->price * (float) pos->amount;
    for (Food* pos = found_pos + 1; !cmp_date(&pos->valid_date, &expiry_date); pos--)
        sum += pos->price * (float) pos->amount;
    return sum;
}

/////////////////////////////////////////////////////////////////
// 3 Succession

#define CHILD_MAX 20

enum Sex {
    F, M
};
enum BOOL {
    no, yes
};

struct Bit_data {
    enum Sex sex: 1;
    enum BOOL in_line: 1;
};

typedef struct {
    char* name;
    struct Bit_data bits;
    Date born;
    char* parent;
} Person;

typedef struct {
    char* par_name;
    int index;
    int no_children;
} Parent;

const Date primo_date = {28, 10, 2011}; // new succession act

int cmp_primo(Person* person1, Person* person2) {
    if (person1->bits.sex == person2->bits.sex) return 0;
    if (person1->bits.sex == F && cmp_date(&person1->born, &primo_date) > 0) return 0;
    if (person2->bits.sex == F && cmp_date(&person2->born, &primo_date) > 0) return 0;
    return person2->bits.sex - person1->bits.sex;
}

// compare persons
int cmp_person(const void* a, const void* b) {
    Person* first = (Person*) a;
    Person* second = (Person*) b;
    if (first->parent == NULL) return -1;
    if (second->parent == NULL) return 1;
    int result = strcmp(first->parent, second->parent);
    if (result == 0) return cmp_primo(first, second);
    return result;
}

int cmp_parent(const char* par1, const char* par2) {
    if (par1 == NULL) return -1;
    if (par2 == NULL) return 1;
    return strcmp(par1, par2);
}

int fill_indices_tab(Parent* idx_tab, const Person* pers_tab, int size) {
    int no_parents = 0;
    for (int i = 1; i < size; i++) {
        idx_tab[no_parents].par_name = pers_tab[i].parent;
        idx_tab[no_parents].index = i;
        idx_tab[no_parents].no_children = 1;
        while (!cmp_parent(pers_tab[i + 1].parent, idx_tab[no_parents].par_name)) {
            i++;
            idx_tab[no_parents].no_children++;
        }
        no_parents++;
    }
    return no_parents;
}

int cmp_par_bs(const void* a, const void* b) {
    char* name1 = (char*) a;
    char* name2 = ((Parent*) b)->par_name;
    return strcmp(name1, name2);
}

void persons_shifts(Person* person_tab, int size, Parent* idx_tab, int no_parents) {
    Person children[CHILD_MAX];
    for (int i = 0; i < size; i++) {
        Parent* parent = bsearch(person_tab[i].name, idx_tab, no_parents, sizeof(Parent), cmp_par_bs);
        if (parent == NULL) continue;
        for (int j = 0; j < parent->no_children; j++)
            children[j] = person_tab[j + parent->index];
        for (int j = parent->index - 1; j > i; j--)
            person_tab[j + parent->no_children] = person_tab[j];
        for (int j = 0; j < parent->no_children; j++)
            person_tab[j + i + 1] = children[j];
        for (int j = 0; j < no_parents; j++)
            if (idx_tab[j].index > i && idx_tab[j].index < parent->index)
                idx_tab[j].index += parent->no_children;
    }
}

int cleaning(Person* person_tab, int n) {
    int deleted = 0;
    int x = 0;
    for (int i = 0; i < n - deleted; i++) {
        while (person_tab[x].bits.in_line == no) {
            deleted++;
            x++;
        }
        person_tab[i] = person_tab[x];
        x++;
    }
    return n - deleted;
}

void print_person(const Person* p) {
    printf("%s\n", p->name);
}

int create_list(Person* person_tab, int n) {
    qsort(person_tab, n, sizeof(Person), cmp_person);
    Parent idx_tab[40];
    int no_parents = fill_indices_tab(idx_tab, person_tab, n);
    persons_shifts(person_tab, n, idx_tab, no_parents);
    return cleaning(person_tab, n);
}

int main(void) {
    Person person_tab[] = {
            {"Charles III",  {M, no},  {14, 11, 1948}, "Elizabeth II"},
            {"Anne",         {F, yes}, {15, 8,  1950}, "Elizabeth II"},
            {"Andrew",       {M, yes}, {19, 2,  1960}, "Elizabeth II"},
            {"Edward",       {M, yes}, {10, 3,  1964}, "Elizabeth II"},
            {"David",        {M, yes}, {3,  11, 1961}, "Margaret"},
            {"Sarah",        {F, yes}, {1,  5,  1964}, "Margaret"},
            {"William",      {M, yes}, {21, 6,  1982}, "Charles III"},
            {"Henry",        {M, yes}, {15, 9,  1984}, "Charles III"},
            {"Peter",        {M, yes}, {15, 11, 1977}, "Anne"},
            {"Zara",         {F, yes}, {15, 5,  1981}, "Anne"},
            {"Beatrice",     {F, yes}, {8,  8,  1988}, "Andrew"},
            {"Eugenie",      {F, yes}, {23, 3,  1990}, "Andrew"},
            {"James",        {M, yes}, {17, 12, 2007}, "Edward"},
            {"Louise",       {F, yes}, {8,  11, 2003}, "Edward"},
            {"Charles",      {M, yes}, {1,  7,  1999}, "David"},
            {"Margarita",    {F, yes}, {14, 5,  2002}, "David"},
            {"Samuel",       {M, yes}, {28, 7,  1996}, "Sarah"},
            {"Arthur",       {M, yes}, {6,  5,  2019}, "Sarah"},
            {"George",       {M, yes}, {22, 7,  2013}, "William"},
            {"George VI",    {M, no},  {14, 12, 1895}, NULL},
            {"Charlotte",    {F, yes}, {2,  5,  2015}, "William"},
            {"Louis",        {M, yes}, {23, 4,  2018}, "William"},
            {"Archie",       {M, yes}, {6,  5,  2019}, "Henry"},
            {"Lilibet",      {F, yes}, {4,  6,  2021}, "Henry"},
            {"Savannah",     {F, yes}, {29, 12, 2010}, "Peter"},
            {"Isla",         {F, yes}, {29, 3,  2012}, "Peter"},
            {"Mia",          {F, yes}, {17, 1,  2014}, "Zara"},
            {"Lena",         {F, yes}, {18, 6,  2018}, "Zara"},
            {"Elizabeth II", {F, no},  {21, 4,  1925}, "George VI"},
            {"Margaret",     {F, no},  {21, 8,  1930}, "George VI"},
            {"Lucas",        {M, yes}, {21, 3,  2021}, "Zara"},
            {"Sienna",       {F, yes}, {18, 9,  2021}, "Beatrice"},
            {"August",       {M, yes}, {9,  2,  2021}, "Eugenie"}
    };

    int to_do, no;
    int n;
    Food food_tab[FOOD_MAX];
    char buff[ART_MAX];
    fgets(buff, ART_MAX, stdin);
    sscanf(buff, "%d", &to_do);

    switch (to_do) {
        case 1:  // bsearch2
            fgets(buff, ART_MAX, stdin);
            sscanf(buff, "%d", &no);
            n = read_goods(food_tab, no, stdin, 1);
            scanf("%s", buff);
            print_art(food_tab, n, buff);
            break;
        case 2: // qsort
            fgets(buff, ART_MAX, stdin);
            sscanf(buff, "%d", &no);
            n = read_goods(food_tab, no, stdin, 0);
            Date curr_date;
            int days;
            scanf("%d %d %d", &curr_date.day, &curr_date.month, &curr_date.year);
            scanf("%d", &days);
            printf("%.2f\n", value(food_tab, (size_t) n, curr_date, days));
            break;
        case 3: // succession
            scanf("%d", &no);
            int no_persons = sizeof(person_tab) / sizeof(Person);
            create_list(person_tab, no_persons);
            print_person(person_tab + no - 1);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
    }
    return 0;
}
