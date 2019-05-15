#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include "iceVoxel.h"
#include "stdio.h"
using namespace std;

struct Node {
	Node();
	Node* get_next();
	IceVoxel* val;
	Node* next;
};

struct LinkedList {
	LinkedList();
	~LinkedList();
	void insert(IceVoxel* ele);
	void clean();
	Node* get_first();

	Node* head;
	Node* end;
};



#endif
