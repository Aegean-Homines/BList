/*!
* \file BList.cpp
* \author Egemen Koku
* \date 12 Feb 2017
* \brief Implementation of @b BList.h
*
* \copyright Digipen Institute of Technology
* \mainpage BList Implementation
*
*/

/*#ifndef BLIST_CPP_
#define BLIST_CPP_

#include "BList.h" */

/**
* @brief Copy Constructor for BNode class
* @param rhs Other BNode to be copied from
*/
template < typename T, int Size >
BList<T, Size>::BNode::BNode ( const BNode& rhs ) {
	next = nullptr;
	prev = nullptr;
	count = rhs.count;
	for (int i = 0; i < Size; ++i) {
		values[i] = rhs.values[i];
	}
}

/**
* @brief Default Const. for BList class
*/
template <typename T, int Size>
BList<T, Size>::BList() : head_(nullptr), tail_(nullptr)
{
	myStats.ArraySize = Size;
	myStats.NodeSize = sizeof(BNode);
}

/**
* @brief Copy Constructor for BList class
* @param rhs other BList to be copied from
*/
template <typename T, int Size>
BList<T, Size>::BList(const BList& rhs)
{
	CopyHelper(rhs);
}

/**
* @brief Dest. for BList class
*/
template <typename T, int Size>
BList<T, Size>::~BList()
{
	clear();
}

/**
* @brief Assignment operator
* @param rhs other BList to be copied from
*/
template <typename T, int Size>
BList<T, Size>& BList<T, Size>::operator=(const BList& rhs)
{
	if (this == &rhs) return *this;

	BNode* originalHead = head_;

	CopyHelper(rhs);

	while(originalHead != nullptr) {
		BNode* temp = originalHead;
		originalHead = originalHead->next;
		delete temp;
	}

	return *this;

}

/**
* @brief Function to push back values to the tail of the list
* @param value Value to be pushed
* @throws BListException
*/
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

/**
* @brief Function to push front values to the head of the list
* @param value Value to be pushed
* @throws BListException
*/
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

		//head_->values[Size - (++head_->count)] = value;
		head_->values[0] = value;
		head_->count += 1;

	}
	catch (std::bad_alloc & e) {
		throw BListException(BListException::BLIST_EXCEPTION::E_NO_MEMORY, std::string("Not enough memory for creating a new node: ") + e.what());
	}

	++myStats.ItemCount;
}

/**
* @brief Function to insert values to the blist
* @param value Value to be inserted
* @throws BListException
*/
template <typename T, int Size>
void BList<T, Size>::insert(const T& value)
{

	// inserting the first element
	if(!head_) { 
		push_front(value);
		return;
	}

	// if size is 1 => this is a normal linked list
	// do normal insert without all those fancy things
	if(Size == 1) {
		RegularLinkedListInsert(value);
		return;
	}
		
	BNode* iterator = FindInsertPosition(value);

	// iterator points to the head or intermediate node to insert
	if(iterator->count == Size) { //at max size => need to split
		Split(iterator);
	}

	// Checking which part of the split to insert (left or right)
	BNode* nextNode = iterator->next;
	if (nextNode && nextNode->values[0] < value)
		iterator = nextNode;

	// insert at the iterator at the right place
	InsertAtNode(iterator, value);

}

/**
* @brief Function to remove values from the blist
* @param index Index of the value to be removed
* @throws BListException
*/
template <typename T, int Size>
void BList<T, Size>::remove(int index)
{

	if (index > myStats.ItemCount)
		throw BListException(BListException::BLIST_EXCEPTION::E_BAD_INDEX, "Subscript index is higher than list size");

	if (index < 0)
		throw BListException(BListException::BLIST_EXCEPTION::E_BAD_INDEX, "Subscript index cannot be a negative value");

	int indexCounter = index;
	BNode* iterator = head_;

	while (indexCounter >= iterator->count) {
		indexCounter -= iterator->count;
		iterator = iterator->next;
	}

	// will skip the loop if this is the only element
	for (int i = indexCounter; i < iterator->count - 1; ++i) {
		iterator->values[i] = iterator->values[i+1];
	}

	if((iterator->count -= 1) == 0) {
		DeallocateNode(iterator);
	}

}

/**
* @brief Function to remove values from the blist
* @param value Value to be removed
*/
template <typename T, int Size>
void BList<T, Size>::remove_by_value(const T& value)
{
	// TODO improve search
	int indexCounter = -1;
	BNode* iterator = head_;
	bool isFound = false;

	while (iterator != nullptr) {

		for (indexCounter = 0; indexCounter < iterator->count; ++indexCounter) {
			if (value == iterator->values[indexCounter]) {
				isFound = true;
				break;
			}
		}

		if(isFound) {
			break;
		}
		iterator = iterator->next;
	}

	if (indexCounter < 0 || iterator == nullptr)
		return;

	--myStats.ItemCount;

	// will skip the loop if this is the only element
	for (int i = indexCounter; i < iterator->count - 1; ++i) {
		iterator->values[i] = iterator->values[i + 1];
	}

	if ((iterator->count -= 1) == 0) {
		DeallocateNode(iterator);
	}

}

