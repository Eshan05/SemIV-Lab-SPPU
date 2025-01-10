// 6. Beginning with an empty binary search tree, Construct binary search tree by inserting the values in the order given. After constructing a binary tree
//    1. Insert new node
//    2. Find number of nodes in longest path from root
//    3. Minimum data value found in the tree
//    4. Change a tree so that the roles of the left and right pointers are swapped at every node
//    5. Search a value

#include <iostream>
#include <string>
using namespace std;

struct Node {
  int data;
  Node *left;
  Node *right;
};

void clearLine() {
  char c;
  while (cin.get(c)) {
    if (c == '\n') break;
  }
}

int readInt(const string &prompt) {
  int x;
  while (true) {
    cout << prompt;
    if (cin >> x) return x;
    cout << "Invalid number. Try again.\n";
    cin.clear();
    clearLine();
  }
}

Node *createNode(int x) {
  Node *n = new Node;
  n->data = x;
  n->left = 0;
  n->right = 0;
  return n;
}

Node *insertNode(Node *root, int x) {
  if (root == 0) return createNode(x);

  Node *cur = root;
  Node *parent = 0;
  while (cur != 0) {
    parent = cur;
    if (x < cur->data) cur = cur->left;
    else if (x > cur->data) cur = cur->right;
    else return root;
  }

  if (x < parent->data) parent->left = createNode(x);
  else parent->right = createNode(x);
  return root;
}

int longestPathNodes(Node *root) {
  if (root == 0) return 0;
  int l = longestPathNodes(root->left);
  int r = longestPathNodes(root->right);
  return (l > r ? l : r) + 1;
}

int minValueTraversal(Node *root) {
  if (root == 0) return 0;
  int m = root->data;
  if (root->left != 0) {
    int ml = minValueTraversal(root->left);
    if (ml < m) m = ml;
  }
  if (root->right != 0) {
    int mr = minValueTraversal(root->right);
    if (mr < m) m = mr;
  }
  return m;
}

void mirrorTree(Node *root) {
  if (root == 0) return;
  Node *t = root->left;
  root->left = root->right;
  root->right = t;
  mirrorTree(root->left);
  mirrorTree(root->right);
}

bool searchValue(Node *root, int x, int &comparisons) {
  comparisons = 0;
  Node *cur = root;
  while (cur != 0) {
    ++comparisons;
    if (cur->data == x) return true;
    if (x < cur->data) cur = cur->left;
    else cur = cur->right;
  }
  return false;
}

void inorder(Node *root) {
  if (root == 0) return;
  inorder(root->left);
  cout << root->data << " ";
  inorder(root->right);
}

void preorder(Node *root) {
  if (root == 0) return;
  cout << root->data << " ";
  preorder(root->left);
  preorder(root->right);
}

void deleteTree(Node *root) {
  if (root == 0) return;
  deleteTree(root->left);
  deleteTree(root->right);
  delete root;
}

int main() {
  Node *root = 0;
  int n = readInt("Enter number of initial nodes: ");
  if (n < 0) n = 0;

  for (int i = 0; i < n; ++i) {
    int x = readInt("Enter value " + to_string(i + 1) + ": ");
    root = insertNode(root, x);
  }

  while (true) {
    cout << "\n1. Insert new node\n";
    cout << "2. Number of nodes in longest path from root\n";
    cout << "3. Minimum data value found in the tree\n";
    cout << "4. Mirror tree (swap left and right pointers)\n";
    cout << "5. Search a value\n";
    cout << "6. Display inorder\n";
    cout << "7. Display preorder\n";
    cout << "8. Exit\n";

    int choice = readInt("Enter choice: ");

    if (choice == 1) {
      int x = readInt("Enter value to insert: ");
      root = insertNode(root, x);
      cout << "Inserted (duplicates ignored).\n";

    } else if (choice == 2) {
      cout << "Nodes in longest path: " << longestPathNodes(root) << "\n";

    } else if (choice == 3) {
      if (root == 0) cout << "Tree is empty\n";
      else cout << "Minimum value: " << minValueTraversal(root) << "\n";

    } else if (choice == 4) {
      mirrorTree(root);
      cout << "Tree mirrored\n";

    } else if (choice == 5) {
      int x = readInt("Enter value to search: ");
      int c = 0;
      if (searchValue(root, x, c)) cout << "Found, comparisons: " << c << "\n";
      else cout << "Not found, comparisons: " << c << "\n";

    } else if (choice == 6) {
      cout << "Inorder: ";
      inorder(root);
      cout << "\n";

    } else if (choice == 7) {
      cout << "Preorder: ";
      preorder(root);
      cout << "\n";

    } else if (choice == 8) {
      break;

    } else {
      cout << "Invalid choice\n";
    }
  }

  deleteTree(root);
  return 0;
}
