#include "list.h"


enum LIST_CODES listCtor(list_t *lst, size_t capacity)
{
    lst->nodes = calloc(capacity + 1, sizeof lst->nodes[0]);
    lst->capacity = capacity;

    return LIST_SUCCESS;
}

