#include "rtos_lib.h"

/************************ Double Linked List Data Structure	************************/
void tNodeInit (tNode * node)
{
	node->nextNode = node;
	node->preNode = node;
}

#define	firstNode headNode.nextNode
#define	lastNode headNode.preNode

void tListInit (tList * list)
{
	list->firstNode = &(list->headNode);
	list->lastNode = &(list->headNode);
	list->nodeCount = 0;
}

uint32_t tListCount (tList * list)
{
	return list->nodeCount;
}

tNode * tListFirst (tList * list)
{
	tNode * node = (tNode *)0;
	
	if (list->nodeCount != 0)
	{
		node = list->firstNode;
	}
	return node;
}

tNode * tListLast (tList * list)
{
	tNode * node = (tNode *)0;
	
	if (list->nodeCount != 0)
	{
		node = list->lastNode;
	}
	return node;
}

tNode * tListPre (tList * list, tNode * node)
{
	if (node->preNode == node)
	{
		return (tNode *)0;
	}
	else
	{
		return node->preNode;
	}
}

tNode * tListNext (tList * list, tNode * node)
{
	if (node->nextNode == node)
	{
		return (tNode *)0;
	}
	else
	{
		return node->nextNode;
	}
}

void tListRemoveAll (tList * list)
{
	uint32_t count;
	tNode * nextNode;
	
	nextNode = list->firstNode;
	for (count = list->nodeCount; count != 0; count--)
	{
		tNode * currentNode = nextNode;
		nextNode = nextNode->nextNode;
		
		currentNode->nextNode = currentNode;
		currentNode->preNode = currentNode;
	}
	
	list->firstNode = &(list->headNode);
	list->lastNode = &(list->headNode);
	list->nodeCount = 0;
}

void tListAddFirst (tList * list, tNode * node)
{
	node->preNode = list->firstNode->preNode;
	node->nextNode = list->firstNode;
	
	list->firstNode->preNode = node;
	list->firstNode = node;
	list->nodeCount++;
}

void tListAddLast (tList * list, tNode * node)
{
	node->nextNode = &(list->headNode);
	node->preNode = list->lastNode;
	
	list->lastNode->nextNode = node;
	list->lastNode = node;
	list->nodeCount++;
}

tNode * tListRemoveFirst (tList * list)
{
	tNode * node = (tNode *)0;
	
	if (list->nodeCount != 0)
	{
		node = list->firstNode;
		
		node->nextNode->preNode = &(list->headNode);
		list->firstNode = node->nextNode;
		list->nodeCount--;
	}
	return node;
}

void tListInsertAfter (tList * list, tNode * nodeAfter, tNode * nodeToInsert)
{
	nodeToInsert->preNode = nodeAfter;
	nodeToInsert->nextNode = nodeAfter->nextNode;
	
	nodeAfter->nextNode->preNode = nodeToInsert;
	nodeAfter->nextNode = nodeToInsert;
	
	list->nodeCount++;
}

void tListRemove (tList * list, tNode * node)
{
	node->preNode->nextNode = node->nextNode;
	node->nextNode->preNode = node->preNode;
	list->nodeCount--;
}

/************************ Bitmap Data Structure	************************/
void tBitmapInit (tBitmap * bitmap)
{
	bitmap->bitmap = 0;
}

uint32_t tBitmapPosCount (void)
{
	return 32;
}

void tBitmapSet (tBitmap * bitmap, uint32_t pos)
{
	bitmap->bitmap |= 1 << pos;
}

void tBitmapClear (tBitmap * bitmap, uint32_t pos)
{
	bitmap->bitmap &= ~(1 << pos);
}

uint32_t tBitmapGetFirstSet (tBitmap * bitmap)
{
	static const uint8_t quickFindTable[] = 
	{
		/* 00 */ 0xff, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 10 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 20 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 30 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 40 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 50 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 60 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 70 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 80 */ 7,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 90 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* A0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* B0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* C0 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* D0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* E0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* F0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
	};
	
	if (bitmap->bitmap & 0xFF)
	{
		return quickFindTable[bitmap->bitmap & 0xFF];
	}
	else if (bitmap->bitmap & 0xFF00)
	{
		return quickFindTable[(bitmap->bitmap >> 8) & 0xFF] + 8;
	}
	else if (bitmap->bitmap & 0xFF0000)
	{
		return quickFindTable[(bitmap->bitmap >> 16) & 0xFF] + 16;
	}
	else if (bitmap->bitmap & 0xFF000000)
	{
		return quickFindTable[(bitmap->bitmap >> 24) & 0xFF] + 24;
	}
	else
	{
		return tBitmapPosCount();
	}
}
