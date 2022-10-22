#ifndef LIST_H_INCLUDED__
#define LIST_H_INCLUDED__


#include "list_settings.h"


typedef struct
{
    listElem_t data;

    listIndex_t next;
    listIndex_t prev;
} listNode_t;

typedef struct
{
    
} list_t;


#endif /* LIST_H_INCLUDED__ */

