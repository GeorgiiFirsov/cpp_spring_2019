#include "BigInt.h"

BigInt::BigInt(int64_t number)	
{	
	if (number < 0) 
	{
		this->sign = false;
		number = -number;
	}
	else this->sign = true;
	do
	{
		this->data.push_back(number % base);
		number /= base;
	} while (number != 0);
}

BigInt::BigInt(const BigInt& number)
{
	sign = number.sign;
	for (size_t i = 0; i < number.data.len(); i++)
		data.push_back(number.data[i]);
}

BigInt& BigInt::operator=(int64_t number)
{
	data.clear();
	if (number < 0)
	{
		this->sign = false;
		number = -number;
	}
	else this->sign = true;
	do 
	{
		this->data.push_back(number % base);
		number /= base;
	} while (number != 0);
	return *this;
}

BigInt& BigInt::operator=(const BigInt& number)
{
	if (this == &number) return *this;
	this->data.clear();
	for (size_t i = 0; i < number.data.len(); i++)
		data.push_back(number.data[i]);
	sign = number.sign;
	return *this;
}

BigInt BigInt::operator-() const
{	
	BigInt tmp = BigInt(*this);
	if (!(data.len() && data[0] == 0)) tmp.sign = !(this->sign);
	return tmp;
}

BigInt BigInt::operator+(const BigInt& number) const
{
	if (sign == number.sign)
	{
		BigInt tmp;
		if (this->data.len() < number.data.len()) tmp = BigInt(number);
		else tmp = BigInt(*this);
		for (size_t i = 0; i < number.data.len() - 1; i++)
		{
			tmp.data[i] += number.data[i];
			if (tmp.data[i] >= base)
			{
				tmp.data[i] = tmp.data[i] % base;
				tmp.data[i + 1] += 1;
			}
		}
		size_t i = number.data.len() - 1;
		tmp.data[i] += number.data[i];
		while (tmp.data[i] >= base)
		{
			tmp.data[i] = tmp.data[i] % base;
			i++;
			if (i == tmp.data.len()) tmp.data.push_back(1);
		}
		return tmp;
	}
	else return (sign ? *this - (-number) : number - (-(*this)));
}

BigInt BigInt::operator-(const BigInt& number) const
{
	if (!(number.sign)) return *this + (-number);
	else
	{
		if (!(this->sign)) return -(-(*this) + number);
		else if (*this < number) return -(number - (*this));
	}
	BigInt tmp = BigInt(*this);
	for (size_t i = 0; i < tmp.data.len(); i++)
	{
		tmp.data[i] -= (i < number.data.len() ? number.data[i] : 0);
		if (tmp.data[i] < 0)
		{
			tmp.data[i] += base;
			tmp.data[i + 1] -= 1;
		}
	}
	tmp.reduce();
	return tmp;
}

bool BigInt::operator==(const BigInt& number) const
{
	if (this->data.len() != number.data.len()) return false;
	if (this->sign != number.sign) return false;
	if (this->data.len() == 0) return true;
	for (size_t i = 0; i < this->data.len(); i++)
		if (number.data[i] != this->data[i]) return false;
	return true;
}

bool BigInt::operator!=(const BigInt& number) const
{
	return !(*this == number);
}

bool BigInt::operator<(const BigInt& number) const
{
	if (number.sign != this->sign) return !sign;
	if (number.data.len() < this->data.len()) return !sign;
	if (number.data.len() > this->data.len()) return sign;
	for (long long i = number.data.len() - 1; i >= 0; i--)
		if (number.data[i] != this->data[i])
		{
			if (this->data[i] > number.data[i]) return !sign;
			else return sign;
		}
	return false;
}

bool BigInt::operator>(const BigInt& number) const
{
	if (*this == number) return false;
	return !(*this < number);
}

bool BigInt::operator<=(const BigInt& number) const
{
	return !(*this > number);
}

bool BigInt::operator>=(const BigInt& number) const
{
	return !(*this < number);
}

std::ostream& operator<<(std::ostream& out, const BigInt& number)
{
	if (number.data.len() == 0) out << 0 ;
	else
	{
		if (!(number.sign)) out << '-';
		out << number.data[number.data.len() - 1];
		char old_fill = out.fill('0');
		for (long long i = static_cast<long long>(number.data.len()) - 2; i >=0; i--)
			out << std::setw(9) << number.data[i];
	}
	return out;
}