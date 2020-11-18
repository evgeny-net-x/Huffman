#include "decode.h"

using namespace std;

int decode(const char *filename)
{
	ios_base::sync_with_stdio(false);

	FILE *file = fopen(filename, "rb");
	if (!file) {
		cerr << "Can't open '" << filename << "' file\n";
		return 1;
	}

	uint16_t size;
	uint64_t codeCount;

	fread(&size, sizeof(size), 1, file);
	fread(&codeCount, sizeof(codeCount), 1, file);

	Trie dict;
	for (int i = 0; i < size; i++) {
		uint8_t sym;
		uint8_t count;
		uint8_t value[4];

		fread(&sym, sizeof(sym), 1, file);
		fread(&count, sizeof(count), 1, file);
		fread(value, sizeof(value[0]), 4, file);

		BV code(value, 32);
		code.resize(count);

		dict.add(code, sym);
	}

	uint8_t byte;
	while (fread(&byte, sizeof(byte), 1, file) != 0) {
		for (int i = 0; i < 8 && codeCount != 0; i++) {
			uint8_t sym;
			bool bit = (byte & (0x80 >> i)) > 0;

			if (dict.staticSearch(bit, &sym) == 0) {
				fwrite(&sym, sizeof(uint8_t), 1, stdout);
				//cout << (char) sym;
				codeCount--;
			}
		}
	}

	fclose(file);
	return 0;
}

