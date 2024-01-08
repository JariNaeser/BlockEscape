/*
 * node.h
 *
 *  Created on: Dec 20, 2023
 *      Author: jarinaser
 */

#ifndef INC_NODE_H_
#define INC_NODE_H_

#include <stdbool.h>

struct Node;

typedef struct Node {
	bool isPixel;
	unsigned int x;
	unsigned int y;
	struct Node* previous;
	struct Node* next;
} Node;

Node* initQueue();
void addNode(Node* queueRoot, Node* newNode);
void removeNode(Node* nodeToRemove);
Node* createNode(bool, int, int);

#endif /* INC_NODE_H_ */
