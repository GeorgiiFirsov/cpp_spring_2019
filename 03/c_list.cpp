#include "c_list.h"

void list::clear()
{
	if(size == 0) return;
	ListNode* tmp;
	for (size_t i = 0; i < size- 1; i++)
	{
		tmp = first;
		if (first->next != nullptr) first = first->next;
		delete tmp;
	}
	delete first;
	size = 0;
}

const int& list::operator[](size_t index) const
{
	if(index >= size) throw std::out_of_range("Invalid index");
	ListNode* tmp = first;
	for (size_t i = 0; i < index; i++) tmp = tmp->next;
	return tmp->element;
}

int& list::operator[](size_t index) 
{
	if(index >= size) throw std::out_of_range("Invalid index");
	ListNode* tmp = first;
	for (size_t i = 0; i < index; i++) tmp = tmp->next;
	return tmp->element;
}

void list::push_back(const int item)
{
	if (size == 0)
	{
		first = new ListNode;
		first->element = item;
		first->next = nullptr;
	}
	else
	{
		ListNode* tmp = first;
		for (size_t i = 0; i < size - 1; i++) tmp = tmp->next;
		tmp->next = new ListNode;
		tmp->next->element = item;
		tmp->next->next = nullptr;
	}
	size++;
}

void list::pop()
{
	ListNode* tmp = first;
	for (size_t i = 0; i < size - 2; i++) tmp = tmp->next;
	delete tmp->next;
	tmp->next = nullptr;
	size--;
}