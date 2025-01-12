// 13. Only rotations in AVL tree (4 cases)
// Demo: show LL, RR, LR, RL cases with before/after structure.
// Extra: sandbox to try Left/Right rotations on a general binary tree (not necessarily BST).
// Build using normal level-order insertion (duplicates NOT allowed), then rotate at a chosen node
// by VALUE (recommended) or by PATH (advanced): '.', L, R, LRL.

#include <iostream>
#include <string>
using namespace std;

struct Node {
  int key;
  Node *left;
  Node *right;
  int h;
  Node(int k) : key(k), left(NULL), right(NULL), h(1) {}
};

static int height(Node *n) { return n ? n->h : 0; }
static int max2(int a, int b) { return (a > b) ? a : b; }
static int bf(Node *n) { return n ? height(n->left) - height(n->right) : 0; }

static int recomputeHeights(Node *n) {
  if (!n) return 0;
  int hl = recomputeHeights(n->left);
  int hr = recomputeHeights(n->right);
  n->h = max2(hl, hr) + 1;
  return n->h;
}

static void upd(Node *n) {
  if (!n) return;
  n->h = max2(height(n->left), height(n->right)) + 1;
}

static Node *rotR(Node *y) {
  Node *x = y->left;
  Node *t2 = x->right;
  x->right = y;
  y->left = t2;
  upd(y);
  upd(x);
  return x;
}

static Node *rotL(Node *x) {
  Node *y = x->right;
  Node *t2 = y->left;
  y->left = x;
  x->right = t2;
  upd(x);
  upd(y);
  return y;
}

static void display(Node *root, int depth) {
  if (!root) return;
  display(root->right, depth + 1);
  for (int i = 0; i < depth; i++) cout << "    ";
  cout << root->key << " (h=" << root->h << ", bf=" << bf(root) << ")\n";
  display(root->left, depth + 1);
}

static void freeTree(Node *root) {
  if (!root) return;
  freeTree(root->left);
  freeTree(root->right);
  delete root;
}

static Node *make3(int a, int b, int c) {
  // Make a simple BST insert order a,b,c (without balancing) so shapes match the AVL cases.
  Node *r = new Node(a);
  Node *n2 = new Node(b);
  Node *n3 = new Node(c);
  // Insert b
  if (b < r->key) r->left = n2;
  else r->right = n2;
  // Insert c
  Node *cur = r;
  while (true) {
    if (c < cur->key) {
      if (!cur->left) {
        cur->left = n3;
        break;
      }
      cur = cur->left;
    } else {
      if (!cur->right) {
        cur->right = n3;
        break;
      }
      cur = cur->right;
    }
  }
  // ensure heights are correct for display
  recomputeHeights(r);
  return r;
}

static void runLL() {
  cout << "\nLL case: insert 30,20,10 => Right rotation at 30\n";
  Node *root = make3(30, 20, 10);
  cout << "Before:\n";
  recomputeHeights(root);
  display(root, 0);
  cout << "RotateRight(30)\n";
  root = rotR(root);
  cout << "After:\n";
  recomputeHeights(root);
  display(root, 0);
  freeTree(root);
}

static void runRR() {
  cout << "\nRR case: insert 10,20,30 => Left rotation at 10\n";
  Node *root = make3(10, 20, 30);
  cout << "Before:\n";
  recomputeHeights(root);
  display(root, 0);
  cout << "RotateLeft(10)\n";
  root = rotL(root);
  cout << "After:\n";
  recomputeHeights(root);
  display(root, 0);
  freeTree(root);
}

static void runLR() {
  cout << "\nLR case: insert 30,10,20 => Left rotation at 10, then Right rotation at 30\n";
  Node *root = make3(30, 10, 20);
  cout << "Before:\n";
  recomputeHeights(root);
  display(root, 0);
  cout << "RotateLeft(10)\n";
  root->left = rotL(root->left);
  cout << "RotateRight(30)\n";
  root = rotR(root);
  cout << "After:\n";
  recomputeHeights(root);
  display(root, 0);
  freeTree(root);
}

static void runRL() {
  cout << "\nRL case: insert 10,30,20 => Right rotation at 30, then Left rotation at 10\n";
  Node *root = make3(10, 30, 20);
  cout << "Before:\n";
  recomputeHeights(root);
  display(root, 0);
  cout << "RotateRight(30)\n";
  root->right = rotR(root->right);
  cout << "RotateLeft(10)\n";
  root = rotL(root);
  cout << "After:\n";
  recomputeHeights(root);
  display(root, 0);
  freeTree(root);
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
    cout << "Invalid number. Try again.\n";
    clearInput();
  }
}

