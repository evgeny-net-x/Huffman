#include "encode.h"

using namespace std;

bool createFrequencyTable(const char *filename, uint64_t *freqTable)
{
	uint8_t buffer[BUFFER_SIZE];
	FILE *file = fopen(filename, "r");
	if (!file)
		return 1;

	int count;
	while ((count = fread(buffer, 1, BUFFER_SIZE, file)) != 0)
		for (int i = 0; i < count; i++)
			freqTable[buffer[i]]++;

	fclose(file);
	return 0;
}

BV *createDictionary(uint64_t *freqTable)
{
	MinHeap heap;

	for (int i = 0; i < 256; i++) {
		if (freqTable[i] == 0)
			continue;

		HuffmanTree *node = new HuffmanTree;
		*node = HuffmanTree(freqTable[i], i);

		heap.push(node);
	}

	int size = heap.size();
	while (--size) {
		HuffmanTree *a = heap.pop();
		HuffmanTree *b = heap.pop();

		HuffmanTree *parent = new HuffmanTree(a->freq+b->freq, 0);
		parent->setLeft(a);
		parent->setRight(b);

		heap.push(parent);
	}

	HuffmanTree *root = heap.pop();
	BV *dict = root->createDictionary();
	delete root;

	return dict;
}

int encode(const char *filename)
{
	uint64_t freqTable[256] = {0};
	if (createFrequencyTable(filename, freqTable)) {
		cerr << "Freq table can't be created" << endl;
		return 1;
	}

	BV *dict = createDictionary(freqTable);

	FILE *file = fopen(filename, "rb");
	if (!file) {
		cerr << "File can't be opened" << endl;
		return 1;
	}

	// Write header
	{
		uint16_t size = 0;
		for (int i = 0; i < 256; i++)
			if (freqTable[i] > 0)
				size++;

		fwrite(&size, sizeof(size), 1, stdout); // Writing size of table

		uint64_t codeCount = 0;
		for (int i = 0; i < 256; i++)
			codeCount += freqTable[i];

		fwrite(&codeCount, sizeof(codeCount), 1, stdout);

		for (int i = 0; i < 256; i++) {
			if (freqTable[i] == 0)
				continue;

			fwrite(&i, sizeof(uint8_t), 1, stdout); // Writing symbol

			BV code = dict[i];

			uint8_t size = code.size();
			fwrite(&size, sizeof(size), 1, stdout); //Writing code size

			code.resize(32);
			fwrite(code.data(), sizeof(uint8_t), 4, stdout); // Writing binary code
		}
	}

	// Write stream of message bits
	BV result;
	uint8_t sym;
	uint16_t byte = 0;
	int bitIndex = 0;
	while (fread(&sym, sizeof(sym), 1, file) != 0) {
		BV &code = dict[sym];
		
		result <<= code.size();
		result |= code;

		// Optimization: the smaller size of bool vector better for <<
		if (result.size()%8 == 0) {
			fwrite(result.data(), result.size()/8, 1, stdout);
			result = BV();
		}
	}

	int size = result.size();
	if (size%8 != 0) {
		result <<= (8 - size%8);
		fwrite(result.data(), result.size()/8, 1, stdout);
	}

	fclose(file);
	delete[] dict;

	return 0;
}

