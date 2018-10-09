/* iteratorG.c
   Generic Iterator implementation, using doubly linked list

   Written by: z5118740
   Date: 28/3/18

    You need to submit ONLY this file.... 

*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "iteratorG.h"

typedef struct Node {
    void *val;
    struct Node *next;
    struct Node *prev; 
} Node;

typedef struct IteratorGRep {
    Node *first;
    Node *last;
    Node *next;
    Node *prev;

    ElmNewFp copyElm;
    ElmCompareFp cmpElm;
    ElmFreeFp freeElm;

} IteratorGRep;

IteratorG newIterator(ElmCompareFp cmp, ElmNewFp copy, ElmFreeFp free){
    IteratorG it = malloc(sizeof(struct IteratorGRep));
    assert(it != NULL);
    it->first = it->last = NULL; 
    it->prev = it->next = NULL;
    it->cmpElm = cmp;
    it->copyElm = copy;
    it->freeElm = free;

    return it;
}

int add(IteratorG it, void *vp){
	assert(it != NULL);
	//create them nodes
	struct Node *n = malloc(sizeof(Node));
	n->val = it->copyElm(vp);

	//empty list
		if(it->next == NULL && it->prev == NULL){
			//prev needed cause iterator is always pointing backwards
			it->first = it->last = it->next = n;
			n->next = n->prev = it->prev = NULL;

			return 1;
		//first item in the iterator
		} else if(it->next == it->first){
			n->next = it->next;
			it->next->prev = n;
			it->next = it->first = n;
			it->prev = n->prev = NULL;

			return 1;
		//last item in the iterator
		} else if(it->prev == it->last){
			n->next = it->next;
			it->last = n;
			n->next = NULL;
			it->next->prev = it->prev;
			n->prev = it->prev;
			it->prev->next = n;
			it->next = n;
			return 1;

		//middle
		} else if(it->prev != NULL && it->next != NULL){
			n->prev = it->prev;
			n->next = it->next;

			it->prev->next = n;
			it->next->prev = n;

			it->next = n;

			return 1;
		//return 0 if error
		} else{
			return 0;
		}

}

int hasNext(IteratorG it){
	assert(it != NULL);
	if(it->next != NULL){
		return 1;
	} else {
		return 0;
	}

}

int hasPrevious(IteratorG it){
	assert(it != NULL);
	if(it->prev != NULL){
		return 1;
	} else{
		return 0;
	}
}

void *next(IteratorG it){
	assert(it != NULL);
	if(it->prev == NULL && it->next == NULL){
		return NULL;
	}
	//middle case
	if(it->next != NULL && it->prev != NULL){
		it->prev = it->next;
		it->next = it->next->next;

		return it->prev->val;
	//last case	
	} else if (it->next == it->last){
		it->prev = it->next;
		it->next = it->next->next;

		return it->prev->val;
	//first case		
	} else if (it->next == it->first){
		it->prev = it->first;
		it->next = it->first->next;

		return it->prev->val;
	} else {
		return NULL;
	}
}

void *previous(IteratorG it){
	assert(it != NULL);
	//middle
	if(it->prev == NULL && it->next == NULL){
		return NULL;

	} else if(it->next == it->first){
		it->next = it->first;
		it->prev = NULL;
		return NULL;
	//middle
	} else if (it->prev != NULL && it->next != NULL){

		it->next = it->prev;
		it->prev = it->prev->prev;

		return it->next->val;

	//first item
	} else if(it->prev == it->first){
		it->prev = NULL;
		it->next = it->first;

		return it->next->val;
	//last item
	} else if(it->prev == it->last){

		it->prev = it->last->prev;
		it->next = it->last;

		return it->next->val;
	} else {
		it->prev = NULL;
		return NULL;
	}
}

int del(IteratorG it){
	assert(it != NULL);
	Node *tmp;
	//empty list
	if(it->next == NULL && it->prev == NULL){
		return 0;

	//first item case
	} else if(it->next == it->first){
		return 0;

	//last item list
	} else if(it->prev == it->last){
		tmp = it->last;
		it->last = it->last->prev;
		it->next = it->last->next = NULL;
		it->prev = it->last;
		//it->freeElm(tmp->val); //free the value
		free(tmp); //free the pointer

		return 1;
	//middle case
	} else{
		if(it->prev->prev != NULL){
			tmp = it->prev;
			it->next->prev = it->prev->prev;
			it->prev->prev->next = it->next;
			it->prev = tmp->prev;
			//it->freeElm(tmp->val); //free the value
			free(tmp); //free the pointer

			return 1;
		} else {
			tmp = it->prev;
			it->prev = NULL;
			free(tmp);

			return 1;
		}
		return 1;
	}

}

int set(IteratorG it, void *vp){
	assert(it != NULL);
	//can't set if nothing before prev
	if(it->prev == NULL){
		return 0;
	} else {
		it->prev->val = vp;
		return 1;
	}
}

IteratorG advance(IteratorG it, int n){
	assert(it != NULL);
	IteratorG newIt = newIterator(it->cmpElm, it->copyElm, it->freeElm);
	int dToEnd = distanceToEnd(it);
	int dToStart = -distanceFromStart(it);
	int counter = 0;
	Node *tmp = NULL;

	//checking forwards
	//if n >= 0 && n > distance to end can't happen
	if(n > 0 && n > dToEnd){
		return NULL;

	//checking backwards
	//if n < 0 && n < distance to Start can't happen 
	} else if (n < 0 && n < dToStart){
		return NULL;
	//moving forward
	} else if (n > 0 && n <= dToEnd){
		counter = 0;
		while(counter < n){
			Node *new = malloc(sizeof(Node));
			new->val = it->next->val;
			//check to see if the list is empty
			if(newIt->next == NULL && newIt->prev == NULL){
				newIt->first = newIt->last = newIt->prev = new;
				newIt->next = NULL;
			} else {
				tmp = newIt->prev;
				newIt->next = new;
				newIt->next->prev = tmp;
				newIt->prev->next = new;
				newIt->prev = newIt->next;
				newIt->next = newIt->next->next;
			}
			it->prev = it->next;
			it->next = it->next->next;
			counter++; 
		}
	} else if(n < 0 && n >= dToStart){
		counter = 0;
		while(counter > n){
			Node *new = malloc(sizeof(Node));
			new->val = it->prev->val;
			//checking backwards
			if(newIt->next == NULL && newIt->prev == NULL){
				newIt->first = newIt->last = newIt->prev = new;
				newIt->next = NULL;
			} else {
				tmp = newIt->prev;
				newIt->next = new;
				newIt->next->prev = tmp;
				newIt->prev->next = new;
				newIt->prev = newIt->next;
				newIt->next = newIt->next->next;
			}
			it->next = it->prev;
			it->prev = it->prev->prev;
			counter--;
		}
	}
	newIt->next = newIt->first;
	newIt->prev = NULL;
	return newIt;
}

void reverse(IteratorG it){
	assert(it != NULL);
	Node *next = it->next;
	Node *prev = it->prev;
	Node *curr =  it->first;
	Node *tmp;

	while(curr != NULL){
		tmp = curr->next;
		curr->next = curr->prev;
		curr->prev = tmp;
		curr = tmp;
	}

	tmp = it->first;
	it->first = it->last;
	it->last = tmp;
	it->next = prev;
	it->prev = next;

}

IteratorG find(IteratorG it, int (*fp) (void *vp) ){
	assert(it != NULL);
        Node *tmp = NULL;

    IteratorG newIt = newIterator(it->cmpElm, it->copyElm, it->freeElm);
    Node *curr = it->next;
    while(curr != NULL){
        if(fp(curr->val) == 1){
            Node *new = malloc(sizeof(Node));
	    new->val = curr->val;
            if(newIt->next == NULL && newIt->prev == NULL){
                newIt->first = newIt->last = newIt->prev = new;
                newIt->next = NULL;
            } else {
                tmp = newIt->prev;
                newIt->next = new;
                newIt->next->prev = tmp;
                newIt->prev->next = new;
                newIt->prev = newIt->next;
                newIt->next = newIt->next->next;
            }
        }
	//reverse(newIt);
        curr = curr->next;
    
}    
    newIt->next = newIt->first;
    newIt->prev = NULL;
    return newIt;

}

int distanceFromStart(IteratorG it){
	assert(it != NULL);
	int counter = 0;
	Node *prev = it->prev;
	//already at the beginning of the iterator
	if(prev == NULL){
		return 0;
	} else {
		while(prev != NULL){
			counter++;
			prev = prev->prev;
		}
	}
	return counter;
}

int distanceToEnd(IteratorG it){
	assert(it != NULL);
	int counter = 0;
	Node *curr = it->next;
	// at the end of the iterator
	if(curr == NULL){
		return 0;
	} else {
		while(curr != NULL){
			counter++;
			curr = curr->next;
		}
	}
	return counter;
}

void reset(IteratorG it){
	assert(it != NULL);
	it->next = it->first;
	it->prev = NULL;
}

void freeIt(IteratorG it){
    assert(it != NULL);
    Node *tmp;
    it->next = it->first;
    it->prev = NULL;
    while(it->next != NULL){
        if(it->next->next != NULL){
            tmp = it->next;
            it->next = it->next->next;
            //free(tmp->val);
            free(tmp);
        } else if(it->next->next == NULL) {
            tmp = it->next;
            it->next = NULL;
            //free(tmp->val);
            free(tmp);
        }
    }
    free(it);

}


