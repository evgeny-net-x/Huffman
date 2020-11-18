#ifndef __TREE_H__
#define __TREE_H__

#include <cstdint>
#include <vector>
#include "BV.h"

class HuffmanTree
{
public:
	uint64_t freq;
	uint8_t index;
private:
	HuffmanTree *left_;
	HuffmanTree *right_;

	void createDictionary_(BV *dict, BV code)
	{
		BV nextCode = code << 1;
		if (left_)
			left_->createDictionary_(dict, nextCode);

		nextCode[0] = 1;
		if (right_)
			right_->createDictionary_(dict, nextCode);

		if (!left_ && !right_)
			dict[index] = code;
	};
public:
	HuffmanTree(uint64_t freq_=0, uint8_t index_=0)
	{
		freq = freq_;
		index = index_;
		left_ = nullptr;
		right_ = nullptr;
	};

	HuffmanTree(const HuffmanTree &tree) { *this = tree; };

	~HuffmanTree(void)
	{
		if (left_)
			delete left_;

		if (right_)
			delete right_;
	};

	HuffmanTree &operator=(const HuffmanTree &tree)
	{
		freq = tree.freq;
		index = tree.index;

		left_ = new HuffmanTree;
		right_ = new HuffmanTree;

		left_ = tree.left_;
		right_ = tree.right_;

		return *this;
	}

	bool operator>(const HuffmanTree &node) {return freq > node.freq;}
	bool operator<(const HuffmanTree &node) {return freq < node.freq;}
	bool operator==(const HuffmanTree &node) {return freq == node.freq;}
	bool operator>=(const HuffmanTree &node) {return freq >= node.freq;}
	bool operator<=(const HuffmanTree &node) {return freq <= node.freq;}

	void setLeft(HuffmanTree *node)
	{
		if (left_ != nullptr)
			delete left_;

		left_ = node;
	};

	void setRight(HuffmanTree *node)
	{
		if (right_ != nullptr)
			delete right_;

		right_ = node;
	};

	BV *createDictionary(void)
	{
		BV code;
		BV *dict = new BV[256];

		this->createDictionary_(dict, code);

		return dict;
	};
};

#endif
