#include "trie.h"

using namespace std;

Trie::~Trie(void)
{
	if (left_)
		delete left_;

	if (right_)
		delete right_;
}

Trie &Trie::operator=(const Trie &tree)
{
	sym_ = tree.sym_;
	isEnd_ = tree.isEnd_;

	if (left_)
		delete left_;

	if (right_)
		delete right_;

	left_ = new Trie;
	right_ = new Trie;

	left_ = tree.left_;
	right_ = tree.right_;

	return *this;
}

void Trie::add(BV key, uint8_t sym)
{
	Trie *node = this;

	while (key.size()) {
		if (key[key.size()-1] == 0) {
			if (node->left_ == nullptr)
				node->left_ = new Trie();

			node = node->left_;
		} else {
			if (node->right_ == nullptr)
				node->right_ = new Trie();

			node = node->right_;
		}

		key.resize(key.size()-1);
	}

	node->isEnd_ = true;
	node->sym_ = sym;
}

bool Trie::staticSearch(bool bit, uint8_t *sym)
{
	static Trie *node = this;

	if (bit == 0)
		node = node->left_;
	else
		node = node->right_;

	if (!node->isEnd_ || node == nullptr)
		return 1;

	*sym = node->sym_;
	node = this;
	return 0;
}

