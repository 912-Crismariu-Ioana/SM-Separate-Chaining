#include "SMIterator.h"
#include "SortedMap.h"
#include <exception>
#include <iostream>
using namespace std;

bool isPrime(int nr)
// Checks if a given number is prime
{
	if (nr == 0 || nr == 1) {
		return false;
	}
	else {
		for (int i = 2; i <= nr / 2; ++i) {
			if (nr % i == 0) {
				return false;
			}
		}
		return true;
	}
}
// Complexity: Theta(nr)

SortedMap::SortedMap(Relation r) {
	this->r = r;
	this->elements = new HashTable;
	this->elements->T = new Node * [13];
	for (int i = 0; i < 13; i++)
	{
		this->elements->T[i] = NULL;
	}
	this->elements->m = 13;
	this->elements->maxloadFactor = 0.75;
	this->nrElements = 0;
}
// Complexity: Theta(m)

void SortedMap::resize()
{
	int i = 0;
	// Double the initial capacity
	int newCapacity = 2 * this->elements->m;
	newCapacity++;
	// Find the smallest prime number larger than the doubled capacity
	while(isPrime(newCapacity) == false)
	{
		newCapacity += 2;
	}
	Node** newElements = new Node * [newCapacity];
	// Initialize the new array, set the capacity and reset number of elements (we're going to call add, which will modify it)
	for (i = 0; i < newCapacity; i++)
	{
		newElements[i] = NULL;
	}
	int oldCapacity = this->elements->m;
	this->elements->m = newCapacity;
	Node** oldTable = this->elements->T;
	this->elements->T = newElements;
	this->nrElements = 0;

	for (i = 0; i < oldCapacity; i++)
	{
		if (oldTable[i] != NULL)
		{
			Node* oldEntry;
			Node* currentEntry = oldTable[i];
			while (currentEntry != NULL)
			{
				// Use the add function to rehash and find the position of each entry from the old table and place it correctly in the new table
				this->add(currentEntry->data.first, currentEntry->data.second);
				oldEntry = currentEntry;
				currentEntry = currentEntry->next;
				delete oldEntry;
			}
		}
	}
	delete[] oldTable;

}
// Best Case: No collisions upon rehashing, so the complexity will be Theta(m) where m is the capacity of the hash table
// Worst Case: All keys hash to the same position and are added in ascending order to the new Table, Theta(m+n^2) where m is the capacity of the hash table and n is the number of occupied entries
// Total Complexity: O(m+n^2)

TValue SortedMap::add(TKey k, TValue v) {
	// Allocate space for a new node
	Node* newNode = new Node;
	newNode->data.first = k;
	newNode->data.second = v;
	newNode->next = NULL;

	// Obtain its position
	int position = this->hash(k);
	

	Node* currentNode = this->elements->T[position];
	if ((currentNode) == NULL)
		// The position is not occupied, we may occupy it
	{
		// Check if a resize is needed to preserve the efficiency of the table
		double loadFactor = (double)this->nrElements / this->elements->m;
		if (loadFactor > this->elements->maxloadFactor)
		{
			this->resize();
			this->add(k, v);
			return NULL_TVALUE;
		}
		this->elements->T[position] = newNode;
		this->nrElements++;
		return NULL_TVALUE;
	}

	if (this->r(k, currentNode->data.first))
		// We need to insert the new node at the beginning of the linked list 
		// situated at the current position
	{
		// The element with the provided key is equal to the element contained in the first node of the linked list, we simply update its value
		if (currentNode->data.first  == k)
		{
			TValue oldValue = currentNode->data.second;
			currentNode->data.second = v;
			delete newNode;
			return oldValue;
		}
		// Else check if a resize is needed
		double loadFactor = (double)this->nrElements / this->elements->m;
		if (loadFactor > this->elements->maxloadFactor)
		{
			this->resize();
			this->add(k, v);
			return NULL_TVALUE;
		}
		newNode->next = currentNode;
		this->elements->T[position] = newNode;
		this->nrElements++;
		return NULL_TVALUE;
	}

	// The new element is larger than the element at the beginning of the linked list
	// situated at the current position, so we need to find where to insert it;
	while (currentNode->next != NULL && (!this->r(k, currentNode->next->data.first)))
	{
		currentNode = currentNode->next;
	}

	// If the relation does not allow equality, then we might need to update the value stored at currentNode 
	if (currentNode->data.first == k)
	{
		TValue oldValue = currentNode->data.second;
		currentNode->data.second = v;
		delete newNode;
		return oldValue;
	}

	Node* nextNode = currentNode->next;
	
	if (nextNode != NULL)
	{
		// If the relation does allow equality, then we might need to update the value stored at the node next to currentNode 
		if (nextNode->data.first == k)
		{
			TValue oldValue = nextNode->data.second;
			nextNode->data.second = v;
			delete newNode;
			return oldValue;
		}
	}
	// We are in neither situations, check is a resize is needed then add the element
	double loadFactor = (double)this->nrElements / this->elements->m;
	if (loadFactor > this->elements->maxloadFactor)
	{
		this->resize();
		this->add(k, v);
		return NULL_TVALUE;
	}
	newNode->next = nextNode;
	currentNode->next = newNode;
	this->nrElements++;
	return NULL_TVALUE;
}
// Best Case: The key of the element hashes to an unocuppied position and no resize is needed
// or it is the smallest compared to all the other keys which hashed to the same position and no resize is needed => Complexity Theta(1)
// Worst Case: A resize is needed and all the existing elements are rehashed to the same position and added again in increasing order
//  => Theta(m+n^2) where m is the number of slots and n the number of occupied entries
// Total Complexity: O(m+n^2)

