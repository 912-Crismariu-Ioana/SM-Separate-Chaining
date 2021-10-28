#pragma once
//DO NOT INCLUDE SORTEDMAPITERATOR

//DO NOT CHANGE THIS PART
typedef int TKey;
typedef int TValue;
#include <utility>
typedef std::pair<TKey, TValue> TElem;
#define NULL_TVALUE -111111
#define NULL_TPAIR pair<TKey, TValue>(-111111, -111111);
class SMIterator;
typedef bool(*Relation)(TKey, TKey);
typedef int(*TFunction)(int);

typedef struct Node
{
	TElem data;
	Node* next;
} Node;

typedef struct HashTable
{
	Node** T;
	int m;
	double maxloadFactor;

} HashTable;

class SortedMap {
	friend class SMIterator;
    private:
		HashTable* elements;
		Relation r;
		int nrElements;
		void resize();
		int hash(TKey k) const { return abs(k % this->elements->m); }
    public:

    // implicit constructor
    SortedMap(Relation r);
	SortedMap(const SortedMap& sm);

	// adds a pair (key,value) to the map
	//if the key already exists in the map, then the value associated to the key is replaced by the new value and the old value is returned
	//if the key SMes not exist, a new pair is added and the value null is returned
	TValue add(TKey c, TValue v);

	//searches for the key and returns the value associated with the key if the map contains the key or null: NULL_TVALUE otherwise
	TValue search(TKey c) const;


	//removes a key from the map and returns the value associated with the key if the key existed ot null: NULL_TVALUE otherwise
	TValue remove(TKey c);

	//returns the number of pairs (key,value) from the map
	int size() const;

	//checks whether the map is empty or not
	bool isEmpty() const;

    // return the iterator for the map
    // the iterator will return the keys following the order given by the relation
    SMIterator iterator() const;

	//modifies the values of those keys that are in sm to be equal to their value from sm (assume both SortedMaps use the same relation) 
	//returns the number of modified pairs 
	int updateValues(SortedMap& sm);

    // destructor
    ~SortedMap();
};

bool isPrime(int nr);