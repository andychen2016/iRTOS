
#ifndef __TLIB_H__
#define __TLIB_H__

#include <stdint.h>

//常用定义

#ifndef NULL
#define NULL 0x00
#endif





//位图搜索相关变量及函数声明
//32位的位图，最高可以支持32个优先级
typedef struct _tBitmap
{
    uint32_t bitmap;
}Bitmap;




void BitmapInit(Bitmap* bm);



uint32_t BitmapMaxBit(void);


void BitmapSet(Bitmap* bm, uint32_t pos);



void BitmapClear(Bitmap* bm, uint32_t pos);



uint32_t BitmapGetFirstSet(Bitmap* bm);



/////双向链表节点定义

typedef struct _tNode
{
    struct _tNode* preNode;
    struct _tNode* nextNode;
}	Node;


void NodeInit(Node* node);





//链表定义

typedef struct _List
{
    Node headNode;
    uint32_t nodeCount;
}List;


//获取父结构体的指针，用于操作父结构体
#define ParentAddress(node, parent, name) (parent*)((uint32_t)node - (uint32_t)&((parent*)0)->name)


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



void ListInsertAfter(List* list, Node* nodeAfter, Node* node_to_insert);



void ListRemove(List* list, Node* node);



#endif





