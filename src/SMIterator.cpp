#include "SMIterator.h"
#include "SortedMap.h"
#include <exception>

using namespace std;

SMIterator::SMIterator(const SortedMap& m) : map(m){
	// First position will contain the position where the smallest element of the map is situated
	this->firstPosition = 0;
	int i;
	// Create a shallow copy of the array of nodes stored in the hash table
	this->copy = new Node * [this->map.elements->m];
	for (i = 0; i < this->map.elements->m; i++)
	{
		this->copy[i] = this->map.elements->T[i];
	}
	// Find where the smallest element is situated 
	// Note that here current means "current minimum"
	Node* current = this->map.elements->T[this->firstPosition];
	for (i = 0; i < this->map.elements->m; i++)
	{
		if (this->map.elements->T[i] != NULL)
		{
			// Initialize current with the first found element, if null
			if (current == NULL)
			{
				current = this->map.elements->T[i];
				this->firstPosition = i;
			}
			else
				// Compare current minimum with the element situated at the current position in terms of the relation 
			{
				if (this->map.r(this->map.elements->T[i]->data.first, current->data.first))
				{
					current = this->map.elements->T[i];
					// Set firstPosition to point to the new minimum
					this->firstPosition = i;
				}
			}

		}
	}
	// Current position will hold the position where the current element of the iterator is situated
	this->currentPosition = firstPosition;
}
// Complexity: Theta(m) 

void SMIterator::first(){
	// Set current position to point back to the element with the smallest key
	this->currentPosition = firstPosition;
	// Copy the nodes once again because the shallow copy might have been altered
	for (int i = 0; i < this->map.elements->m; i++)
	{
		this->copy[i] = this->map.elements->T[i];
	}
}
// Complexity: Theta(m)

void SMIterator::next(){
	// In order to find the next "smallest" element we will "remove" the node stored at currentPosition and compare the rest
	// More specifically, we will set it to its next and we will traverse the array once again
	Node* current = this->copy[this->currentPosition];
	if (current != NULL)
	{
		this->copy[this->currentPosition] = current->next;
		current = current->next;
		for (int i = 0; i < this->map.elements->m; i++)
		{
			if (this->copy[i] != NULL)
			{
				// Initialize current, if null, with the first nonnull value
				if (current == NULL)
				{
					current = this->copy[i];
					this->currentPosition = i;
				}
				// Compare current minimum with the element situated at the current position in terms of the relation
				else
				{
					if (this->map.r(this->copy[i]->data.first, current->data.first))
					{
						current = this->copy[i];
						// Set currentPosition to point to the new minimum
						this->currentPosition = i;
					}
				}

			}
		}
		return;
	}
	throw std::exception();
}
// Complexity: Theta(m)

bool SMIterator::valid() const{
	if (this->copy[this->currentPosition] != NULL)
		return true;
	return false;
}
// Comnplexity: Theta(1)

TElem SMIterator::getCurrent() const{
	if (this->copy[this->currentPosition] != NULL)
	{
		return this->copy[this->currentPosition]->data;
	}
	throw std::exception();
}
// Complexity: Theta(1)

SMIterator::~SMIterator()
{
	delete[] this->copy;
}
// Complexity: Theta(1)