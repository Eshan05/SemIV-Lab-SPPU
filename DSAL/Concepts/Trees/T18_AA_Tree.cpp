// 16. AA Tree
// Demo: AA tree insertion (skew/split) and deletion (rebalancing rules).
// Supports: insert, delete, search, display, invariants, height, #nodes.

#include <iostream>
using namespace std;

struct Node {
  int key;
  int level;
  Node *left;
  Node *right;
};

static Node *NIL = NULL;

static void initNIL() {
  if (NIL) return;
  NIL = new Node();
  NIL->key = 0;
  NIL->level = 0;
  NIL->left = NIL->right = NIL;
}

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

static Node *newNode(int key) {
  Node *n = new Node();
  n->key = key;
  n->level = 1;
  n->left = n->right = NIL;
  return n;
}

static Node *skew(Node *x) {
  if (x == NIL) return x;
  if (x->left->level == x->level) {
    Node *l = x->left;
    cout << "skew at " << x->key << ": right-rotate to remove left horizontal link.\n";
    x->left = l->right;
    l->right = x;
    return l;
  }
  return x;
}

static Node *split(Node *x) {
  if (x == NIL) return x;
  if (x->right->right->level == x->level) {
    Node *r = x->right;
    cout << "split at " << x->key << ": left-rotate and raise level of " << r->key << ".\n";
    x->right = r->left;
    r->left = x;
    r->level++;
    return r;
  }
  return x;
}

static Node *insert(Node *root, int key, bool &inserted) {
  if (root == NIL) {
    cout << "Insert " << key << " at NIL leaf.\n";
    inserted = true;
    return newNode(key);
  }
  cout << "Compare " << key << " with " << root->key << ": ";
  if (key < root->key) {
    cout << "go left\n";
    root->left = insert(root->left, key, inserted);
  } else if (key > root->key) {
    cout << "go right\n";
    root->right = insert(root->right, key, inserted);
  } else {
    cout << "duplicate, stop\n";
    return root;
  }
  root = skew(root);
  root = split(root);
  return root;
}

static int min2(int a, int b) { return (a < b) ? a : b; }

static Node *decreaseLevel(Node *x) {
  if (x == NIL) return x;
  int shouldBe = min2(x->left->level, x->right->level) + 1;
  if (shouldBe < x->level) {
    x->level = shouldBe;
    if (shouldBe < x->right->level) x->right->level = shouldBe;
  }
  return x;
}

static Node *removeRec(Node *root, int key) {
  if (root == NIL) return root;
  if (key < root->key) {
    cout << "Delete " << key << ": at " << root->key << " go left\n";
    root->left = removeRec(root->left, key);
  } else if (key > root->key) {
    cout << "Delete " << key << ": at " << root->key << " go right\n";
    root->right = removeRec(root->right, key);
  } else {
    cout << "Delete found " << root->key << "\n";
    // found
    if (root->left == NIL && root->right == NIL) {
      delete root;
      return NIL;
    }
    if (root->left != NIL) {
      Node *pred = root->left;
      while (pred->right != NIL) pred = pred->right;
      cout << "Use predecessor " << pred->key << "\n";
      root->key = pred->key;
      root->left = removeRec(root->left, pred->key);
    } else {
      Node *succ = root->right;
      while (succ->left != NIL) succ = succ->left;
      cout << "Use successor " << succ->key << "\n";
      root->key = succ->key;
      root->right = removeRec(root->right, succ->key);
    }
  }

  root = decreaseLevel(root);
  root = skew(root);
  root->right = skew(root->right);
  root->right->right = skew(root->right->right);
  root = split(root);
  root->right = split(root->right);
  return root;
}

static bool search(Node *root, int key, int &comps) {
  comps = 0;
  Node *cur = root;
  while (cur != NIL) {
    comps++;
    cout << "Compare " << key << " with " << cur->key << "\n";
    if (key == cur->key) return true;
    if (key < cur->key) cur = cur->left;
    else cur = cur->right;
  }
  return false;
}

static void inorder(Node *root) {
  if (root == NIL) return;
  inorder(root->left);
  cout << root->key << "(L" << root->level << ") ";
  inorder(root->right);
}

static void displaySideways(Node *root, int depth) {
  if (root == NIL) return;
  displaySideways(root->right, depth + 1);
  for (int i = 0; i < depth; i++) cout << "    ";
  cout << root->key << "[" << root->level << "]\n";
  displaySideways(root->left, depth + 1);
}

static int countNodes(Node *root) {
  if (root == NIL) return 0;
  return 1 + countNodes(root->left) + countNodes(root->right);
}

static int height(Node *root) {
  if (root == NIL) return 0;
  int hl = height(root->left);
  int hr = height(root->right);
  return (hl > hr ? hl : hr) + 1;
}

static bool checkAA(Node *root, int minKey, int maxKey) {
  if (root == NIL) return true;
  if (root->key <= minKey || root->key >= maxKey) return false;
  // left level is exactly one less (or NIL)
  if (root->left->level != root->level - 1) return false;
  // right level is equal or one less
  if (!(root->right->level == root->level || root->right->level == root->level - 1)) return false;
  // right-right level must be less
  if (root->right->right->level >= root->level) return false;
  // nodes with level > 1 must have two children
  if (root->level > 1) {
    if (root->left == NIL || root->right == NIL) return false;
  }
  return checkAA(root->left, minKey, root->key) && checkAA(root->right, root->key, maxKey);
}

static void freeTree(Node *root) {
  if (root == NIL) return;
  freeTree(root->left);
  freeTree(root->right);
  delete root;
}

int main() {
  initNIL();
  Node *root = NIL;
  int choice = 0;
  while (choice != 9) {
    cout << "\n=== T18: AA Tree ===\n";
    cout << "1. Insert\n";
    cout << "2. Delete\n";
    cout << "3. Search\n";
    cout << "4. Display inorder (with levels)\n";
    cout << "5. Display sideways\n";
    cout << "6. Height / #Nodes\n";
    cout << "7. Check invariants\n";
    cout << "8. Clear\n";
    cout << "9. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      int k = readInt("Key: ");
      bool inserted = false;
      root = insert(root, k, inserted);
      cout << (inserted ? "Inserted.\n" : "Duplicate ignored.\n");
    } else if (choice == 2) {
      int k = readInt("Delete key: ");
      int comps = 0;
      if (search(root, k, comps)) {
        root = removeRec(root, k);
        cout << "Deleted.\n";
      } else {
        cout << "Not found; no delete performed.\n";
      }
    } else if (choice == 3) {
      int k = readInt("Search key: ");
      int comps = 0;
      bool ok = search(root, k, comps);
      cout << (ok ? "Found" : "Not found") << ", comparisons=" << comps << "\n";
    } else if (choice == 4) {
      cout << "Inorder: ";
      inorder(root);
      cout << "\n";
    } else if (choice == 5) {
      cout << "\nSideways view:\n";
      displaySideways(root, 0);
    } else if (choice == 6) {
      cout << "Height=" << height(root) << ", Nodes=" << countNodes(root) << "\n";
    } else if (choice == 7) {
      bool ok = checkAA(root, -2147483647, 2147483647);
      cout << (ok ? "OK (invariants hold)\n" : "Violation detected\n");
    } else if (choice == 8) {
      freeTree(root);
      root = NIL;
      cout << "Cleared.\n";
    } else if (choice == 9) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  freeTree(root);
  return 0;
}