static char readLR(const string &prompt) {
  while (true) {
    cout << prompt;
    string s;
    cin >> s;
    if (!cin) {
      clearInput();
      continue;
    }
    if (s.size() == 1) {
      char c = s[0];
      if (c >= 'a' && c <= 'z') c = char(c - 'a' + 'A');
      if (c == 'L' || c == 'R') return c;
    }
    cout << "Enter L or R.\n";
  }
}

static int readRotationType() {
  int t;
  while (true) {
    cout << "Rotation type:\n";
    cout << "1. LL (Rotate RIGHT at node)\n";
    cout << "2. RR (Rotate LEFT at node)\n";
    cout << "3. LR (Rotate LEFT at left child, then RIGHT at node)\n";
    cout << "4. RL (Rotate RIGHT at right child, then LEFT at node)\n";
    cout << "Select: ";
    cin >> t;
    if (cin && t >= 1 && t <= 4) return t;
    cout << "Invalid option. Try again.\n";
    clearInput();
  }
}

static const char *rotationName(int t) {
  if (t == 1) return "LL";
  if (t == 2) return "RR";
  if (t == 3) return "LR";
  if (t == 4) return "RL";
  return "?";
}

static bool normalizePath(string &p) {
  // '.' means root
  if (p == "." || p == "root" || p == "ROOT" || p == "Root") {
    p = "";
    return true;
  }
  for (size_t i = 0; i < p.size(); i++) {
    char c = p[i];
    if (c >= 'a' && c <= 'z') c = char(c - 'a' + 'A');
    if (c != 'L' && c != 'R') return false;
    p[i] = c;
  }
  return true;
}

static Node **nodeLinkByPath(Node **rootLink, string path) {
  if (!normalizePath(path)) return NULL;
  Node **link = rootLink;
  for (size_t i = 0; i < path.size(); i++) {
    if (!*link) return NULL;
    if (path[i] == 'L') link = &((*link)->left);
    else link = &((*link)->right);
  }
  return link;
}

static string showPath(const string &p) { return p.empty() ? string(".") : p; }

struct LinkItem {
  Node **link;
  string path;  // "" for root, else "L...R"
  LinkItem() : link(NULL), path("") {}
  LinkItem(Node **l, const string &p) : link(l), path(p) {}
};

struct LinkQueue {
  LinkItem *a;
  int cap;
  int head;
  int tail;
  int n;

  LinkQueue(int c) {
    cap = (c < 8) ? 8 : c;
    a = new LinkItem[cap];
    head = tail = n = 0;
  }
  ~LinkQueue() { delete[] a; }

  bool empty() const { return n == 0; }

