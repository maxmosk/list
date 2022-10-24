#ifndef LIST_H_INCLUDED__
#define LIST_H_INCLUDED__


#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "list_settings.h"


/*(===========================================================================*/
typedef struct
{
    listElem_t data;

    listIndex_t next;
    listIndex_t prev;
} listNode_t;

typedef uint_fast16_t listStatus_t;

typedef struct
{
    listNode_t *nodes;
    size_t capacity;
    listIndex_t free;

    listStatus_t status;
} list_t;
/*(===========================================================================*/


/*(===========================================================================*/
enum LIST_CODES
{
    LIST_SUCCESS = 0,

    LIST_NULLPTR = 1,
    LIST_NOMEM   = 2,
    LIST_INITERR = 3,
    LIST_SIZEERR = 4,
    LIST_INVALID = 5
};

enum LIST_STATUS
{
    INVALID_DATA_PTR = 1 << 0,
    INVALID_CAPACITY = 1 << 1
};
/*(===========================================================================*/


/*(===========================================================================*/
enum LIST_CODES listCtor(list_t *lst, size_t capacity);

listIndex_t listPushBack(list_t *lst, listElem_t newelem);

listIndex_t listPushFront(list_t *lst, listElem_t newelem);

enum LIST_CODES listDtor(list_t *lst);

enum LIST_CODES listVerify(list_t *lst);

bool listEmpty(const list_t *lst);
/*(===========================================================================*/


#endif /* LIST_H_INCLUDED__ */

