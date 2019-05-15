#include "data_struct.h"

Node::Node() {
	next = 0;
	val = 0;
}

Node* Node::get_next() {
	if (next == 0) {
		return 0;
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


void LinkedList::clean() {
	Node* cur = head;
	while (cur != 0) {
		Node* next = cur->next;
		if (next == 0) {
			break;
		}
		if (next->val->prev_state <= 0) {
			cur->next = next->next;
			delete next;
		} else {
			cur = next;
		}
	}
	end = head;
	while (end->next != 0) {
		end = end->next;
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

