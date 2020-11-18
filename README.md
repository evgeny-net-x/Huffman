# Huffman compressor/decompressor

## Header of encoded file:
| 2 bytes                    | 8 bytes                         | 6 bytes * amount of codes  |
| -------------------------- | ------------------------------- | -------------------------- |
| Amount of codes in table   | Amount of codes in file         | sym(1), count(1), code(4)  |

Example:
- `./Huffman encode file.txt > file_compressed`
- `./Huffman decode file_compressed > file_copy.txt`
