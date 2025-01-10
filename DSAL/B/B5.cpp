// 5. A book consists of chapters, chapters consist of sections and sections consist of subsections. Construct a tree and print the nodes. Find the time and space requirements of your method.

#include <iostream>
#include <string>
using namespace std;

struct Node {
  string name;
  int type; // 0 = Book, 1 = C, 2 = S, 3 = SS
  Node *parent;
  Node *firstChild;
  Node *nextSibling;
};

void clearLine() {
  char c;
  while (cin.get(c)) {
    if (c == '\n') break;
  }
}

int readInt(const string &prompt) {
  int x;
  while (true) {
    cout << prompt;
    if (cin >> x) return x;
    cout << "Invalid number. Try again.\n";
    cin.clear();
    clearLine();
  }
}

string readLine(const string &prompt) {
  cout << prompt;
  string s;
  getline(cin >> ws, s);
  return s;
}

Node *createNode(const string &name, int type) {
  Node *n = new Node;
  n->name = name;
  n->type = type;
  n->parent = 0;
  n->firstChild = 0;
  n->nextSibling = 0;
  return n;
}

void addChild(Node *parent, Node *child) {
  child->parent = parent;
  if (parent->firstChild == 0) {
    parent->firstChild = child;
    return;
  }
  Node *cur = parent->firstChild;
  while (cur->nextSibling != 0) cur = cur->nextSibling;
  cur->nextSibling = child;
}

Node *findChildByName(Node *parent, const string &name, int type) {
  if (parent == 0) return 0;
  Node *cur = parent->firstChild;
  while (cur != 0) {
    if (cur->type == type && cur->name == name) return cur;
    cur = cur->nextSibling;
  }
  return 0;
}

void detachFromParent(Node *node) {
  if (node == 0 || node->parent == 0) return;
  Node *p = node->parent;
  Node *cur = p->firstChild;
  Node *prev = 0;
  while (cur != 0) {
    if (cur == node) {
      if (prev == 0) p->firstChild = cur->nextSibling;
      else prev->nextSibling = cur->nextSibling;
      cur->nextSibling = 0;
      cur->parent = 0;
      return;
    }
    prev = cur;
    cur = cur->nextSibling;
  }
}

void deleteSubtree(Node *root) {
  if (root == 0) return;
  Node *ch = root->firstChild;
  while (ch != 0) {
    Node *nxt = ch->nextSibling;
    deleteSubtree(ch);
    ch = nxt;
  }
  delete root;
}

string typeLabel(int t) {
  if (t == 0) return "Book";
  if (t == 1) return "Chapter";
  if (t == 2) return "Section";
  return "Subsection";
}

void displayTree(Node *root, int depth) {
  if (root == 0) return;
  for (int i = 0; i < depth; ++i) cout << "  ";
  cout << "- " << typeLabel(root->type) << ": " << root->name << "\n";
  Node *ch = root->firstChild;
  while (ch != 0) {
    displayTree(ch, depth + 1);
    ch = ch->nextSibling;
  }
}

void listChildren(Node *parent, int childType) {
  if (parent == 0) return;
  Node *cur = parent->firstChild;
  int idx = 1;
  while (cur != 0) {
    if (cur->type == childType) {
      cout << idx << ". " << cur->name << "\n";
      ++idx;
    }
    cur = cur->nextSibling;
  }
  if (idx == 1) cout << "(none)\n";
}

