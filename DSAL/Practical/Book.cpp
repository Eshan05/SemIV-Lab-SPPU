#include <cstring>
#include <iostream>

#define MAX_CHILDREN 10
#define MAX_NAME_LEN 100

using namespace std;

class Node {
 public:
  char name[MAX_NAME_LEN];
  int childCount;
  Node *children[MAX_CHILDREN];

  Node() {
    childCount = 0;
    for (int i = 0; i < MAX_CHILDREN; ++i)
      children[i] = NULL;
  }

  ~Node() {
    for (int i = 0; i < childCount; ++i) delete children[i];
  }

  void setName(const char *newName) {
    strncpy(name, newName, MAX_NAME_LEN);
    name[MAX_NAME_LEN - 1] = '\0';
  }

  void addChild(Node *child) {
    if (childCount < MAX_CHILDREN) {
      children[childCount++] = child;
    } else {
      cout << "Maximum children reached for " << name << endl;
      delete child;
    }
  }
};

void printTree(Node *node, int level = 0) {
  if (node == NULL) return;
  for (int i = 0; i < level; ++i) cout << "  ";
  cout << "- " << node->name << endl;
  for (int i = 0; i < node->childCount; ++i)
    printTree(node->children[i], level + 1);
}

void displayChapters(Node *book) {
  for (int i = 0; i < book->childCount; ++i) {
    cout << i + 1 << ". " << book->children[i]->name << endl;
  }
}

void displaySections(Node *chapter) {
  for (int i = 0; i < chapter->childCount; ++i) {
    cout << i + 1 << ". " << chapter->children[i]->name << endl;
  }
}

Node *getChild(Node *parent, int index) {
  if (!parent || index < 0 || index >= parent->childCount) {
    return nullptr; // Indicate invalid index
  }
  return parent->children[index];
}

int main() {
  Node *book = new Node();
  char bookTitle[MAX_NAME_LEN];
  cout << "Enter Book Title: ";
  cin.getline(bookTitle, MAX_NAME_LEN);
  book->setName(bookTitle);

  int choice;
  do {
    cout << "\nMenu:\n";
    cout << "1. Add Chapter\n";
    cout << "2. Add Section\n";
    cout << "3. Add Subsection\n";
    cout << "4. Display Book Structure\n";
    cout << "5. Exit\n";
    cout << "Enter your choice: ";
    cin >> choice;
    cin.ignore();

    switch (choice) {
      case 1: {
        Node *chapter = new Node();
        char name[MAX_NAME_LEN];
        cout << "Enter Chapter Name: ";
        cin.getline(name, MAX_NAME_LEN);
        chapter->setName(name);
        book->addChild(chapter);
        break;
      }
      case 2: {
        if (book->childCount == 0) {
          cout << "No chapters exist. Add a chapter first.\n";
          break;
        }

        cout << "Select Chapter to add Section:\n";
        displayChapters(book);
        int ch;
        cout << "Enter choice: ";
        cin >> ch;
        cin.ignore();
        if (ch < 1 || ch > book->childCount) {
          cout << "Invalid chapter number.\n";
          break;
        }

        Node *chapter = book->children[ch - 1];
        Node *section = new Node();
        char name[MAX_NAME_LEN];
        cout << "Enter Section Name: ";
        cin.getline(name, MAX_NAME_LEN);
        section->setName(name);
        chapter->addChild(section);
        break;
      }
      case 3: {
        if (book->childCount == 0) {
          cout << "No chapters exist. Add a chapter first.\n";
          break;
        }

        cout << "Select Chapter:\n";
        displayChapters(book);
        int ch;
        cout << "Enter chapter number: ";
        cin >> ch;
        cin.ignore();
        if (ch < 1 || ch > book->childCount) {
          cout << "Invalid chapter number.\n";
          break;
        }

        Node *chapter = book->children[ch - 1];

        if (chapter->childCount == 0) {
          cout << "No sections exist in this chapter. Add a section first.\n";
          break;
        }

        cout << "Select Section:\n";
        displaySections(chapter);
        int sec;
        cout << "Enter section number: ";
        cin >> sec;
        cin.ignore();
        if (sec < 1 || sec > chapter->childCount) {
          cout << "Invalid section number.\n";
          break;
        }

        Node *section = chapter->children[sec - 1];
        Node *subsection = new Node();
        char name[MAX_NAME_LEN];
        cout << "Enter Subsection Name: ";
        cin.getline(name, MAX_NAME_LEN);
        subsection->setName(name);
        section->addChild(subsection);
        break;
      }
      case 4: {
        printTree(book);
        break;
      }
      case 5: {
        cout << "Exiting...\n";
        break;
      }
      default: {
        cout << "Invalid choice. Please try again.\n";
        break;
      }
    }
  } while (choice != 5);

  return 0;
}