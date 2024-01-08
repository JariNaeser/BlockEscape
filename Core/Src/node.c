/*
 * node.c
 *
 *  Created on: Dec 20, 2023
 *      Author: jarinaser
 */

#include <stdlib.h>
#include "node.h"

Node* initQueue(){
	return createNode(false, -1, -1);
}

void addNode(Node* queueRoot, Node* newNode){

	Node* tmp = queueRoot;
	while(tmp->next != NULL){
		tmp = tmp->next;
	}

	newNode->previous = tmp;
	newNode->next = NULL;
	tmp->next = newNode;
}

Node* createNode(bool isPixel, int x, int y){
	Node* tmp = (Node*) malloc(sizeof(Node));
	tmp->isPixel = isPixel;
	tmp->x = x;
	tmp->y = y;
	tmp->previous = NULL;
	tmp->next = NULL;

	return tmp;
}

void removeNode(Node* nodeToRemove){
	nodeToRemove->previous->next = nodeToRemove->next;
	nodeToRemove->next->previous = nodeToRemove->previous;
	// Delete object
	free(nodeToRemove);
}
