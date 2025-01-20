// 27. Convert Binary Tree -> BST (structure preserved)
// Demo: Build a general binary tree using level-order insertion (NOT BST insertion).
// Then convert it into a BST by collecting all keys, sorting, and writing them back
// in inorder. This keeps the original shape; only the values are rearranged.
//
// Note: duplicates are NOT allowed (keeps strict BST property after conversion).
// Note: if you insert/delete after conversion using BT operations, the tree may
// no longer be a BST; you can convert again to restore BST order.

#include <iostream>
#include <string>
using namespace std;

struct Node {
  int key;
  Node *left;
  Node *right;
  Node(int k) : key(k), left(NULL), right(NULL) {}
};

struct NodeQueue {
  Node **a;
  int cap;
  int head;
  int tail;
  int n;

  NodeQueue(int c) {
    cap = (c < 8) ? 8 : c;
    a = new Node *[cap];
    head = tail = n = 0;
  }
  ~NodeQueue() { delete[] a; }

  bool empty() const { return n == 0; }

  void push(Node *x) {
    if (n == cap) {
      int newCap = cap * 2;
      Node **b = new Node *[newCap];
      for (int i = 0; i < n; i++) b[i] = a[(head + i) % cap];
      delete[] a;
      a = b;
      cap = newCap;
      head = 0;
      tail = n;
    }
    a[tail] = x;
    tail = (tail + 1) % cap;
    n++;
  }

  Node *pop() {
    if (n == 0) return NULL;
    Node *x = a[head];
    head = (head + 1) % cap;
    n--;
    return x;
  }
};

struct IntList {
  int *a;
  int n;
  int cap;

  IntList(int c = 16) {
    cap = (c < 8) ? 8 : c;
    a = new int[cap];
    n = 0;
  }

  ~IntList() { delete[] a; }

  void push(int x) {
    if (n == cap) {
      int newCap = cap * 2;
      int *b = new int[newCap];
      for (int i = 0; i < n; i++) b[i] = a[i];
      delete[] a;
      a = b;
      cap = newCap;
    }
    a[n++] = x;
  }
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
    cout << "Invalid number. Try again.\n";
    clearInput();
  }
}

static void freeTree(Node *root) {
  if (!root) return;
  freeTree(root->left);
  freeTree(root->right);
  delete root;
}

static void inorder(Node *root) {
  if (!root) return;
  inorder(root->left);
  cout << root->key << " ";
  inorder(root->right);
}

static void preorder(Node *root) {
  if (!root) return;
  cout << root->key << " ";
  preorder(root->left);
  preorder(root->right);
}

static void postorder(Node *root) {
  if (!root) return;
  postorder(root->left);
  postorder(root->right);
  cout << root->key << " ";
}

static void displaySideways(Node *root, int depth) {
  if (!root) return;
  displaySideways(root->right, depth + 1);
  for (int i = 0; i < depth; i++) cout << "    ";
  cout << root->key << "\n";
  displaySideways(root->left, depth + 1);
}

static void displayLevelOrder(Node *root) {
  if (!root) {
    cout << "(empty)\n";
    return;
  }
  NodeQueue q(32);
  q.push(root);
  cout << "Level-order: ";
  while (!q.empty()) {
    Node *cur = q.pop();
    cout << cur->key << " ";
    if (cur->left) q.push(cur->left);
    if (cur->right) q.push(cur->right);
  }
  cout << "\n";
}

static bool containsKeyBFS(Node *root, int key) {
  if (!root) return false;
  NodeQueue q(32);
  q.push(root);
  while (!q.empty()) {
    Node *cur = q.pop();
    if (cur->key == key) return true;
    if (cur->left) q.push(cur->left);
    if (cur->right) q.push(cur->right);
  }
  return false;
}

