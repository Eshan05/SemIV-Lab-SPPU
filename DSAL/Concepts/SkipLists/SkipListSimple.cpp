#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;

const int SIMPLE_MAX_LEVEL = 5;
const float SIMPLE_PROBABILITY = 0.6f;

struct SimpleNode {
  int key;
  SimpleNode **next;

  SimpleNode(int new_key, int height) : key(new_key) {
    next = new SimpleNode *[height];
    for (int i = 0; i < height; ++i)
      next[i] = nullptr;
  }

  ~SimpleNode() { delete[] next; }
};

class SimpleSkipList {
  int level;
  SimpleNode *header;

  int randomLevel() const {
    int lvl = 1;
    while (lvl < SIMPLE_MAX_LEVEL && (static_cast<float>(rand()) / RAND_MAX) < SIMPLE_PROBABILITY)
      ++lvl;
    return lvl;
  }

 public:
  SimpleSkipList() : level(1) { header = new SimpleNode(-1, SIMPLE_MAX_LEVEL); }

  ~SimpleSkipList() {
    SimpleNode *current = header->next[0];
    while (current) {
      SimpleNode *next = current->next[0];
      delete current;
      current = next;
    }
    delete header;
  }

  void insert(int key) {
    SimpleNode *update[SIMPLE_MAX_LEVEL];
    SimpleNode *current = header;
    cout << "\n[Simple] Traversing to insert " << key << ":\n";
    for (int i = level - 1; i >= 0; --i) {
      cout << " Level " << i << " visits";
      while (current->next[i] && current->next[i]->key < key) {
        cout << " -> " << current->next[i]->key;
        current = current->next[i];
      }
      update[i] = current;
      cout << " (stops at " << (current->next[i] ? current->next[i]->key : -1) << ")\n";
    }
    current = current->next[0];
    if (current && current->key == key) {
      cout << " Simple list already contains " << key << ", skipping insert.\n";
      return;
    }

    int newLevel = randomLevel();
    if (newLevel > level) {
      for (int i = level; i < newLevel; ++i)
        update[i] = header;
      level = newLevel;
      cout << " Growing height to " << level << " for new node.\n";
    }

    SimpleNode *node = new SimpleNode(key, SIMPLE_MAX_LEVEL);
    for (int i = 0; i < newLevel; ++i) {
      node->next[i] = update[i]->next[i];
      update[i]->next[i] = node;
      cout << " Linking level " << i << " to " << key << "\n";
    }
  }

  bool search(int key) const {
    SimpleNode *current = header;
    cout << "\n[Simple] Search path for " << key << ":\n";
    for (int i = level - 1; i >= 0; --i) {
      cout << " Level " << i << " ->";
      while (current->next[i] && current->next[i]->key < key) {
        cout << " " << current->next[i]->key;
        current = current->next[i];
      }
      cout << " (stops at " << (current->next[i] ? current->next[i]->key : -1) << ")\n";
    }
    current = current->next[0];
    if (current && current->key == key) {
      cout << " Found " << key << " at bottom level.\n";
      return true;
    }
    cout << " Not found.\n";
    return false;
  }

  bool erase(int key) {
    SimpleNode *update[SIMPLE_MAX_LEVEL];
    SimpleNode *current = header;
    cout << "\n[Simple] Locating " << key << " to delete:\n";
    for (int i = level - 1; i >= 0; --i) {
      cout << " Level " << i << " path";
      while (current->next[i] && current->next[i]->key < key) {
        cout << " -> " << current->next[i]->key;
        current = current->next[i];
      }
      update[i] = current;
      cout << " (prep to bypass " << (current->next[i] ? current->next[i]->key : -1) << ")\n";
    }
    current = current->next[0];
    if (!current || current->key != key) {
      cout << " Element " << key << " not present.\n";
      return false;
    }

    for (int i = 0; i < level; ++i) {
      if (update[i]->next[i] != current)
        break;
      update[i]->next[i] = current->next[i];
      cout << " Bypassing " << key << " at level " << i << "\n";
    }

    delete current;
    while (level > 1 && header->next[level - 1] == nullptr)
      --level;
    return true;
  }

  void dumpLevels() const {
    cout << "\n[Simple] Skip list levels:\n";
    for (int i = level - 1; i >= 0; --i) {
      cout << " Level " << i << ": ";
      SimpleNode *node = header->next[i];
      if (!node)
        cout << "<empty>";
      while (node) {
        cout << node->key << " ";
        node = node->next[i];
      }
      cout << "\n";
    }
  }
};

int main() {
  srand(static_cast<unsigned>(time(nullptr)));
  SimpleSkipList simple;
  int choice = 0;
  while (choice != 5) {
    cout << "\n=== Simple Skip List Walk-through ===\n";
    cout << "1. Insert an integer" << "\n";
    cout << "2. Search for an integer" << "\n";
    cout << "3. Delete an integer" << "\n";
    cout << "4. Show levels" << "\n";
    cout << "5. Exit" << "\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      cin.clear();
      cin.ignore(1024, '\n');
      choice = 0;
    }
    switch (choice) {
      case 1: {
        int key;
        cout << " Enter integer to insert: ";
        cin >> key;
        simple.insert(key);
        break;
      }
      case 2: {
        int key;
        cout << " Enter integer to search: ";
        cin >> key;
        simple.search(key);
        break;
      }
      case 3: {
        int key;
        cout << " Enter integer to delete: ";
        cin >> key;
        if (simple.erase(key))
          cout << " Removal confirmed.\n";
        break;
      }
      case 4:
        simple.dumpLevels();
        break;
      case 5:
        cout << " Bye from simple demo.\n";
        break;
      default:
        cout << " Invalid input.\n";
    }
  }
  return 0;
}
