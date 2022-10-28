#include "debug.h"
#include "list.h"


static const char gvizbuf[] = "gvizbuffer";


/*(===========================================================================*/
static enum LIST_CODES listInitNodes(list_t *lst, listIndex_t start);

static listNode_t *listAlloc(size_t nmemb);

static listNode_t *listRealloc(listNode_t *buffer, size_t nmemb);

static enum LIST_CODES listIncrease(list_t *lst, size_t incsize);

static void listDump(const list_t *lst);

static void listCringe(void);

static void listGraphDump(const list_t *lst, const char *filename);
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
    lst->nodes[NULL_INDEX].data = DATA_POISON;
    lst->nodes[NULL_INDEX].next = NULL_INDEX;
    lst->nodes[NULL_INDEX].prev = NULL_INDEX;

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
    CHECK(INDEX_POISON != lst->nodes[iter].prev, NULL_INDEX);

    if (0 == lst->capacity)
    {
        enum LIST_CODES status = listCtor(lst, 4);
        CHECK(LIST_SUCCESS == status, status);
    }

    if (NULL_INDEX == lst->free)
    {
        CHECK(LIST_SUCCESS == listIncrease(lst, 1), NULL_INDEX);
    }
    
    if (NULL_INDEX != lst->nodes[iter].next)
    {
        lst->linearized = false;
    }

    lst->nodes[lst->nodes[iter].next].prev = lst->free;
    listIndex_t newfree = lst->nodes[lst->free].next;
    lst->nodes[lst->free].next = lst->nodes[iter].next;
    lst->nodes[iter].next = lst->free;
    lst->free = newfree;
    lst->nodes[lst->nodes[iter].next].prev = iter;
    lst->nodes[lst->nodes[iter].next].data = newelem;


    return lst->nodes[iter].next;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
listIndex_t listInsertBefore(list_t *lst, listIndex_t iter, listElem_t newelem)
{
    CHECK(NULL != lst, LIST_NULLPTR);
    CHECK(LIST_SUCCESS == listVerify(lst), NULL_INDEX);

    CHECK(iter <= lst->capacity, NULL_INDEX);
    CHECK(INDEX_POISON != lst->nodes[iter].prev, NULL_INDEX);

    if (0 == lst->capacity)
    {
        enum LIST_CODES status = listCtor(lst, 4);
        CHECK(LIST_SUCCESS == status, status);
    }

    if (NULL_INDEX == lst->free)
    {
        CHECK(LIST_SUCCESS == listIncrease(lst, 1), NULL_INDEX);
    }
 
    if (NULL_INDEX != lst->nodes[iter].prev)
    {
        lst->linearized = false;
    }

    lst->nodes[lst->nodes[iter].prev].next = lst->free;
    lst->nodes[lst->free].prev = lst->nodes[NULL_INDEX].prev;
    lst->nodes[iter].prev = lst->free;
    lst->free = lst->nodes[lst->free].next;
    lst->nodes[lst->nodes[iter].prev].next = iter;
    lst->nodes[lst->nodes[iter].prev].data = newelem;
    

    return lst->nodes[iter].prev;
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

    listIndex_t iter = lst->nodes[NULL_INDEX].next;

    for (size_t i = 0; (i < index) && (iter != NULL_INDEX); i++)
    {
        iter = lst->nodes[iter].next;
    }

    return iter;
}