/**
* @brief Function to find values in the blist
* @param value Value to be found
* @returns returns index, -1 if not found
*/
template <typename T, int Size>
int BList<T, Size>::find(const T& value) const
{
	// TODO improve search
	int indexCounter = 0;
	BNode* iterator = head_;
	while (iterator != nullptr ) {
		for (int i = 0; i < iterator->count; ++i) {
			if (value == iterator->values[i]) {
				return indexCounter + i;
			}
		}

		indexCounter += iterator->count;
		iterator = iterator->next;
	}

	return -1;
}

/**
* @brief Random access operator for blist
* @param index Index of the data
* @returns value stored in the list
*/
template <typename T, int Size>
T& BList<T, Size>::operator[](int index)
{
	return GetValue(index);
}

/**
* @brief Random access operator for blist
* @param index Index of the data
* @returns value stored in the list
*/
template <typename T, int Size>
const T& BList<T, Size>::operator[](int index) const
{
	return GetValue(index);
}

/**
* @brief Size of the list
* @returns number of nodes in the list
*/
template <typename T, int Size>
size_t BList<T, Size>::size() const
{
	return myStats.ItemCount;
}

/**
* @brief Removes everything in the list
*/
template <typename T, int Size>
void BList<T, Size>::clear()
{
	BNode* iterator;
	while(head_) {
		iterator = head_;
		head_ = head_->next;

		delete iterator;
	}

	myStats.ItemCount = 0;
	myStats.NodeCount = 0;
}

/**
* @brief Getter for list stats
* @returns list stats
*/
template <typename T, int Size>
BListStats BList<T, Size>::GetStats() const
{
	return myStats;
}

/**
* @brief Helper function for allocating new nodes
* @param next Next Pointer to be set for the new node
* @param prev Prev Pointer to be set for the new node
* @returns allocated node
*/
template < typename T, int Size >
typename BList<T, Size>::BNode* BList<T, Size>::AllocateNewNode ( BNode* next, BNode* prev ) {
	BNode* newNode = new BNode(next, prev);
	if(next)
		next->prev = newNode;
	if(prev)
		prev->next = newNode;

	++myStats.NodeCount;
	return newNode;
}

/**
* @brief Helper function for deallocating a node
* @param nodeToRemove Node to be deallocated
*/
template<typename T, int Size>
void BList<T, Size>::DeallocateNode(BNode * nodeToRemove)
{

	if (nodeToRemove == head_) {
		head_ = head_->next;
		if(head_)
			head_->prev = nullptr;
	}
	else if (nodeToRemove == tail_) {
		tail_ = tail_->prev;
		if(tail_)
			tail_->next = nullptr;
	}else {
		BNode* nextItem = nodeToRemove->next;
		BNode* prevItem = nodeToRemove->prev;

		nextItem->prev = prevItem;
		prevItem->next = nextItem;
	}

	--myStats.NodeCount;
	delete nodeToRemove;

}

/**
* @brief Helper function for getting a value at index
* @param index Index of the data
* @returns data
*/
template<typename T, int Size>
T & BList<T, Size>::GetValue(int index) const
{
	if (index > myStats.ItemCount)
		throw BListException(BListException::BLIST_EXCEPTION::E_BAD_INDEX, "Subscript index is higher than list size");

	if (index < 0)
		throw BListException(BListException::BLIST_EXCEPTION::E_BAD_INDEX, "Subscript index cannot be a negative value");

	if (index == 0)
		return head_->values[0];

	int indexCounter = index;
	BNode* iterator = head_;

	while(indexCounter >= iterator->count) {
		indexCounter -= iterator->count;
		iterator = iterator->next;
	}

	return iterator->values[indexCounter];


	// Beautiful method to find the indexed element fast
	// Unfortunately only works if all nodes are filled
	/*int listIndex = index / myStats.ArraySize;
	int arrayIndex = index % myStats.ArraySize;
	int listMidIndex = (myStats.NodeCount - 1) / 2;

	BNode* listIterator;
	// check a way to make this only one block? iterator progression seems to be the problem
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
	return listIterator->values[arrayIndex];*/

}

