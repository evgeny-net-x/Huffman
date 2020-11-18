#include <iostream>
#include <fstream>
#include <cstring>

#include "encode.h"
#include "decode.h"

using namespace std;

int main(int argc, char **argv)
{
	if (argc != 3) {
		cout << "Usage: " << argv[0] << " mode(encode/decode) filename" << endl;
		return 1;
	}

	if (strcmp(argv[1], "encode") == 0)
		encode(argv[2]);
	else if (strcmp(argv[1], "decode") == 0)
		decode(argv[2]);
	else {
		cout << "Incorrect mode. Allowed modes: encode, decode" << endl;
		return 1;
	}

	return 0;
}