  void push(const LinkItem &x) {
    if (n == cap) {
      int newCap = cap * 2;
      LinkItem *b = new LinkItem[newCap];
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

  LinkItem pop() {
    if (n == 0) return LinkItem();
    LinkItem x = a[head];
    head = (head + 1) % cap;
    n--;
    return x;
  }
};

static Node **findByValueUnique(Node **rootLink, int key, string *foundPath) {
  if (!rootLink || !*rootLink) return NULL;
  LinkQueue q(32);
  q.push(LinkItem(rootLink, ""));
  while (!q.empty()) {
    LinkItem it = q.pop();
    if (!it.link || !*(it.link)) continue;
    Node *cur = *(it.link);
    if (cur->key == key) {
      if (foundPath) *foundPath = it.path;
      return it.link;
    }
    q.push(LinkItem(&(cur->left), it.path + "L"));
    q.push(LinkItem(&(cur->right), it.path + "R"));
  }
  return NULL;
}

static void insertLevelOrderNoDup(Node *&root, int key) {
  if (findByValueUnique(&root, key, NULL)) {
    cout << "Value " << key << " already exists. Duplicates not allowed.\n";
    return;
  }

  LinkQueue q(32);
  q.push(LinkItem(&root, ""));
  while (!q.empty()) {
    LinkItem it = q.pop();
    if (!it.link) break;
    if (!*(it.link)) {
      *(it.link) = new Node(key);
      cout << "Inserted " << key << " at path [" << showPath(it.path) << "]\n";
      recomputeHeights(root);
      return;
    }
    Node *cur = *(it.link);
    q.push(LinkItem(&(cur->left), it.path + "L"));
    q.push(LinkItem(&(cur->right), it.path + "R"));
  }
}

static void attachAtParentPathNoDup(Node *&root, string parentPath, char side, int key) {
  if (!root) {
    cout << "Tree is empty.\n";
    return;
  }
  if (findByValueUnique(&root, key, NULL)) {
    cout << "Value " << key << " already exists. Duplicates not allowed.\n";
    return;
  }
  if (!normalizePath(parentPath)) {
    cout << "Invalid parent path.\n";
    return;
  }

  Node **plink = nodeLinkByPath(&root, parentPath);
  if (!plink || !*plink) {
    cout << "Invalid parent path.\n";
    return;
  }
  Node *p = *plink;
  string childPath = parentPath + ((side == 'L') ? "L" : "R");
  if (side == 'L') {
    if (p->left) {
      cout << "LEFT child already exists at [" << showPath(childPath) << "].\n";
      return;
    }
    p->left = new Node(key);
  } else {
    if (p->right) {
      cout << "RIGHT child already exists at [" << showPath(childPath) << "].\n";
      return;
    }
    p->right = new Node(key);
  }
  recomputeHeights(root);
  cout << "Attached " << key << " at path [" << showPath(childPath) << "]\n";
}

static bool rotateAtLink(Node **link, int type) {
  if (!link || !*link) {
    cout << "Invalid target node.\n";
    return false;
  }
  Node *z = *link;
  if (type == 1) {
    // LL => right rotation at z
    if (!z->left) {
      cout << "LL needs a LEFT child at the node.\n";
      return false;
    }
    *link = rotR(z);
    return true;
  }
  if (type == 2) {
    // RR => left rotation at z
    if (!z->right) {
      cout << "RR needs a RIGHT child at the node.\n";
      return false;
    }
    *link = rotL(z);
    return true;
  }
  if (type == 3) {
    // LR => left rotation at left child, then right rotation at z
    if (!z->left || !z->left->right) {
      cout << "LR needs a LEFT child, and that LEFT child must have a RIGHT child.\n";
      return false;
    }
    z->left = rotL(z->left);
    *link = rotR(z);
    return true;
  }
  if (type == 4) {
    // RL => right rotation at right child, then left rotation at z
    if (!z->right || !z->right->left) {
      cout << "RL needs a RIGHT child, and that RIGHT child must have a LEFT child.\n";
      return false;
    }
    z->right = rotR(z->right);
    *link = rotL(z);
    return true;
  }
  cout << "Invalid rotation type.\n";
  return false;
}

static void rotateByValue(Node *&root) {
  if (!root) {
    cout << "Tree is empty. Insert some nodes first.\n";
    return;
  }
  int key = readInt("Node value: ");
  string where;
  Node **link = findByValueUnique(&root, key, &where);
  if (!link) {
    cout << "Value " << key << " not found.\n";
    return;
  }
  int type = readRotationType();
  cout << "Apply " << rotationName(type) << " at value " << key << " (path [" << showPath(where)
       << "])\n";
  if (rotateAtLink(link, type)) {
    recomputeHeights(root);
  }
}

static void rotateByPath(Node *&root) {
  if (!root) {
    cout << "Tree is empty. Insert some nodes first.\n";
    return;
  }
  string inPath;
  cout << "Node path (. for root): ";
  cin >> inPath;
  if (!cin) {
    clearInput();
    cout << "Invalid input.\n";
    return;
  }
  string path = inPath;
  if (!normalizePath(path)) {
    cout << "Invalid path.\n";
    return;
  }
  Node **link = nodeLinkByPath(&root, path);
  if (!link || !*link) {
    cout << "Invalid path.\n";
    return;
  }
  int beforeKey = (*link)->key;
  int type = readRotationType();
  cout << "Apply " << rotationName(type) << " at path [" << showPath(path) << "] (value "
       << beforeKey << ")\n";
  if (rotateAtLink(link, type)) {
    recomputeHeights(root);
  }
}

static void displayWithPath(Node *root, int depth, const string &path) {
  if (!root) return;
  displayWithPath(root->right, depth + 1, path + "R");
  for (int i = 0; i < depth; i++) cout << "    ";
  cout << root->key << " [" << (path.empty() ? string(".") : path)
       << "] (h=" << root->h << ", bf=" << bf(root) << ")\n";
  displayWithPath(root->left, depth + 1, path + "L");
}

static Node *makeSampleBT() {
  // A small non-BST-friendly sample (values don't matter for rotations)
  // Shape:
  //         10
  //       /    \ (right)
  //      5      20
  //     / \    / \ (right)
  //    3   7  15  30
  Node *r = new Node(10);
  r->left = new Node(5);
  r->right = new Node(20);
  r->left->left = new Node(3);
  r->left->right = new Node(7);
  r->right->left = new Node(15);
  r->right->right = new Node(30);
  recomputeHeights(r);
  return r;
}

static void runSandbox() {
  Node *root = NULL;
  int ch = 0;
  while (ch != 9) {
    cout << "\n=== Rotation Sandbox (General Binary Tree) ===\n";
    cout << "Rule: duplicates are NOT allowed\n";
    cout << "Insert methods: level-order (normal) OR attach-by-path (advanced)\n";
    cout << "Rotate methods: by VALUE (recommended) OR by PATH (advanced)\n";
    cout << "Path syntax: '.' for root, then L/R (example: LRL)\n";
    cout << "1. Load sample tree\n";
    cout << "2. Insert (level-order)\n";
    cout << "3. Insert (attach at parent path)\n";
    cout << "4. Rotate at value (choose LL/RR/LR/RL)\n";
    cout << "5. Rotate at path (choose LL/RR/LR/RL)\n";
    cout << "6. Display tree (with paths)\n";
    cout << "7. Clear tree\n";
    cout << "9. Back\n";
    cout << "Select: ";
    cin >> ch;
    if (!cin) {
      clearInput();
      ch = 0;
    }

    if (ch == 1) {
      freeTree(root);
      root = makeSampleBT();
      cout << "Sample tree loaded.\n";
      recomputeHeights(root);
      displayWithPath(root, 0, "");
    } else if (ch == 2) {
      int v = readInt("Value to insert: ");
      insertLevelOrderNoDup(root, v);
      if (!root) cout << "(empty)\n";
      else displayWithPath(root, 0, "");
    } else if (ch == 3) {
      if (!root) {
        int v = readInt("Tree empty. Root value: ");
        root = new Node(v);
        recomputeHeights(root);
        cout << "Created root " << v << " at path [.]\n";
        displayWithPath(root, 0, "");
        continue;
      }
      string parentPath;
      cout << "Parent path (. for root): ";
      cin >> parentPath;
      if (!cin) {
        clearInput();
        cout << "Invalid input.\n";
        continue;
      }
      char side = readLR("Attach as (L/R): ");
      int v = readInt("Child value: ");
      attachAtParentPathNoDup(root, parentPath, side, v);
      displayWithPath(root, 0, "");
    } else if (ch == 4) {
      rotateByValue(root);
      if (!root) cout << "(empty)\n";
      else displayWithPath(root, 0, "");
    } else if (ch == 5) {
      rotateByPath(root);
      if (!root) cout << "(empty)\n";
      else displayWithPath(root, 0, "");
    } else if (ch == 6) {
      if (!root) cout << "(empty)\n";
      else {
        recomputeHeights(root);
        displayWithPath(root, 0, "");
      }
    } else if (ch == 7) {
      freeTree(root);
      root = NULL;
      cout << "Cleared.\n";
    } else if (ch == 9) {
      // back
    } else {
      cout << "Invalid option.\n";
    }
  }
  freeTree(root);
}

int main() {
  int choice = 0;
  while (choice != 5) {
    cout << "\n=== T15: AVL Rotations Only (LL, RR, LR, RL) ===\n";
    cout << "1. LL\n";
    cout << "2. RR\n";
    cout << "3. LR\n";
    cout << "4. RL\n";
    cout << "5. Exit\n";
    cout << "6. Sandbox (rotate any binary tree)\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }
    if (choice == 1) {
      runLL();
    } else if (choice == 2) {
      runRR();
    } else if (choice == 3) {
      runLR();
    } else if (choice == 4) {
      runRL();
    }
    else if (choice == 5) cout << "Bye.\n";
    else if (choice == 6) runSandbox();
    else cout << "Invalid option.\n";
  }
  return 0;
}
