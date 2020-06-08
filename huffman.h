#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct Tree Tree;
typedef struct List List;

typedef struct Code {
	float freq;
	uint8_t sym;
	uint8_t code;
	uint8_t count;
} Code;


/*----------------------------------------------------*/
void createEncodingTable(Code *, uint8_t);
void createEncodingTableByTree(Tree *, Code *, Code);

/*----------------------------------------------------*/
static Tree *Tree_New(float, uint8_t);
static void Tree_Delete(Tree *);

/*----------------------------------------------------*/
static List *List_New(void);
static int List_Insert(List *, Tree *);
static Tree *List_ExtractFirst(List *);
static void List_Delete(List *);

#endif
