#include "debug.h"
#include "list.h"


static enum LIST_CODES listInitNodes(list_t *lst);


enum LIST_CODES listCtor(list_t *lst, size_t capacity)
{
    CHECK(NULL != lst, LIST_NULLPTR);
    CHECK(SIZE_MAX != capacity, LIST_SIZEERR);


    lst->nodes = calloc(capacity + 1, sizeof lst->nodes[0]);
    CHECK(NULL != lst->nodes, LIST_NOMEM);

    lst->capacity = capacity;
    lst->nodes[NULL_INDEX].data = DATA_POISON;
    lst->nodes[NULL_INDEX].next = NULL_INDEX;
    lst->nodes[NULL_INDEX].prev = NULL_INDEX;
    CHECK(LIST_SUCCESS == listInitNodes(lst), LIST_INITERR);

    lst->free = 1;
    lst->dummy.data = DATA_POISON;
    lst->dummy.next = NULL_INDEX;
    lst->dummy.prev = NULL_INDEX;


    return LIST_SUCCESS;
}

static enum LIST_CODES listInitNodes(list_t *lst)
{
    CHECK(NULL != lst, LIST_NULLPTR);
    CHECK(NULL != lst->nodes, LIST_NULLPTR);

    for (listIndex_t i = 1; i <= lst->capacity; i++)
    {
        lst->nodes[i].data = DATA_POISON;
        lst->nodes[i].next = i + 1;
        lst->nodes[i].prev = INDEX_POISON;
    }

    lst->nodes[lst->capacity - 1].next = NULL_INDEX;

    return LIST_SUCCESS;
}

