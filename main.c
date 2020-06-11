#include "huffman.h"
#include <string.h>

#define printBits(word, count) \
do { \
	for (int i = 0; i < count; i++) \
		printf("%d", ((word) & (1 << (count-1-i))) > 0); \
} while (0)

void printCode(Code *code)
{
	for (int i = 0; i < code->count; i++)
		printf("%d", (code->code & (1 << (code->count-1-i))) > 0);
}

void printTable(Code *table, int length)
{
	for (int i = 0; i < length; i++) {
		if (table[i].sym == '\n')
			printf("\tSym %d, Freq %f, Code '", table[i].sym, table[i].freq);
		else
			printf("\tSym %d(%c), Freq %f, Code '", table[i].sym, table[i].sym, table[i].freq);

		printCode(&table[i]);
		printf("'\n");
	}
}

Code *searchCodeBySymbol(Code *table, int length, uint8_t sym)
{
	int low = 0;
	int high = length-1;

	while (low <= high) {
		int mid  = low + (high-low) / 2;

		if (table[mid].sym == sym)
			return &table[mid];
		else if (table[mid].sym < sym)
			low = mid+1;
		else
			high = mid-1;
	}

	return NULL;
}

Code *searchCodeByCode(Code *table, int length, uint8_t code, uint8_t count)
{
	for (int i = 0; i < length; i++)
		if (table[i].count == count && table[i].code == code)
			return &table[i];
	return NULL;
}

Code *createFrequencyTable(char *filename, int *length)
{
	uint8_t buffer[1024];
	float freqTable[256] = {0};

	FILE *file = fopen(filename, "rb");
	if (!file)
		return NULL;

	int count;
	while ((count = fread(buffer, 1, 1024, file)) != 0)
		for (int i = 0; i < count; i++)
			freqTable[buffer[i]]++;

	count = 0;
	float freqSum = 0;
	for (int i = 0; i < 256; i++) {
		if (freqTable[i] > 0) {
			count++;
			freqSum += freqTable[i];
		}
	}

	Code *table = malloc(count*sizeof(Code));
	for (int i = 0, j = 0; i < 256; i++) {
		if (freqTable[i] > 0) {
			table[j].freq = freqTable[i];// / freqSum;
			table[j].sym = i;
			j++;
		}
	}

	*length = count;
	fclose(file);	
	return table;
}

int encode(char *filename)
{
	int length;
	Code *table = createFrequencyTable(filename, &length);
	createEncodingTable(table, length);

	FILE *file = fopen(filename, "rb");
	if (!file)
		goto FileError;

	// Запись заголовка: кол-во кодов в таблице (1), кол-во бит сообщения (8), коды таблицы из sym/code/count (24*length), сообщение (...)
	{
		uint8_t codeCount = length;
		fwrite(&codeCount, 1, 1, stdout);

		uint64_t msgLength = 0;
		for (int i = 0; i < length; i++)
			msgLength += table[i].freq*table[i].count;
		fwrite(&msgLength, 8, 1, stdout);

		for (int i = 0; i < length; i++) {
			fwrite(&table[i].sym, 1, 1, stdout);
			fwrite(&table[i].code, 1, 1, stdout);
			fwrite(&table[i].count, 1, 1, stdout);
		}
	}

	// Запись сообщения
	uint8_t sym;
	uint16_t byte = 0;
	int bitIndex = 0;
	while (fread(&sym, 1, 1, file) != 0) {
		Code *code = searchCodeBySymbol(table, length, sym);

		for (int i = 0; i < code->count; i++) {
			byte |= (((code->code & (1 << (code->count-1-i))) > 0) << (15-bitIndex));
			bitIndex++;
		}
		
		if (bitIndex >= 8) {
			uint8_t data = (byte & 0xFF00) >> 8;
			fwrite(&data, 1, 1, stdout);
			byte <<= 8;
			bitIndex -= 8;
		}
	}

	uint8_t data = (byte & 0xFF00) >> 8;
	fwrite(&data, 1, 1, stdout);

	fclose(file);
	return 0;
	FileError:
		return 1;
}

int decode(char *filename)
{
	FILE *file = fopen(filename, "rb");
	if (!file)
		goto FileError;

	uint8_t length;
	fread(&length, 1, 1, file);

	uint64_t msgLength;
	fread(&msgLength, 8, 1, file);

	Code *table = malloc(length*sizeof(Code));
	if (!table)
		goto MallocError;

	for (int i = 0; i < length; i++) {
		table[i].freq = 0;
		fread(&table[i].sym, 1, 1, file);
		fread(&table[i].code, 1, 1, file);
		fread(&table[i].count, 1, 1, file);
	}

	// Чтение сообщения
	uint8_t byte;
	uint8_t sym = 0;
	uint8_t count = 0;
	while (fread(&byte, 1, 1, file) != 0) {
		for (int i = 0; i < 8; i++) {
			sym = 2*sym + ((byte & (1 << (7-i))) > 0);
			count++;
		
			Code *code = searchCodeByCode(table, length, sym, count);
			if (code) {
				sym = 0;
				count = 0;
				fwrite(&code->sym, 1, 1, stdout);
			}
		}
	}

	fclose(file);
	return 0;
	MallocError:
		fclose(file);
	FileError:
		return 1;
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		printf("Usage: %s mode filename\n", argv[0]);
		return 1;
	}

	if (strcmp(argv[1], "zip") == 0)
		encode(argv[2]);
	else if (strcmp(argv[1], "unzip") == 0)
		decode(argv[2]);
	else {
		printf("Incorrect mode. Allowed modes: zip, unzip\n");
		return 1;
	}

	return 0;
}
