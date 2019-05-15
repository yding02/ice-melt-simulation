#include "data_struct.h"

Node* Node::get_next() {
	if (next == 0) {
		return 0;
	}
	if (next->val->prev_state <= 0) {
		next = next->next;
		delete next;
		return this->get_next();
	}
	return next;
}

LinkedList::LinkedList() {
	head = new Node();
	end = head;
}

LinkedList::~LinkedList() {
	Node* cur = head;
	Node* temp = cur;
	while (cur != 0) {
		temp = cur->next;
		delete cur;
		cur = temp;
	}
}


void LinkedList::insert(IceVoxel* ele) {
	Node* next = new Node();
	next->val = ele;
	end->next = next;
	end = next;
}

Node* LinkedList::get_first() {
	return head->get_next();
} 

