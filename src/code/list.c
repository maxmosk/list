#include "debug.h"
#include "list.h"


/*(===========================================================================*/
#define DATA(list, iter) list->nodes[iter].data

#define NEXT(list, iter) list->nodes[iter].next

#define PREV(list, iter) list->nodes[iter].prev
/*)===========================================================================*/


static const char gvizbuf[] = "gvizbuffer";


/*(===========================================================================*/
static enum LIST_CODES listInitNodes(list_t *lst, listIndex_t start);

static listNode_t *listAlloc(size_t nmemb);

static listNode_t *listRealloc(listNode_t *buffer, size_t nmemb);

static enum LIST_CODES listIncrease(list_t *lst, size_t incsize);

static void listDump(const list_t *lst);

static void listCringe(void);

static void listGraphDump(const list_t *lst, const char *filename);

static bool listCycle(const list_t *lst);
/*)===========================================================================*/


/*(===========================================================================*/
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
        lst->linearized = true;
        return LIST_SUCCESS;
    }

    lst->nodes = listAlloc(capacity);
    CHECK(NULL != lst->nodes, LIST_NOMEM);

    lst->capacity = capacity;
    DATA(lst, NULL_INDEX) = DATA_POISON;
    NEXT(lst, NULL_INDEX) = NULL_INDEX;
    PREV(lst, NULL_INDEX) = NULL_INDEX;

    lst->free = NULL_INDEX;

    CHECK(LIST_SUCCESS == listInitNodes(lst, 1), LIST_INITERR);
    lst->free = 1;

    lst->linearized = true;


    return LIST_SUCCESS;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