static bool searchBFSVerbose(Node *root, int key) {
  if (!root) {
    cout << "Tree empty.\n";
    return false;
  }
  NodeQueue q(32);
  q.push(root);
  cout << "Search path (level-order):\n";
  int idx = 0;
  bool found = false;
  while (!q.empty()) {
    Node *cur = q.pop();
    cout << "  visit index " << idx << " value " << cur->key;
    if (cur->key == key) {
      cout << " <- match";
      found = true;
    }
    cout << "\n";
    if (cur->left) q.push(cur->left);
    if (cur->right) q.push(cur->right);
    idx++;
  }
  cout << (found ? "Found." : "Not found.") << "\n";
  return found;
}

static void insertLevelOrderNoDup(Node *&root, int key) {
  if (containsKeyBFS(root, key)) {
    cout << "Duplicate key " << key << " not allowed.\n";
    return;
  }

  Node *nn = new Node(key);
  if (!root) {
    root = nn;
    cout << "Inserted as root.\n";
    displayLevelOrder(root);
    return;
  }

  NodeQueue q(32);
  q.push(root);
  while (!q.empty()) {
    Node *cur = q.pop();

    if (!cur->left) {
      cur->left = nn;
      cout << "Inserted " << key << " as LEFT child of " << cur->key << "\n";
      cout << "First empty slot found during level-order scan.\n";
      displayLevelOrder(root);
      return;
    }

    if (!cur->right) {
      cur->right = nn;
      cout << "Inserted " << key << " as RIGHT child of " << cur->key << "\n";
      cout << "Left was full, so the first empty level-order slot was right.\n";
      displayLevelOrder(root);
      return;
    }

    q.push(cur->left);
    q.push(cur->right);
  }
}

static bool deleteByValue(Node *&root, int key) {
  if (!root) return false;

  NodeQueue q(32);
  q.push(root);
  Node *target = NULL;
  Node *last = NULL;
  Node *parentOfLast = NULL;
  int idx = 0;
  int targetIdx = -1;
  int lastIdx = -1;

  cout << "Delete scan (level-order):\n";
  while (!q.empty()) {
    Node *cur = q.pop();
    cout << "  visit index " << idx << " value " << cur->key;
    if (cur->key == key && !target) {
      cout << " <- match";
      target = cur;
      targetIdx = idx;
    }
    cout << "\n";

    if (cur->left) {
      parentOfLast = cur;
      q.push(cur->left);
    }
    if (cur->right) {
      parentOfLast = cur;
      q.push(cur->right);
    }

    last = cur;
    lastIdx = idx;
    idx++;
  }

  if (!target) return false;

  cout << "Deepest node is index " << lastIdx << " value " << last->key << ".\n";
  cout << "Copy deepest value " << last->key << " into target index " << targetIdx
       << " (old value " << target->key << ").\n";
  target->key = last->key;

  // Remove deepest node
  if (last == root) {
    cout << "Deepest node was the root; tree becomes empty.\n";
    delete root;
    root = NULL;
    return true;
  }

  if (parentOfLast && parentOfLast->right == last) {
    cout << "Remove deepest node from RIGHT of parent " << parentOfLast->key << ".\n";
    delete parentOfLast->right;
    parentOfLast->right = NULL;
  } else if (parentOfLast && parentOfLast->left == last) {
    cout << "Remove deepest node from LEFT of parent " << parentOfLast->key << ".\n";
    delete parentOfLast->left;
    parentOfLast->left = NULL;
  }

  return true;
}

static void collectKeys(Node *root, IntList &out) {
  if (!root) return;
  collectKeys(root->left, out);
  out.push(root->key);
  collectKeys(root->right, out);
}

static void swapInt(int &x, int &y) {
  int t = x;
  x = y;
  y = t;
}

static void quickSort(int *a, int lo, int hi) {
  if (lo >= hi) return;
  int pivot = a[(lo + hi) / 2];
  int i = lo;
  int j = hi;
  while (i <= j) {
    while (a[i] < pivot) i++;
    while (a[j] > pivot) j--;
    if (i <= j) {
      swapInt(a[i], a[j]);
      i++;
      j--;
    }
  }
  if (lo < j) quickSort(a, lo, j);
  if (i < hi) quickSort(a, i, hi);
}