TValue SortedMap::search(TKey k) const {
	// Hash table contains no entries
	if (this->nrElements == 0)
		return NULL_TVALUE;

	// Obtain the position where it should be
	int position = this->hash(k);

	Node* currentNode = this->elements->T[position];

	// Nothing is found at the determined position
	if (currentNode == NULL)
		return NULL_TVALUE;

	// The element contained in the first node situated at the determined position is greater than or equal to the searched element
	if (this->r(k, currentNode->data.first))
	{
		if (currentNode->data.first == k)
		{
			return currentNode->data.second;
		}
		return NULL_TVALUE;
	}
	
	// Traverse the linked list until we find the nodes in between which our element could be situated
	while (currentNode->next != NULL && (!this->r(k, currentNode->next->data.first)))
	{
		currentNode = currentNode->next;
	}

	// If the relation does not allow equality, currentNode could contain the element we are looking for 
	if (currentNode->data.first == k)
	{
		return currentNode->data.second;
	}

	Node* nextNode = currentNode->next;

	if (nextNode != NULL)
	{
		// If the relation does allow equality, then the element next of currentNode could contain the element we are looking for
		if (nextNode->data.first == k)
		{
			return nextNode->data.second;
		}
	}

	// We are in neither cases, thus the element is not in the list
	return NULL_TVALUE;
}
// Best Case: The key of the element to be searched for is the smallest or the only one to be hashed to a certain position 
// (regardless of whether it is actually in the map or not) => Theta(1)
// Worst Case: All keys of the map, including the one of the element we are searching for, 
// hash to the same position and the searched key is the largest of them all => Theta(n)
// Total Complexity: O(n)

