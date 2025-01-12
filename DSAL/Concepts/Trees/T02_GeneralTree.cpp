// 2. Tree with basic CRUD + properties (Leaf, height, depth, degree, siblings)
// Demo: General tree

#include <iostream>
#include <string>
using namespace std;

struct Node {
  string label;
  Node *firstChild;
  Node *nextSibling;

  Node(const string &s) : label(s), firstChild(NULL), nextSibling(NULL) {}
};

static void clearInput() {
  cin.clear();
  cin.ignore(1024, '\n');
}

static string readToken(const string &prompt) {
  cout << prompt;
  string s;
  cin >> s;
  return s;
}

static void freeTree(Node *root) {
  if (!root) return;
  Node *ch = root->firstChild;
  while (ch) {
    Node *nxt = ch->nextSibling;
    freeTree(ch);
    ch = nxt;
  }
  delete root;
}

static void display(Node *root, int depth) {
  if (!root) return;
  for (int i = 0; i < depth; i++) cout << "  ";
  cout << "- " << root->label << "\n";
  for (Node *ch = root->firstChild; ch; ch = ch->nextSibling) display(ch, depth + 1);
}

static Node *findNode(Node *root, const string &label) {
  if (!root) return NULL;
  if (root->label == label) return root;
  for (Node *ch = root->firstChild; ch; ch = ch->nextSibling) {
    Node *got = findNode(ch, label);
    if (got) return got;
  }
  return NULL;
}

static bool findWithParent(Node *root, const string &label, Node *&outNode, Node *&outParent) {
  outNode = NULL;
  outParent = NULL;
  if (!root) return false;
  if (root->label == label) {
    outNode = root;
    outParent = NULL;
    return true;
  }

  Node *parent = root;
  for (Node *ch = root->firstChild; ch; ch = ch->nextSibling) {
    if (ch->label == label) {
      outNode = ch;
      outParent = parent;
      return true;
    }
    Node *n2 = NULL;
    Node *p2 = NULL;
    if (findWithParent(ch, label, n2, p2)) {
      outNode = n2;
      outParent = p2;
      return true;
    }
  }
  return false;
}

static int degree(Node *node) {
  if (!node) return 0;
  int d = 0;
  for (Node *ch = node->firstChild; ch; ch = ch->nextSibling) d++;
  return d;
}

static int height(Node *node) {
  if (!node) return 0;
  int best = 0;
  for (Node *ch = node->firstChild; ch; ch = ch->nextSibling) {
    int h = height(ch);
    if (h > best) best = h;
  }
  return best + 1;
}

static int depthFrom(Node *root, const string &label, int depth) {
  if (!root) return -1;
  if (root->label == label) return depth;
  for (Node *ch = root->firstChild; ch; ch = ch->nextSibling) {
    int d = depthFrom(ch, label, depth + 1);
    if (d >= 0) return d;
  }
  return -1;
}

static void addChild(Node *parent, const string &childLabel) {
  if (!parent) return;
  Node *nn = new Node(childLabel);
  if (!parent->firstChild) {
    cout << "Parent '" << parent->label << "' has no child; set FC -> '"
         << childLabel << "'.\n";
    parent->firstChild = nn;
    return;
  }
  Node *cur = parent->firstChild;
  cout << "Walk siblings under '" << parent->label << "': " << cur->label;
  while (cur->nextSibling) {
    cur = cur->nextSibling;
    cout << " -> " << cur->label;
  }
  cout << "\n";
  cout << "Set NS of '" << cur->label << "' -> '" << childLabel << "'.\n";
  cur->nextSibling = nn;
}

static bool deleteSubtree(Node *&root, const string &label) {
  if (!root) return false;
  if (root->label == label) {
    cout << "Delete root '" << root->label << "'; whole tree is removed.\n";
    freeTree(root);
    root = NULL;
    return true;
  }

  // Find node + parent
  Node *node = NULL;
  Node *parent = NULL;
  if (!findWithParent(root, label, node, parent) || !node || !parent) return false;

  // Unlink node from parent's child list
  Node *prev = NULL;
  Node *cur = parent->firstChild;
  while (cur && cur != node) {
    cout << "Delete scan under '" << parent->label << "': skip '" << cur->label << "'\n";
    prev = cur;
    cur = cur->nextSibling;
  }
  if (!cur) return false;
  if (!prev) {
    cout << "Delete '" << cur->label << "': parent FC now points to "
         << (cur->nextSibling ? cur->nextSibling->label : string("NULL")) << "\n";
    parent->firstChild = cur->nextSibling;
  } else {
    cout << "Delete '" << cur->label << "': '" << prev->label
         << "'. NS now points to "
         << (cur->nextSibling ? cur->nextSibling->label : string("NULL")) << "\n";
    prev->nextSibling = cur->nextSibling;
  }
  cur->nextSibling = NULL;
  freeTree(cur);
  return true;
}

