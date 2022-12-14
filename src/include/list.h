#ifndef LIST_H_INCLUDED__
#define LIST_H_INCLUDED__


#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
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

    bool linearized;
} list_t;
/*(===========================================================================*/


/*(===========================================================================*/
enum LIST_CODES
{
    LIST_SUCCESS = 0,

    LIST_NULLPTR   = 1,
    LIST_NOMEM     = 2,
    LIST_INITERR   = 3,
    LIST_SIZEERR   = 4,
    LIST_INVALID   = 5,
    LIST_OUTRANGE  = 6,
    LIST_WRONGITER = 7,
    LIST_NULLINDEX = 8
};

enum LIST_STATUS
{
    INVALID_DATA_PTR = 1 << 0,
    INVALID_CAPACITY = 1 << 1,
    INVALID_LINKING  = 1 << 2
};
/*(===========================================================================*/


/*(===========================================================================*/
enum LIST_CODES listCtor(list_t *lst, size_t capacity);

listIndex_t listInsertAfter(list_t *lst, listIndex_t iter, listElem_t newelem);

listIndex_t listInsertBefore(list_t *lst, listIndex_t iter, listElem_t newelem);

listIndex_t listPushBack(list_t *lst, listElem_t newelem);

listIndex_t listPushFront(list_t *lst, listElem_t newelem);

listIndex_t listIndex(list_t *lst, size_t index);

listIndex_t listNext(list_t *lst, listIndex_t iter);

listIndex_t listPrev(list_t *lst, listIndex_t iter);

enum LIST_CODES listIter(list_t *lst, listIndex_t iter, listElem_t *dest);

enum LIST_CODES listBack(list_t *lst, listElem_t *dest);

enum LIST_CODES listFront(list_t *lst, listElem_t *dest);

enum LIST_CODES listRemove(list_t *lst, listIndex_t iter);

enum LIST_CODES listLinearize(list_t *lst);

enum LIST_CODES listDtor(list_t *lst);

enum LIST_CODES listVerify(list_t *lst);

enum LIST_CODES listClear(list_t *lst);

bool listEmpty(const list_t *lst);
/*(===========================================================================*/


#endif /* LIST_H_INCLUDED__ */

