/*
*	Operating System Lab
*	    Lab2 (Synchronization)
*	    Student id : 
*	    Student name : 
*
*   lab2_bst.c :
*       - thread-safe bst code.
*       - coarse-grained, fine-grained lock code
*
*   Implement thread-safe bst for coarse-grained version and fine-grained version.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

#include "lab2_sync_types.h"

pthread_mutex_t c_lock = PTHREAD_MUTEX_INITIALIZER;
/*
 * TODO
 *  Implement funtction which traverse BST in in-order
 *  
 *  @param lab2_tree *tree  : bst to print in-order. 
 *  @return                 : status (success or fail)
 */
int lab2_node_print_inorder(lab2_tree *tree) {
    // You need to implement lab2_node_print_inorder function.
}

/*
 * TODO
 *  Implement function which creates struct lab2_tree
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_tree )
 * 
 *  @return                 : bst which you created in this function.
 */
lab2_tree *lab2_tree_create() {
    // You need to implement lab2_tree_create function.

	lab2_tree *tree = (lab2_tree *) malloc(sizeof(lab2_tree));
	tree->root = NULL;
	pthread_mutex_init(&(tree->t_mutex), NULL);
	return tree;
}

/*
 * TODO
 *  Implement function which creates struct lab2_node
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param int key          : bst node's key to creates
 *  @return                 : bst node which you created in this function.
 */
lab2_node * lab2_node_create(int key) {
    // You need to implement lab2_node_create function.
	lab2_node* newNode = (lab2_node *)malloc(sizeof(lab2_node));
	newNode->key = key;
	newNode->left = NULL;
	newNode->right = NULL;
	pthread_mutex_init(&(newNode->mutex), NULL);

	return newNode;
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST. 
 *  
 *  @param lab2_tree *tree      : bst which you need to insert new node.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                 : satus (success or fail)
 */
int lab2_node_insert(lab2_tree *tree, lab2_node *new_node){
    // You need to implement lab2_node_insert function.
	lab2_node *p = tree->root;
	lab2_node *q = NULL;

	while(p){
		q = p;
		if(new_node->key == p->key) return false;
		if(new_node->key < p->key) p=p->left;
		else p=p->right;
	}
	p = new_node;
	if(!tree->root) tree->root = p;
	else if(new_node->key < q->key) q->left = p;
	else q->right = p;

	return true;
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST in fine-garined manner.
 *
 *  @param lab2_tree *tree      : bst which you need to insert new node in fine-grained manner.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                     : status (success or fail)
 */
int lab2_node_insert_fg(lab2_tree *tree, lab2_node *new_node){
      // You need to implement lab2_node_insert_fg function.
	if(tree->root){
		pthread_mutex_lock(&(tree->root->mutex));
	} else pthread_mutex_lock(&(tree->t_mutex));
	lab2_node *p = tree->root;
	lab2_node *q = NULL;

	while(p){
		if(q) pthread_mutex_unlock(&(q->mutex));
		q = p;
		if(new_node->key == p->key){
			pthread_mutex_unlock(&(p->mutex));
			return false;
		}
		if(new_node->key < p->key){
			if(p->left) pthread_mutex_lock(&(p->left->mutex));
			p=p->left;
		}
		else{
			if(p->right) pthread_mutex_lock(&(p->right->mutex));
			p=p->right;
		}
	}
	p = new_node;
	if(!(tree->root)){
		tree->root = p;
		pthread_mutex_unlock(&(tree->t_mutex));
		return true;
	}
	else if(new_node->key < q->key) q->left = p;
	else q->right = p;
	pthread_mutex_unlock(&(q->mutex));

	return true;
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST in coarse-garined manner.
 *
 *  @param lab2_tree *tree      : bst which you need to insert new node in coarse-grained manner.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                     : status (success or fail)
 */
int lab2_node_insert_cg(lab2_tree *tree, lab2_node *new_node){
    // You need to implement lab2_node_insert_cg function.
	pthread_mutex_lock(&c_lock);
	lab2_node *p = tree->root;
	lab2_node *q = NULL;

	while(p){
		q = p;
		if(new_node->key == p->key){
			pthread_mutex_unlock(&c_lock);
			return false;
		}
		if(new_node->key < p->key) p=p->left;
		else p=p->right;
	}
	p = new_node;
	if(!(tree->root)) tree->root = p;
	else if(new_node->key < q->key) q->left = p;
	else q->right = p;

	pthread_mutex_unlock(&c_lock);
	return true;
}

/* 
 * TODO
 *  Implement a function which remove nodes from the BST.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
int lab2_node_remove(lab2_tree *tree, int key) {
    // You need to implement lab2_node_remove function.
}

/* 
 * TODO
 *  Implement a function which remove nodes from the BST in fine-grained manner.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node in fine-grained manner from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
int lab2_node_remove_fg(lab2_tree *tree, int key) {
    // You need to implement lab2_node_remove_fg function.
}


/* 
 * TODO
 *  Implement a function which remove nodes from the BST in coarse-grained manner.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node in coarse-grained manner from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
int lab2_node_remove_cg(lab2_tree *tree, int key) {
    // You need to implement lab2_node_remove_cg function.
}

lab2_node *lab2_node_search(lab2_tree *tree, int key){
	for(lab2_node *t = tree->root; t; ){
		if(key == t->key) return t;
		if(key < t->key) t = t->left;
		else t = t->right;
	}
	return NULL;
}

lab2_node *lab2_node_search_fg(lab2_tree *tree, int key){
	if(!(tree->root)) pthread_mutex_lock(&(tree->root->mutex));
	for(lab2_node *t = tree->root; t; ){
		if(key == t->key){
			pthread_mutex_unlock(&(t->mutex));
			return t;
		}
		if(key < t->key){
			pthread_mutex_unlock(&(t->mutex));
			if(!(t->left)) pthread_mutex_lock(&(t->left->mutex));
			t = t->left;
		}
		else{
			pthread_mutex_unlock(&(t->mutex));
			if(!(t->right)) pthread_mutex_lock(&(t->right->mutex));
			t = t->right;
		}
	}
	return NULL;
}

lab2_node *lab2_node_search_cg(lab2_tree *tree, int key){
	pthread_mutex_lock(&c_lock);
	for(lab2_node *t = tree->root; t; ){
		if(key == t->key) return t;
		if(key < t->key) t = t->left;
		else t = t->right;
	}
	pthread_mutex_unlock(&c_lock);
	return NULL;
}


/*
 * TODO
 *  Implement function which delete struct lab2_tree
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param lab2_tree *tree  : bst which you want to delete. 
 *  @return                 : status(success or fail)
 */
void lab2_tree_delete(lab2_tree *tree) {
    // You need to implement lab2_tree_delete function.
	if(tree->root){
		printf("Tree is not empty\n");
//		return;
	}
	free(tree);
}

/*
 * TODO
 *  Implement function which delete struct lab2_node
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param lab2_tree *tree  : bst node which you want to remove. 
 *  @return                 : status(success or fail)
 */
void lab2_node_delete(lab2_node *node) {
    // You need to implement lab2_node_delete function.
	free(node);
}
