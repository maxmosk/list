#include <stdio.h>
#include <stdlib.h>
#include "list.h"


int main(void)
{
    list_t myList = {0};

    size_t cap = 0;
    scanf("%zu", &cap);
    listCtor(&myList, cap);

    return EXIT_SUCCESS;
}
