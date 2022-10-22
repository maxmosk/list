#include "debug.h"
#include "list.h"


enum LIST_CODES listCtor(list_t *lst, size_t capacity)
{
    CHECK(NULL != lst, LIST_NULLPTR);


    lst->nodes = calloc(capacity + 1, sizeof lst->nodes[0]);
    CHECK(NULL != lst->nodes, LIST_NOMEM);

    lst->capacity = capacity;
    lst->dummy.data = DATA_POISON;


    return LIST_SUCCESS;
}

