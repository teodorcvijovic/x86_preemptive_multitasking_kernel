
// Helper structs 'PCBTime' and 'PCBTimeList' for class KernelSem.

#ifndef SEM_HELP_H
#define SEM_HELP_H

#include "types.h"

class PCB;

struct PCBTime{
	PCB* pcb;
	Time waitingTime;
	PCBTime(PCB* p, Time t){
		pcb = p;
		waitingTime = t;
	}
	~PCBTime(){
		pcb = nullptr;
	}
};

class PCBTimeList{
public:
	friend class KernelSem;

	struct Node{
		PCBTime* info;
		Node* next;
		Node(PCBTime* p){
			info = p;
			next = nullptr;
		}
		~Node() { info = nullptr; next = nullptr; }
	};

	PCBTimeList() {
		first = nullptr;
		last = nullptr;
		curr = nullptr;
		prev = nullptr;
		n = 0;
	}
	~PCBTimeList() { empty(); }

	int getN() const { return n; }

	PCBTime* getFirst();
	PCBTime* getLast();
	PCBTime* getCurr();
	PCBTime* getPrev();

	void removeFirst(); // doesn't delete the pcb

	bool isEmpty() { return n==0; }
	void empty();
	void append(PCBTime* pcb_and_time);
	void remove(PCBTime* pcb_and_time);

	// helper methods for semaphore implementation

	void insert(PCBTime* pcb_and_time);
	void frontInsert(PCBTime* pcb_and_time);
	void insertBeforeCurr(PCBTime* pcb_and_time);

	// iterator methods

	void gotoFirst();
	void gotoNext();
	bool hasCurr() const;
	bool hasNext() const;
	bool hasPrev() const;
	bool end() const;

private:
	Node* first, *last, *curr, *prev;
	int n;


};

#endif
