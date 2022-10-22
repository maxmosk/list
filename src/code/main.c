#include <stdio.h>
#include <stdlib.h>
#include "list.h"


int main(void)
{
    list_t myList = {0};

    size_t cap = 0;
    scanf("%zu", &cap);
    listCtor(&myList, cap);

    printf("%lg ... %lg\n", *listIndex(&myList, 0), *listIndex(&myList, cap - 1));

    listDtor(&myList);
    return EXIT_SUCCESS;
}

