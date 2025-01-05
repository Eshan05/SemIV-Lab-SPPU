/*
Convert given binary tree into threaded binary tree.
Analyze time and space complexity of the algorithm.
*/
#include <iostream>
using namespace std;

struct Node {
  int value;
  Node *left, *right;
  bool rightThread;
  bool leftThread;
  Node() : left(nullptr), right(nullptr), rightThread(false) {}
};

Node *convert(Node *root) {
  if (root == nullptr) {
    return nullptr;
  }
  if (root->left == nullptr && root->right == nullptr) {
    return root;
  }
  if (root->left != nullptr) {
    Node *leftmostRight = convert(root->left);
    leftmostRight->right = root;
    leftmostRight->rightThread = true;
  }
  if (root->right == nullptr) {
    return root;
  }
  return convert(root->right);
}

Node *findLeftmost(Node *root) {
  while (root != nullptr && root->left != nullptr) {
    root = root->left;
  }
  return root;
}

void inorder(Node *root) {
  if (root == nullptr)
    return;
  Node *current = findLeftmost(root);
  while (current != nullptr) {
    cout << current->value << " ";
    if (current->rightThread) {
      current = current->right;
    } else {
      current = findLeftmost(current->right);
    }
  }
  cout << endl;
}

Node *createNewNode(int value) {
  Node *temp = new Node;
  temp->left = temp->right = nullptr;
  temp->value = value;
  temp->rightThread = false; // Initialize rightThread to false
  temp->leftThread = false;
  return temp;
}

void buildChildren(Node *parent) {
  if (parent == nullptr) {
    return;
  }

  int leftValue;
  cout << "Enter the left child value for " << parent->value << " (-1 for no child): ";
  cin >> leftValue;
  if (leftValue != -1) {
    parent->left = createNewNode(leftValue);
    buildChildren(parent->left);
  }

  int rightValue;
  cout << "Enter the right child value for " << parent->value << " (-1 for no child): ";
  cin >> rightValue;
  if (rightValue != -1) {
    parent->right = createNewNode(rightValue);
    buildChildren(parent->right);
  }
}

// Function to build the tree level by level based on user input
Node *buildTree() {
  int rootValue;
  cout << "Enter the value for the root node (-1 for no node): ";
  cin >> rootValue;
  if (rootValue == -1) {
    return nullptr;
  }
  Node *root = createNewNode(rootValue);
  buildChildren(root);
  return root;
}

int main() {
  Node *root = buildTree();
  convert(root);
  cout << "InOrder Traversal of Created Threaded Binary Tree is\n";
  inorder(root);
  cout << endl;
  return 0;
}

// 10, 5, 15, 2, -1, -1, 20, -1, -1, -1, -1