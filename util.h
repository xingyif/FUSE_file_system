#ifndef UTIL_H
#define UTIL_H

#include <string.h>

#include "slist.h"

static int
streq(const char *aa, const char *bb) {
    return strcmp(aa, bb) == 0;
}

static int
min(int x, int y) {
    return (x < y) ? x : y;
}

static int
max(int x, int y) {
    return (x > y) ? x : y;
}

static int
clamp(int x, int v0, int v1) {
    return max(v0, min(x, v1));
}

//this functionality makes no sense just look through slist not through array
char*
slist_close(slist *list) {
    int size = list->index;
    char clist[size];
    for (int i = 0; i < size; i++) {
        clist[i] = list->data;
        list = list->next;

    }
    return clist;
}

#endif
