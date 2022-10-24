#include "debug.h"
#include "list.h"


static enum LIST_CODES listInitNodes(list_t *lst, listIndex_t start);

static listNode_t *listAlloc(size_t nmemb);

static listNode_t *listRealloc(listNode_t *buffer, size_t nmemb);

static enum LIST_CODES listIncrease(list_t *lst, size_t incsize);

static void listDump(const list_t *lst);


/*(---------------------------------------------------------------------------*/
enum LIST_CODES listCtor(list_t *lst, size_t capacity)
{
    CHECK(NULL != lst, LIST_NULLPTR);
    CHECK(SIZE_MAX != capacity, LIST_SIZEERR);

    if (0 == capacity)
    {
        lst->nodes = NULL;
        lst->free = NULL_INDEX;
        lst->capacity = 0;
        return LIST_SUCCESS;
    }

    lst->nodes = listAlloc(capacity);
    CHECK(NULL != lst->nodes, LIST_NOMEM);

    lst->capacity = capacity;
    lst->nodes[NULL_INDEX].data = DATA_POISON;
    lst->nodes[NULL_INDEX].next = NULL_INDEX;
    lst->nodes[NULL_INDEX].prev = NULL_INDEX;

    lst->free = NULL_INDEX;

    CHECK(LIST_SUCCESS == listInitNodes(lst, 1), LIST_INITERR);
    lst->free = 1;


    return LIST_SUCCESS;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
static enum LIST_CODES listInitNodes(list_t *lst, listIndex_t start)
{
    CHECK(NULL != lst, LIST_NULLPTR);
    CHECK(SIZE_MAX != start, LIST_SIZEERR);

    for (listIndex_t i = start; i <= lst->capacity; i++)
    {
        lst->nodes[i].data = DATA_POISON;
        lst->nodes[i].next = i + 1;
        lst->nodes[i].prev = INDEX_POISON;
    }

    lst->nodes[lst->capacity].next = lst->free;

    return LIST_SUCCESS;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
static listNode_t *listAlloc(size_t nmemb)
{
    listNode_t *buffer = calloc(nmemb + 1, sizeof *buffer);

    return buffer;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
listIndex_t listPushBack(list_t *lst, listElem_t newelem)
{
    CHECK(NULL != lst, NULL_INDEX);

    if (0 == lst->capacity)
    {
        enum LIST_CODES status = listCtor(lst, 4);
        CHECK(LIST_SUCCESS == status, status);
    }

    if (NULL_INDEX == lst->free)
    {
        listNode_t *newnodes = listRealloc(lst->nodes, lst->capacity + 4);
        CHECK(NULL != newnodes, NULL_INDEX);
        lst->nodes = newnodes;

        size_t oldcap = lst->capacity;
        lst->capacity += 4;
        CHECK(LIST_SUCCESS == listInitNodes(lst, oldcap + 1), NULL_INDEX);
        lst->free = oldcap + 1;
    }

    lst->nodes[lst->free].prev = lst->nodes[NULL_INDEX].prev;
    lst->nodes[NULL_INDEX].prev = lst->free;
    lst->free = lst->nodes[lst->free].next;
    lst->nodes[lst->nodes[NULL_INDEX].prev].next = NULL_INDEX;
    lst->nodes[lst->nodes[NULL_INDEX].prev].data = newelem;


    return lst->nodes[NULL_INDEX].prev;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
listIndex_t listPushFront(list_t *lst, listElem_t newelem)
{
    CHECK(NULL != lst, LIST_NULLPTR);


    if (0 == lst->capacity)
    {
        enum LIST_CODES status = listCtor(lst, 4);
        CHECK(LIST_SUCCESS == status, status);
    }

    if (NULL_INDEX == lst->free)
    {
        CHECK(LIST_SUCCESS == listIncrease(lst, 1), NULL_INDEX);
    }


    listDump(lst);
    if (0 == lst->nodes[NULL_INDEX].next)
    {
        lst->nodes[NULL_INDEX].next = lst->nodes[NULL_INDEX].prev = lst->free;
        lst->free = lst->nodes[lst->free].next;
        lst->nodes[lst->nodes[NULL_INDEX].prev].next =
            lst->nodes[lst->nodes[NULL_INDEX].prev].prev = NULL_INDEX;
        lst->nodes[lst->nodes[NULL_INDEX].next].data = newelem;
    }
    else
    {
        lst->nodes[lst->nodes[NULL_INDEX].next].prev = lst->free;
        listIndex_t newfree = lst->nodes[lst->free].next;
        lst->nodes[lst->free].next = lst->nodes[NULL_INDEX].next;
        lst->nodes[NULL_INDEX].next = lst->free;
        lst->free = newfree;
        lst->nodes[lst->nodes[NULL_INDEX].next].prev = NULL_INDEX;
        lst->nodes[lst->nodes[NULL_INDEX].next].data = newelem;
    }
    listDump(lst);


    return lst->nodes[NULL_INDEX].next;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
enum LIST_CODES listDtor(list_t *lst)
{
    CHECK(NULL != lst, LIST_NULLPTR);

    free(lst->nodes);
    lst->nodes = NULL;
    lst->capacity = SIZE_MAX;
    lst->free = NULL_INDEX;

    return LIST_SUCCESS;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
static listNode_t *listRealloc(listNode_t *buffer, size_t nmemb)
{
    listNode_t *newbuffer = realloc(buffer, (nmemb + 1) * sizeof *newbuffer);

    return newbuffer;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
static enum LIST_CODES listIncrease(list_t *lst, size_t incsize)
{
    CHECK(NULL != lst, LIST_NULLPTR);
    CHECK(SIZE_MAX != incsize, LIST_SIZEERR);


    CHECK(0 != incsize, LIST_SUCCESS);

    listNode_t *newnodes = listRealloc(lst->nodes, lst->capacity + incsize);
    CHECK(NULL != newnodes, LIST_NOMEM);
    lst->nodes = newnodes;

    size_t oldcap = lst->capacity;
    lst->capacity += incsize;
    CHECK(LIST_SUCCESS == listInitNodes(lst, oldcap + 1), LIST_INITERR);
    lst->free = oldcap + 1;


    return LIST_SUCCESS;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
static void listDump(const list_t *lst)
{
    CHECK(NULL != lst, ;);
    CHECK(NULL != lst->nodes, ;);

    LOGOPEN();
    
    LOGPRINTF("list_t [%p]\n", (const void *) lst);
    LOGPRINTF("{\n");

    LOGPRINTF("    capacity = %zu\n", lst->capacity);
    LOGPRINTF("    free = %zu\n", lst->free);

    LOGPRINTF("    data: ");
    for (size_t i = 0; i < lst->capacity + 1; i++)
    {
        LOGPRINTF("%20lg ", lst->nodes[i].data);
    }
    LOGPRINTF("\n");

    LOGPRINTF("    next: ");
    for (size_t i = 0; i < lst->capacity + 1; i++)
    {
        LOGPRINTF("%20zu ", lst->nodes[i].next);
    }
    LOGPRINTF("\n");

    LOGPRINTF("    prev: ");
    for (size_t i = 0; i < lst->capacity + 1; i++)
    {
        LOGPRINTF("%20zu ", lst->nodes[i].prev);
    }
    LOGPRINTF("\n");

    LOGPRINTF("}\n");
    LOGCLOSE();
}
/*)---------------------------------------------------------------------------*/

