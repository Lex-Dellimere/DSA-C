#include <stdlib.h>
#include <stdio.h>

typedef struct Node {
  int value;
  struct Node *parent;
  struct Node *right;
  struct Node *left;
} Node;

// init a node in the BST
Node *insert(Node *node, int value) {
  if (!node) {
    // init a new node
    Node *new_node = malloc(sizeof(Node));
    new_node->value = value;
    new_node->parent = NULL;
    new_node->right = NULL;
    new_node->left = NULL;

    return new_node;
  }

  if (value < node->value) {
    node->left = insert(node->left, value);
    node->left->parent = node;
  } else {
    node->right = insert(node->right, value);
    node->right->parent = node;
  }
  return node;
}

// find a node and return its memory location in the BST
Node *find(Node *node, int value) {
  Node *current = node;
  while (current && current->value != value) {
    if (value < current->value) {
      current = current->left;
    } else {
      current = current->right;
    }
  }
  if (current)
    return current;
  return NULL;
}

void free_BST(Node *node) {
  if (!node)
    return;
  free_BST(node->left);
  free_BST(node->right);
  free(node);
}

// traversal
void in_order(Node *node) {
  if (!node)
    return;

  in_order(node->left);
  printf("%d ", node->value);
  in_order(node->right);
}

void pre_order(Node *node) {
  if (!node)
    return;
  printf("%d ", node->value);
  pre_order(node->left);
  pre_order(node->right);
}

void post_order(Node *node) {
  if (!node)
    return;
  post_order(node->left);
  post_order(node->right);
  printf("%d ", node->value);
}


int main(void) {

  int values[] = {5, 3, 7, 2, 4, 6, 8};

  Node *root = NULL;

  for (int i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
    root = insert(root, values[i]);
  }

  Node *found = find(root, 7);

  printf("%d\n", found->value);

  printf("In order: ");
  in_order(root);
  printf("\n");

  printf("Pre order: ");
  pre_order(root);
  printf("\n");

  printf("Post order: ");
  post_order(root);
  printf("\n");

  free_BST(root);

  return 0;
}
