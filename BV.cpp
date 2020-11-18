#include "BV.h"

using namespace std;

BoolRank &BoolRank::operator=(bool value)
{
	bv_.set(index_, value);
	return *this;
};

BoolRank &BoolRank::operator=(const BoolRank &rank)
{
	bv_[index_] = rank.bv_.at(rank.index_);
	return *this;
};

BoolRank::operator bool(void)
{
	return bv_.at(index_);
}


void BV::resize(int size)
{
	if (size < 0)
		return;

	if (8*maxsize_ < size) {
		int bytes = 2*this->bit2byte(size);
		uint8_t *mem = new uint8_t[bytes];

		int begin = bytes-maxsize_;
		int bvBegin = maxsize_-maxsize_;
		for (int i = 0; i < maxsize_; i++)
			mem[begin+i] = bytes_[bvBegin+i];

		for (int i = 0; i < bytes-maxsize_; i++)
			mem[i] = 0;

		if (bytes_)
			delete[] bytes_;

		bytes_ = mem;
		size_ = size;
		maxsize_ = bytes;
	} else {
		int maxSize = max(size_, size);
		for (int i = min(size_, size); i < maxSize; i++)
			bytes_[maxsize_-1 - i/8] &= ~(1 << i%8);

		size_ = size;
	}
}

BV::BV(int size, bool value)
{
	if (size < 0)
		size = 0;

	size_ = size;
	maxsize_ = this->bit2byte(size);
	bytes_ = new uint8_t[maxsize_];

	this->setAll(value);
}

BV::BV(uint8_t *bits, int size)
{
	if (size < 0)
		size = 0;

	size_ = size;
	maxsize_ = this->bit2byte(size);
	bytes_ = new uint8_t[maxsize_];

	for (int i = 0; i < maxsize_; i++)
		bytes_[i] = bits[i];

	//printf("VALUE: %u\n", bytes_[0]);
}

BV::BV(const char *str)
{
	size_ = strlen(str);
	maxsize_ = this->bit2byte(size_);
	bytes_ = new uint8_t[maxsize_];

	for (int i = 0; i < size_; i++) {
		if (str[i] != '1')
			continue;

		int index = i;
		if (size_%8 != 0)
			index += 8-size_%8;
		bytes_[index/8] |= 0x80 >> (index%8);
	}
}

BV &BV::operator=(const BV &bv)
{
	if (this == &bv)
		return *this;

	size_ = bv.size_;
	maxsize_ = bv.maxsize_;

	delete[] bytes_;
	bytes_ = new uint8_t[maxsize_];

	for (int i = 0; i < maxsize_; i++)
		bytes_[i] = bv.bytes_[i];

	return *this;
}

BV BV::operator~(void)
{
	BV bv(*this);

	for (int i = 0; i < bv.maxsize_; i++)
		bv.bytes_[i] = ~bv.bytes_[i];

	for (int i = bv.size_; i < 8*bv.maxsize_; i++)
		bv.bytes_[bv.maxsize_-1 - i/8] &= ~(1 << i%8);

	return bv;
}

BV &BV::operator&=(const BV &bv)
{
	int minSize = min(maxsize_, bv.maxsize_);
	for (int i = 0; i < minSize; i++)
		bytes_[maxsize_-1 - i] &= bv.bytes_[bv.maxsize_-1 - i];

	if (bv.size_ < size_)
		for (int i = this->bit2byte(bv.size_); i < size_; i++)
			bytes_[i] = 0;

	return *this;
}

BV &BV::operator|=(const BV &bv)
{
	int minSize = min(maxsize_, bv.maxsize_);
	int max_ = maxsize_ - minSize;
	int bvMax_ = bv.maxsize_ - minSize;

	for (int i = 0; i < minSize; i++)
		bytes_[max_ + i] |= bv.bytes_[bvMax_ + i];

	return *this;
}

BV &BV::operator^=(const BV &bv)
{
	int minSize = min(maxsize_, bv.maxsize_);
	for (int i = 0; i < minSize; i++)
		bytes_[maxsize_-1 - i] ^= bv.bytes_[bv.maxsize_-1 - i];

	return *this;
}

