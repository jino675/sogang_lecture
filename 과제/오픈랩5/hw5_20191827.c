#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

struct threadedTree {
	short int leftThread;
	struct threadedTree* leftChild;
	char data;
	struct threadedTree* rightChild;
	short int rightThread;
};
typedef struct threadedTree threadedTree;
typedef struct threadedTree* threadedPointer;


void construct_tree(threadedPointer tree);
threadedPointer insucc(threadedPointer tree);
void tinorder(threadedPointer tree);
void insertRight(threadedPointer s, threadedPointer r);
void insert(threadedPointer s, char data);

void main()
{
	threadedPointer root = (threadedPointer)malloc(sizeof(threadedTree));
	root->rightChild = root;
	root->rightThread = false;

	construct_tree(root);
	insert(root->leftChild->rightChild, 'E');
	insert(root->leftChild->leftChild->rightChild, 'F');
	insert(root->leftChild->leftChild, 'G');
	tinorder(root);

	return;
}

void construct_tree(threadedPointer tree)
{
	threadedPointer temp, temp_root;

	temp_root = tree;
	/* insert node 'A' to the left of the head */
	temp = (threadedPointer)malloc(sizeof(threadedTree));
	temp->leftThread = temp->rightThread = true;
	temp->leftChild = temp->rightChild = temp;
	temp->data = 'A';

	temp_root->leftChild = temp;
	temp_root->leftThread = false;

	temp_root = tree->leftChild;
	/* insert node 'B' to the left of the temp_head */
	temp = (threadedPointer)malloc(sizeof(threadedTree));
	temp->rightThread = temp->leftThread = true;
	temp->leftChild = tree;
	temp->rightChild = temp;
	temp->data = 'B';

	temp_root->leftChild = temp;
	temp_root->leftThread = false;

	/* insert node 'C' to the right of the temp_head */
	temp = (threadedPointer)malloc(sizeof(threadedTree));
	temp->leftThread = temp->rightThread = true;
	temp->leftChild = temp_root;
	temp->rightChild = tree;
	temp->data = 'C';

	temp_root->rightChild = temp;
	temp_root->rightThread = false;

	temp_root = temp_root->leftChild;
	/* insert node 'D' to the right of the temp_head */
	temp = (threadedPointer)malloc(sizeof(threadedTree));
	temp->leftThread = temp->rightThread = true;
	temp->leftChild = temp_root;
	temp->rightChild = tree->leftChild;
	temp->data = 'D';

	temp_root->rightChild = temp;
	temp_root->rightThread = false;

	return;
}

threadedPointer insucc(threadedPointer tree)
{
	threadedPointer temp;
	temp = tree->rightChild;
	if (tree->rightThread == false) {
		while (temp->leftThread == false) {
			temp = temp->leftChild;
		}
	}

	return temp;
}

void tinorder(threadedPointer tree)
{
	threadedPointer temp = tree;
	while (true) {
		temp = insucc(temp);
		if (temp == tree) break;
		printf("%c ", temp->data);
	}
	printf("\n\n");

	return;
}

void insert(threadedPointer parent, char data)
{
	threadedPointer newchild = (threadedPointer)malloc(sizeof(threadedTree));
	newchild->data = data;
	insertRight(parent,newchild);

	return;
}

void insertRight(threadedPointer parent, threadedPointer newchild)
{
	threadedPointer temp;
	newchild->rightChild = parent->rightChild;
	newchild->rightThread = parent->rightThread;
	newchild->leftChild = parent;
	newchild->leftThread = true;
	parent->rightChild = newchild;
	parent->rightThread = false;
	if (newchild->rightThread == false) {
		temp = insucc(newchild);
		temp->leftChild = newchild;
	}

	return;
}