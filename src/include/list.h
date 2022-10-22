#ifndef LIST_H_INCLUDED__
#define LIST_H_INCLUDED__


#include <stddef.h>
#include <stdlib.h>
#include "list_settings.h"


typedef struct
{
    listElem_t data;

    listIndex_t next;
    listIndex_t prev;
} listNode_t;

typedef struct
{
    listNode_t dummy;
    listNode_t *nodes;
    size_t capacity;

    listIndex_t free;
} list_t;


enum LIST_CODES
{
    LIST_SUCCESS = 0,

    LIST_NULLPTR = 1,
    LIST_NOMEM   = 2,
    LIST_INITERR = 3,
    LIST_SIZEERR = 4
};


enum LIST_CODES listCtor(list_t *lst, size_t capacity);

enum LIST_CODES listDtor(list_t *lst);


#endif /* LIST_H_INCLUDED__ */