BV &BV::operator<<=(int offset)
{
	if (offset < 0)
		return *this;

	int bitOffset = offset%8;
	int byteOffset = offset/8;

	this->resize(size_+offset);

	int shift = 8 - bitOffset;
	uint8_t mask = 0xFF << shift;
	for (int i = byteOffset; i < maxsize_-1; i++) {
		uint8_t byte = bytes_[i] << bitOffset;
		uint8_t carry = (bytes_[i+1] & mask) >> shift;

		bytes_[i-byteOffset] = byte | carry;
	}

	bytes_[maxsize_-1 - byteOffset] = bytes_[maxsize_-1] << bitOffset;

	this->set(0, 0, offset);

	return *this;
}

BV &BV::operator>>=(int offset)
{
	int byteOffset = offset/8;
	int bitOffset = offset%8;
	
	for (int i = maxsize_-1-byteOffset; i >= 0; i--) {
		uint8_t byte = bytes_[i] >> bitOffset;
		uint8_t mask = 0xFF >> (8-bitOffset);

		uint8_t carry = 0;
		if (i != 0)
			carry = (bytes_[i-1] & mask) << (8-offset);

		bytes_[i+byteOffset] = byte | carry;
	}

	this->resize(size_-offset);

	return *this;
}

bool BV::operator==(const BV &bv)
{
	if (size_ != bv.size_)
		return false;

	for (int i = 0; i < this->bit2byte(size_); i++)
		if (bytes_[maxsize_-1 - i] != bv.bytes_[bv.maxsize_-1 - i])
			return false;

	return true;
}

ostream &operator<<(ostream &out, const BV &bv)
{
	for (int i = 8*bv.maxsize_-bv.size_; i < 8*bv.maxsize_; i++)
		out << ((bv.bytes_[i/8] & (0x80 >> (i%8))) > 0);

	return out;
}

istream &operator>>(istream &in, BV &bv)
{
	string str;
	in >> str;

	bv = BV(str.c_str());
	return in;
}

void BV::set(int index, bool value, int count)
{
	if (index+count < 0 || count < 1)
		return;

	if (index+count > size_)
		this->resize(index+count);

	int minSize = min(size_, index+count);
	if (!value) {
		/*for (int i = max(0, index/8); i < minSize; i++)
			bytes_[maxsize_-1 - i/8] &= ~(1 << i%8);*/

		for (int i = max(0, index/8); i < minSize/8; i++)
			bytes_[maxsize_-1 - i/8] &= 0x00;

		for (int i = 8*(minSize/8); i < minSize; i++)
			bytes_[maxsize_-1 - i/8] &= ~(1 << i%8);
	} else {
		/*for (int i = max(0, index/8); i < minSize; i++)
			bytes_[maxsize_-1 - i] |= (1 << i%8);*/

		for (int i = max(0, index/8); i < minSize/8; i++)
			bytes_[maxsize_-1 - i/8] |= 0xFF;

		for (int i = 8*(minSize/8); i < minSize; i++)
			bytes_[maxsize_-1 - i/8] |= (1 << i%8);
	}
}

void BV::setAll(bool value)
{
	uint8_t byte = value ? 0xFF : 0x00;
	for (int i = 0; i < this->bit2byte(size_); i++)
		bytes_[maxsize_-1 - i] = byte;

	if (value == 1 && size_%8 != 0)
		bytes_[maxsize_ - this->bit2byte(size_)] &= (0xFF >> (8-size_%8));
}

void BV::inverse(int index, int count)
{
	if (index+count < 0 || count < 1)
		return;

	if (index+count > size_)
		this->resize(index+count);

	int bitCount = min(size_, index+count);
	for (int i = max(0, index/8); i < bitCount/8; i++)
		bytes_[maxsize_-1 - i] ^= 0xFF;

	bytes_[maxsize_-1 - bitCount/8] ^= (0xFF >> (8-bitCount%8));
}

int BV::weight(void)
{
	int weight = 0;

	// count weight of fully using bytes
	for (int i = 0; i < size_/8; i++) {
		int byte = bytes_[maxsize_-1 - i];

		while (byte) {
			byte &= byte-1; // reseting far right non-zero bit
			weight++;
		}
	}

	// Count weight of last using byte
	for (int i = 0; i < size_%8; i++)
		weight += (bytes_[maxsize_-1 - size_/8] & (1 << i)) > 0;

	return weight;
}

