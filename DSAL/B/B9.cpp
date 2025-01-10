// 9. Convert given binary tree into threaded binary tree. Analyze time and space complexity of the algorithm.

#include <iostream>
#include <string>
using namespace std;

struct Node {
  int data;
  Node *left;
  Node *right;
  int lthread;
  int rthread;
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
  n->lthread = 0;
  n->rthread = 0;
  return n;
}

Node *insertBST(Node *root, int x) {
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

void inorderNormal(Node *root) {
  if (root == 0) return;
  if (root->lthread == 0) inorderNormal(root->left);
  cout << root->data << " ";
  if (root->rthread == 0) inorderNormal(root->right);
}

void preorderNormal(Node *root) {
  if (root == 0) return;
  cout << root->data << " ";
  if (root->lthread == 0) preorderNormal(root->left);
  if (root->rthread == 0) preorderNormal(root->right);
}

void postorderNormal(Node *root) {
  if (root == 0) return;
  if (root->lthread == 0) postorderNormal(root->left);
  if (root->rthread == 0) postorderNormal(root->right);
  cout << root->data << " ";
}

void printSideways(Node *root, int depth) {
  if (root == 0) return;
  if (root->rthread == 0) printSideways(root->right, depth + 1);
  for (int i = 0; i < depth; ++i) cout << "    ";
  cout << root->data << "\n";
  if (root->lthread == 0) printSideways(root->left, depth + 1);
}

void threadInorder(Node *root, Node *&prev) {
  if (root == 0) return;
  threadInorder(root->left, prev);

  if (root->left == 0) {
    root->left = prev;
    root->lthread = 1;
  }
  if (prev != 0 && prev->right == 0) {
    prev->right = root;
    prev->rthread = 1;
  }
  prev = root;

  threadInorder(root->right, prev);
}

Node *leftMost(Node *root) {
  if (root == 0) return 0;
  Node *cur = root;
  while (cur->lthread == 0 && cur->left != 0) cur = cur->left;
  return cur;
}

void inorderThreaded(Node *root) {
  Node *cur = leftMost(root);
  while (cur != 0) {
    cout << cur->data << " ";
    if (cur->rthread == 1) cur = cur->right;
    else cur = leftMost(cur->right);
  }
}

void showThreadTable(Node *root) {
  cout << "Node | Left Link | Right Link\n";
  Node *cur = leftMost(root);
  while (cur != 0) {
    cout << cur->data << " | ";

    if (cur->lthread == 1) {
      if (cur->left != 0) cout << "Thread->" << cur->left->data;
      else cout << "Thread->NULL";
    } else {
      if (cur->left != 0) cout << "Child->" << cur->left->data;
      else cout << "Child->NULL";
    }

    cout << " | ";
    if (cur->rthread == 1) {
      if (cur->right != 0) cout << "Thread->" << cur->right->data;
      else cout << "Thread->NULL";
    } else {
      if (cur->right != 0) cout << "Child->" << cur->right->data;
      else cout << "Child->NULL";
    }
    cout << "\n";

    if (cur->rthread == 1) cur = cur->right;
    else cur = leftMost(cur->right);
  }
}

void deleteTree(Node *root) {
  if (root == 0) return;
  if (root->lthread == 0) deleteTree(root->left);
  if (root->rthread == 0) deleteTree(root->right);
  delete root;
}

int main() {
  Node *root = 0;
  int threaded = 0;

  while (true) {
    cout << "\n===== Inorder Threaded Binary Tree TUI =====\n";
    cout << "Note: Nodes are inserted using BST rule.\n";
    cout << "1. Insert a node\n";
    cout << "2. Bulk insert N nodes\n";
    cout << "3. Display current binary tree traversals\n";
    cout << "4. Display current tree shape\n";
    cout << "5. Convert to inorder threaded binary tree\n";
    cout << "6. Inorder traversal using threads\n";
    cout << "7. Show thread link table\n";
    cout << "8. Reset tree\n";
    cout << "9. Exit\n";

    int ch = readInt("Enter choice: ");

    if (ch == 1) {
      if (threaded) {
        cout << "Tree is already threaded. Reset tree before inserting more nodes.\n";
      } else {
        int x = readInt("Enter value: ");
        root = insertBST(root, x);
        cout << "Inserted\n";
      }

    } else if (ch == 2) {
      if (threaded) {
        cout << "Tree is already threaded. Reset tree before bulk insert.\n";
      } else {
        int n = readInt("Enter N: ");
        for (int i = 0; i < n; ++i) {
          int x = readInt("Value " + to_string(i + 1) + ": ");
          root = insertBST(root, x);
        }
        cout << "Bulk insert done\n";
      }

    } else if (ch == 3) {
      if (root == 0) {
        cout << "Tree empty\n";
      } else {
        cout << "Inorder: ";
        inorderNormal(root);
        cout << "\nPreorder: ";
        preorderNormal(root);
        cout << "\nPostorder: ";
        postorderNormal(root);
        cout << "\n";
      }

    } else if (ch == 4) {
      if (root == 0) cout << "Tree empty\n";
      else {
        cout << "Tree shape (rotated left):\n";
        printSideways(root, 0);
      }

    } else if (ch == 5) {
      if (root == 0) {
        cout << "Tree empty\n";
      } else if (threaded) {
        cout << "Already threaded\n";
      } else {
        Node *prev = 0;
        threadInorder(root, prev);
        threaded = 1;
        cout << "Converted to inorder threaded binary tree\n";
      }

    } else if (ch == 6) {
      if (!threaded) {
        cout << "Tree is not threaded yet. Use option 5 first.\n";
      } else {
        cout << "Threaded inorder traversal: ";
        inorderThreaded(root);
        cout << "\n";
      }

    } else if (ch == 7) {
      if (!threaded) {
        cout << "Tree is not threaded yet. Use option 5 first.\n";
      } else {
        showThreadTable(root);
      }

    } else if (ch == 8) {
      deleteTree(root);
      root = 0;
      threaded = 0;
      cout << "Tree reset\n";

    } else if (ch == 9) {
      break;

    } else {
      cout << "Invalid choice\n";
    }
  }

  deleteTree(root);
  return 0;
}
