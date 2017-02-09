#ifndef BLIST_CPP_
#define BLIST_CPP_

#include "BList.h"

template <typename T, int Size>
BList<T, Size>::BList() : head_(nullptr), tail_(nullptr)
{
	myStats.ArraySize = Size;
	myStats.NodeSize = sizeof(BNode);
}

template <typename T, int Size>
BList<T, Size>::BList(const BList& rhs)
{
	std::cout << rhs.GetStats().ArraySize << std::endl;
}

template <typename T, int Size>
BList<T, Size>::~BList()
{
}

template <typename T, int Size>
BList<T, Size>& BList<T, Size>::operator=(const BList& rhs)
{
	std::cout << rhs.GetStats().ArraySize << std::endl;
	return *this;
}

template <typename T, int Size>
void BList<T, Size>::push_back(const T& value)
{
	try {
		if(!tail_) { // tail doesn't exist -> First element in the list
			tail_ = AllocateNewNode();
			if (!head_)
				head_ = tail_;
		}

		if(tail_->count == Size) { //tail's array is full -> creating a new node and push tail forward
			BNode* newNode = AllocateNewNode(nullptr, tail_);
			tail_->next = newNode;
			tail_ = newNode;
		}

		// tail has enough space
		tail_->values[tail_->count++] = value;
	} catch (std::bad_alloc & e) {
		throw BListException(BListException::BLIST_EXCEPTION::E_NO_MEMORY, std::string("Not enough memory for creating a new node: ") + e.what());
	}

	++myStats.ItemCount;
}

template <typename T, int Size>
void BList<T, Size>::push_front(const T& value)
{
	try {
		if (!head_) { // head doesn't exist -> First element in the list
			head_ = AllocateNewNode();
			if (!tail_)
				tail_ = head_;
		}

		if (head_->count == Size) { //head's array is full -> creating a new node and push head backwards
			BNode* newNode = AllocateNewNode(head_, nullptr);
			head_->prev = newNode;
			head_ = newNode;
		}

		// head has enough space
		for (int i = head_->count - 1; i >= 0; --i) {
			head_->values[i + 1] = head_->values[i]; // shifting existing data
		}

		//head_->values[Size - (++head_->count)] = value; //check In the FAQ, it pushes elements towards the end as it inserts
		head_->values[0] = value;
		head_->count += 1;

	}
	catch (std::bad_alloc & e) {
		throw BListException(BListException::BLIST_EXCEPTION::E_NO_MEMORY, std::string("Not enough memory for creating a new node: ") + e.what());
	}

	++myStats.ItemCount;
}

template <typename T, int Size>
void BList<T, Size>::insert(const T& value)
{
	std::cout << value << std::endl;
}

template <typename T, int Size>
void BList<T, Size>::remove(int index)
{
	// NOTE FOR IMPLEMENTATION:
	/*
	 * When a node contains 0 items, remove the empty node from the list and delete it
	 */
	std::cout << index << std::endl;

}

template <typename T, int Size>
void BList<T, Size>::remove_by_value(const T& value)
{
	std::cout << value << std::endl;
}

template <typename T, int Size>
int BList<T, Size>::find(const T& value) const
{
	std::cout << value << std::endl;
	return 5;
}

template <typename T, int Size>
T& BList<T, Size>::operator[](int index)
{
	return GetValue(index);
}

template <typename T, int Size>
const T& BList<T, Size>::operator[](int index) const
{
	return GetValue(index);
}

template <typename T, int Size>
size_t BList<T, Size>::size() const
{
	return myStats.ItemCount;
}

template <typename T, int Size>
void BList<T, Size>::clear()
{
}

template <typename T, int Size>
BListStats BList<T, Size>::GetStats() const
{
	return myStats;
}

template < typename T, int Size >
typename BList<T, Size>::BNode* BList<T, Size>::AllocateNewNode ( BNode* next, BNode* prev ) {
	BNode* newNode = new BNode(next, prev);
	++myStats.NodeCount;
	return newNode;
}

template<typename T, int Size>
T & BList<T, Size>::GetValue(int index) const
{
	if (index > myStats.ItemCount)
		throw BListException(BListException::BLIST_EXCEPTION::E_BAD_INDEX, "Subscript index is higher than list size");

	if (index < 0)
		throw BListException(BListException::BLIST_EXCEPTION::E_BAD_INDEX, "Subscript index cannot be a negative value");

	if (index == 0)
		return head_->values[0];

	int listIndex = index / myStats.ArraySize;
	int arrayIndex = index % myStats.ArraySize;
	int listMidIndex = (myStats.NodeCount - 1) / 2;

	BNode* listIterator;

	if (listMidIndex < listIndex) { // the element is closer to the tail
		listIterator = tail_;
		int i = myStats.NodeCount - 1;

		while (i-- != listIndex) // go to the index
			listIterator = listIterator->prev;

	}
	else { // the element is closer to the head
		listIterator = head_;
		int i = 0;

		while (i++ != listIndex) // go to the index
			listIterator = listIterator->next;
	}

	// At this point, listIterator should point to the node the index is in
	return listIterator->values[arrayIndex];

}

template <typename T, int Size>
size_t BList<T, Size>::nodesize(void)
{
	return sizeof(BNode);
}

template <typename T, int Size>
const typename BList<T, Size>::BNode* BList<T, Size>::GetHead() const
{
	return head_;
}

#endif