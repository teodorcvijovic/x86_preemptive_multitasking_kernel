#include "list_pcb.h"

PCB* PCBList::getFirst(){
	if(first==nullptr) return nullptr;
	return first->info;
}

PCB* PCBList::getLast(){
	if(last==nullptr) return nullptr;
	return last->info;
}

PCB* PCBList::getCurr(){
	if(curr==nullptr) return nullptr;
	return curr->info;
}

void PCBList::removeFirst(){
	if (first==nullptr) return;
	if (first==last) last=nullptr;
	if (first==curr) curr=curr->next;
	Node* old = first;
	first = first->next;
	n--;
	delete old;
}

void PCBList::empty(){
	for(gotoFirst(); hasCurr(); removeFirst()){

	}
	n=0;
}

void PCBList::append(PCB* pcb){
	Node* node = new Node(pcb);
	n++;
	(first==nullptr ? first : last->next) = node;
	last = node;
}

void PCBList::remove(PCB* pcb){
	if (pcb == nullptr) return;
	Node* prev = nullptr;
	for(Node* i = first ; i != nullptr; i=i->next){
		if(i!=nullptr && i->info==pcb){
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

void PCBList::gotoFirst(){
	curr = first;
}

void PCBList::gotoNext(){
	if(curr==nullptr) return;
	curr = curr->next;
}

bool PCBList::hasCurr() const{
	return curr!=nullptr;
}

bool PCBList::hasNext() const{
	if(curr==nullptr) return false;
	return curr->next==nullptr;
}

bool PCBList::end() const{
	return curr==last;
}

