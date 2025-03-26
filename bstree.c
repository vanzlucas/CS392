/*******************************************************************************
 * Name        : bstree.c
 * Author      : Lucas Vanzelli
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
 #include "bstree.h"
 //(1) void* : this is where you pass the address of a new data you want to add to the tree;
 //(2) size_t : indicates the number of bytes of the new data;
 //(3) tree_t* : points to the object of tree_t you created for testing;
 //(4) int (*)(void*,void*) : a function pointer pointing to a type-specific cmpr function in utils.h .
 void add_node(void* addy, size_t num_bytes, tree_t* tree , int (*cmpr)(void*,void*)) {
    //Allocate memory for the node
    node_t* new_node = (node_t*)malloc(sizeof(node_t));

    //Allocate memory for the data of the node
    new_node->data = malloc(num_bytes);

    //Copy data 
    char* source = (char*) addy;
    char* destination = (char*) new_node->data;
    for (size_t i = 0; i < num_bytes; i++) {
        destination[i] = source[i];
    }

    //Assigning left and right children
    new_node->left = NULL;
    new_node->right = NULL;

    //If the root of the BST is NULL, then this would become the root
    if (tree->root == NULL) {
        tree->root = new_node;
        return;
    } else { //Root is not NULL
        node_t* parent_node = NULL;
        node_t* child_node = tree->root;

        //Going down the BST until we find the correct position of new_node
        while (child_node != NULL) {
            parent_node = child_node;
            if (cmpr(addy, child_node->data) < 0) {
                child_node = child_node->left;
            } else {
                child_node = child_node->right;
            }
        }

        //Determine if the new node should be a left or right child
        if (cmpr(addy, parent_node->data) < 0) {
            parent_node->left = new_node;
         } else {
            parent_node->right = new_node;
         }
    }
}

//In-order traversal of the tree to print the data
void print_tree(node_t* start_node, void (*print)(void*)) {
    //if the start_node is NULL then theres nothing to print durr
    if (start_node == NULL) {
        return;
    }
    print_tree(start_node->left, print);
    print(start_node->data);
    print_tree(start_node->right, print);
}

// Destroy the tree and free the allocated memory
void destroy(tree_t* tree) {
    //If the root is null then theres no tree to destory durr
    if (tree->root == NULL) {
        return;
    }

    node_t* node = tree->root;

    //Make trees with the root being the left and right chidren of the given tree to call destory recursively
    if (node->left != NULL) {
        tree_t left_root_tree;
        left_root_tree.root = node->left;
        destroy(&left_root_tree);
    }
    if (node->right != NULL) {
        tree_t right_root_tree;
        right_root_tree.root = node->right;
        destroy(&right_root_tree);
    }

    //Free up that data. Be gone
    free(node->data);
    free(node);

    //Root is now NULL
    tree->root = NULL;
}
 
 
  