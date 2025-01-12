// 29. Threaded Binary Tree (Postorder threading)
// Demo: start with a normal BST, convert to postorder-threaded, traverse without stack,
// then convert back to normal BST.
//
// Note: postorder successor is not as direct as inorder/preorder; this demo keeps a
// parent pointer to make traversal reliable without using an external stack.

#include <iostream>
#include <string>
using namespace std;

struct Node {
  int key;
  Node *left;
  Node *right;
  Node *parent;
  bool lthread;
  bool rthread;

  Node(int k)
      : key(k), left(NULL), right(NULL), parent(NULL), lthread(false), rthread(false) {}
};

static void clearInput() {
  cin.clear();
  cin.ignore(1024, '\n');
}

static int readInt(const string &prompt) {
  int x;
  while (true) {
    cout << prompt;
    if (cin >> x) return x;
    cout << "Invalid number.\n";
    clearInput();
  }
}

static bool insertBST(Node *&root, int key) {
  if (!root) {
    root = new Node(key);
    return true;
  }

  Node *cur = root;
  Node *par = NULL;
  while (cur) {
    par = cur;
    if (key < cur->key) {
      cur = cur->left;
    } else if (key > cur->key) {
      cur = cur->right;
    } else {
      return false;
    }
  }

  Node *nn = new Node(key);
  nn->parent = par;
  if (key < par->key) par->left = nn;
  else par->right = nn;
  return true;
}

static void postorderBST(Node *root) {
  if (!root) return;
  postorderBST(root->left);
  postorderBST(root->right);
  cout << root->key << " ";
}

static void freeBST(Node *root) {
  if (!root) return;
  // In threaded mode, threads must be removed before using this.
  freeBST(root->left);
  freeBST(root->right);
  delete root;
}

static void threadPostorder(Node *root, Node *&prev) {
  if (!root) return;

  // Postorder: left, right, visit (only follow child links)
  if (!root->lthread) threadPostorder(root->left, prev);
  if (!root->rthread) threadPostorder(root->right, prev);

  // Visit
  if (!root->left) {
    root->lthread = true;
    root->left = prev;
  }
  if (prev && !prev->right) {
    prev->rthread = true;
    prev->right = root;
  }
  prev = root;
}

static void convertBSTtoTBT(Node *root) {
  Node *prev = NULL;
  threadPostorder(root, prev);
}

static Node *firstPostorder(Node *root) {
  if (!root) return NULL;
  Node *cur = root;
  while (true) {
    if (cur && !cur->lthread && cur->left) {
      cur = cur->left;
      continue;
    }
    if (cur && !cur->rthread && cur->right) {
      cur = cur->right;
      continue;
    }
    break;
  }
  return cur;
}

static Node *postorderSuccessor(Node *p) {
  if (!p) return NULL;
  if (p->rthread) return p->right;

  Node *par = p->parent;
  if (!par) return NULL;

  // If p is right child OR parent has no right child, next is parent.
  if (par->rthread || par->right == p) return par;

  // Otherwise p was left child and parent's right subtree is unvisited.
  return firstPostorder(par->right);
}

static void postorderTBT(Node *root) {
  if (!root) {
    cout << "(empty)\n";
    return;
  }
  Node *cur = firstPostorder(root);
  while (cur) {
    cout << cur->key << " ";
    cur = postorderSuccessor(cur);
  }
  cout << "\n";
}

static void removeThreads(Node *root) {
  if (!root) return;
  if (!root->lthread) removeThreads(root->left);
  if (!root->rthread) removeThreads(root->right);

  if (root->lthread) {
    root->left = NULL;
    root->lthread = false;
  }
  if (root->rthread) {
    root->right = NULL;
    root->rthread = false;
  }
}

int main() {
  Node *root = NULL;
  bool isThreaded = false;
  int choice = 0;

  while (choice != 7) {
    cout << "\n=== T33: BST <-> Threaded BT (Postorder) Conversion ===\n";
    cout << "Mode: " << (isThreaded ? "THREADED" : "BST") << "\n";
    cout << "1. Insert into BST\n";
    cout << "2. Display postorder\n";
    cout << "3. Convert BST -> Threaded\n";
    cout << "4. Display threaded postorder (no stack)\n";
    cout << "5. Convert Threaded -> BST\n";
    cout << "6. Clear\n";
    cout << "7. Exit\n";
    cout << "Select: ";

    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      if (isThreaded) {
        cout << "Currently threaded. Convert back to BST first.\n";
        continue;
      }
      int k = readInt("Key: ");
      bool ok = insertBST(root, k);
      cout << (ok ? "Inserted (BST).\n" : "Duplicate, ignored.\n");
    } else if (choice == 2) {
      if (isThreaded) {
        cout << "Postorder(threaded): ";
        postorderTBT(root);
      } else {
        cout << "Postorder(BST): ";
        postorderBST(root);
        cout << "\n";
      }
    } else if (choice == 3) {
      if (isThreaded) {
        cout << "Already threaded.\n";
        continue;
      }
      convertBSTtoTBT(root);
      isThreaded = true;
      cout << "Converted to postorder-threaded form.\n";
    } else if (choice == 4) {
      if (!isThreaded) {
        cout << "Not threaded yet. Use option 3.\n";
        continue;
      }
      cout << "Postorder(threaded): ";
      postorderTBT(root);
    } else if (choice == 5) {
      if (!isThreaded) {
        cout << "Already BST.\n";
        continue;
      }
      removeThreads(root);
      isThreaded = false;
      cout << "Converted back to BST (threads removed).\n";
    } else if (choice == 6) {
      if (isThreaded) {
        removeThreads(root);
        isThreaded = false;
      }
      freeBST(root);
      root = NULL;
      cout << "Cleared.\n";
    } else if (choice == 7) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }

  if (isThreaded) removeThreads(root);
  freeBST(root);
  return 0;
}