static void assignInorder(Node *root, const IntList &sorted, int &idx) {
  if (!root) return;
  assignInorder(root->left, sorted, idx);
  if (idx < sorted.n) root->key = sorted.a[idx++];
  assignInorder(root->right, sorted, idx);
}

static void convertBinaryTreeToBST(Node *root) {
  if (!root) {
    cout << "Tree empty.\n";
    return;
  }

  IntList keys;
  collectKeys(root, keys);

  cout << "Collected " << keys.n << " keys (inorder of original tree).\n";
  if (keys.n <= 1) {
    cout << "Nothing to convert (0/1 node).\n";
    return;
  }

  quickSort(keys.a, 0, keys.n - 1);
  cout << "Sorted keys: ";
  for (int i = 0; i < keys.n; i++) cout << keys.a[i] << " ";
  cout << "\n";

  int idx = 0;
  assignInorder(root, keys, idx);
  cout << "Converted: assigned sorted keys back using inorder traversal.\n";
}

static bool isBSTInorder(Node *root, int &prev, bool &hasPrev) {
  if (!root) return true;
  if (!isBSTInorder(root->left, prev, hasPrev)) return false;
  if (hasPrev && root->key <= prev) return false;
  prev = root->key;
  hasPrev = true;
  return isBSTInorder(root->right, prev, hasPrev);
}

static bool isBST(Node *root) {
  int prev = 0;
  bool hasPrev = false;
  return isBSTInorder(root, prev, hasPrev);
}

int main() {
  Node *root = NULL;
  int choice = 0;

  while (choice != 10) {
    cout << "\n=== T31: Binary Tree -> BST Conversion (Structure Preserved) ===\n";
    cout << "1. Insert (level-order, no duplicates)\n";
    cout << "2. Search (BFS)\n";
    cout << "3. Delete by value (swap with deepest)\n";
    cout << "4. Display level-order\n";
    cout << "5. Display sideways\n";
    cout << "6. Display traversals (in/pre/post)\n";
    cout << "7. Convert Binary Tree -> BST\n";
    cout << "8. Check BST property\n";
    cout << "9. Clear\n";
    cout << "10. Exit\n";
    cout << "Select: ";

    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      int k = readInt("Key: ");
      insertLevelOrderNoDup(root, k);
    } else if (choice == 2) {
      int k = readInt("Search key: ");
      searchBFSVerbose(root, k);
    } else if (choice == 3) {
      int k = readInt("Delete key: ");
      if (deleteByValue(root, k)) cout << "Deleted.\n";
      else cout << "Key not found.\n";
    } else if (choice == 4) {
      displayLevelOrder(root);
    } else if (choice == 5) {
      cout << "\nSideways view (right is up):\n";
      if (!root) cout << "(empty)\n";
      else displaySideways(root, 0);
    } else if (choice == 6) {
      cout << "Inorder: ";
      inorder(root);
      cout << "\n";
      cout << "Preorder: ";
      preorder(root);
      cout << "\n";
      cout << "Postorder: ";
      postorder(root);
      cout << "\n";
    } else if (choice == 7) {
      cout << "Before conversion, inorder is: ";
      inorder(root);
      cout << "\n";
      convertBinaryTreeToBST(root);
      cout << "After conversion, inorder is: ";
      inorder(root);
      cout << "\n";
    } else if (choice == 8) {
      cout << "BST check (strict): " << (isBST(root) ? "YES" : "NO") << "\n";
      if (!isBST(root)) {
        cout << "Tip: use option 7 to convert/re-convert.\n";
      }
    } else if (choice == 9) {
      freeTree(root);
      root = NULL;
      cout << "Cleared.\n";
    } else if (choice == 10) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }

  freeTree(root);
  return 0;
}
