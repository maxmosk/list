#include "debug.h"
#include "list.h"


static enum LIST_CODES listInitNodes(list_t *lst, listIndex_t start);

static listNode_t *listAlloc(size_t nmemb);

static listNode_t *listRealloc(listNode_t *buffer, size_t nmemb);

static void listDump(const list_t *lst);


/*(---------------------------------------------------------------------------*/
enum LIST_CODES listCtor(list_t *lst, size_t capacity)
{
    CHECK(NULL != lst, LIST_NULLPTR);
    CHECK(SIZE_MAX != capacity, LIST_SIZEERR);


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
    CHECK(NULL != lst->nodes, NULL_INDEX);


    if (NULL_INDEX == lst->free)
    {
        listNode_t *newnodes = listRealloc(lst->nodes, lst->capacity + 5);
        CHECK(NULL != newnodes, NULL_INDEX);
        lst->nodes = newnodes;

        size_t oldcap = lst->capacity;
        lst->capacity += 5;
        CHECK(LIST_SUCCESS == listInitNodes(lst, oldcap + 1), NULL_INDEX);
        lst->free = oldcap + 1;
    }


    listDump(lst);

    lst->nodes[lst->free].prev = lst->nodes[NULL_INDEX].prev;
    lst->nodes[NULL_INDEX].prev = lst->free;
    lst->free = lst->nodes[lst->free].next;
    lst->nodes[lst->nodes[NULL_INDEX].prev].next = NULL_INDEX;
    lst->nodes[lst->nodes[NULL_INDEX].prev].data = newelem;

    listDump(lst);


    return lst->nodes[NULL_INDEX].prev;
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
    for (size_t i = 0; i < lst->capacity; i++)
    {
        LOGPRINTF("%20lg ", lst->nodes[i].data);
    }
    LOGPRINTF("\n");

    LOGPRINTF("    next: ");
    for (size_t i = 0; i < lst->capacity; i++)
    {
        LOGPRINTF("%20zu ", lst->nodes[i].next);
    }
    LOGPRINTF("\n");

    LOGPRINTF("    prev: ");
    for (size_t i = 0; i < lst->capacity; i++)
    {
        LOGPRINTF("%20zu ", lst->nodes[i].prev);
    }
    LOGPRINTF("\n");

    LOGPRINTF("}\n");
    LOGCLOSE();
}
/*)---------------------------------------------------------------------------*/

