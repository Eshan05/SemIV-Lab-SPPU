// 26. Treap (Tree + Heap)
// Demo: Treap where keys follow BST property and priorities follow MAX-heap property.
// Supports: insert(key,priority), delete(key), search, display.

#include <iostream>
using namespace std;

struct Node {
  int key;
  int pr;
  Node *left;
  Node *right;
  Node(int k, int p) : key(k), pr(p), left(NULL), right(NULL) {}
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

static Node *rotR(Node *y) {
  Node *x = y->left;
  cout << "  rotateRight(" << y->key << "): priority " << x->pr << " rises.\n";
  y->left = x->right;
  x->right = y;
  return x;
}

static Node *rotL(Node *x) {
  Node *y = x->right;
  cout << "  rotateLeft(" << x->key << "): priority " << y->pr << " rises.\n";
  x->right = y->left;
  y->left = x;
  return y;
}

static Node *insert(Node *root, int key, int pr, bool &inserted) {
  if (!root) {
    cout << "Insert new node key=" << key << ", priority=" << pr << "\n";
    inserted = true;
    return new Node(key, pr);
  }
  if (key < root->key) {
    cout << "Compare " << key << " with " << root->key << ": go left\n";
    root->left = insert(root->left, key, pr, inserted);
    if (root->left && root->left->pr > root->pr) {
      cout << "RotateRight at " << root->key << " (heap fix)\n";
      root = rotR(root);
    }
  } else if (key > root->key) {
    cout << "Compare " << key << " with " << root->key << ": go right\n";
    root->right = insert(root->right, key, pr, inserted);
    if (root->right && root->right->pr > root->pr) {
      cout << "RotateLeft at " << root->key << " (heap fix)\n";
      root = rotL(root);
    }
  } else {
    cout << "Duplicate key found at " << root->key << "; no new node.\n";
  }
  return root;
}

static Node *erase(Node *root, int key) {
  if (!root) return NULL;
  if (key < root->key) root->left = erase(root->left, key);
  else if (key > root->key) root->right = erase(root->right, key);
  else {
    // rotate down until leaf-ish
    if (!root->left && !root->right) {
      delete root;
      return NULL;
    }
    if (!root->left) {
      root = rotL(root);
      root->left = erase(root->left, key);
    } else if (!root->right) {
      root = rotR(root);
      root->right = erase(root->right, key);
    } else {
      if (root->left->pr > root->right->pr) {
        root = rotR(root);
        root->right = erase(root->right, key);
      } else {
        root = rotL(root);
        root->left = erase(root->left, key);
      }
    }
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
  cout << root->key << "(p=" << root->pr << ") ";
  inorder(root->right);
}

static void displaySideways(Node *root, int depth) {
  if (!root) return;
  displaySideways(root->right, depth + 1);
  for (int i = 0; i < depth; i++) cout << "    ";
  cout << root->key << "(" << root->pr << ")\n";
  displaySideways(root->left, depth + 1);
}

static int countNodes(Node *root) {
  if (!root) return 0;
  return 1 + countNodes(root->left) + countNodes(root->right);
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
    cout << "\n=== T30: Treap (BST + Max-Heap by priority) ===\n";
    cout << "1. Insert (key,priority)\n";
    cout << "2. Delete key\n";
    cout << "3. Search key\n";
    cout << "4. Display inorder\n";
    cout << "5. Display sideways\n";
    cout << "6. #Nodes\n";
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
      int p = readInt("Priority (bigger = higher): ");
      bool inserted = false;
      root = insert(root, k, p, inserted);
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
      cout << "\nSideways view (shows heap by priorities too):\n";
      displaySideways(root, 0);
    } else if (choice == 6) {
      cout << "Nodes=" << countNodes(root) << "\n";
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
