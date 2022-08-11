#ifndef LIST_PCB_H
#define LIST_PCB_H

#include "types.h"

class PCB;

class PCBList {
public:

	struct Node{
		PCB* info;
		Node* next;
		Node(PCB* pcb){
			info = pcb;
			next = nullptr;
		}
		~Node() { info = nullptr; next = nullptr; }
	};

	PCBList() {
		first = nullptr;
		last = nullptr;
		curr = nullptr;
		n = 0;
	}
	~PCBList() { empty(); }

	int getN() const { return n; }

	PCB* getFirst();
	PCB* getLast();
	PCB* getCurr();
	void removeFirst(); // doesn't delete the pcb

	bool isEmpty() { return n==0; }
	void empty();
	void append(PCB* pcb);
	void remove(PCB* pcb);

	// iterator methods

	void gotoFirst();
	void gotoNext();
	bool hasCurr() const;
	bool hasNext() const;
	bool end() const;

private:
	Node* first, *last, *curr;
	int n;
};


#endif
