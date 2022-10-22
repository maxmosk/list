#include <stdio.h>
#include <stdlib.h>
#include "list.h"


int main(void)
{
    list_t myList = {0};

    size_t cap = 0;
    scanf("%zu", &cap);
    listCtor(&myList, cap);

    listPushBack(&myList, 12.5);

    listDtor(&myList);
    return EXIT_SUCCESS;
}

