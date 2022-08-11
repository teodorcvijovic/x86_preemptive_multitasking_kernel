#include "list_sem.h"

KernelSem* SemList::getFirst(){
	if(first==nullptr) return nullptr;
	return first->info;
}

KernelSem* SemList::getLast(){
	if(last==nullptr) return nullptr;
	return last->info;
}

KernelSem* SemList::getCurr(){
	if(curr==nullptr) return nullptr;
	return curr->info;
}

void SemList::removeFirst(){
	if (first==nullptr) return;
	if (first==last) last=nullptr;
	if (first==curr) curr=curr->next;
	Node* old = first;
	first = first->next;
	n--;
	delete old;
}

void SemList::empty(){
	for(gotoFirst(); hasCurr(); removeFirst()){

	}
	n=0;
}

void SemList::append(KernelSem* sem){
	Node* node = new Node(sem);
	n++;
	(first==nullptr ? first : last->next) = node;
	last = node;
}

void SemList::remove(KernelSem* sem){
	if (sem == nullptr) return;
	Node* prev = nullptr;
	for(Node* i = first ; i != nullptr; i=i->next){
		if(i!=nullptr && i->info==sem){
			(prev==nullptr ? first : prev->next) = i->next;
			if(i==last) last = prev;
			n--;
			delete i;
			return;
		}
		prev=i;

	}
}

// iterator methods

void SemList::gotoFirst(){
	curr = first;
}

void SemList::gotoNext(){
	if(curr==nullptr) return;
	curr = curr->next;
}

bool SemList::hasCurr() const{
	return curr!=nullptr;
}

bool SemList::hasNext() const{
	if(curr==nullptr) return false;
	return curr->next==nullptr;
}

bool SemList::end() const{
	return curr==last;
}
