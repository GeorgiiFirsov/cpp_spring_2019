#pragma once

#include <iostream>
#include <exception>

class list
{
private: 
    struct ListNode
    {
	    ListNode* next;
	    int element;
    };

public:
	list() : size(0) {}

	~list() { clear(); }

	void clear();
	const int& operator[](size_t index) const;
	int& operator[](size_t index);
	void push_back(const int item);
	void pop();
	size_t len() const { return size; }

private:

	size_t size;
	ListNode* first;
};