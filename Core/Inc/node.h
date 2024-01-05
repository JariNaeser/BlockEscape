/*
 * node.h
 *
 *  Created on: Dec 20, 2023
 *      Author: jarinaser
 */

#ifndef INC_NODE_H_
#define INC_NODE_H_

#include <stdbool.h>

typedef struct node {
	bool isPixel;
	unsigned int x;
	unsigned int y;
	struct node* previous;
	struct node* next;
} Node;

Node* initQueue();
void addNode(Node* queueRoot, Node* newNode);
void removeNode(Node* nodeToRemove);

#endif /* INC_NODE_H_ */
