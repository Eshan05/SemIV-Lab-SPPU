// 10. Consider threading a binary tree using preorder threads rather than inorder threads. Design an algorithm for traversal without using stack and analyze its complexity

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

void preorderCollect(Node *root, Node **arr, int &n) {
  if (root == 0) return;
  arr[n++] = root;
  if (root->lthread == 0) preorderCollect(root->left, arr, n);
  if (root->rthread == 0) preorderCollect(root->right, arr, n);
}

void createPreorderThreads(Node *root) {
  if (root == 0) return;

  Node *arr[1200];
  int n = 0;
  preorderCollect(root, arr, n);

  for (int i = 0; i < n; ++i) {
    Node *cur = arr[i];
    Node *nxt = (i + 1 < n) ? arr[i + 1] : 0;

    if (cur->left == 0) {
      cur->lthread = 1;
      cur->left = 0;
    }
    if (cur->right == 0) {
      cur->rthread = 1;
      cur->right = nxt;
    }
  }
}

Node *preorderSuccessor(Node *cur) {
  if (cur == 0) return 0;
  if (cur->lthread == 0 && cur->left != 0) return cur->left;
  if (cur->rthread == 0 && cur->right != 0) return cur->right;
  return cur->right;
}

void preorderThreadedTraversal(Node *root) {
  Node *cur = root;
  while (cur != 0) {
    cout << cur->data << " ";
    cur = preorderSuccessor(cur);
  }
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

void printSideways(Node *root, int depth) {
  if (root == 0) return;
  if (root->rthread == 0) printSideways(root->right, depth + 1);
  for (int i = 0; i < depth; ++i) cout << "    ";
  cout << root->data << "\n";
  if (root->lthread == 0) printSideways(root->left, depth + 1);
}

void showThreadTable(Node *root) {
  Node *arr[1200];
  int n = 0;
  preorderCollect(root, arr, n);
  cout << "Node | Left Link | Right Link\n";
  for (int i = 0; i < n; ++i) {
    Node *cur = arr[i];
    cout << cur->data << " | ";
    if (cur->lthread == 1) cout << "Thread->NULL";
    else if (cur->left != 0) cout << "Child->" << cur->left->data;
    else cout << "Child->NULL";

    cout << " | ";
    if (cur->rthread == 1) {
      if (cur->right != 0) cout << "Thread->" << cur->right->data;
      else cout << "Thread->NULL";
    } else {
      if (cur->right != 0) cout << "Child->" << cur->right->data;
      else cout << "Child->NULL";
    }
    cout << "\n";
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
    cout << "\n===== Preorder Threaded Binary Tree TUI =====\n";
    cout << "Note: Nodes are inserted using BST rule.\n";
    cout << "1. Insert a node\n";
    cout << "2. Bulk insert N nodes\n";
    cout << "3. Display current binary tree traversals\n";
    cout << "4. Display current tree shape\n";
    cout << "5. Convert to preorder threaded tree\n";
    cout << "6. Preorder traversal without stack (threads)\n";
    cout << "7. Show thread link table\n";
    cout << "8. Reset tree\n";
    cout << "9. Exit\n";

    int ch = readInt("Enter choice: ");

    if (ch == 1) {
      if (threaded) {
        cout << "Tree is already threaded. Reset tree before inserting.\n";
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
      }

    } else if (ch == 3) {
      if (root == 0) cout << "Tree empty\n";
      else {
        cout << "Inorder: ";
        inorderNormal(root);
        cout << "\nPreorder: ";
        preorderNormal(root);
        cout << "\n";
      }

    } else if (ch == 4) {
      if (root == 0) cout << "Tree empty\n";
      else {
        cout << "Tree shape (rotated left):\n";
        printSideways(root, 0);
      }

    } else if (ch == 5) {
      if (root == 0) cout << "Tree empty\n";
      else if (threaded) cout << "Already threaded\n";
      else {
        createPreorderThreads(root);
        threaded = 1;
        cout << "Converted to preorder threaded tree\n";
      }

    } else if (ch == 6) {
      if (!threaded) cout << "Tree is not threaded yet. Use option 5 first.\n";
      else {
        cout << "Preorder threaded traversal: ";
        preorderThreadedTraversal(root);
        cout << "\n";
      }

    } else if (ch == 7) {
      if (!threaded) cout << "Tree is not threaded yet. Use option 5 first.\n";
      else showThreadTable(root);

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