TValue SortedMap::remove(TKey k) {

	// Hash table contains no entries
	if (this->nrElements == 0)
		return NULL_TVALUE;

	// Obtain the position where it should be
	int position = this->hash(k);

	Node* currentNode = this->elements->T[position];

	// Nothing is found at the determined position
	if (currentNode == NULL)
		return NULL_TVALUE;

	// The element contained in the first node situated at the determined position is greater than or equal to the element to be removed
	if (this->r(k, currentNode->data.first))
	{
		if (currentNode->data.first == k)
		{
			TValue oldValue = currentNode->data.second;
			this->elements->T[position] = currentNode->next;
			delete currentNode;
			this->nrElements--;
			return oldValue;
		}
		return  NULL_TVALUE;
	}

	// Traverse the linked list until we find the nodes in between which our element could be situated
	// Keep count of the previous of currentNode in case currentNode is to be removed
	Node* prevNode = this->elements->T[position];
	while (currentNode->next != NULL && (!this->r(k, currentNode->next->data.first)))
	{
		prevNode = currentNode;
		currentNode = currentNode->next;
	}

	// If the relation does not allow equality, currentNode could contain the element we want to delete
	if (currentNode->data.first == k)
	{
		TValue oldValue = currentNode->data.second;
		prevNode->next = currentNode->next;
		delete currentNode;
		this->nrElements--;
		return oldValue;
	}

	Node* nextNode = currentNode->next;

	if (nextNode != NULL)
	{
		// If the relation does allow equality, then the node next to currentNode could contain the element we want to delete
		if (nextNode->data.first == k)
		{
			TValue oldValue = nextNode->data.second;
			currentNode->next = nextNode->next;
			delete nextNode;
			this->nrElements--;
			return oldValue;
		}
	}
	// We are in neither cases, thus the element is not part of the map
	return NULL_TVALUE;
}
// Best Case: The key of the element to be deleted is the smallest or the only one to be hashed to a certain position 
// (regardless of whether it is actually in the map or not) => Theta(1)
// Worst Case: All keys of the map, including the one of the element we want to delete, 
// hash to the same position and the searched key is the largest of them all => Theta(n)
// Total Complexity: O(n)

int SortedMap::size() const {
	return this->nrElements;
}
// Complexity: Theta(1)

bool SortedMap::isEmpty() const {
	if (this->nrElements == 0)
	{
		return true;
	}
	return false;
}
// Complexity: Theta(1)

SMIterator SortedMap::iterator() const {
	return SMIterator(*this);
}
// Complexity: Theta(m)


int SortedMap::updateValues(SortedMap& sm)
{
	Node* elem;
	int counter = 0;
	for (int i = 0; i < this->elements->m; i++)
	{
		elem = this->elements->T[i];
		while (NULL != elem)
		{
			TValue newVal = sm.search(elem->data.first); // BC: Theta(1), WC: Theta(sm.size)
			if (newVal != NULL_TVALUE)
			{
				if (newVal != elem->data.second)
				{
					this->add(elem->data.first, newVal);// BC: Theta(1), WC: Theta(size bc no resize)
					counter++;
				}
				
			}
			elem = elem->next;
		}
		
	}
	return counter;
}

// Best Case: Theta(m), occurs when each key whose value we update is the smallest or the only one to be hashed to a certain position
// both in our map and in the parameter map, or when the elements in both maps have the same value for the same key
// Worst Case: Theta(m + n*(sm.size)), occurs when each element that we search for is the largest element to be hashed to a 
// certain position in the parameter map
// Total Complexity: O(m + n*(sm.size))

SortedMap::~SortedMap() {
	// Explicit destructor of the map
	Node* temp;
	Node* temp_next;
	for (int i = 0; i < this->elements->m; i++) { 
		temp = this->elements->T[i];
		while (NULL != temp) {
			temp_next = temp->next;
			delete temp;
			temp = temp_next;
		} 
		this->elements->T[i] = NULL;
	} 
	delete[] this->elements->T;
	delete this->elements;
}
// Complexity: Theta(m)

SortedMap::SortedMap(const SortedMap& sm)
{
	// Deep copy constructor 
	this->r = sm.r;
	this->elements = new HashTable;
	this->elements->T = new Node * [sm.elements->m];
	for (int i = 0; i < sm.elements->m; i++)
	{
		this->elements->T[i] = new Node;
		Node* currentMap = this->elements->T[i];
		Node* otherMap = sm.elements->T[i];
		if (otherMap != NULL)
		{
			currentMap->data = otherMap->data;
			while (otherMap->next != NULL)
			{
				Node* newNode = new Node;
				newNode->data = otherMap->next->data;
				currentMap->next = newNode;
				currentMap = newNode;
				otherMap = otherMap->next;
			}
			currentMap->next = NULL;
		}
		else
			this->elements->T[i] = NULL;
	}
	this->elements->m = sm.elements->m;
	this->elements->maxloadFactor = sm.elements->maxloadFactor;
	this->nrElements = sm.nrElements;

}
// Complexity: Theta(m)