#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include "iceVoxel.h"
using namespace std;

struct Node {
	Node() {}
	Node* get_next();
	IceVoxel* val;
	Node* next;
};

struct LinkedList {
	LinkedList();
	~LinkedList();
	void insert(IceVoxel* ele);
	Node* get_first();

	Node* head;
	Node* end;
};



#endif
