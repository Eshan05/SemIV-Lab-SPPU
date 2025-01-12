// 10. TBT <-> BST conversion
// Demo: start with a normal BST, convert to inorder-threaded, traverse without stack,
// then convert back to normal BST.

#include <iostream>
using namespace std;

struct Node {
  int key;
  Node *left;
  Node *right;
  bool lthread;
  bool rthread;

  Node(int k) : key(k), left(NULL), right(NULL), lthread(false), rthread(false) {}
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

static Node *insertBST(Node *root, int key, bool &inserted) {
  if (!root) {
    cout << "Reached NULL -> insert " << key << " here.\n";
    inserted = true;
    return new Node(key);
  }
  cout << "Compare " << key << " with " << root->key << ": ";
  if (key < root->key) {
    cout << "go left\n";
    root->left = insertBST(root->left, key, inserted);
  } else if (key > root->key) {
    cout << "go right\n";
    root->right = insertBST(root->right, key, inserted);
  } else {
    cout << "duplicate, no insert\n";
  }
  return root;
}

static void inorderBST(Node *root) {
  if (!root) return;
  inorderBST(root->left);
  cout << root->key << " ";
  inorderBST(root->right);
}

static void freeBST(Node *root) {
  if (!root) return;
  // In threaded mode, threads must be removed before using this.
  freeBST(root->left);
  freeBST(root->right);
  delete root;
}

static void threadInorder(Node *root, Node *&prev) {
  if (!root) return;
  threadInorder(root->left, prev);
  if (!root->left) {
    root->lthread = true;
    root->left = prev;
    cout << "Node " << root->key << ": left NULL -> thread to predecessor "
         << (prev ? to_string(prev->key) : string("NULL")) << "\n";
  }
  if (prev && !prev->right) {
    prev->rthread = true;
    prev->right = root;
    cout << "Node " << prev->key << ": right NULL -> thread to successor "
         << root->key << "\n";
  }
  prev = root;
  threadInorder(root->right, prev);
}

static void convertBSTtoTBT(Node *root) {
  // Clear any old thread flags first
  // (In BST mode they should already be false.)
  Node *prev = NULL;
  threadInorder(root, prev);
}

static Node *inorderSuccessor(Node *p) {
  if (!p) return NULL;
  if (p->rthread) return p->right;
  p = p->right;
  while (p && !p->lthread) p = p->left;
  return p;
}

static void inorderTBT(Node *root) {
  if (!root) {
    cout << "(empty)\n";
    return;
  }
  Node *cur = root;
  while (cur && !cur->lthread) cur = cur->left;
  while (cur) {
    cout << cur->key << " ";
    cur = inorderSuccessor(cur);
  }
  cout << "\n";
}

static void collectInorder(Node *root, Node **out, int &n, int cap) {
  n = 0;
  if (!root) return;
  Node *cur = root;
  while (cur && !cur->lthread) cur = cur->left;
  while (cur && n < cap) {
    out[n++] = cur;
    cur = inorderSuccessor(cur);
  }
}

static void convertTBTtoBST(Node *root) {
  // Break threads safely: first collect nodes via threaded traversal.
  Node *nodes[1024];
  int n = 0;
  collectInorder(root, nodes, n, 1024);
  for (int i = 0; i < n; i++) {
    Node *x = nodes[i];
    if (x->lthread) {
      cout << "Node " << x->key << ": remove left predecessor thread\n";
      x->left = NULL;
      x->lthread = false;
    }
    if (x->rthread) {
      cout << "Node " << x->key << ": remove right successor thread\n";
      x->right = NULL;
      x->rthread = false;
    }
  }
}

int main() {
  Node *root = NULL;
  bool isThreaded = false;
  int choice = 0;

  while (choice != 7) {
    cout << "\n=== T12: BST <-> Threaded BT (Inorder) Conversion ===\n";
    cout << "Mode: " << (isThreaded ? "THREADED" : "BST") << "\n";
    cout << "1. Insert into BST\n";
    cout << "2. Display inorder\n";
    cout << "3. Convert BST -> Threaded\n";
    cout << "4. Display threaded inorder (no stack)\n";
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
      bool inserted = false;
      root = insertBST(root, k, inserted);
      cout << (inserted ? "Inserted (BST).\n" : "Duplicate ignored.\n");
    } else if (choice == 2) {
      if (isThreaded) {
        cout << "Inorder(threaded): ";
        inorderTBT(root);
      } else {
        cout << "Inorder(BST): ";
        inorderBST(root);
        cout << "\n";
      }
    } else if (choice == 3) {
      if (isThreaded) {
        cout << "Already threaded.\n";
        continue;
      }
      if (!root) {
        cout << "Tree empty; nothing to thread.\n";
        continue;
      }
      convertBSTtoTBT(root);
      isThreaded = true;
      cout << "Converted to inorder-threaded form.\n";
    } else if (choice == 4) {
      if (!isThreaded) {
        cout << "Not threaded yet. Use option 3.\n";
        continue;
      }
      cout << "Inorder(threaded): ";
      inorderTBT(root);
    } else if (choice == 5) {
      if (!isThreaded) {
        cout << "Already BST.\n";
        continue;
      }
      convertTBTtoBST(root);
      isThreaded = false;
      cout << "Converted back to BST (threads removed).\n";
    } else if (choice == 6) {
      if (isThreaded) {
        convertTBTtoBST(root);
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

  if (isThreaded) convertTBTtoBST(root);
  freeBST(root);
  return 0;
}