static void listSiblings(Node *root, const string &label) {
  Node *node = NULL;
  Node *parent = NULL;
  if (!findWithParent(root, label, node, parent) || !node) {
    cout << "Node not found.\n";
    return;
  }
  if (!parent) {
    cout << "Root has no siblings.\n";
    return;
  }
  cout << "Siblings of '" << label << "': ";
  int shown = 0;
  for (Node *ch = parent->firstChild; ch; ch = ch->nextSibling) {
    if (ch == node) continue;
    cout << ch->label << " ";
    shown++;
  }
  if (shown == 0) cout << "(none)";
  cout << "\n";
}

int main() {
  Node *root = NULL;
  int choice = 0;
  while (choice != 10) {
    cout << "\n=== T02: General Tree CRUD + Properties ===\n";
    cout << "1. Set root\n";
    cout << "2. Add child\n";
    cout << "3. Rename node\n";
    cout << "4. Delete subtree\n";
    cout << "5. Display tree\n";
    cout << "6. Is leaf?\n";
    cout << "7. Degree of node\n";
    cout << "8. Height and depth of node\n";
    cout << "9. List siblings\n";
    cout << "10. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      string r = readToken("Root label: ");
      freeTree(root);
      root = new Node(r);
      cout << "Root set to '" << r << "'.\n";
    } else if (choice == 2) {
      if (!root) {
        cout << "Set root first.\n";
        continue;
      }
      string parent = readToken("Parent label: ");
      Node *p = findNode(root, parent);
      if (!p) {
        cout << "Parent not found.\n";
        continue;
      }
      string child = readToken("Child label: ");
      if (findNode(root, child)) {
        cout << "Label already exists. Use unique labels.\n";
        continue;
      }
      addChild(p, child);
      cout << "Added '" << child << "' under '" << parent << "'.\n";
    } else if (choice == 3) {
      if (!root) {
        cout << "Tree empty.\n";
        continue;
      }
      string oldL = readToken("Existing label: ");
      Node *n = findNode(root, oldL);
      if (!n) {
        cout << "Node not found.\n";
        continue;
      }
      string newL = readToken("New label: ");
      if (findNode(root, newL)) {
        cout << "New label already exists.\n";
        continue;
      }
      n->label = newL;
      cout << "Renamed '" << oldL << "' -> '" << newL << "'.\n";
    } else if (choice == 4) {
      if (!root) {
        cout << "Tree empty.\n";
        continue;
      }
      string label = readToken("Delete subtree rooted at: ");
      if (deleteSubtree(root, label)) cout << "Deleted.\n";
      else cout << "Node not found; no delete performed.\n";
    } else if (choice == 5) {
      if (!root) cout << "(empty)\n";
      else display(root, 0);
    } else if (choice == 6) {
      if (!root) {
        cout << "Tree empty.\n";
        continue;
      }
      string label = readToken("Node label: ");
      Node *n = findNode(root, label);
      if (!n) cout << "Node not found.\n";
      else cout << (n->firstChild ? "Not a leaf.\n" : "Leaf node.\n");
    } else if (choice == 7) {
      if (!root) {
        cout << "Tree empty.\n";
        continue;
      }
      string label = readToken("Node label: ");
      Node *n = findNode(root, label);
      if (!n) cout << "Node not found.\n";
      else cout << "Degree: " << degree(n) << "\n";
    } else if (choice == 8) {
      if (!root) {
        cout << "Tree empty.\n";
        continue;
      }
      string label = readToken("Node label: ");
      Node *n = findNode(root, label);
      if (!n) {
        cout << "Node not found.\n";
        continue;
      }
      cout << "Height (From node): " << height(n) << "\n";
      cout << "Depth (From root): " << depthFrom(root, label, 0) << "\n";
    } else if (choice == 9) {
      if (!root) {
        cout << "Tree empty.\n";
        continue;
      }
      string label = readToken("Node label: ");
      listSiblings(root, label);
    } else if (choice == 10) {
      cout << "Exiting...\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  freeTree(root);
  return 0;
}
