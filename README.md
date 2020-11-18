File format
Header:
	2 byte - amount of Huffman codes in Huffman table
	8 bytes - amount of codes in whole file
	6 bytes on each * amount - huffman code with sym(1)/count(1)/code(4)
Body:
	Stream of bits
