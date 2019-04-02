#pragma once

#include "c_list.h"
#include <iomanip>

class BigInt 
{

public:
    BigInt() : sign(true)
    {
        data.push_back(0);
    }
	BigInt(int64_t number);
	BigInt(const BigInt& number);

	BigInt& operator=(int64_t number);
	BigInt& operator=(const BigInt& number);
	BigInt operator-() const;
	BigInt operator+(const BigInt& number) const;
	BigInt operator-(const BigInt& number) const;
	bool operator==(const BigInt& number) const;
	bool operator!=(const BigInt& number) const;
	bool operator<(const BigInt& number) const;
	bool operator>(const BigInt& number) const;
	bool operator<=(const BigInt& number) const;
	bool operator>=(const BigInt& number) const;

	friend std::ostream& operator <<(std::ostream& out, const BigInt& number);

private:

	static const int64_t base = 1000000000;

	bool sign;
	list data; // не библиотечный! написан в файлах c_list

	void reduce()
	{
		while (data.len() > 1 && data[data.len() - 1] == 0)	data.pop();
		if (data.len() == 1 && data[0] == 0) sign = true;
	}

};

std::ostream& operator<<(std::ostream& out, const BigInt& number);