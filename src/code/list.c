#include "debug.h"
#include "list.h"


static enum LIST_CODES listInitNodes(list_t *lst);

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
    CHECK(LIST_SUCCESS == listInitNodes(lst), LIST_INITERR);

    lst->free = (capacity != 0) ? 1 : NULL_INDEX;
    lst->dummy.data = DATA_POISON;
    lst->dummy.next = NULL_INDEX;
    lst->dummy.prev = NULL_INDEX;


    return LIST_SUCCESS;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
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
        CHECK(NULL != newnodes, 0);

        lst->nodes = newnodes;
        lst->capacity += 5;
    }


    listDump(lst);
    if (lst->dummy.next == NULL_INDEX && lst->dummy.prev == NULL_INDEX)
    {
        lst->dummy.prev = lst->dummy.next = lst->free;
        lst->free = lst->nodes[lst->free].next;

        lst->nodes[lst->dummy.prev].next = NULL_INDEX;
        lst->nodes[lst->dummy.prev].prev = NULL_INDEX;
    }
    else
    {
        lst->nodes[lst->dummy.prev].next = lst->free;
        lst->free = lst->nodes[lst->free].next;

        lst->nodes[lst->nodes[lst->dummy.prev].next].prev = lst->dummy.next;
        lst->dummy.prev = lst->nodes[lst->dummy.prev].next;
        lst->nodes[lst->dummy.prev].next = NULL_INDEX;
    }

    lst->nodes[lst->dummy.prev].data = newelem;

    listDump(lst);


    return lst->dummy.prev;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
enum LIST_CODES listDtor(list_t *lst)
{
    CHECK(NULL != lst, LIST_NULLPTR);

    free(lst->nodes);
    lst->nodes = NULL;
    lst->capacity = SIZE_MAX;
    lst->dummy.next = NULL_INDEX;
    lst->dummy.prev = NULL_INDEX;
    lst->free = NULL_INDEX;

    return LIST_SUCCESS;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
static listNode_t *listRealloc(listNode_t *buffer, size_t nmemb)
{
    listNode_t *newbuffer = realloc(buffer, (nmemb + 1) * sizeof *buffer);

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

    LOGPRINTF("    dummy\n");
    LOGPRINTF("    {\n");
    LOGPRINTF("        data = %20lg\n", lst->dummy.data);
    LOGPRINTF("        next = %20zu\n", lst->dummy.next);
    LOGPRINTF("        prev = %20zu\n", lst->dummy.prev);
    LOGPRINTF("    }\n");

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

