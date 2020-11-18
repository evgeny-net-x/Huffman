#ifndef __TRIE_H__
#define __TRIE_H__

#include <cstdio>
#include <cstdint>
#include "BV.h"

class Trie
{
private:
	Trie *left_;
	Trie *right_;
	bool isEnd_;
	uint8_t sym_;

public:
	Trie(void)
	{
		left_ = nullptr;
		right_ = nullptr;
		isEnd_ = false;
		sym_ = 0;
	};

	Trie(const Trie &trie)
	{
		left_ = nullptr;
		right_ = nullptr;
		*this = trie;
	}

	~Trie(void);

	Trie &operator=(const Trie &);

	void add(BV key, uint8_t sym);
	bool staticSearch(bool bit, uint8_t *sym);
};

#endif
