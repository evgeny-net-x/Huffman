#ifndef __ENCODE_H__
#define __ENCODE_H__

#include <iostream>
#include <fstream>

#include "heap.h"
#include "tree.h"

#define BUFFER_SIZE 1024

bool createFrequencyTable(const char *, uint64_t *);
BV *createDictionary(uint64_t *);
int encode(const char *);

#endif
