// 14. RB Tree: Construction + Properties
// Demo: Red-Black Tree insertion (construction) + property checks.
// Also shows: #Red, #Black, Height.

#include <iostream>
using namespace std;

enum Color { RED = 0,
             BLACK = 1 };

struct Node {
  int key;
  Color color;
  Node *left;
  Node *right;
  Node *parent;
};

static Node *NIL = NULL;

static void initNIL() {
  if (NIL) return;
  NIL = new Node();
  NIL->key = 0;
  NIL->color = BLACK;
  NIL->left = NIL->right = NIL->parent = NIL;
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
  Node *z = new Node();
  z->key = key;
  z->color = RED;
  z->left = z->right = z->parent = NIL;
  return z;
}

static void leftRotate(Node *&root, Node *x) {
  Node *y = x->right;
  cout << "  leftRotate(" << x->key << "): " << y->key << " moves above it.\n";
  x->right = y->left;
  if (y->left != NIL) y->left->parent = x;
  y->parent = x->parent;
  if (x->parent == NIL) root = y;
  else if (x == x->parent->left) x->parent->left = y;
  else x->parent->right = y;
  y->left = x;
  x->parent = y;
}

static void rightRotate(Node *&root, Node *y) {
  Node *x = y->left;
  cout << "  rightRotate(" << y->key << "): " << x->key << " moves above it.\n";
  y->left = x->right;
  if (x->right != NIL) x->right->parent = y;
  x->parent = y->parent;
  if (y->parent == NIL) root = x;
  else if (y == y->parent->left) y->parent->left = x;
  else y->parent->right = x;
  x->right = y;
  y->parent = x;
}

static void insertFixup(Node *&root, Node *z) {
  while (z->parent->color == RED) {
    if (z->parent == z->parent->parent->left) {
      Node *y = z->parent->parent->right; // uncle
      if (y->color == RED) {
        cout << "Recolor (parent+uncle BLACK, grandparent RED)\n";
        z->parent->color = BLACK;
        y->color = BLACK;
        z->parent->parent->color = RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->right) {
          cout << "RotateLeft at parent " << z->parent->key << " (prepare)\n";
          z = z->parent;
          leftRotate(root, z);
        }
        cout << "RotateRight at grandparent " << z->parent->parent->key << "\n";
        z->parent->color = BLACK;
        z->parent->parent->color = RED;
        rightRotate(root, z->parent->parent);
      }
    } else {
      Node *y = z->parent->parent->left; // uncle
      if (y->color == RED) {
        cout << "Recolor (parent+uncle BLACK, grandparent RED)\n";
        z->parent->color = BLACK;
        y->color = BLACK;
        z->parent->parent->color = RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->left) {
          cout << "RotateRight at parent " << z->parent->key << " (prepare)\n";
          z = z->parent;
          rightRotate(root, z);
        }
        cout << "RotateLeft at grandparent " << z->parent->parent->key << "\n";
        z->parent->color = BLACK;
        z->parent->parent->color = RED;
        leftRotate(root, z->parent->parent);
      }
    }
  }
  root->color = BLACK;
}

static bool insert(Node *&root, int key) {
  Node *z = newNode(key);
  Node *y = NIL;
  Node *x = root;
  while (x != NIL) {
    y = x;
    if (key < x->key) x = x->left;
    else if (key > x->key) x = x->right;
    else {
      delete z;
      return false;
    }
  }
  z->parent = y;
  cout << "Insert " << key << " as RED leaf";
  if (y == NIL) cout << " at root";
  else cout << " under parent " << y->key;
  cout << "\n";
  if (y == NIL) root = z;
  else if (key < y->key) y->left = z;
  else y->right = z;
  insertFixup(root, z);
  return true;
}

static bool search(Node *root, int key, int &comps) {
  comps = 0;
  Node *cur = root;
  while (cur != NIL) {
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
  cout << "Reached NIL.\n";
  return false;
}

static void inorder(Node *root) {
  if (root == NIL) return;
  inorder(root->left);
  cout << root->key << (root->color == RED ? "(R) " : "(B) ");
  inorder(root->right);
}

static void displaySideways(Node *root, int depth) {
  if (root == NIL) return;
  displaySideways(root->right, depth + 1);
  for (int i = 0; i < depth; i++) cout << "    ";
  cout << root->key << (root->color == RED ? "(R)" : "(B)") << "\n";
  displaySideways(root->left, depth + 1);
}

static int height(Node *root) {
  if (root == NIL) return 0;
  int hl = height(root->left);
  int hr = height(root->right);
  return (hl > hr ? hl : hr) + 1;
}

static void countColors(Node *root, int &red, int &black) {
  if (root == NIL) return;
  if (root->color == RED) red++;
  else black++;
  countColors(root->left, red, black);
  countColors(root->right, red, black);
}

static int checkProperties(Node *root) {
  // returns black-height if ok; -1 if violation
  if (root == NIL) return 1;
  if (root->color == RED) {
    if (root->left->color == RED || root->right->color == RED) return -1;
  }
  int leftBH = checkProperties(root->left);
  if (leftBH < 0) return -1;
  int rightBH = checkProperties(root->right);
  if (rightBH < 0) return -1;
  if (leftBH != rightBH) return -1;
  return leftBH + (root->color == BLACK ? 1 : 0);
}

static void printRBProperties(Node *root) {
  cout << "\nRed-Black properties:\n";
  cout << "1. Every node is RED or BLACK: shown by (R)/(B) in displays.\n";
  cout << "2. Root is BLACK: " << ((root == NIL || root->color == BLACK) ? "OK" : "FAIL") << "\n";
  cout << "3. Every NIL leaf is BLACK: OK (single BLACK sentinel is used).\n";

  int bh = checkProperties(root);
  if (bh < 0) {
    cout << "4. RED nodes have BLACK children: FAIL or black-height mismatch found.\n";
    cout << "5. Same black count on every root-to-leaf path: FAIL.\n";
  } else {
    cout << "4. RED nodes have BLACK children: OK.\n";
    cout << "5. Same black count on every root-to-leaf path: OK, black-height=" << bh << "\n";
  }
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
  while (choice != 8) {
    cout << "\n=== T16: Red-Black Tree (Properties Focus) ===\n";
    cout << "1. Insert\n";
    cout << "2. Search\n";
    cout << "3. Display inorder (with colors)\n";
    cout << "4. Display sideways\n";
    cout << "5. #Red / #Black\n";
    cout << "6. Height\n";
    cout << "7. Check RB properties\n";
    cout << "8. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      int k = readInt("Key: ");
      bool ok = insert(root, k);
      cout << (ok ? "Inserted.\n" : "Duplicate ignored.\n");
    } else if (choice == 2) {
      int k = readInt("Search key: ");
      int comps = 0;
      bool ok = search(root, k, comps);
      cout << (ok ? "Found" : "Not found") << ", comparisons=" << comps << "\n";
    } else if (choice == 3) {
      cout << "Inorder: ";
      inorder(root);
      cout << "\n";
    } else if (choice == 4) {
      cout << "\nSideways view:\n";
      displaySideways(root, 0);
    } else if (choice == 5) {
      int r = 0, b = 0;
      countColors(root, r, b);
      cout << "Red=" << r << ", Black=" << b << "\n";
    } else if (choice == 6) {
      cout << "Height(levels)=" << height(root) << "\n";
    } else if (choice == 7) {
      printRBProperties(root);
    } else if (choice == 8) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  freeTree(root);
  // keep NIL allocated (program exit)
  return 0;
}
