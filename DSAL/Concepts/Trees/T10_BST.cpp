// 8. BST: insertion, deletion, searching, display
// Demo: classic Binary Search Tree with walk-through style prints.

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

static void freeTree(Node *root) {
  if (!root) return;
  freeTree(root->left);
  freeTree(root->right);
  delete root;
}

static Node *insertRec(Node *root, int key) {
  if (!root) {
    cout << "Reached NULL link -> insert new leaf here.\n";
    return new Node(key);
  }
  cout << "At node " << root->key << ": ";
  if (key < root->key) {
    cout << "go LEFT\n";
    root->left = insertRec(root->left, key);
  } else if (key > root->key) {
    cout << "go RIGHT\n";
    root->right = insertRec(root->right, key);
  } else {
    cout << "duplicate, ignore\n";
  }
  return root;
}

static bool searchIter(Node *root, int key, int &comps) {
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

static Node *minNode(Node *root) {
  Node *cur = root;
  while (cur && cur->left) cur = cur->left;
  return cur;
}

static Node *deleteRec(Node *root, int key) {
  if (!root) return NULL;
  if (key < root->key) {
    root->left = deleteRec(root->left, key);
  } else if (key > root->key) {
    root->right = deleteRec(root->right, key);
  } else {
    // found
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
    cout << "Two children: replace " << root->key << " with successor " << succ->key << "\n";
    root->key = succ->key;
    root->right = deleteRec(root->right, succ->key);
  }
  return root;
}

static int height(Node *root) {
  if (!root) return 0;
  int hl = height(root->left);
  int hr = height(root->right);
  return (hl > hr ? hl : hr) + 1;
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

int main() {
  Node *root = NULL;
  int choice = 0;
  while (choice != 11) {
    cout << "\n=== T10: Binary Search Tree (BST) ===\n";
    cout << "1. Insert\n";
    cout << "2. Search\n";
    cout << "3. Delete\n";
    cout << "4. Display inorder\n";
    cout << "5. Display preorder\n";
    cout << "6. Display postorder\n";
    cout << "7. Display sideways\n";
    cout << "8. Min / Max\n";
    cout << "9. Height\n";
    cout << "10. Clear\n";
    cout << "11. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      int k = readInt("Key: ");
      root = insertRec(root, k);
    } else if (choice == 2) {
      int k = readInt("Search key: ");
      int comps = 0;
      bool ok = searchIter(root, k, comps);
      cout << (ok ? "Found" : "Not found") << ", comparisons=" << comps << "\n";
    } else if (choice == 3) {
      int k = readInt("Delete key: ");
      int comps = 0;
      if (searchIter(root, k, comps)) {
        root = deleteRec(root, k);
        cout << "Deleted.\n";
      } else {
        cout << "Not found; no delete performed.\n";
      }
    } else if (choice == 4) {
      cout << "Inorder: ";
      inorder(root);
      cout << "\n";
    } else if (choice == 5) {
      cout << "Preorder: ";
      preorder(root);
      cout << "\n";
    } else if (choice == 6) {
      cout << "Postorder: ";
      postorder(root);
      cout << "\n";
    } else if (choice == 7) {
      cout << "\nSideways view (right is up):\n";
      displaySideways(root, 0);
    } else if (choice == 8) {
      if (!root) {
        cout << "Tree empty.\n";
        continue;
      }
      Node *mn = minNode(root);
      Node *cur = root;
      while (cur && cur->right) cur = cur->right;
      cout << "Min=" << (mn ? mn->key : -1) << ", Max=" << (cur ? cur->key : -1) << "\n";
    } else if (choice == 9) {
      cout << "Height(levels)=" << height(root) << "\n";
    } else if (choice == 10) {
      freeTree(root);
      root = NULL;
      cout << "Cleared.\n";
    } else if (choice == 11) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  freeTree(root);
  return 0;
}
