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
#include <errno.h>
#include <time.h>
#include "lab2_sync_types.h"

pthread_mutex_t c_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t free_lock = PTHREAD_MUTEX_INITIALIZER;
/*
 * TODO
 *  Implement funtction which traverse BST in in-order
 *  
 *  @param lab2_tree *tree  : bst to print in-order. 
 *  @return                 : status (success or fail)
 */
int lab2_node_print_inorder(lab2_tree *tree) {
    // You need to implement lab2_node_print_inorder function.
    lab2_node * root = tree -> root;

    if(root == NULL){
        return false;
    }else{
        inorder(root);
        return true;
    }


}

void inorder(lab2_node * currentNode){
    if(currentNode){
        inorder(currentNode -> left);
        printf("%d ", currentNode -> key);
        inorder(currentNode -> right);
    }
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
	pthread_mutex_init(&(tree -> t_mutex), NULL);
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
    lab2_node * child = tree -> root; //Fugitive
    lab2_node * parent = NULL; //Chaser


    //Find node that has same key
    while(child && child-> key != key){
        parent = child;
        if(key > child -> key){
            child = child -> right;
        }else if(key < child -> key){
            child = child -> left;
        }
    }

    //No such key
    if(child == NULL){
        return false;
    }

    //Delete leap node
    if(child -> left == NULL && child -> right == NULL){
        //root only 
        if(parent == NULL){
            tree -> root = NULL;
        }else{
            if(parent -> right == child){
                parent -> right = NULL;
            }else{
                parent -> left = NULL;
            }
        }
    }
    //delete node that has two children
    else if(child -> left != NULL && child -> right != NULL){
        lab2_node * temp = child;
        parent = child;
        child = child -> left;

        while(child -> right){
            parent = child;
            child = child -> right;
        }
        if(parent -> left == child){
            parent -> left = child -> left;
        }else{
            parent -> right = child -> left;
        }
        temp -> key = child -> key;
    }
    //delete node that has a child
    else{
        lab2_node * temp = (child -> left != NULL) ? child -> left : child -> right;
        if(parent != NULL){
            if(parent -> left == child){
                parent -> left = temp;
            }else{
                parent -> right = temp;
            }
        }else{
            tree -> root = temp;
        }
    }
    free(child);
    child = NULL; 
    return true;
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
    if(tree -> root == NULL){
        return false;
    }
    struct timespec etime;
    pthread_mutex_lock(&tree -> root -> mutex);
    lab2_node * child = tree -> root; //Fugitive
    pthread_mutex_unlock(&tree -> root -> mutex);
    
    lab2_node * parent = NULL; //Chaser

    //Find node that has same key
    for(pthread_mutex_lock(&child -> mutex); child && child-> key != key;){
        parent = child;
        if(key > child -> key){
            if(child -> right != NULL){
                pthread_mutex_lock(&child -> right -> mutex);
                pthread_mutex_unlock(&child -> mutex);
                child = child -> right;
            }else{
                pthread_mutex_unlock(&child -> mutex);
                child = NULL;
                break;
            }
        }else if(key < child -> key){
            if(child -> left != NULL){
                pthread_mutex_lock(&child -> left -> mutex);
                pthread_mutex_unlock(&child -> mutex);
                child = child -> left;
            }else{
                pthread_mutex_unlock(&child -> mutex);
                child = NULL;
                break;
            }
        }
    }

    //No such key
    if(child == NULL){
        return false;
    }
    
    //Delete leap node
    if(child -> left == NULL && child -> right == NULL){
        //root only 
        if(parent == NULL){
            pthread_mutex_unlock(&child -> mutex);
            tree -> root = NULL;
        }else{
            pthread_mutex_unlock(&child -> mutex);
            clock_gettime(CLOCK_REALTIME, &etime);
            etime.tv_sec += 1;
            if(pthread_mutex_timedlock(&parent -> mutex, &etime) == ETIMEDOUT){ 
                return false;
            }
            if(parent -> right == child){
                parent -> right = NULL;
            }else{
                parent -> left = NULL;
            }
            pthread_mutex_unlock(&parent -> mutex);
        }
    }
    
    //delete node that has two children
    else if(child -> left != NULL && child -> right != NULL){
        lab2_node * temp = child;
        parent = child;
        pthread_mutex_unlock(&child -> mutex); 
        pthread_mutex_lock(&child -> left -> mutex);
        child = child -> left;
        
        while(child -> right){
            parent = child;
            pthread_mutex_unlock(&child -> mutex);
            pthread_mutex_lock(&child -> right -> mutex);
            child = child -> right;
        }
        clock_gettime(CLOCK_REALTIME, &etime);
        etime.tv_sec += 1;
        if(pthread_mutex_timedlock(&parent -> mutex, &etime) == ETIMEDOUT){
            pthread_mutex_unlock(&child ->mutex);
            return false;
        }
        if(parent -> left == child){
            parent -> left = child -> left;
            temp -> key = child -> key;
            pthread_mutex_unlock(&child -> mutex);
            pthread_mutex_unlock(&temp -> mutex);
        }else{
            parent -> right = child -> left;
            temp -> key = child -> key;
            pthread_mutex_unlock(&temp -> mutex);
            pthread_mutex_unlock(&child -> mutex);
            pthread_mutex_unlock(&parent -> mutex);
        }
    }
    //delete node that has a child
    else{
        lab2_node * temp;
        if(child -> left != NULL){
            pthread_mutex_unlock(&child -> mutex);
            pthread_mutex_lock(&child -> left -> mutex);
            temp = child -> left;
        }else{
            pthread_mutex_unlock(&child -> mutex);
            pthread_mutex_lock(&child -> right -> mutex);
            temp = child -> right;
        }
       
        if(parent != NULL){
            if(parent -> left == child){
                parent -> left = temp;
            }else{
                parent -> right = temp;
            }
        }else{
            tree -> root = temp;
        }
        pthread_mutex_unlock(&child -> mutex);
        pthread_mutex_unlock(&temp -> mutex);
    }
    pthread_mutex_lock(&free_lock);
    free(child);
    pthread_mutex_unlock(&free_lock);
    return true;
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
    pthread_mutex_lock(&c_lock);
    lab2_node * child = tree -> root; //Fugitive
    lab2_node * parent = NULL; //Chaser

    //Find node that has same key
    while(child && child-> key != key){
        parent = child;
        if(key > child -> key){
            child = child -> right;
        }else if(key < child -> key){
            child = child -> left;
        }
    }

    //No such key
    if(child == NULL){
        pthread_mutex_unlock(&c_lock);
        return false;
    }

    //Delete leap node
    if(child -> left == NULL && child -> right == NULL){
        //root only 
        if(parent == NULL){
            tree -> root = NULL;
        }else{
            if(parent -> right == child){
                parent -> right = NULL;
            }else{
                parent -> left = NULL;
            }
        }
    }
    //delete node that has two children
    else if(child -> left != NULL && child -> right != NULL){
        lab2_node * temp = child;
        parent = child;
        child = child -> left;

        while(child -> right){
            parent = child;
            child = child -> right;
        }
        if(parent -> left == child){
            parent -> left = child -> left;
        }else{
            parent -> right = child -> left;
        }
        temp -> key = child -> key;
    }
    //delete node that has a child
    else{
        lab2_node * temp = (child -> left != NULL) ? child -> left : child -> right;
        if(parent != NULL){
            if(parent -> left == child){
                parent -> left = temp;
            }else{
                parent -> right = temp;
            }
        }else{
            tree -> root = temp;
        }
    }
    free(child);
    pthread_mutex_unlock(&c_lock);  
    return true;
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
		if(key == t->key){
			pthread_mutex_unlock(&c_lock);
			return t;
		}
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
void lab2_tree_delete(lab2_tree *tree, lab2_node *current) {
    // You need to implement lab2_tree_delete function.
	if(current){
//		printf("Tree is not empty\n");
		lab2_tree_delete(tree, current->left);
		lab2_tree_delete(tree, current->right);
		lab2_node_delete(current);
	}
	else return;
	if(tree->root == current) free(tree);
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

