#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "cs402.h"

#include "my402list.h"

//Returns the number of elements in the list.
int  My402ListLength(My402List* list) {
	return list->num_members;
}

//Returns TRUE if the list is empty. Returns FALSE otherwise.
int  My402ListEmpty(My402List* list) {
	return list->num_members <= 0 ? 1 : 0;
}

//If list is empty, just add obj to the list. Otherwise, add obj after Last(). 
//This function returns TRUE if the operation is performed successfully and returns FALSE otherwise.
int  My402ListAppend(My402List* list, void* obj) {
	My402ListElem *newElem = (My402ListElem*)malloc(1*sizeof(My402ListElem));
	if(newElem == NULL)
		return FALSE;
	newElem->obj = obj;

	if (My402ListEmpty(list))
	{
		list->anchor.next = newElem;
		list->anchor.prev = newElem;
		newElem->next = &list->anchor;
		newElem->prev = &list->anchor;
	}
	else {
		My402ListElem *lastElem = My402ListLast(list);
		lastElem->next = newElem;
		list->anchor.prev = newElem;
		newElem->prev = lastElem;
		newElem->next = &list->anchor;
	}

	list->num_members++;
	return TRUE;
}

//If list is empty, just add obj to the list. Otherwise, add obj before First(). 
//This function returns TRUE if the operation is performed successfully and returns FALSE otherwise.
int  My402ListPrepend(My402List* list, void* obj) {
	My402ListElem *newElem = (My402ListElem*)malloc(1*sizeof(My402ListElem));
	if(newElem == NULL)
		return FALSE;
	newElem->obj = obj;

	if (My402ListEmpty(list))
	{
		list->anchor.next = newElem;
		list->anchor.prev = newElem;
		newElem->next = &list->anchor;
		newElem->prev = &list->anchor;
	}
	else {
		My402ListElem *firstElem = My402ListFirst(list);
		firstElem->prev = newElem;
		list->anchor.next = newElem;
		newElem->next = firstElem;
		newElem->prev = &list->anchor;
	}

	list->num_members++;
	return TRUE;
}

//Unlink and delete elem from the list. 
//Please do not delete the object pointed to by elem and do not check if elem is on the list.
void My402ListUnlink(My402List* list, My402ListElem* elem) {
	if (My402ListEmpty(list)) 
		return;
	elem->prev->next = elem->next;
	elem->next->prev = elem->prev;
	free(elem);
	list->num_members--;
}

//Unlink and delete all elements from the list and make the list empty. 
//Please do not delete the objects pointed to be the list elements.
void My402ListUnlinkAll(My402List* list) {	
	for(; list->num_members > 0; list->num_members--) {
		My402ListElem *temp = My402ListFirst(list);
		My402ListUnlink(list, temp);
	}
}


//Insert obj between elem and elem->prev. If elem is NULL, then this is the same as Prepend(). 
//This function returns TRUE if the operation is performed successfully and returns FALSE otherwise. Please do not check if elem is on the list.
int  My402ListInsertBefore(My402List* list, void* obj, My402ListElem* elem) {
	My402ListElem *newElem = (My402ListElem*)malloc(1*sizeof(My402ListElem));
	if(newElem == NULL)
		return FALSE;
	if (elem == NULL) {
		My402ListPrepend(list, obj);
	}
	else {
		newElem->obj = obj;
		newElem->next = elem;
		newElem->prev = elem->prev;
		newElem->prev->next = newElem;
		elem->prev = newElem;
		list->num_members++;
	}
	return TRUE;
}

//Insert obj between elem and elem->next. If elem is NULL, then this is the same as Append(). 
//This function returns TRUE if the operation is performed successfully and returns FALSE otherwise. Please do not check if elem is on the list.
int  My402ListInsertAfter(My402List* list, void* obj, My402ListElem* elem) {
	My402ListElem *newElem = (My402ListElem*)malloc(1*sizeof(My402ListElem));
	if(newElem == NULL)
		return FALSE;
	if (elem == NULL) {
		My402ListAppend(list, obj);
	}
	else {
		newElem->obj = obj;
		newElem->prev = elem;
		newElem->next = elem->next;
		newElem->next->prev = newElem;
		elem->next = newElem;
		list->num_members++;
	}
	return TRUE;
}

//Returns the first list element or NULL if the list is empty.
My402ListElem *My402ListFirst(My402List* list) {
	if(My402ListEmpty(list))
		return NULL;
	else
		return list->anchor.next;
}

//Returns the last list element or NULL if the list is empty.
My402ListElem *My402ListLast(My402List* list) {
	if(My402ListEmpty(list))
		return NULL;
	else
		return list->anchor.prev;
}

//Returns elem->prev or NULL if elem is the first item on the list. Please do not check if elem is on the list.
My402ListElem *My402ListPrev(My402List* list, My402ListElem* elem) {
	if(My402ListFirst(list) == NULL || My402ListFirst(list) == elem)
		return NULL;
	else
		return elem->prev;
}

//Returns elem->next or NULL if elem is the last item on the list. Please do not check if elem is on the list.
My402ListElem *My402ListNext(My402List* list, My402ListElem* elem) {
	if(My402ListLast(list) == NULL || My402ListLast(list) == elem)
		return NULL;
	else
		return elem->next;
}

//Returns the list element elem such that elem->obj == obj. Returns NULL if no such element can be found.
My402ListElem *My402ListFind(My402List* list, void* obj) {
	My402ListElem *temp = My402ListFirst(list);
	while (temp != NULL) {
		if (temp->obj == obj)
			return temp;
		else {
			temp = temp->next;
		}
	}
	free(temp);
	return NULL;
}

//Initialize the list into an empty list. Returns TRUE if all is well and returns FALSE if there is an error initializing the list.
int My402ListInit(My402List* list) {
	My402ListElem *anch = &list->anchor;
	if(anch == NULL)
		return FALSE;	
	else{
		anch->obj = NULL;
		anch->next = anch;		
		anch->prev = anch;
		list->num_members = 0;
		return TRUE;
	}
}

