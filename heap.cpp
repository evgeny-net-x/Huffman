#include "heap.h"

using namespace std;

HuffmanTree *MinHeap::pop(void)
{
	if (nums_.size() == 0)
		return 0;

	HuffmanTree *min = nums_[0];
	nums_[0] = nums_.back();
	nums_.pop_back();

	int i = 0;
	while (i < nums_.size()) {
		int min_index = i;

		if (2*i + 1 < nums_.size() && nums_[2*i + 1]->freq < nums_[min_index]->freq)
			min_index = 2*i + 1;

		if (2*i + 2 < nums_.size() && nums_[2*i + 2]->freq < nums_[min_index]->freq)
			min_index = 2*i + 2;

		if (i == min_index)
			break;

		swap(nums_[i], nums_[min_index]);
		i = min_index;
	}

	return min;
}

void MinHeap::push(HuffmanTree *num)
{
	nums_.push_back(num);

	int i = nums_.size()-1;
	while (i > 0 && nums_[i]->freq < nums_[(i-1)/2]->freq) {
		swap(nums_[i], nums_[(i-1)/2]);

		i = (i-1)/2;
	}
}
