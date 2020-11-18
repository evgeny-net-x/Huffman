#ifndef __HEAP_H__
#define __HEAP_H__

#include <vector>
#include "tree.h"

//template<class HuffmanTree>
class MinHeap
{
private:
	std::vector<HuffmanTree *> nums_;

public:
	MinHeap(void) {};
	MinHeap(const MinHeap &heap) { nums_ = heap.nums_; };

	int size(void) { return nums_.size(); };

	HuffmanTree *pop(void);
	void push(HuffmanTree *);
};

#endif
