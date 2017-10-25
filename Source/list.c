

#include "lib.h"


void NodeInit(Node* node)
{
    node->preNode = node;
    node->nextNode = node;
}

void ListInit(List* list)
{
    list->headNode.nextNode = &(list->headNode);
    list->headNode.preNode = &(list->headNode);
    list->nodeCount = 0;
}

uint32_t ListNodeCount(List* list)
{
    return list->nodeCount;
}

Node* ListFirst(List* list)
{
    Node* node = (Node*)0;

    if(list->nodeCount > 0)
    {
        node = list->headNode.nextNode;
    }
    return node;
}

Node* ListLast(List* list)
{
    Node* node = (Node*)0;

    if(list->nodeCount > 0)
    {
        node = list->headNode.preNode;
    }
    return node;
}

Node* ListPreNode(List* list, Node* node)
{
    if(node->preNode == node)
    {
        return (Node*)0;
    }
    else
    {
        return node->preNode;
    }
}

Node* ListNextNode(List* list, Node* node)
{
    if(node->nextNode == node)
    {
        return (Node*)0;
    }
    else
    {
        return node->nextNode;
    }
}

//防范是遍历所有节点，然后将所有节点前后节点指向自己,最后将List至于初始化状态
void ListRemoveAll(List* list)
{
    uint32_t count;
    Node* nextNode;
    Node* tempNode;

    nextNode = list->headNode.nextNode;
    for(count = list->nodeCount; count != 0; count--)
    {
        tempNode = nextNode;
        nextNode = nextNode->nextNode;

        tempNode->preNode = tempNode;
        tempNode->nextNode = tempNode;
    }

    list->headNode.preNode = &(list->headNode);
    list->headNode.nextNode = &(list->headNode);
    list->nodeCount = 0;
}

void ListAddFirst(List* list, Node* node)
{
    node->preNode = list->headNode.nextNode->preNode;
    node->nextNode = list->headNode.nextNode;

    list->headNode.nextNode->preNode = node;
    list->headNode.nextNode = node;
    
    list->nodeCount++;
}

void ListAddLast(List* list, Node* node)
{
    node->nextNode = &(list->headNode);
    node->preNode = list->headNode.preNode;

    list->headNode.preNode->nextNode = node;
    list->headNode.preNode = node;

    list->nodeCount ++;

}

Node* ListRemoveFirst(List* list)
{
    Node* node = (Node*)0;

    if(list->nodeCount > 0)
    {
        node = list->headNode.nextNode;
        list->headNode.nextNode = node->nextNode;
        node->nextNode->preNode = &(list->headNode);
        list->nodeCount--;
    }
    return node;
}

Node* ListRemovLast(List* list)
{
    Node* node = (Node*)0;

    if(list->nodeCount > 0)
    {
        node = list->headNode.preNode;
        list->headNode.preNode = node->preNode;
        node->preNode->nextNode = &(list->headNode);
        list->nodeCount --;
    }
    return node;
}

void ListInsertAfter(List* list, Node* nodeAfter, Node* nodeToInsert)
{
    nodeToInsert->preNode = nodeAfter;
    nodeToInsert->nextNode = nodeAfter->nextNode;

    nodeAfter->nextNode->preNode = nodeToInsert;
    nodeAfter->nextNode = nodeToInsert;

    list->nodeCount --;
}


void ListRemove(List* list, Node* node)
{
    node->preNode->nextNode = node->nextNode;
    node->nextNode->preNode = node->preNode;
    list->nodeCount --;
}

