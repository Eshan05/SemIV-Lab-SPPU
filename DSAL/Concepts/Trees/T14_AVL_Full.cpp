// 12. AVL tree: insertion, deletion, rotations, display, balance factor, height, searching
// Demo: AVL with rotation tracing.

#include <iostream>
using namespace std;

struct Node {
  int key;
  Node *left;
  Node *right;
  int h;
  Node(int k) : key(k), left(NULL), right(NULL), h(1) {}
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

static int height(Node *n) { return n ? n->h : 0; }
static int max2(int a, int b) { return (a > b) ? a : b; }
static int bf(Node *n) { return n ? (height(n->left) - height(n->right)) : 0; }

static Node *rotateRight(Node *y) {
  Node *x = y->left;
  Node *t2 = x->right;
  cout << "  rightRotate(" << y->key << "): " << x->key << " moves up, "
       << y->key << " becomes its right child.\n";
  x->right = y;
  y->left = t2;
  y->h = max2(height(y->left), height(y->right)) + 1;
  x->h = max2(height(x->left), height(x->right)) + 1;
  return x;
}

static Node *rotateLeft(Node *x) {
  Node *y = x->right;
  Node *t2 = y->left;
  cout << "  leftRotate(" << x->key << "): " << y->key << " moves up, "
       << x->key << " becomes its left child.\n";
  y->left = x;
  x->right = t2;
  x->h = max2(height(x->left), height(x->right)) + 1;
  y->h = max2(height(y->left), height(y->right)) + 1;
  return y;
}

static Node *insert(Node *node, int key, bool &inserted) {
  if (!node) {
    cout << "Inserted leaf " << key << "\n";
    inserted = true;
    return new Node(key);
  }
  if (key < node->key) node->left = insert(node->left, key, inserted);
  else if (key > node->key) node->right = insert(node->right, key, inserted);
  else {
    cout << "Duplicate key at " << node->key << "; no insert.\n";
    return node;
  }

  node->h = max2(height(node->left), height(node->right)) + 1;
  int b = bf(node);

  // LL
  if (b > 1 && key < node->left->key) {
    cout << "Rotation: LL at " << node->key << " (Right-rotate)\n";
    return rotateRight(node);
  }
  // RR
  if (b < -1 && key > node->right->key) {
    cout << "Rotation: RR at " << node->key << " (Left-rotate)\n";
    return rotateLeft(node);
  }
  // LR
  if (b > 1 && key > node->left->key) {
    cout << "Rotation: LR at " << node->key << " (Left-rotate left child, then Right-rotate)\n";
    node->left = rotateLeft(node->left);
    return rotateRight(node);
  }
  // RL
  if (b < -1 && key < node->right->key) {
    cout << "Rotation: RL at " << node->key << " (Right-rotate right child, then Left-rotate)\n";
    node->right = rotateRight(node->right);
    return rotateLeft(node);
  }

  return node;
}

static Node *minNode(Node *n) {
  Node *cur = n;
  while (cur && cur->left) cur = cur->left;
  return cur;
}

static Node *erase(Node *root, int key) {
  if (!root) return NULL;
  if (key < root->key) root->left = erase(root->left, key);
  else if (key > root->key) root->right = erase(root->right, key);
  else {
    // node with 0 or 1 child
    if (!root->left || !root->right) {
      Node *child = root->left ? root->left : root->right;
      if (!child) {
        delete root;
        return NULL;
      }
      Node *tmp = child;
      delete root;
      root = tmp;
    } else {
      Node *succ = minNode(root->right);
      root->key = succ->key;
      root->right = erase(root->right, succ->key);
    }
  }

  if (!root) return NULL;

  root->h = max2(height(root->left), height(root->right)) + 1;
  int b = bf(root);

  // LL
  if (b > 1 && bf(root->left) >= 0) {
    cout << "Rebalance: LL at " << root->key << "\n";
    return rotateRight(root);
  }
  // LR
  if (b > 1 && bf(root->left) < 0) {
    cout << "Rebalance: LR at " << root->key << "\n";
    root->left = rotateLeft(root->left);
    return rotateRight(root);
  }
  // RR
  if (b < -1 && bf(root->right) <= 0) {
    cout << "Rebalance: RR at " << root->key << "\n";
    return rotateLeft(root);
  }
  // RL
  if (b < -1 && bf(root->right) > 0) {
    cout << "Rebalance: RL at " << root->key << "\n";
    root->right = rotateRight(root->right);
    return rotateLeft(root);
  }

  return root;
}

static bool search(Node *root, int key, int &comps) {
  comps = 0;
  Node *cur = root;
  while (cur) {
    comps++;
    cout << "Compare " << key << " with " << cur->key << ": ";
    if (key == cur->key) return true;
    if (key < cur->key) {
      cout << "go left\n";
      cur = cur->left;
    } else {
      cout << "go right\n";
      cur = cur->right;
    }
  }
  cout << "Reached NULL.\n";
  return false;
}

static void inorder(Node *root) {
  if (!root) return;
  inorder(root->left);
  cout << root->key << "(h=" << root->h << ",bf=" << bf(root) << ") ";
  inorder(root->right);
}

static void displaySideways(Node *root, int depth) {
  if (!root) return;
  displaySideways(root->right, depth + 1);
  for (int i = 0; i < depth; i++) cout << "    ";
  cout << root->key << "\n";
  displaySideways(root->left, depth + 1);
}

static void freeTree(Node *root) {
  if (!root) return;
  freeTree(root->left);
  freeTree(root->right);
  delete root;
}

int main() {
  Node *root = NULL;
  int choice = 0;
  while (choice != 8) {
    cout << "\n=== T14: AVL Tree (Full) ===\n";
    cout << "1. Insert\n";
    cout << "2. Delete\n";
    cout << "3. Search\n";
    cout << "4. Display inorder (with h,bf)\n";
    cout << "5. Display sideways\n";
    cout << "6. Height of tree\n";
    cout << "7. Clear\n";
    cout << "8. Exit\n";
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
        root = erase(root, k);
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
      cout << "Height(levels)=" << height(root) << "\n";
    } else if (choice == 7) {
      freeTree(root);
      root = NULL;
      cout << "Cleared.\n";
    } else if (choice == 8) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  freeTree(root);
  return 0;
}
