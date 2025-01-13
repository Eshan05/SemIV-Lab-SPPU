// 18. Splay Tree
// Demo: Zig (single), Zig-Zig, Zig-Zag operations via splaying.
// Supports: insert, search (splay to root), delete, display.

#include <iostream>
using namespace std;

struct Node {
  int key;
  Node *left;
  Node *right;
  Node(int k) : key(k), left(NULL), right(NULL) {}
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

static Node *rotR(Node *x) {
  Node *y = x->left;
  cout << "  rotateRight(" << x->key << "): " << y->key << " moves up.\n";
  x->left = y->right;
  y->right = x;
  return y;
}

static Node *rotL(Node *x) {
  Node *y = x->right;
  cout << "  rotateLeft(" << x->key << "): " << y->key << " moves up.\n";
  x->right = y->left;
  y->left = x;
  return y;
}

static Node *splay(Node *root, int key) {
  if (!root) {
    cout << "Reached NULL during splay.\n";
    return root;
  }
  cout << "Compare " << key << " with root " << root->key << "\n";
  if (root->key == key) return root;

  if (key < root->key) {
    cout << key << " < " << root->key << " -> left side\n";
    if (!root->left) {
      cout << "No left child; last accessed node stays at root.\n";
      return root;
    }
    cout << "Compare " << key << " with left child " << root->left->key << "\n";

    if (key < root->left->key) {
      // Zig-Zig (LL)
      cout << "Zig-Zig (LL) at " << root->key << "\n";
      root->left->left = splay(root->left->left, key);
      root = rotR(root);
    } else if (key > root->left->key) {
      // Zig-Zag (LR)
      cout << "Zig-Zag (LR) at " << root->key << "\n";
      root->left->right = splay(root->left->right, key);
      if (root->left->right) root->left = rotL(root->left);
    }

    // Zig
    if (!root->left) return root;
    cout << "Zig (Right rotate) at " << root->key << "\n";
    return rotR(root);
  }

  // key > root->key
  cout << key << " > " << root->key << " -> right side\n";
  if (!root->right) {
    cout << "No right child; last accessed node stays at root.\n";
    return root;
  }
  cout << "Compare " << key << " with right child " << root->right->key << "\n";

  if (key > root->right->key) {
    // Zig-Zig (RR)
    cout << "Zig-Zig (RR) at " << root->key << "\n";
    root->right->right = splay(root->right->right, key);
    root = rotL(root);
  } else if (key < root->right->key) {
    // Zig-Zag (RL)
    cout << "Zig-Zag (RL) at " << root->key << "\n";
    root->right->left = splay(root->right->left, key);
    if (root->right->left) root->right = rotR(root->right);
  }

  if (!root->right) return root;
  cout << "Zig (Left rotate) at " << root->key << "\n";
  return rotL(root);
}

static bool search(Node *&root, int key) {
  root = splay(root, key);
  return root && root->key == key;
}

static void insert(Node *&root, int key) {
  if (!root) {
    root = new Node(key);
    return;
  }
  root = splay(root, key);
  if (root->key == key) {
    cout << "Duplicate, ignored.\n";
    return;
  }
  Node *nn = new Node(key);
  if (key < root->key) {
    nn->right = root;
    nn->left = root->left;
    root->left = NULL;
  } else {
    nn->left = root;
    nn->right = root->right;
    root->right = NULL;
  }
  root = nn;
}

static bool erase(Node *&root, int key) {
  if (!root) return false;
  root = splay(root, key);
  if (!root || root->key != key) return false;

  Node *victim = root;
  if (!root->left) {
    root = root->right;
  } else {
    Node *leftSub = root->left;
    // Bring max of left subtree to root
    leftSub = splay(leftSub, key);
    leftSub->right = victim->right;
    root = leftSub;
  }
  delete victim;
  return true;
}

static void inorder(Node *root) {
  if (!root) return;
  inorder(root->left);
  cout << root->key << " ";
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
  while (choice != 7) {
    cout << "\n=== T20: Splay Tree ===\n";
    cout << "1. Insert\n";
    cout << "2. Search (splay)\n";
    cout << "3. Delete\n";
    cout << "4. Display inorder\n";
    cout << "5. Display sideways\n";
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
      insert(root, k);
      cout << "Root now: " << (root ? root->key : -1) << "\n";
    } else if (choice == 2) {
      int k = readInt("Search key: ");
      bool ok = search(root, k);
      cout << (ok ? "Found" : "Not found") << ". Root now: " << (root ? root->key : -1) << "\n";
    } else if (choice == 3) {
      int k = readInt("Delete key: ");
      bool ok = erase(root, k);
      cout << (ok ? "Deleted" : "Not found") << ". Root now: " << (root ? root->key : -1) << "\n";
    } else if (choice == 4) {
      cout << "Inorder: ";
      inorder(root);
      cout << "\n";
    } else if (choice == 5) {
      cout << "\nSideways view:\n";
      displaySideways(root, 0);
    } else if (choice == 6) {
      freeTree(root);
      root = NULL;
      cout << "Cleared.\n";
    } else if (choice == 7) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  freeTree(root);
  return 0;
}