int main() {
  Node *book = 0;

  while (true) {
    cout << "\n===== Book Tree TUI =====\n";
    cout << "1. Create Book\n";
    cout << "2. Add Chapter\n";
    cout << "3. Add Section under Chapter\n";
    cout << "4. Add Subsection under Section\n";
    cout << "5. Edit Name\n";
    cout << "6. Delete Node\n";
    cout << "7. Display Book Tree\n";
    cout << "8. Exit\n";

    int ch = readInt("Enter choice: ");

    if (ch == 1) {
      string name = readLine("Enter book title: ");
      if (book != 0) {
        int yn = readInt("Book already exists. Replace it? (1=yes, 0=no): ");
        if (yn == 1) {
          deleteSubtree(book);
          book = createNode(name, 0);
          cout << "Book recreated\n";
        } else {
          cout << "Cancelled\n";
        }
      } else {
        book = createNode(name, 0);
        cout << "Book created\n";
      }

    } else if (ch == 2) {
      if (book == 0) {
        cout << "Create book first\n";
        continue;
      }
      string cname = readLine("Enter chapter name: ");
      if (findChildByName(book, cname, 1) != 0) {
        cout << "Chapter already exists\n";
      } else {
        addChild(book, createNode(cname, 1));
        cout << "Chapter added\n";
      }

    } else if (ch == 3) {
      if (book == 0) {
        cout << "Create book first\n";
        continue;
      }
      cout << "Available chapters:\n";
      listChildren(book, 1);
      string cname = readLine("Enter chapter name where section is to be added: ");
      Node *chapter = findChildByName(book, cname, 1);
      if (chapter == 0) {
        cout << "Chapter not found\n";
      } else {
        string sname = readLine("Enter section name: ");
        if (findChildByName(chapter, sname, 2) != 0) {
          cout << "Section already exists in this chapter\n";
        } else {
          addChild(chapter, createNode(sname, 2));
          cout << "Section added\n";
        }
      }

    } else if (ch == 4) {
      if (book == 0) {
        cout << "Create book first\n";
        continue;
      }
      string cname = readLine("Enter chapter name: ");
      Node *chapter = findChildByName(book, cname, 1);
      if (chapter == 0) {
        cout << "Chapter not found\n";
        continue;
      }
      cout << "Sections under chapter '" << cname << "':\n";
      listChildren(chapter, 2);
      string sname = readLine("Enter section name: ");
      Node *section = findChildByName(chapter, sname, 2);
      if (section == 0) {
        cout << "Section not found\n";
      } else {
        string ssname = readLine("Enter subsection name: ");
        if (findChildByName(section, ssname, 3) != 0) {
          cout << "Subsection already exists in this section\n";
        } else {
          addChild(section, createNode(ssname, 3));
          cout << "Subsection added\n";
        }
      }

    } else if (ch == 5) {
      if (book == 0) {
        cout << "Create book first\n";
        continue;
      }
      cout << "Edit type: 0=Book, 1=Chapter, 2=Section, 3=Subsection\n";
      int t = readInt("Enter type: ");

      if (t == 0) {
        string newName = readLine("Enter new book name: ");
        book->name = newName;
        cout << "Book renamed\n";

      } else if (t == 1) {
        string oldName = readLine("Enter chapter name to edit: ");
        Node *chapter = findChildByName(book, oldName, 1);
        if (chapter == 0) cout << "Chapter not found\n";
        else {
          string newName = readLine("Enter new chapter name: ");
          chapter->name = newName;
          cout << "Chapter renamed\n";
        }

      } else if (t == 2) {
        string cname = readLine("Enter chapter name: ");
        Node *chapter = findChildByName(book, cname, 1);
        if (chapter == 0) {
          cout << "Chapter not found\n";
        } else {
          string oldName = readLine("Enter section name to edit: ");
          Node *section = findChildByName(chapter, oldName, 2);
          if (section == 0) cout << "Section not found\n";
          else {
            string newName = readLine("Enter new section name: ");
            section->name = newName;
            cout << "Section renamed\n";
          }
        }

      } else if (t == 3) {
        string cname = readLine("Enter chapter name: ");
        Node *chapter = findChildByName(book, cname, 1);
        if (chapter == 0) {
          cout << "Chapter not found\n";
        } else {
          string sname = readLine("Enter section name: ");
          Node *section = findChildByName(chapter, sname, 2);
          if (section == 0) {
            cout << "Section not found\n";
          } else {
            string oldName = readLine("Enter subsection name to edit: ");
            Node *sub = findChildByName(section, oldName, 3);
            if (sub == 0) cout << "Subsection not found\n";
            else {
              string newName = readLine("Enter new subsection name: ");
              sub->name = newName;
              cout << "Subsection renamed\n";
            }
          }
        }
      } else {
        cout << "Invalid type\n";
      }

    } else if (ch == 6) {
      if (book == 0) {
        cout << "Create book first\n";
        continue;
      }
      cout << "Delete type: 0=Book, 1=Chapter, 2=Section, 3=Subsection\n";
      int t = readInt("Enter type: ");

      if (t == 0) {
        int yn = readInt("Delete whole book tree? (1=yes, 0=no): ");
        if (yn == 1) {
          deleteSubtree(book);
          book = 0;
          cout << "Book tree deleted\n";
        } else {
          cout << "Cancelled\n";
        }

      } else if (t == 1) {
        string cname = readLine("Enter chapter name to delete: ");
        Node *chapter = findChildByName(book, cname, 1);
        if (chapter == 0) cout << "Chapter not found\n";
        else {
          detachFromParent(chapter);
          deleteSubtree(chapter);
          cout << "Chapter deleted\n";
        }

      } else if (t == 2) {
        string cname = readLine("Enter chapter name: ");
        Node *chapter = findChildByName(book, cname, 1);
        if (chapter == 0) cout << "Chapter not found\n";
        else {
          string sname = readLine("Enter section name to delete: ");
          Node *section = findChildByName(chapter, sname, 2);
          if (section == 0) cout << "Section not found\n";
          else {
            detachFromParent(section);
            deleteSubtree(section);
            cout << "Section deleted\n";
          }
        }

      } else if (t == 3) {
        string cname = readLine("Enter chapter name: ");
        Node *chapter = findChildByName(book, cname, 1);
        if (chapter == 0) cout << "Chapter not found\n";
        else {
          string sname = readLine("Enter section name: ");
          Node *section = findChildByName(chapter, sname, 2);
          if (section == 0) cout << "Section not found\n";
          else {
            string ssname = readLine("Enter subsection name to delete: ");
            Node *sub = findChildByName(section, ssname, 3);
            if (sub == 0) cout << "Subsection not found\n";
            else {
              detachFromParent(sub);
              deleteSubtree(sub);
              cout << "Subsection deleted\n";
            }
          }
        }
      } else {
        cout << "Invalid type\n";
      }

    } else if (ch == 7) {
      if (book == 0) cout << "No book created yet\n";
      else displayTree(book, 0);

    } else if (ch == 8) {
      break;

    } else {
      cout << "Invalid choice\n";
    }
  }

  deleteSubtree(book);
  return 0;
}
