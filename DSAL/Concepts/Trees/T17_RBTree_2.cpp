// 15. RB Tree: Insertion, Deletion, Rotations, Display, Searching
// Demo: Full Red-Black Tree (CLRS style) with a NIL sentinel.
// Insert/Delete show rotation & recolor steps.

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

struct RBTree {
  Node *NIL;
  Node *root;

  RBTree() {
    NIL = new Node();
    NIL->key = 0;
    NIL->color = BLACK;
    NIL->left = NIL->right = NIL->parent = NIL;
    root = NIL;
  }

  ~RBTree() {
    clear();
    delete NIL;
  }

  Node *newNode(int key) {
    Node *z = new Node();
    z->key = key;
    z->color = RED;
    z->left = z->right = z->parent = NIL;
    return z;
  }

  void leftRotate(Node *x) {
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

  void rightRotate(Node *y) {
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

  void insertFixup(Node *z) {
    while (z->parent->color == RED) {
      if (z->parent == z->parent->parent->left) {
        Node *y = z->parent->parent->right;
        if (y->color == RED) {
          cout << "InsertFix: Recolor (parent+uncle BLACK, grandparent RED)\n";
          z->parent->color = BLACK;
          y->color = BLACK;
          z->parent->parent->color = RED;
          z = z->parent->parent;
        } else {
          if (z == z->parent->right) {
            cout << "InsertFix: RotateLeft at parent " << z->parent->key << "\n";
            z = z->parent;
            leftRotate(z);
          }
          cout << "InsertFix: RotateRight at grandparent " << z->parent->parent->key << "\n";
          z->parent->color = BLACK;
          z->parent->parent->color = RED;
          rightRotate(z->parent->parent);
        }
      } else {
        Node *y = z->parent->parent->left;
        if (y->color == RED) {
          cout << "InsertFix: Recolor (parent+uncle BLACK, grandparent RED)\n";
          z->parent->color = BLACK;
          y->color = BLACK;
          z->parent->parent->color = RED;
          z = z->parent->parent;
        } else {
          if (z == z->parent->left) {
            cout << "InsertFix: RotateRight at parent " << z->parent->key << "\n";
            z = z->parent;
            rightRotate(z);
          }
          cout << "InsertFix: RotateLeft at grandparent " << z->parent->parent->key << "\n";
          z->parent->color = BLACK;
          z->parent->parent->color = RED;
          leftRotate(z->parent->parent);
        }
      }
    }
    root->color = BLACK;
  }

  bool insert(int key) {
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
    insertFixup(z);
    return true;
  }

  Node *searchNode(int key, int &comps) const {
    comps = 0;
    Node *cur = root;
    while (cur != NIL) {
      comps++;
      cout << "Compare " << key << " with " << cur->key << ": ";
      if (key == cur->key) return cur;
      if (key < cur->key) {
        cout << "go left\n";
        cur = cur->left;
      } else {
        cout << "go right\n";
        cur = cur->right;
      }
    }
    cout << "Reached NIL.\n";
    return NIL;
  }

  Node *minimum(Node *x) const {
    while (x->left != NIL) x = x->left;
    return x;
  }

  void transplant(Node *u, Node *v) {
    if (u->parent == NIL) root = v;
    else if (u == u->parent->left) u->parent->left = v;
    else u->parent->right = v;
    v->parent = u->parent;
  }

  void deleteFixup(Node *x) {
    while (x != root && x->color == BLACK) {
      if (x == x->parent->left) {
        Node *w = x->parent->right;
        if (w->color == RED) {
          cout << "DeleteFix: sibling RED -> rotateLeft parent\n";
          w->color = BLACK;
          x->parent->color = RED;
          leftRotate(x->parent);
          w = x->parent->right;
        }
        if (w->left->color == BLACK && w->right->color == BLACK) {
          cout << "DeleteFix: sibling BLACK with 2 BLACK children -> recolor sibling RED\n";
          w->color = RED;
          x = x->parent;
        } else {
          if (w->right->color == BLACK) {
            cout << "DeleteFix: sibling BLACK, near child RED -> rotateRight sibling\n";
            w->left->color = BLACK;
            w->color = RED;
            rightRotate(w);
            w = x->parent->right;
          }
          cout << "DeleteFix: rotateLeft parent, recolor\n";
          w->color = x->parent->color;
          x->parent->color = BLACK;
          w->right->color = BLACK;
          leftRotate(x->parent);
          x = root;
        }
      } else {
        Node *w = x->parent->left;
        if (w->color == RED) {
          cout << "DeleteFix: sibling RED -> rotateRight parent\n";
          w->color = BLACK;
          x->parent->color = RED;
          rightRotate(x->parent);
          w = x->parent->left;
        }
        if (w->right->color == BLACK && w->left->color == BLACK) {
          cout << "DeleteFix: sibling BLACK with 2 BLACK children -> recolor sibling RED\n";
          w->color = RED;
          x = x->parent;
        } else {
          if (w->left->color == BLACK) {
            cout << "DeleteFix: sibling BLACK, near child RED -> rotateLeft sibling\n";
            w->right->color = BLACK;
            w->color = RED;
            leftRotate(w);
            w = x->parent->left;
          }
          cout << "DeleteFix: rotateRight parent, recolor\n";
          w->color = x->parent->color;
          x->parent->color = BLACK;
          w->left->color = BLACK;
          rightRotate(x->parent);
          x = root;
        }
      }
    }
    x->color = BLACK;
  }

  bool erase(int key) {
    int comps = 0;
    Node *z = searchNode(key, comps);
    if (z == NIL) return false;

    Node *y = z;
    Color yOrig = y->color;
    Node *x;

    if (z->left == NIL) {
      x = z->right;
      transplant(z, z->right);
    } else if (z->right == NIL) {
      x = z->left;
      transplant(z, z->left);
    } else {
      y = minimum(z->right);
      yOrig = y->color;
      x = y->right;
      if (y->parent == z) {
        x->parent = y;
      } else {
        transplant(y, y->right);
        y->right = z->right;
        y->right->parent = y;
      }
      transplant(z, y);
      y->left = z->left;
      y->left->parent = y;
      y->color = z->color;
    }

    delete z;
    if (yOrig == BLACK) deleteFixup(x);
    return true;
  }

  void inorder(Node *x) const {
    if (x == NIL) return;
    inorder(x->left);
    cout << x->key << (x->color == RED ? "(R) " : "(B) ");
    inorder(x->right);
  }

  void displaySideways(Node *x, int depth) const {
    if (x == NIL) return;
    displaySideways(x->right, depth + 1);
    for (int i = 0; i < depth; i++) cout << "    ";
    cout << x->key << (x->color == RED ? "(R)" : "(B)") << "\n";
    displaySideways(x->left, depth + 1);
  }

  int height(Node *x) const {
    if (x == NIL) return 0;
    int hl = height(x->left);
    int hr = height(x->right);
    return (hl > hr ? hl : hr) + 1;
  }

  void countColors(Node *x, int &red, int &black) const {
    if (x == NIL) return;
    if (x->color == RED) red++;
    else black++;
    countColors(x->left, red, black);
    countColors(x->right, red, black);
  }

  bool redChildrenBlack(Node *x) const {
    if (x == NIL) return true;
    bool here = true;
    if (x->color == RED) {
      here = (x->left->color == BLACK && x->right->color == BLACK);
      if (!here) cout << "Red violation at node " << x->key << "\n";
    }
    return here && redChildrenBlack(x->left) && redChildrenBlack(x->right);
  }

  int blackHeightOrFail(Node *x) const {
    if (x == NIL) return 1;
    int l = blackHeightOrFail(x->left);
    int r = blackHeightOrFail(x->right);
    if (l == 0 || r == 0 || l != r) {
      cout << "Black-height mismatch at node " << x->key
           << ": left=" << l << ", right=" << r << "\n";
      return 0;
    }
    return l + (x->color == BLACK ? 1 : 0);
  }

  void printRBProperties() const {
    bool rootBlack = (root == NIL || root->color == BLACK);
    bool redOk = redChildrenBlack(root);
    int bh = blackHeightOrFail(root);
    cout << "RB property check:\n";
    cout << "1. Every stored node has a color tag: OK\n";
    cout << "2. Root is black: " << (rootBlack ? "OK" : "FAIL") << "\n";
    cout << "3. NIL leaves are black: OK\n";
    cout << "4. Red node children are black: " << (redOk ? "OK" : "FAIL") << "\n";
    cout << "5. Each root-to-NIL path has same black count: "
         << (bh > 0 ? "OK" : "FAIL") << "\n";
    if (bh > 0) cout << "   black-height including NIL = " << bh << "\n";
  }

  void freeSubtree(Node *x) {
    if (x == NIL) return;
    freeSubtree(x->left);
    freeSubtree(x->right);
    delete x;
  }

  void clear() {
    freeSubtree(root);
    root = NIL;
  }
};

int main() {
  RBTree t;
  int choice = 0;
  while (choice != 8) {
    cout << "\n=== T17: Red-Black Tree (Full) ===\n";
    cout << "1. Insert\n";
    cout << "2. Delete\n";
    cout << "3. Search\n";
    cout << "4. Display inorder\n";
    cout << "5. Display sideways\n";
    cout << "6. #Red/#Black + Height\n";
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
      bool ok = t.insert(k);
      cout << (ok ? "Inserted.\n" : "Duplicate ignored.\n");
    } else if (choice == 2) {
      int k = readInt("Delete key: ");
      bool ok = t.erase(k);
      cout << (ok ? "Deleted.\n" : "Not found.\n");
    } else if (choice == 3) {
      int k = readInt("Search key: ");
      int comps = 0;
      Node *found = t.searchNode(k, comps);
      if (found != t.NIL) cout << "Found (" << (found->color == RED ? "R" : "B") << "), comparisons=" << comps << "\n";
      else cout << "Not found, comparisons=" << comps << "\n";
    } else if (choice == 4) {
      cout << "Inorder: ";
      t.inorder(t.root);
      cout << "\n";
    } else if (choice == 5) {
      cout << "\nSideways view:\n";
      t.displaySideways(t.root, 0);
    } else if (choice == 6) {
      int r = 0, b = 0;
      t.countColors(t.root, r, b);
      cout << "Red=" << r << ", Black=" << b << ", Height=" << t.height(t.root) << "\n";
      t.printRBProperties();
    } else if (choice == 7) {
      t.clear();
      cout << "Cleared.\n";
    } else if (choice == 8) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  return 0;
}