listIndex_t listInsertAfter(list_t *lst, listIndex_t iter, listElem_t newelem)
{
    CHECK(NULL != lst, LIST_NULLPTR);
    CHECK(LIST_SUCCESS == listVerify(lst), NULL_INDEX);

    CHECK(iter <= lst->capacity, NULL_INDEX);

    if (0 == lst->capacity)
    {
        enum LIST_CODES status = listCtor(lst, 4);
        CHECK(LIST_SUCCESS == status, status);
    }

    CHECK(INDEX_POISON != PREV(lst, iter), NULL_INDEX);


    if (NULL_INDEX == lst->free)
    {
        CHECK(LIST_SUCCESS == listIncrease(lst, 1), NULL_INDEX);
    }
    
    if (NULL_INDEX != NEXT(lst, iter))
    {
        lst->linearized = false;
    }

    PREV(lst, NEXT(lst, iter)) = lst->free;
    listIndex_t newfree = NEXT(lst, lst->free);
    NEXT(lst, lst->free) = NEXT(lst, iter);
    NEXT(lst, iter) = lst->free;
    lst->free = newfree;
    PREV(lst, NEXT(lst, iter)) = iter;
    DATA(lst, NEXT(lst, iter)) = newelem;


    return NEXT(lst, iter);
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
listIndex_t listInsertBefore(list_t *lst, listIndex_t iter, listElem_t newelem)
{
    CHECK(NULL != lst, LIST_NULLPTR);
    CHECK(LIST_SUCCESS == listVerify(lst), NULL_INDEX);

    CHECK(iter <= lst->capacity, NULL_INDEX);

    if (0 == lst->capacity)
    {
        enum LIST_CODES status = listCtor(lst, 4);
        CHECK(LIST_SUCCESS == status, status);
    }

    CHECK(INDEX_POISON != PREV(lst, iter), NULL_INDEX);


    if (NULL_INDEX == lst->free)
    {
        CHECK(LIST_SUCCESS == listIncrease(lst, 1), NULL_INDEX);
    }
 
    if (NULL_INDEX != PREV(lst, iter))
    {
        lst->linearized = false;
    }

    NEXT(lst, PREV(lst, iter)) = lst->free;
    PREV(lst, lst->free) = PREV(lst, NULL_INDEX);
    PREV(lst, iter) = lst->free;
    lst->free = NEXT(lst, lst->free);
    NEXT(lst, PREV(lst, iter)) = iter;
    DATA(lst, PREV(lst, iter)) = newelem;
    

    return PREV(lst, iter);
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
listIndex_t listPushBack(list_t *lst, listElem_t newelem)
{
    return listInsertBefore(lst, NULL_INDEX, newelem);
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
listIndex_t listPushFront(list_t *lst, listElem_t newelem)
{
    return listInsertAfter(lst, NULL_INDEX, newelem);
}
/*)---------------------------------------------------------------------------*/

listIndex_t listIndex(list_t *lst, size_t index)
{
    CHECK(NULL != lst, NULL_INDEX);
    CHECK(LIST_SUCCESS == listVerify(lst), NULL_INDEX);

    CHECK(index <= lst->capacity, NULL_INDEX);
    CHECK(SIZE_MAX != index, NULL_INDEX);

    if (lst->linearized)
    {
        return index + 1;
    }

#ifdef CRINGE
    listCringe();
#endif

    listIndex_t iter = NEXT(lst, NULL_INDEX);

    for (size_t i = 0; (i < index) && (iter != NULL_INDEX); i++)
    {
        iter = NEXT(lst, iter);
    }

    return iter;
}

/*(---------------------------------------------------------------------------*/
listIndex_t listNext(list_t *lst, listIndex_t iter)
{
    CHECK(NULL != lst, NULL_INDEX);
    CHECK(LIST_SUCCESS == listVerify(lst), NULL_INDEX);

    CHECK(iter <= lst->capacity, NULL_INDEX);
    CHECK(INDEX_POISON != PREV(lst, iter), NULL_INDEX);

    return NEXT(lst, iter);
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
listIndex_t listPrev(list_t *lst, listIndex_t iter)
{
    CHECK(NULL != lst, NULL_INDEX);
    CHECK(LIST_SUCCESS == listVerify(lst), NULL_INDEX);

    CHECK(iter <= lst->capacity, NULL_INDEX);
    CHECK(INDEX_POISON != PREV(lst, iter), NULL_INDEX);

    return PREV(lst, iter);
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
enum LIST_CODES listIter(list_t *lst, listIndex_t iter, listElem_t *dest)
{
    CHECK(NULL != lst, LIST_NULLPTR);
    CHECK(LIST_SUCCESS == listVerify(lst), LIST_INVALID);
    CHECK(NULL != dest, LIST_NULLPTR);

    CHECK(NULL_INDEX != iter, LIST_NULLINDEX);
    CHECK(iter <= lst->capacity, LIST_OUTRANGE);
    CHECK(INDEX_POISON != PREV(lst, iter), LIST_WRONGITER);

    *dest = DATA(lst, iter);
    return LIST_SUCCESS;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
enum LIST_CODES listBack(list_t *lst, listElem_t *dest)
{
    return listIter(lst, listPrev(lst, NULL_INDEX), dest);
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
enum LIST_CODES listFront(list_t *lst, listElem_t *dest)
{
    return listIter(lst, listNext(lst, NULL_INDEX), dest);
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
enum LIST_CODES listRemove(list_t *lst, listIndex_t iter)
{
    CHECK(NULL != lst, LIST_NULLPTR);
    CHECK(LIST_SUCCESS == listVerify(lst), LIST_INVALID);

    CHECK(NULL_INDEX != iter, LIST_NULLINDEX);
    CHECK(iter <= lst->capacity, LIST_OUTRANGE);
    CHECK(INDEX_POISON != PREV(lst, iter), LIST_WRONGITER);

    NEXT(lst, PREV(lst, iter)) = NEXT(lst, iter);
    PREV(lst, NEXT(lst, iter)) = PREV(lst, iter);

    DATA(lst, iter) = DATA_POISON;
    NEXT(lst, iter) = lst->free;
    PREV(lst, iter) = INDEX_POISON;
    lst->free = iter;

    if (NULL_INDEX != NEXT(lst, iter))
    {
        lst->linearized = false;
    }

    return LIST_SUCCESS;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
enum LIST_CODES listLinearize(list_t *lst)
{
    CHECK(NULL != lst, LIST_NULLPTR);
    CHECK(LIST_SUCCESS == listVerify(lst), LIST_INVALID);

    if (lst->linearized)
    {
        return LIST_SUCCESS;
    }

#ifdef CRINGE
    listCringe();
#endif


    listNode_t *oldnodes = lst->nodes;
    listNode_t *newnodes = listAlloc(lst->capacity);
    CHECK(NULL != newnodes, LIST_NOMEM);
    lst->nodes = newnodes;

    lst->free = NULL_INDEX;
    CHECK(LIST_SUCCESS == listInitNodes(lst, 1), LIST_INITERR);
    lst->free = 1;

    size_t i = 0;
    listIndex_t iter = oldnodes[NULL_INDEX].next;
    for (i = 0; (i < lst->capacity) && (iter != NULL_INDEX); i++)
    {
        listIndex_t status = listPushBack(lst, oldnodes[iter].data);
        CHECK(NULL_INDEX != status, status);
        iter = oldnodes[iter].next;
    }

    free(oldnodes);


    return LIST_SUCCESS;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
enum LIST_CODES listDtor(list_t *lst)
{
    CHECK(NULL != lst, LIST_NULLPTR);
    CHECK(LIST_SUCCESS == listVerify(lst), LIST_INVALID);

    free(lst->nodes);
    lst->nodes = NULL;
    lst->capacity = SIZE_MAX;
    lst->free = NULL_INDEX;

    return LIST_SUCCESS;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
enum LIST_CODES listVerify(list_t *lst)
{
    CHECK(NULL != lst, LIST_NULLPTR);
    
    listDump(lst);
    if (listEmpty(lst))
    {
        return LIST_SUCCESS;
    }
    
    if (NULL == lst->nodes)
    {
        lst->status |= INVALID_DATA_PTR;
    }

    if ((0 == lst->capacity) || (SIZE_MAX == lst->capacity))
    {
        lst->status |= INVALID_CAPACITY;
    }

    if (listCycle(lst))
    {
        lst->status |= INVALID_LINKING;
    }


    if (0 == lst->status)
    {
        return LIST_SUCCESS;
    }
    else
    {
        return LIST_INVALID;
    }
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
bool listEmpty(const list_t *lst)
{
    CHECK(NULL != lst, true);

    return (0 == lst->free) && (NULL == lst->nodes) && (0 == lst->capacity);
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
enum LIST_CODES listClear(list_t *lst)
{
    CHECK(NULL != lst, LIST_NULLPTR);
    CHECK(LIST_SUCCESS == listVerify(lst), NULL_INDEX);

    free(lst->nodes);
    lst->nodes = NULL;

    lst->free = NULL_INDEX;
    lst->capacity = 0;

    return LIST_SUCCESS;
}
/*)---------------------------------------------------------------------------*/
/*)===========================================================================*/


/*(===========================================================================*/
/*(---------------------------------------------------------------------------*/
static enum LIST_CODES listInitNodes(list_t *lst, listIndex_t start)
{
    CHECK(NULL != lst, LIST_NULLPTR);
    CHECK(SIZE_MAX != start, LIST_SIZEERR);

    for (listIndex_t i = start; i <= lst->capacity; i++)
    {
        DATA(lst, i) = DATA_POISON;
        NEXT(lst, i) = i + 1;
        PREV(lst, i) = INDEX_POISON;
    }

    NEXT(lst, lst->capacity) = lst->free;

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


    static size_t ngraphs = 0;
    char namebuf[64] = "";
    sprintf(namebuf, "graph%zu.jpg", ngraphs);

    LOGOPEN("log.html");
    LOGPRINTF("<pre>\n");
    LOGPRINTF("<style>body {background-color:white;font-size:20px;}</style>\n");
    
    LOGPRINTF("list_t [<b style=\"color:red;\">%p</b>]\n", (const void *) lst);
    LOGPRINTF("{\n");

    LOGPRINTF("    capacity = <b style=\"color:red;\">%zu</b>\n", lst->capacity);
    LOGPRINTF("    free = <b style=\"color:red;\">%zu</b>\n", lst->free);

    LOGPRINTF("    data: ");
    if (NULL != lst->nodes)
    {
        for (size_t i = 0; i < lst->capacity + 1; i++)
        {
            LOGPRINTF("<b style=\"color:red;\">%20lg</b> ", DATA(lst, i));
        }
        LOGPRINTF("\n");

        LOGPRINTF("    next: ");
        for (size_t i = 0; i < lst->capacity + 1; i++)
        {
            LOGPRINTF("<b style=\"color:green;\">%20zu</b> ", NEXT(lst, i));
        }
        LOGPRINTF("\n");

        LOGPRINTF("    prev: ");
        for (size_t i = 0; i < lst->capacity + 1; i++)
        {
            LOGPRINTF("<b style=\"color:blue;\">%20zu</b> ", PREV(lst, i));
        }
        LOGPRINTF("\n");
    }

    LOGPRINTF("<img src=\"%s\">\n", namebuf);

    LOGPRINTF("}\n");

    LOGPRINTF("</pre>\n");
    LOGCLOSE();


    listGraphDump(lst, namebuf);
    ngraphs++;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
static void listCringe(void)
{
    srand(time(NULL));
    char src = (char) rand();
    printf("Input %c to continue: ", src);

    char buf = 0;
    do
    {
        scanf("%c", &buf);
    }
    while (buf != src);
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
static void listGraphDump(const list_t *lst, const char *filename)
{
    CHECK(NULL != filename, ;);
    CHECK(NULL != lst, ;);
    

    if (NULL == lst->nodes)
    {
        return;
    }
    
    FILE *dotfile = fopen(gvizbuf, "w");
    CHECK(NULL != dotfile, ;);

    fprintf(dotfile, "digraph\n");
    fprintf(dotfile, "{\n");

    fprintf(dotfile, "    rankdir=LR;\n");

    listIndex_t node = NEXT(lst, NULL_INDEX);
    for (size_t i = 1; (i <= lst->capacity) && (node != NULL_INDEX); i++)
    {
        fprintf(dotfile,
                "    usednode%zu[style=\"rounded\",shape=record,color=\"red\",label="
                    "\" physical index=%zu | index=%zu | data=%lg | <next> next=%zu "
                    "| <prev> prev= %zu \"];\n", 
                i,
                node,
                i - 1,
                DATA(lst, node),
                NEXT(lst, node),
                PREV(lst, node)
                );

        node = NEXT(lst, node);
    }

    fprintf(dotfile, "    front[shape=\"octagon\",label=\"front = %zu\"];\n",
            NEXT(lst, NULL_INDEX));
    fprintf(dotfile, "    back[shape=\"octagon\",label=\"back = %zu\"];\n",
            PREV(lst, NULL_INDEX));

    size_t listlen = 0;
    node = NEXT(lst, NULL_INDEX);
    for (size_t i = 1; (i <= lst->capacity) && (node != NULL_INDEX); i++, listlen++)
    {
        if (1 == i)
        {
            fprintf(dotfile, "    front->");
        }

        fprintf(dotfile, "usednode%zu:next", i);
        node = NEXT(lst, node);

        if (NULL_INDEX != node)
        {
            fprintf(dotfile, "->");
        }
        else
        {
            fprintf(dotfile, ";\n");
        }
    }

    node = PREV(lst, NULL_INDEX);
    for (size_t i = listlen; (i >= 1) && (node != NULL_INDEX); i--)
    {
        if (listlen == i)
        {
            fprintf(dotfile, "    back->");
        }

        fprintf(dotfile, "usednode%zu:prev", i);
        node = PREV(lst, node);

        if (NULL_INDEX != node)
        {
            fprintf(dotfile, "->");
        }
        else
        {
            fprintf(dotfile, "[color=\"red\"];\n");
        }
    }

    
    fprintf(dotfile, "    free[shape=\"octagon\",label=\"free = %zu\"];\n", lst->free);
    
    node = lst->free;
    for (size_t i = 1; (i <= lst->capacity) && (node != NULL_INDEX); i++)
    {
        fprintf(dotfile,
                "    freenode%zu[style=\"rounded\",shape=record,color=\"blue\",label="
                        "\" physical index=%zu | index=%zu | data=%lg | <next> next=%zu "
                        "| <prev> prev= %zu \"];\n", 
                i,
                node,
                i - 1,
                DATA(lst, node),
                NEXT(lst, node),
                PREV(lst, node)
                );

        node = NEXT(lst, node);
    }

    node = lst->free;
    for (size_t i = 1; (i <= lst->capacity) && (node != NULL_INDEX); i++, listlen++)
    {
        if (1 == i)
        {
            fprintf(dotfile, "    free->");
        }

        fprintf(dotfile, "freenode%zu:next", i);
        node = NEXT(lst, node);

        if (NULL_INDEX != node)
        {
            fprintf(dotfile, "->");
        }
        else
        {
            fprintf(dotfile, "[color=\"blue\"];\n");
        }
    }

   
    fprintf(dotfile, "}");
    fclose(dotfile);

    char cmdbuf[128] = "";

    sprintf(cmdbuf, "dot %s -o %s -Tjpg", gvizbuf, filename);
    CHECK(0 == system(cmdbuf), ;);
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
/*******************************************************************************
 *
 * Function finds cycle in list by any algorithm with linear time from list len.
 *
 * Should be used only from verificator after all other checkers. Be careful.
 * Returns true if list cycled.
 *
 ******************************************************************************/
static bool listCycle(const list_t *lst)
{
    size_t tortoise = 0, hare = 0;
    for (
            tortoise = 0, hare = 0;
            (tortoise != hare) && (NULL_INDEX != hare) && (NULL_INDEX != NEXT(lst, hare));
            hare = NEXT(lst, NEXT(lst, hare)), tortoise = NEXT(lst, tortoise)
        )
    {
        ;
    }

    return (NULL_INDEX != hare) && (tortoise == hare);
}
/*)---------------------------------------------------------------------------*/
/*)===========================================================================*/

