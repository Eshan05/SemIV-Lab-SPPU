// 9. Threaded Binary Tree (Inorder threading)
// Demo: Inorder-threaded BST (with lthread/rthread flags).
// Supports: insertion, deletion, searching, inorder traversal without stack.

#include <iostream>
#include <string>
using namespace std;

struct Node {
  int key;
  Node *left;
  Node *right;
  bool lthread;
  bool rthread;

  Node(int k) : key(k), left(NULL), right(NULL), lthread(true), rthread(true) {}
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

static Node *inorderSuccessor(Node *p) {
  if (!p) return NULL;
  if (p->rthread) return p->right;
  p = p->right;
  while (p && !p->lthread) p = p->left;
  return p;
}

static Node *inorderPredecessor(Node *p) {
  if (!p) return NULL;
  if (p->lthread) return p->left;
  p = p->left;
  while (p && !p->rthread) p = p->right;
  return p;
}

static void inorderTraversal(Node *root) {
  if (!root) {
    cout << "(empty)\n";
    return;
  }
  // leftmost
  Node *cur = root;
  while (!cur->lthread) cur = cur->left;
  while (cur) {
    cout << cur->key << " ";
    cur = inorderSuccessor(cur);
  }
  cout << "\n";
}

static bool search(Node *root, int key, int &comps) {
  comps = 0;
  Node *cur = root;
  while (cur) {
    comps++;
    cout << "Compare " << key << " with " << cur->key << ": ";
    if (key == cur->key) return true;
    if (key < cur->key) {
      if (cur->lthread) {
        cout << "left is a thread, stop\n";
        break;
      }
      cout << "go left child\n";
      cur = cur->left;
    } else {
      if (cur->rthread) {
        cout << "right is a thread, stop\n";
        break;
      }
      cout << "go right child\n";
      cur = cur->right;
    }
  }
  return false;
}

static bool insert(Node *&root, int key) {
  cout << "\nInsert " << key << " into threaded BST (inorder threads)\n";
  Node *parent = NULL;
  Node *cur = root;
  while (cur) {
    if (key == cur->key) {
      cout << "Duplicate found at node " << cur->key << " -> insertion cancelled.\n";
      return false;
    }
    parent = cur;
    if (key < cur->key) {
      cout << "At node " << cur->key << ": " << key << " < " << cur->key << " -> ";
      if (cur->lthread) {
        cout << "LEFT is a THREAD, stop here (this is the insertion point).\n";
        break;
      }
      cout << "go LEFT child " << (cur->left ? cur->left->key : -1) << "\n";
      cur = cur->left;
    } else {
      cout << "At node " << cur->key << ": " << key << " > " << cur->key << " -> ";
      if (cur->rthread) {
        cout << "RIGHT is a THREAD, stop here (this is the insertion point).\n";
        break;
      }
      cout << "go RIGHT child " << (cur->right ? cur->right->key : -1) << "\n";
      cur = cur->right;
    }
  }

  Node *nn = new Node(key);
  if (!parent) {
    root = nn;
    nn->left = NULL;
    nn->right = NULL;
    cout << "Tree was empty -> new node becomes ROOT. (Both thread pointers = NULL)\n";
    return true;
  }

  if (key < parent->key) {
    cout << "Attach as LEFT child of " << parent->key << "\n";
    cout << "Before: parent->lthread=T and parent->left(thread) points to predecessor "
         << (parent->left ? parent->left->key : -1) << "\n";
    nn->left = parent->left; // Predecessor
    nn->right = parent;      // Successor
    parent->lthread = false;
    parent->left = nn;
    cout << "After: parent->lthread=C and parent->left(child)=" << nn->key << "\n";
    cout << "New node threads: left(pred)=" << (nn->left ? nn->left->key : -1)
         << ", right(succ)=" << (nn->right ? nn->right->key : -1) << "\n";
  } else {
    cout << "Attach as RIGHT child of " << parent->key << "\n";
    cout << "Before: parent->rthread=T and parent->right(thread) points to successor "
         << (parent->right ? parent->right->key : -1) << "\n";
    nn->left = parent;         // Predecessor
    nn->right = parent->right; // Successor
    parent->rthread = false;
    parent->right = nn;
    cout << "After: parent->rthread=C and parent->right(child)=" << nn->key << "\n";
    cout << "New node threads: left(pred)=" << (nn->left ? nn->left->key : -1)
         << ", right(succ)=" << (nn->right ? nn->right->key : -1) << "\n";
  }
  return true;
}

static bool erase(Node *&root, int key) {
  Node *parent = NULL;
  Node *cur = root;
  bool found = false;
  while (cur) {
    if (key == cur->key) {
      found = true;
      break;
    }
    parent = cur;
    if (key < cur->key) {
      if (cur->lthread) break;
      cur = cur->left;
    } else {
      if (cur->rthread) break;
      cur = cur->right;
    }
  }
  if (!found || !cur) return false;

  // If node has two children: Replace with inorder successor and delete successor.
  if (!cur->lthread && !cur->rthread) {
    Node *ps = cur;
    Node *s = cur->right;
    while (!s->lthread) {
      ps = s;
      s = s->left;
    }
    cur->key = s->key;
    parent = ps;
    cur = s;
  }

  bool hasLeft = !cur->lthread;
  bool hasRight = !cur->rthread;

  // Case: Leaf
  if (!hasLeft && !hasRight) {
    if (!parent) {
      delete root;
      root = NULL;
      return true;
    }
    if (parent->left == cur) {
      parent->lthread = true;
      parent->left = cur->left; // Predecessor thread
    } else {
      parent->rthread = true;
      parent->right = cur->right; // Successor thread
    }
    delete cur;
    return true;
  }

  // Case: one child
  Node *child = hasLeft ? cur->left : cur->right;

  if (hasLeft && !hasRight) {
    // Fix threads: Rightmost of left subtree should thread to cur->right (Successor)
    Node *pred = child;
    while (!pred->rthread) pred = pred->right;
    pred->right = cur->right;
    if (cur->right) {
      Node *succ = cur->right;
      if (succ->lthread) succ->left = pred;
    }
  } else if (!hasLeft && hasRight) {
    // Fix threads: Leftmost of right subtree should thread to cur->left (Predecessor)
    Node *succ = child;
    while (!succ->lthread) succ = succ->left;
    succ->left = cur->left;
    if (cur->left) {
      Node *pred = cur->left;
      if (pred->rthread) pred->right = succ;
    }
  }

  if (!parent) {
    root = child;
  } else if (parent->left == cur) {
    parent->left = child;
    parent->lthread = false;
  } else {
    parent->right = child;
    parent->rthread = false;
  }

  delete cur;
  return true;
}

static void dumpInorderWithThreads(Node *root) {
  if (!root) {
    cout << "(empty)\n";
    return;
  }
  Node *cur = root;
  while (!cur->lthread) cur = cur->left;
  cout << "\nKey  [Lflag->ptr]  [Rflag->ptr]\n";
  while (cur) {
    Node *pred = inorderPredecessor(cur);
    Node *succ = inorderSuccessor(cur);
    cout << cur->key << "  ";
    cout << "L(" << (cur->lthread ? 'T' : 'C') << ")->";
    cout << (cur->left ? cur->left->key : -1) << "  ";
    cout << "R(" << (cur->rthread ? 'T' : 'C') << ")->";
    cout << (cur->right ? cur->right->key : -1);
    cout << "  pred=" << (pred ? pred->key : -1);
    cout << " succ=" << (succ ? succ->key : -1);
    cout << "\n";
    cur = succ;
  }
}

static void freeAll(Node *&root) {
  // Delete by repeatedly erasing inorder nodes
  Node *cur = root;
  if (!cur) return;
  while (!cur->lthread) cur = cur->left;
  while (cur) {
    Node *nxt = inorderSuccessor(cur);
    erase(root, cur->key);
    cur = nxt;
  }
}

int main() {
  Node *root = NULL;
  int choice = 0;
  while (choice != 7) {
    cout << "\n=== T11: Threaded BST (Inorder) ===\n";
    cout << "1. Insert\n";
    cout << "2. Search\n";
    cout << "3. Delete\n";
    cout << "4. Inorder traversal (no stack)\n";
    cout << "5. Dump inorder with thread flags\n";
    cout << "6. Clear\n";
    cout << "7. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      int k = readInt("Key: ");
      if (insert(root, k)) {
        cout << "Inserted.\n";
      } else {
        cout << "Duplicate.\n";
      }
    } else if (choice == 2) {
      int k = readInt("Search key: ");
      int comps = 0;
      bool ok = search(root, k, comps);
      cout << (ok ? "Found" : "Not found") << ", comparisons=" << comps << "\n";
    } else if (choice == 3) {
      int k = readInt("Delete key: ");
      if (erase(root, k)) cout << "Deleted.\n";
      else cout << "Key not found.\n";
    } else if (choice == 4) {
      cout << "Inorder traversal without stack: go to leftmost, then repeatedly follow inorderSuccessor()\n";
      cout << "Inorder: ";
      inorderTraversal(root);
    } else if (choice == 5) {
      dumpInorderWithThreads(root);
    } else if (choice == 6) {
      freeAll(root);
      root = NULL;
      cout << "Cleared.\n";
    } else if (choice == 7) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  freeAll(root);
  return 0;
}