/**
* @brief Helper function for splitting a node in two
* @param nodeToSplit node to be split
*/
template<typename T, int Size>
void BList<T, Size>::Split(BNode * nodeToSplit)
{
	// Create a new node between
	BNode* newNode = AllocateNewNode(nodeToSplit->next, nodeToSplit);

	// Find indices to move elements between
	int middleIndex = (Size) / 2;
	int indexForNewNode = 0;
	int indexForSource = middleIndex;

	// move half of the split node
	while(indexForSource < Size) {
		newNode->values[indexForNewNode++] = nodeToSplit->values[indexForSource++];
	}

	// update node information
	nodeToSplit->count = middleIndex;
	newNode->count = indexForNewNode;

	// adjust if tail was split
	if (tail_->next)
		tail_ = tail_->next;
}


/**
* @brief Helper function for inserting a value at a node
* @param nodeToInsert node to insert value at
* @param value data to be inserted
* @warning assumes the array is not full
*/
template < typename T, int Size >
void BList<T, Size>::InsertAtNode ( BNode* nodeToInsert, const T& value ) {

	// TODO divide by half optimization
	//int halfIndex = nodeToInsert->count / 2;

	int indexToInsert = 0;

	// will quit loop when index = the index to insert
	while (indexToInsert < nodeToInsert->count && nodeToInsert->values[indexToInsert] < value) {
		++indexToInsert;
	};

	// shift everything
	for (int counter = nodeToInsert->count; counter >= indexToInsert; --counter) {
		nodeToInsert->values[counter] = nodeToInsert->values[counter - 1];
	}

	nodeToInsert->values[indexToInsert] = value;
	nodeToInsert->count += 1;
	++myStats.ItemCount;

	
}


/**
* @brief Helper function for linked list insert when size is 1
* @param value data to be inserted
*/
template<typename T, int Size>
void BList<T, Size>::RegularLinkedListInsert(const T & value)
{
	BNode* iterator;
	// precheck if the value is higher than tail's beginning -> I have to insert at the tail
	if (tail_->values[0] < value) { // inserting after tail
		push_back(value);
	}
	else if (value < head_->values[0]) { //inserting before head
		push_front(value);
	}else{ //TODO divide by half optimization

		iterator = head_->next;
		while (iterator->values[0] < value) {
			iterator = iterator->next;
		}
		
		iterator = iterator->prev;

		BNode* newNode = AllocateNewNode(iterator->next, iterator);
		newNode->values[0] = value;
		newNode->count = 1;

		++myStats.ItemCount;
	}

}

/**
* @brief Helper function for finding where to insert the value
* @param value data to be inserted
* @returns node to be inserted at
*/
template<typename T, int Size>
typename BList<T, Size>::BNode * BList<T, Size>::FindInsertPosition(const T & value)
{
	BNode* retVal;
	// precheck if the value is higher than tail's beginning -> I have to insert at the tail
	if (tail_->values[0] < value) {
		retVal = tail_;
	}
	else {
		retVal = head_;
		while (retVal->values[0] < value) {
			retVal = retVal->next;
		}

		/*	Two conditions for reaching here
		*	1-) prevNode == nullptr -> inserting at the head node, continue with the procedure normally
		*	2-) can't work around splitting and shifting
		*	As a result, use the current node
		*/

		BNode* prevNode = retVal->prev;
		// either prev node still has space or both are full
		if (prevNode && (value < prevNode->values[prevNode->count - 1] || prevNode->count != Size || retVal->count == Size)) {
			retVal = prevNode;
		}
	}

	return retVal;
}

/**
* @brief Helper function for copying data from other to this
* @param other blist to be copied from
*/
template<typename T, int Size>
void BList<T, Size>::CopyHelper(BList const & other)
{
	try {
		if(other.head_) {
			head_ = new BNode(*other.head_);
			head_->prev = nullptr;
			BNode* currentNode = head_->next;
			BNode* prevNode = head_;
			BNode* otherCurrent = other.head_->next;
			while(otherCurrent) {
				currentNode = new BNode(*otherCurrent);
				prevNode->next = currentNode;
				currentNode = currentNode->next;
				prevNode = prevNode->next;
				otherCurrent = otherCurrent->next;
			}

			tail_ = prevNode;
			tail_->next = nullptr;
		}
	}catch(std::bad_alloc& e) {
		BNode* iterator = head_;

		while(iterator) {
			BNode* next = iterator->next;
			delete iterator;
			iterator = next;
		}

		throw BListException(BListException::BLIST_EXCEPTION::E_DATA_ERROR, std::string("Exception thrown during copy construction: ") + e.what());
	}

	myStats = other.myStats;
}

/**
* @brief Getter for the size of a node
* @returns size of each node
*/
template <typename T, int Size>
size_t BList<T, Size>::nodesize(void)
{
	return sizeof(BNode);
}

/**
* @brief Getter for the head pointer
* @returns the head of the list
*/
template <typename T, int Size>
const typename BList<T, Size>::BNode* BList<T, Size>::GetHead() const
{
	return head_;
}

//#endif