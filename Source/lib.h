
#ifndef __TLIB_H__
#define __TLIB_H__

#include <stdint.h>


//位图搜索相关函数
typedef struct
{
    uint32_t bitmap;
}tBitmap;


void BitmapInit(tBitmap* bm);

uint32_t BitmapMaxBit(void);

void BitmapSet(tBitmap* bm, uint32_t pos);

void BitmapClear(tBitmap* bm, uint32_t pos);

uint32_t BitmapGetFirstSet(tBitmap* bm);



/////双向链表

typedef struct _Node
{
    struct _Node* preNode;
    struct _Node* nextNode;
}Node;


void NodeInit(Node* node);

typedef struct _List
{
    Node headNode;
    uint32_t nodeCount;
}List;

#define ParentAddress(node, Parent, name) (Parent*)((uint32_t)node - (uint32_t)&((Parent*)0)->name)


void ListInit(List* list);

uint32_t ListNodeCount(List* list);

Node* ListFirst(List* list);

Node* ListLast(List* list);

Node* ListPreNode(List* list, Node* node);

Node* ListNextNode(List* list, Node* node);

void NodeRemoveAll(List* list);

void ListAddFirst(List* list, Node* node);

void ListAddLast(List* list, Node* node);

Node* ListRemoveFirst(List* list);

Node* ListRemoveLast(List* list);

void ListInsertAfter(List* list, Node* nodeAfter, Node* nodeToInsert);

void ListRemove(List* list, Node* node);



#endif





