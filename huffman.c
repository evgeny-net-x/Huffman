#include "huffman.h"

typedef struct Tree {
	Tree *left;
	Tree *right;
	float freq;
	uint8_t index;
} Tree;

typedef struct List {
	List *next;
	Tree *node;
} List;

// При наличии двух и более частот равных 0 получаем бесконечный цикл
void createEncodingTable(Code *table, uint8_t length)
{
	List *nodeList = List_New();

	// Создаем список деревьев состоящих из одной вершины
	for (int i = 0; i < length; i++)
		List_Insert(nodeList, Tree_New(table[i].freq, i));

	// Объединяем по алгоритму Хаффмана деревья
	while (--length) {
		Tree *a = List_ExtractFirst(nodeList);
		Tree *b = List_ExtractFirst(nodeList);

		Tree *parent = Tree_New(a->freq+b->freq, 0);
		parent->left = a;
		parent->right = b;

		List_Insert(nodeList, parent);
	}

	Code code = {0};
	Tree *root = List_ExtractFirst(nodeList);
	createEncodingTableByTree(root, table, code);

	Tree_Delete(root);
	List_Delete(nodeList);
}

void createEncodingTableByTree(Tree *node, Code *table, Code code)
{
	Code tmp = {.code=2*code.code, .count=code.count+1};
	if (node->left)
		createEncodingTableByTree(node->left, table, tmp);

	tmp.code += 1;
	if (node->right)
		createEncodingTableByTree(node->right, table, tmp);

	if (node->left == NULL && node->right == NULL) {
		table[node->index].code = code.code;
		table[node->index].count = code.count;
	}
}

/*----------------------------------------------------*/
static Tree *Tree_New(float freq, uint8_t index)
{
	Tree *this = malloc(sizeof(Tree));
	if (!this)
		return NULL;

	*this = (Tree) {
		.freq = freq,
		.index = index
	};

	return this;
}

static void Tree_Delete(Tree *this)
{
	if (this->left)
		Tree_Delete(this->left);

	if (this->right)
		Tree_Delete(this->right);

	this->left = NULL;
	this->right = NULL;
	free(this);
}

/*----------------------------------------------------*/
static List *List_New(void)
{
	List *this = malloc(sizeof(List));
	if (!this)
		return NULL;

	*this = (List) {0};
	return this;
}

static int List_Insert(List *this, Tree *node)
{
	List *new = List_New();
	if (!new)
		return 1;
	
	new->node = node;

	List *list = this->next;
	List *prev = this;
	while (list) {
		if (list->node->freq > node->freq) {
			new->next = list;
			prev->next = new;
			return 0;
		}

		prev = list;
		list = list->next;
	}

	prev->next = new;
	return 0;
}

static Tree *List_ExtractFirst(List *this)
{
	Tree *node = this->next->node;
	List *list = this->next->next;

	free(this->next);
	this->next = list;

	return node;
}

static void List_Delete(List *this)
{
	List *list = this;
	List *next;

	while (list) {
		next = list->next;
		free(list);

		list = next;
	}
}

