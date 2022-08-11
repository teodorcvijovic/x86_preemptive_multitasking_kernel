#include "sem_help.h"

PCBTime* PCBTimeList::getFirst(){
	if(first==nullptr) return nullptr;
	return first->info;
}

PCBTime* PCBTimeList::getLast(){
	if(last==nullptr) return nullptr;
	return last->info;
}

PCBTime* PCBTimeList::getCurr(){
	if(curr==nullptr) return nullptr;
	return curr->info;
}

PCBTime* PCBTimeList::getPrev(){
	if(prev==nullptr) return nullptr;
	return prev->info;
}

void PCBTimeList::removeFirst(){ // doesn't delete the pcb
	if (first==nullptr) return;
	if (first==last) last=nullptr;
	if (first==curr) curr=curr->next;
	Node* old = first;
	first = first->next;
	n--;
	delete old;
}

void PCBTimeList::empty(){
	for(gotoFirst(); hasCurr(); removeFirst()){

	}
	n=0;
}

void PCBTimeList::append(PCBTime* pcb_and_time){			// called also from insert, increments the 'n'
	Node* node = new Node(pcb_and_time);
	n++;
	(first==nullptr ? first : last->next) = node;
	last = node;
}

void PCBTimeList::remove(PCBTime* pcb_and_time){
	if (pcb_and_time == nullptr) return;
	Node* _prev = nullptr;
	for(Node* i = first ; i != nullptr; i=i->next){
		if(i!=nullptr && i->info==pcb_and_time){
				(_prev==nullptr ? first : _prev->next) = i->next;
				if(i==last) last = _prev;
				n--;
				delete i;
				return;
		}
		_prev=i;
	}
}

	// helper methods for semaphore implementation

/* insertion example for insert method:
 *
 * 	5, 3, 8, 8, 9 (waiting times)
 *
 * list : 5
 * list : 3->2
 * ...
 * list : 3->2->3->0->1
 */

void PCBTimeList::insert(PCBTime* pcb_and_time){
	if(pcb_and_time == nullptr) return;
	for(gotoFirst(); hasCurr() && pcb_and_time->waitingTime >= getCurr()->waitingTime ; gotoNext()){
		pcb_and_time->waitingTime -= getCurr()->waitingTime;
	}

	if(hasCurr()) getCurr()->waitingTime -= pcb_and_time->waitingTime;

	insertBeforeCurr(pcb_and_time);


}

void PCBTimeList::frontInsert(PCBTime* pcb_and_time){ 		// insert helper, increments the 'n'
	Node* node = new Node(pcb_and_time);
	if(first == nullptr) last = node;
	node->next = first;
	first = node;
	n++;
}

void PCBTimeList::insertBeforeCurr(PCBTime* pcb_and_time){ 	// insert helper, increments the 'n'
	if(curr == nullptr) append(pcb_and_time);
	else if (prev == nullptr) frontInsert(pcb_and_time);
	else{
		Node* node = new Node(pcb_and_time);
		node->next = curr;
		prev->next = node;
		n++;
	}
}

	// iterator methods

void PCBTimeList::gotoFirst(){
	curr = first;
	prev = nullptr;
}

void PCBTimeList::gotoNext(){
	if(curr==nullptr) return;
	prev = curr;
	curr = curr->next;
}

bool PCBTimeList::hasCurr() const{
	return curr!=nullptr;
}

bool PCBTimeList::hasNext() const{
	if(curr==nullptr) return false;
	return curr->next==nullptr;
}

bool PCBTimeList::hasPrev() const{
	return prev!=nullptr;
}

bool PCBTimeList::end() const{
	return curr==last;
}
