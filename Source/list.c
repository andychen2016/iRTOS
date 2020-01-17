

#include "lib.h"


void NodeInit(Node* node)
{
    node->preNode = node;
    node->nextNode = node;
}

void ListInit(List* list)
{
	/**初始化头节点均指向自己**/
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
    Node* node = (Node*)NULL;

    if(list->nodeCount > 0)
    {
        node = list->headNode.nextNode;
    }
    return node;
}

Node* ListLast(List* list)
{
    Node* node = (Node*)NULL;

    if(list->nodeCount > 0)
    {
        node = list->headNode.preNode;
    }
    return node;
}

//多余函数
Node* ListPreNode(List* list, Node* node)
{
	
	if(node->preNode == node)
    {
        return (Node*)NULL;
    }
    else
    {
        return node->preNode;
    }
}

//多余函数
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


//方法是遍历所有节点，然后将所有节点前后节点指向自己,最后将List至于初始化状态
void ListRemoveAll(List* list)
{
    uint32_t count;
    Node* next_node;
    Node* temp_node;
	
	//先获取第一个Node
    next_node = list->headNode.nextNode;
	
    for(count = list->nodeCount; count != 0; count--)
    {
        temp_node = next_node;
		//next_node一直都指向下一个
        next_node = next_node->nextNode;
		//下一个Node和前一个Node均直接指向本Node
        temp_node->preNode = temp_node;
        temp_node->nextNode = temp_node;
    }
	//List中Node前一个Node和下一个Node均指向headNode
	//同ListInit函数
    list->headNode.preNode = &(list->headNode);
    list->headNode.nextNode = &(list->headNode);
    list->nodeCount = 0;
}

/*
 *
 */
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
    Node* node = (Node*)NULL;

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

void ListInsertAfter(List* list, Node* node_after, Node* node_to_insert)
{
    node_to_insert->preNode = node_after;
    node_to_insert->nextNode = node_after->nextNode;

    node_after->nextNode->preNode = node_to_insert;
    node_after->nextNode = node_to_insert;

    list->nodeCount ++;
}


void ListRemove(List* list, Node* node)
{
    node->preNode->nextNode = node->nextNode;
    node->nextNode->preNode = node->preNode;
    list->nodeCount --;
}

