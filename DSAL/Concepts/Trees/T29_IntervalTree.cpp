// 25. Interval Tree
// Demo: interval tree as an augmented BST keyed by low endpoint.
// Each node stores maxHigh in its subtree.
// Supports: insertion, deletion, overlap search, display.

#include <iostream>
using namespace std;

struct Node {
  int low;
  int high;
  int maxHigh;
  Node *left;
  Node *right;
  Node(int l, int h) : low(l), high(h), maxHigh(h), left(NULL), right(NULL) {}
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

static int max2(int a, int b) { return (a > b) ? a : b; }

static void updateMax(Node *n) {
  if (!n) return;
  int m = n->high;
  if (n->left) m = max2(m, n->left->maxHigh);
  if (n->right) m = max2(m, n->right->maxHigh);
  if (n->maxHigh != m) {
    cout << "update max at [" << n->low << "," << n->high << "]: "
         << n->maxHigh << " -> " << m << "\n";
  }
  n->maxHigh = m;
}

static bool overlap(int l1, int h1, int l2, int h2) {
  return l1 <= h2 && l2 <= h1;
}

static Node *insert(Node *root, int low, int high) {
  if (!root) {
    cout << "insert new interval leaf [" << low << "," << high << "]\n";
    return new Node(low, high);
  }
  if (low < root->low) {
    cout << "[" << low << "," << high << "] goes left of [" << root->low << "," << root->high << "]\n";
    root->left = insert(root->left, low, high);
  } else {
    cout << "[" << low << "," << high << "] goes right of [" << root->low << "," << root->high << "]\n";
    root->right = insert(root->right, low, high);
  }
  updateMax(root);
  return root;
}

static Node *minNode(Node *root) {
  Node *cur = root;
  while (cur && cur->left) cur = cur->left;
  return cur;
}

static Node *erase(Node *root, int low, int high) {
  if (!root) return NULL;
  if (low < root->low) {
    cout << "delete [" << low << "," << high << "] goes left of ["
         << root->low << "," << root->high << "]\n";
    root->left = erase(root->left, low, high);
  } else if (low > root->low) {
    cout << "delete [" << low << "," << high << "] goes right of ["
         << root->low << "," << root->high << "]\n";
    root->right = erase(root->right, low, high);
  }
  else {
    // match by both low & high
    if (root->high != high) {
      cout << "same low at [" << root->low << "," << root->high
           << "] but high differs; continue right\n";
      root->right = erase(root->right, low, high);
    } else {
      cout << "delete exact interval [" << low << "," << high << "]\n";
      if (!root->left) {
        Node *r = root->right;
        delete root;
        return r;
      }
      if (!root->right) {
        Node *l = root->left;
        delete root;
        return l;
      }
      Node *succ = minNode(root->right);
      cout << "two children: replace with successor [" << succ->low << "," << succ->high << "]\n";
      root->low = succ->low;
      root->high = succ->high;
      root->right = erase(root->right, succ->low, succ->high);
    }
  }
  updateMax(root);
  return root;
}

static bool containsExact(Node *root, int low, int high, bool verbose) {
  Node *cur = root;
  while (cur) {
    if (verbose) {
      cout << "Compare [" << low << "," << high << "] with ["
           << cur->low << "," << cur->high << "]: ";
    }
    if (low == cur->low && high == cur->high) {
      if (verbose) cout << "match\n";
      return true;
    }
    if (low < cur->low) {
      if (verbose) cout << "go left\n";
      cur = cur->left;
    } else {
      if (verbose) cout << "go right\n";
      cur = cur->right;
    }
  }
  if (verbose) cout << "Reached NULL.\n";
  return false;
}

static Node *searchOverlap(Node *root, int ql, int qh) {
  Node *cur = root;
  while (cur) {
    cout << "check [" << cur->low << "," << cur->high << "] max=" << cur->maxHigh << "\n";
    if (overlap(cur->low, cur->high, ql, qh)) return cur;
    if (cur->left && cur->left->maxHigh >= ql) {
      cout << "left max can still overlap -> go left\n";
      cur = cur->left;
    } else {
      cout << "left cannot overlap -> go right\n";
      cur = cur->right;
    }
  }
  return NULL;
}

static void inorder(Node *root) {
  if (!root) return;
  inorder(root->left);
  cout << "[" << root->low << "," << root->high << "]";
  cout << " max=" << root->maxHigh << "  ";
  inorder(root->right);
}

static void displaySideways(Node *root, int depth) {
  if (!root) return;
  displaySideways(root->right, depth + 1);
  for (int i = 0; i < depth; i++) cout << "    ";
  cout << "[" << root->low << "," << root->high << "]";
  cout << "(max=" << root->maxHigh << ")\n";
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
    cout << "\n=== T29: Interval Tree ===\n";
    cout << "1. Insert interval [l,h]\n";
    cout << "2. Delete interval [l,h]\n";
    cout << "3. Search any overlap with query [l,h]\n";
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
      int l = readInt("l: ");
      int h = readInt("h: ");
      if (l > h) {
        cout << "Input interval [" << l << "," << h << "] has l > h; normalizing to ["
             << h << "," << l << "].\n";
        int t = l;
        l = h;
        h = t;
      }
      if (root && containsExact(root, l, h, true)) {
        cout << "Duplicate interval ignored.\n";
      } else {
        root = insert(root, l, h);
        cout << "Inserted.\n";
      }
    } else if (choice == 2) {
      int l = readInt("l: ");
      int h = readInt("h: ");
      if (l > h) {
        cout << "Input interval [" << l << "," << h << "] has l > h; normalizing to ["
             << h << "," << l << "].\n";
        int t = l;
        l = h;
        h = t;
      }
      if (containsExact(root, l, h, true)) {
        root = erase(root, l, h);
        cout << "Deleted.\n";
      } else {
        cout << "Not found; no delete performed.\n";
      }
    } else if (choice == 3) {
      int l = readInt("query l: ");
      int h = readInt("query h: ");
      if (l > h) {
        cout << "Query interval [" << l << "," << h << "] has l > h; normalizing to ["
             << h << "," << l << "].\n";
        int t = l;
        l = h;
        h = t;
      }
      Node *ans = searchOverlap(root, l, h);
      if (!ans) cout << "No overlap found.\n";
      else {
        cout << "Overlaps with: [" << ans->low << "," << ans->high << "]\n";
      }
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
