// 4. General tree -> Binary tree conversion
// Concept: represent an N-ary tree as a binary tree using Left-Child Right-Sibling (LCRS).
// Demo: build general tree, then show its LCRS binary form.

#include <iostream>
#include <string>
using namespace std;

struct GNode {
  string label;
  GNode *firstChild;
  GNode *nextSibling;
  GNode(const string &s) : label(s), firstChild(NULL), nextSibling(NULL) {}
};

struct BNode {
  string label;
  BNode *left;  // first child
  BNode *right; // next sibling
  BNode(const string &s) : label(s), left(NULL), right(NULL) {}
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

static void freeGeneral(GNode *root) {
  if (!root) return;
  GNode *ch = root->firstChild;
  while (ch) {
    GNode *nxt = ch->nextSibling;
    freeGeneral(ch);
    ch = nxt;
  }
  delete root;
}

static void freeBinary(BNode *root) {
  if (!root) return;
  freeBinary(root->left);
  freeBinary(root->right);
  delete root;
}

static GNode *findGeneral(GNode *root, const string &label) {
  if (!root) return NULL;
  if (root->label == label) return root;
  for (GNode *ch = root->firstChild; ch; ch = ch->nextSibling) {
    GNode *got = findGeneral(ch, label);
    if (got) return got;
  }
  return NULL;
}

static void addChild(GNode *parent, const string &childLabel) {
  if (!parent) return;
  GNode *nn = new GNode(childLabel);
  if (!parent->firstChild) {
    cout << parent->label << ".firstChild <- " << childLabel << "\n";
    parent->firstChild = nn;
    return;
  }
  GNode *cur = parent->firstChild;
  cout << "Walk children of " << parent->label << ": " << cur->label;
  while (cur->nextSibling) {
    cur = cur->nextSibling;
    cout << " -> " << cur->label;
  }
  cout << "\n";
  cout << cur->label << ".nextSibling <- " << childLabel << "\n";
  cur->nextSibling = nn;
}

static void displayGeneral(GNode *root, int depth) {
  if (!root) return;
  for (int i = 0; i < depth; i++) cout << "  ";
  cout << "- " << root->label << "\n";
  for (GNode *ch = root->firstChild; ch; ch = ch->nextSibling) displayGeneral(ch, depth + 1);
}

static BNode *convertToLCRS(GNode *g, const string &from) {
  if (!g) return NULL;
  cout << "Create binary node " << g->label << " from " << from << "\n";
  BNode *b = new BNode(g->label);
  if (g->firstChild) cout << g->label << ".left <- " << g->firstChild->label << "\n";
  b->left = convertToLCRS(g->firstChild, string("first child of ") + g->label);
  if (g->nextSibling) cout << g->label << ".right <- " << g->nextSibling->label << "\n";
  b->right = convertToLCRS(g->nextSibling, string("next sibling of ") + g->label);
  return b;
}

static void displayBinaryLCRS(BNode *root, int depth, const string &edge) {
  if (!root) return;
  for (int i = 0; i < depth; i++) cout << "  ";
  cout << edge << root->label << "\n";
  displayBinaryLCRS(root->left, depth + 1, "L(child)-> ");
  displayBinaryLCRS(root->right, depth + 1, "R(sib)-> ");
}

int main() {
  GNode *groot = NULL;
  BNode *broot = NULL;
  int choice = 0;

  while (choice != 6) {
    cout << "\n=== T04: General Tree -> Binary Tree (LCRS) ===\n";
    cout << "1. Set/Replace root\n";
    cout << "2. Add child\n";
    cout << "3. Display general tree\n";
    cout << "4. Convert + display LCRS binary form\n";
    cout << "5. Clear\n";
    cout << "6. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      string r = readToken("Root label: ");
      freeGeneral(groot);
      groot = new GNode(r);
      freeBinary(broot);
      broot = NULL;
      cout << "Root set.\n";
    } else if (choice == 2) {
      if (!groot) {
        cout << "Set root first.\n";
        continue;
      }
      string p = readToken("Parent label: ");
      GNode *parent = findGeneral(groot, p);
      if (!parent) {
        cout << "Parent not found.\n";
        continue;
      }
      string c = readToken("Child label: ");
      if (findGeneral(groot, c)) {
        cout << "Label already exists; use a unique label so parent lookup is not ambiguous.\n";
        continue;
      }
      addChild(parent, c);
      cout << "Added '" << c << "' under '" << p << "'.\n";
    } else if (choice == 3) {
      if (!groot) cout << "(empty)\n";
      else displayGeneral(groot, 0);
    } else if (choice == 4) {
      freeBinary(broot);
      broot = convertToLCRS(groot, "general root");
      if (!broot) {
        cout << "(empty)\n";
      } else {
        cout << "\nBinary view (L=first child, R=next sibling):\n";
        displayBinaryLCRS(broot, 0, "ROOT-> ");
      }
    } else if (choice == 5) {
      freeGeneral(groot);
      freeBinary(broot);
      groot = NULL;
      broot = NULL;
      cout << "Cleared.\n";
    } else if (choice == 6) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }

  freeGeneral(groot);
  freeBinary(broot);
  return 0;
}
