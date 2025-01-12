// 1. Simple tree structure construction
// Demo: Build a simple general tree

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

static Node *findNode(Node *root, const string &label) {
  if (!root) return NULL;
  if (root->label == label) return root;
  for (Node *ch = root->firstChild; ch; ch = ch->nextSibling) {
    Node *got = findNode(ch, label);
    if (got) return got;
  }
  return NULL;
}

static int countNodes(Node *root) {
  if (!root) return 0;
  int total = 1;
  for (Node *ch = root->firstChild; ch; ch = ch->nextSibling) total += countNodes(ch);
  return total;
}

static int height(Node *root) {
  if (!root) return 0;
  int best = 0;
  for (Node *ch = root->firstChild; ch; ch = ch->nextSibling) {
    int h = height(ch);
    if (h > best) best = h;
  }
  return best + 1;
}

static void display(Node *root, int depth) {
  if (!root) return;
  for (int i = 0; i < depth; i++) cout << "  ";
  cout << "- " << root->label << "\n";
  for (Node *ch = root->firstChild; ch; ch = ch->nextSibling) display(ch, depth + 1);
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

static void addChild(Node *parent, const string &childLabel) {
  if (!parent) return;
  Node *nn = new Node(childLabel);
  if (!parent->firstChild) {
    cout << "Parent '" << parent->label << "' has no child. Set FC -> '"
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

int main() {
  Node *root = NULL;
  int choice = 0;
  while (choice != 6) {
    cout << "\n=== T01: Simple General Tree (first-child / next-sibling) ===\n";
    cout << "1. Set root\n";
    cout << "2. Insert node\n";
    cout << "3. Display tree (outline)\n";
    cout << "4. Count nodes\n";
    cout << "5. Height of tree\n";
    cout << "6. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      string r = readToken("Root label (No spaces): ");
      freeTree(root);
      root = new Node(r);
      cout << "Root set to '" << r << "'.\n";
    } else if (choice == 2) {
      if (!root) {
        cout << "Tree is empty. Set root first.\n";
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
        cout << "Label already exists.\n";
        continue;
      }
      addChild(p, child);
      cout << "Added child '" << child << "' under '" << parent << "'.\n";
    } else if (choice == 3) {
      if (!root) cout << "(empty)\n";
      else display(root, 0);
    } else if (choice == 4) {
      cout << "Nodes: " << countNodes(root) << "\n";
    } else if (choice == 5) {
      cout << "Height (Levels): " << height(root) << "\n";
    } else if (choice == 6) {
      cout << "Exiting...\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  freeTree(root);
  return 0;
}
