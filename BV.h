#ifndef __BV_H__
#define __BV_H__

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <iomanip>

class BV;

class BoolRank {
private:
	int index_;
	BV &bv_;

public:
	BoolRank(BV &bv, int index): bv_(bv), index_(index) {};

	BoolRank &operator=(bool);
	BoolRank &operator=(const BoolRank &);
	operator bool(void);
};

class BV {
private:
	uint8_t *bytes_;
	int size_; // actually amount of using bytes
	int maxsize_; // amount of allocated bytes

	int bit2byte(int bits) {return (bits+7) / 8;} // ceil(amount of bytes)
public:
	int size(void) { return size_; };
	uint8_t *data(void) { return &bytes_[maxsize_ - (size_+7)/8]; };

	void resize(int size);

	BV(int size = 0, bool value = 0);
	BV(uint8_t *bits, int size); //  8*(arrSize-1) <= size <= 8*arrSize-1
	BV(const char *str);

	BV(const BV &bv)
	{
		bytes_ = new uint8_t[0];
		*this = bv;
	};

	~BV(void) { delete[] bytes_; };

	BV operator~(void);
	BV &operator=(const BV &);
	BoolRank operator[](int index)
	{
		if (index < 0 || index > size_-1)
			index = 0;

		return BoolRank(*this, index);
	};

	BV operator&(const BV &bv) { BV vec(*this); vec &= bv; return vec; };
	BV operator|(const BV &bv) { BV vec(*this); vec |= bv; return vec; };
	BV operator^(const BV &bv) { BV vec(*this); vec ^= bv; return vec; };
	BV operator<<(int offset)  { BV vec(*this); vec <<= offset; return vec; };
	BV operator>>(int offset)  { BV vec(*this); vec >>= offset; return vec; };

	BV &operator&=(const BV &);
	BV &operator|=(const BV &);
	BV &operator^=(const BV &);
	BV &operator<<=(int);
	BV &operator>>=(int);

	bool operator==(const BV &);

	friend std::ostream &operator<<(std::ostream &, const BV &);
	friend std::istream &operator>>(std::istream &, BV &);

	bool at(int index)
	{
		if (index < 0 || index > size_-1)
			index = 0;

		return (bytes_[maxsize_-1 - index/8] & (1 << index%8)) > 0;
	};

	void set(int index, bool value = 1, int count = 1);
	void setAll(bool value = 1);
	void inverse(int index, int count = 1);

	int weight(void);
};

#endif