/*(---------------------------------------------------------------------------*/
listIndex_t listNext(list_t *lst, listIndex_t iter)
{
    CHECK(NULL != lst, NULL_INDEX);
    CHECK(LIST_SUCCESS == listVerify(lst), NULL_INDEX);

    CHECK(iter <= lst->capacity, NULL_INDEX);
    CHECK(INDEX_POISON != lst->nodes[iter].prev, NULL_INDEX);

    return lst->nodes[iter].next;
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
listIndex_t listPrev(list_t *lst, listIndex_t iter)
{
    CHECK(NULL != lst, NULL_INDEX);
    CHECK(LIST_SUCCESS == listVerify(lst), NULL_INDEX);

    CHECK(iter <= lst->capacity, NULL_INDEX);
    CHECK(INDEX_POISON != lst->nodes[iter].prev, NULL_INDEX);

    return lst->nodes[iter].prev;
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
    CHECK(INDEX_POISON != lst->nodes[iter].prev, LIST_WRONGITER);

    *dest = lst->nodes[iter].data;
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
    CHECK(INDEX_POISON != lst->nodes[iter].prev, LIST_WRONGITER);

    lst->nodes[lst->nodes[iter].prev].next = lst->nodes[iter].next;
    lst->nodes[lst->nodes[iter].next].prev = lst->nodes[iter].prev;

    lst->nodes[iter].data = DATA_POISON;
    lst->nodes[iter].next = lst->free;
    lst->nodes[iter].prev = INDEX_POISON;
    lst->free = iter;

    if (NULL_INDEX != lst->nodes[iter].next)
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

    listIndex_t node = lst->nodes[NULL_INDEX].next;
    for (size_t i = 1; (i <= lst->capacity) && (node != NULL_INDEX); i++)
    {
        node = lst->nodes[node].next;
    }

    if (NULL_INDEX != node)
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
/*)===========================================================================*/


/*(===========================================================================*/
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

    static size_t ngraphs = 0;
    char namebuf[64] = "";
    sprintf(namebuf, "graph%zu.jpg", ngraphs);

    LOGOPEN("log.html");
    LOGPRINTF("<pre>\n");
    LOGPRINTF("<style>body {background-color: white;}</style>\n");
    
    LOGPRINTF("list_t [<b style=\"color:red;\">%p</b>]\n", (const void *) lst);
    LOGPRINTF("{\n");

    LOGPRINTF("    capacity = <b style=\"color:red;\">%zu</b>\n", lst->capacity);
    LOGPRINTF("    free = <b style=\"color:red;\">%zu</b>\n", lst->free);

    LOGPRINTF("    data: ");
    if (NULL != lst->nodes)
    {
        for (size_t i = 0; i < lst->capacity + 1; i++)
        {
            LOGPRINTF("<b style=\"color:red;\">%20lg</b> ", lst->nodes[i].data);
        }
        LOGPRINTF("\n");

        LOGPRINTF("    next: ");
        for (size_t i = 0; i < lst->capacity + 1; i++)
        {
            LOGPRINTF("<b style=\"color:green;\">%20zu</b> ", lst->nodes[i].next);
        }
        LOGPRINTF("\n");

        LOGPRINTF("    prev: ");
        for (size_t i = 0; i < lst->capacity + 1; i++)
        {
            LOGPRINTF("<b style=\"color:blue;\">%20zu</b> ", lst->nodes[i].prev);
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
    int num = rand();
    printf("Input %d to continue: ", num);

    int buf = 0;
    do
    {
        scanf("%d", &buf);
    }
    while (buf != num);
}
/*)---------------------------------------------------------------------------*/

/*(---------------------------------------------------------------------------*/
static void listGraphDump(const list_t *lst, const char *filename)
{
    CHECK(NULL != filename, ;);
    CHECK(NULL != lst, ;);
    CHECK(NULL != lst->nodes, ;);
    
    FILE *dotfile = fopen(gvizbuf, "w");
    CHECK(NULL != dotfile, ;);

    fprintf(dotfile, "digraph\n");
    fprintf(dotfile, "{\n");

    fprintf(dotfile, "    rankdir=LR;\n");

    listIndex_t node = lst->nodes[NULL_INDEX].next;
    for (size_t i = 1; (i <= lst->capacity) && (node != NULL_INDEX); i++)
    {
        fprintf(dotfile,
                "    usednode%zu[style=\"rounded\",shape=record,color=\"red\",label="
                    "\" physical index=%zu | index=%zu | data=%lg | <next> next=%zu "
                    "| <prev> prev= %zu \"];\n", 
                i,
                node,
                i - 1,
                lst->nodes[node].data,
                lst->nodes[node].next,
                lst->nodes[node].prev
                );
        node = lst->nodes[node].next;
    }

    fprintf(dotfile, "    front[shape=\"octagon\",label=\"front = %zu\"];\n",
            lst->nodes[NULL_INDEX].next);
    fprintf(dotfile, "    back[shape=\"octagon\",label=\"back = %zu\"];\n",
            lst->nodes[NULL_INDEX].prev);

    size_t listlen = 0;
    node = lst->nodes[NULL_INDEX].next;
    for (size_t i = 1; (i <= lst->capacity) && (node != NULL_INDEX); i++, listlen++)
    {
        if (1 == i)
        {
            fprintf(dotfile, "    front->");
        }

        fprintf(dotfile, "usednode%zu:next", i);
        node = lst->nodes[node].next;

        if (NULL_INDEX != node)
        {
            fprintf(dotfile, "->");
        }
        else
        {
            fprintf(dotfile, ";\n");
        }
    }

    node = lst->nodes[NULL_INDEX].prev;
    for (size_t i = listlen; (i >= 1) && (node != NULL_INDEX); i--)
    {
        if (listlen == i)
        {
            fprintf(dotfile, "    back->");
        }

        fprintf(dotfile, "usednode%zu:prev", i);
        node = lst->nodes[node].prev;

        if (NULL_INDEX != node)
        {
            fprintf(dotfile, "->");
        }
        else
        {
            fprintf(dotfile, ";\n");
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
                lst->nodes[node].data,
                lst->nodes[node].next,
                lst->nodes[node].prev
                );
        node = lst->nodes[node].next;
    }

    node = lst->free;
    for (size_t i = 1; (i <= lst->capacity) && (node != NULL_INDEX); i++, listlen++)
    {
        if (1 == i)
        {
            fprintf(dotfile, "    free->");
        }

        fprintf(dotfile, "freenode%zu:next", i);
        node = lst->nodes[node].next;

        if (NULL_INDEX != node)
        {
            fprintf(dotfile, "->");
        }
        else
        {
            fprintf(dotfile, ";\n");
        }
    }

   
    fprintf(dotfile, "}");
    fclose(dotfile);

    char cmdbuf[128] = "";

    sprintf(cmdbuf, "dot %s -o %s -Tjpg", gvizbuf, filename);
    CHECK(0 == system(cmdbuf), ;);
}
/*)---------------------------------------------------------------------------*/
/*)===========================================================================*/

