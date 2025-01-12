// 28. Threaded Binary Tree (Preorder threading)
// Demo: start with a normal BST, convert to preorder-threaded, traverse without stack,
// then convert back to normal BST.

#include <iostream>
#include <string>
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

static bool insertBST(Node *&root, int key) {
  if (!root) {
    root = new Node(key);
    return true;
  }
  Node *cur = root;
  while (cur) {
    if (key < cur->key) {
      if (!cur->left) {
        cur->left = new Node(key);
        return true;
      }
      cur = cur->left;
    } else if (key > cur->key) {
      if (!cur->right) {
        cur->right = new Node(key);
        return true;
      }
      cur = cur->right;
    } else {
      return false;
    }
  }
  return false;
}

static void preorderBST(Node *root) {
  if (!root) return;
  cout << root->key << " ";
  preorderBST(root->left);
  preorderBST(root->right);
}

static void freeBST(Node *root) {
  if (!root) return;
  // In threaded mode, threads must be removed before using this.
  freeBST(root->left);
  freeBST(root->right);
  delete root;
}

static void threadPreorder(Node *root, Node *&prev) {
  if (!root) return;

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

  // Preorder: left, then right (only follow child links)
  if (!root->lthread) threadPreorder(root->left, prev);
  if (!root->rthread) threadPreorder(root->right, prev);
}

static void convertBSTtoTBT(Node *root) {
  Node *prev = NULL;
  threadPreorder(root, prev);
}

static void preorderTBT(Node *root) {
  if (!root) {
    cout << "(empty)\n";
    return;
  }
  Node *cur = root;
  while (cur) {
    cout << cur->key << " ";
    if (!cur->lthread) {
      cur = cur->left;
    } else {
      cur = cur->right;
    }
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
    cout << "\n=== T32: BST <-> Threaded BT (Preorder) Conversion ===\n";
    cout << "Mode: " << (isThreaded ? "THREADED" : "BST") << "\n";
    cout << "1. Insert into BST\n";
    cout << "2. Display preorder\n";
    cout << "3. Convert BST -> Threaded\n";
    cout << "4. Display threaded preorder (no stack)\n";
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
        cout << "Preorder(threaded): ";
        preorderTBT(root);
      } else {
        cout << "Preorder(BST): ";
        preorderBST(root);
        cout << "\n";
      }
    } else if (choice == 3) {
      if (isThreaded) {
        cout << "Already threaded.\n";
        continue;
      }
      convertBSTtoTBT(root);
      isThreaded = true;
      cout << "Converted to preorder-threaded form.\n";
    } else if (choice == 4) {
      if (!isThreaded) {
        cout << "Not threaded yet. Use option 3.\n";
        continue;
      }
      cout << "Preorder(threaded): ";
      preorderTBT(root);
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
