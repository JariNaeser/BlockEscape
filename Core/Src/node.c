/*
 * node.c
 *
 *  Created on: Dec 20, 2023
 *      Author: jarinaser
 */

#include <stdlib.h>
#include "node.h";

Node* initQueue(){
	Node* root = (Node*) malloc(sizeof(Node));
	root->isPixel = false;
	return root;
}

void addNode(Node* queueRoot, Node* newNode){
	Node* tmp = queueRoot;
	while(tmp->next != NULL) tmp = tmp->next;

	newNode->previous = tmp;
	tmp->next = newNode;
}

void removeNode(Node* nodeToRemove){
	nodeToRemove->previous->next = nodeToRemove->next;
	// Delete object
	free(nodeToRemove);
}
