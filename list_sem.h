#ifndef LIST_SEM_H
#define LIST_SEM_H

#include "types.h"

class KernelSem;

class SemList {
public:

	struct Node{
		KernelSem* info;
		Node* next;
		Node(KernelSem* sem){
			info = sem;
			next = nullptr;
		}
		~Node() { info = nullptr; next = nullptr; }
	};

	SemList() {
		first = nullptr;
		last = nullptr;
		curr = nullptr;
		n = 0;
	}
	~SemList() { empty(); }

	int getN() const { return n; }

	KernelSem* getFirst();
	KernelSem* getLast();
	KernelSem* getCurr();
	void removeFirst(); // doesn't delete the semaphore

	bool isEmpty() { return n==0; }
	void empty();
	void append(KernelSem* sem);
	void remove(KernelSem* sem);

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
