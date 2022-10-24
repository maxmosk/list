#include <stdio.h>
#include <stdlib.h>
#include "list.h"


int main(void)
{
    list_t myList = {0};

    size_t cap = 0;
    scanf("%zu", &cap);
    listCtor(&myList, cap);

    int n = 0;
    scanf("%d", &n);
    for (int i = 0; i < n; i++)
    {
        listPushFront(&myList, 10.5);
    }

    listDtor(&myList);

    return EXIT_SUCCESS;
}

