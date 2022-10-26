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
        listIndex_t ind = listPushBack(&myList, 10 * i);

        listElem_t buf = NAN;
        listIter(&myList, ind, &buf);
        printf("[%d] = %lf\n", i, buf);
    }

    listDtor(&myList);

    return EXIT_SUCCESS;
}

