#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <utility>
using namespace std;

template <class K, class V>
struct skipNode {
  typedef pair<const K, V> pair_type;
  pair_type element;
  skipNode<K, V> **next;

  skipNode(const pair_type &new_pair, int MAX) : element(new_pair) {
    next = new skipNode<K, V> *[MAX];
    for (int i = 0; i < MAX; ++i)
      next[i] = nullptr;
  }

  ~skipNode() { delete[] next; }
};

template <class K, class V>
class SkipList {
  static const int MAX_LEVEL = 6;
  float probability;
  int level;
  skipNode<K, V> *header;

 public:
  SkipList(float prob = 0.5f) : probability(prob), level(1) {
    typename skipNode<K, V>::pair_type sentinel(K{}, V{});
    header = new skipNode<K, V>(sentinel, MAX_LEVEL);
  }

  ~SkipList() {
    clear();
    delete header;
  }

  void clear() {
    skipNode<K, V> *current = header->next[0];
    while (current) {
      skipNode<K, V> *next = current->next[0];
      delete current;
      current = next;
    }
    for (int i = 0; i < MAX_LEVEL; ++i)
      header->next[i] = nullptr;
    level = 1;
  }

  int randomLevel() const {
    int lvl = 1;
    while (lvl < MAX_LEVEL && (static_cast<float>(rand()) / RAND_MAX) < probability)
      ++lvl;
    return lvl;
  }

  void display() const {
    cout << "\nCurrent skip list layout by level:\n";
    for (int i = level - 1; i >= 0; --i) {
      cout << " Level " << i << ": ";
      skipNode<K, V> *node = header->next[i];
      if (!node) {
        cout << "<empty>";
      } else {
        while (node) {
          cout << "[" << node->element.first << ":" << node->element.second << "] ";
          node = node->next[i];
        }
      }
      cout << "\n";
    }
  }

  void insert(const K &key, const V &value) {
    skipNode<K, V> *update[MAX_LEVEL];
    skipNode<K, V> *current = header;
    cout << "\nSearching for insert position for key '" << key << "':\n";
    for (int i = level - 1; i >= 0; --i) {
      cout << " Level " << i << " traversal:";
      while (current->next[i] && current->next[i]->element.first < key) {
        cout << " -> " << current->next[i]->element.first;
        current = current->next[i];
      }
      cout << " (stop at ";
      if (current->next[i])
        cout << current->next[i]->element.first << ")\n";
      else
        cout << "end)\n";
      update[i] = current;
    }

    skipNode<K, V> *candidate = current->next[0];
    if (candidate && candidate->element.first == key) {
      cout << " Key found, updating value and preserving structure.\n";
      candidate->element.second = value;
      return;
    }

    int newLevel = randomLevel();
    if (newLevel > level) {
      cout << " Expanding list from level " << level << " to " << newLevel << "\n";
      for (int i = level; i < newLevel; ++i)
        update[i] = header;
      level = newLevel;
    }

    skipNode<K, V> *inserted = new skipNode<K, V>(typename skipNode<K, V>::pair_type(key, value), MAX_LEVEL);
    for (int i = 0; i < newLevel; ++i) {
      inserted->next[i] = update[i]->next[i];
      update[i]->next[i] = inserted;
      cout << " Linking level " << i << " successor to " << key << "\n";
    }
  }

  bool search(const K &key) const {
    skipNode<K, V> *current = header;
    cout << "\nSearching for key '" << key << "':\n";
    for (int i = level - 1; i >= 0; --i) {
      cout << " Level " << i << " path:";
      while (current->next[i] && current->next[i]->element.first < key) {
        cout << " " << current->next[i]->element.first;
        current = current->next[i];
      }
      cout << " (stopped at ";
      if (current->next[i])
        cout << current->next[i]->element.first << ")\n";
      else
        cout << "end)\n";
    }
    current = current->next[0];
    if (current && current->element.first == key) {
      cout << " Found value: " << current->element.second << "\n";
      return true;
    }
    cout << " Key not present.\n";
    return false;
  }

  bool erase(const K &key) {
    skipNode<K, V> *update[MAX_LEVEL];
    skipNode<K, V> *current = header;
    cout << "\nLocating key '" << key << "' for removal:\n";
    for (int i = level - 1; i >= 0; --i) {
      cout << " Level " << i << " traversal:";
      while (current->next[i] && current->next[i]->element.first < key) {
        cout << " -> " << current->next[i]->element.first;
        current = current->next[i];
      }
      cout << " (stop at ";
      if (current->next[i])
        cout << current->next[i]->element.first << ")\n";
      else
        cout << "end)\n";
      update[i] = current;
    }
    current = current->next[0];
    if (!current || current->element.first != key) {
      cout << " Key missing, nothing to remove.\n";
      return false;
    }
    for (int i = 0; i < level; ++i) {
      if (update[i]->next[i] != current)
        break;
      update[i]->next[i] = current->next[i];
      cout << " Level " << i << " bypassed " << key << "\n";
    }
    delete current;
    while (level > 1 && header->next[level - 1] == nullptr)
      --level;
    return true;
  }
};

int main() {
  srand(static_cast<unsigned>(time(nullptr)));
  SkipList<string, string> demo;
  int choice = 0;
  while (choice != 5) {
    cout << "\n=== Templated Skip List (pair-based) ===\n";
    cout << "1. Insert key-value" << "\n";
    cout << "2. Search key" << "\n";
    cout << "3. Delete key" << "\n";
    cout << "4. Display structure" << "\n";
    cout << "5. Exit" << "\n";
    cout << "Choice: ";
    cin >> choice;

    if (!cin) {
      cin.clear();
      cin.ignore(1024, '\n');
      choice = 0;
    }

    switch (choice) {
      case 1: {
        string key, value;
        cout << " Enter key (no spaces): ";
        cin >> key;
        cout << " Enter value (no spaces): ";
        cin >> value;
        demo.insert(key, value);
        break;
      }
      case 2: {
        string key;
        cout << " Enter key to search: ";
        cin >> key;
        demo.search(key);
        break;
      }
      case 3: {
        string key;
        cout << " Enter key to delete: ";
        cin >> key;
        if (demo.erase(key))
          cout << " Deletion complete.\n";
        break;
      }
      case 4:
        demo.display();
        break;
      case 5:
        cout << " Exiting templated demo.\n";
        break;
      default:
        cout << " Invalid option. Try again.\n";
    }
  }
  return 0;
}
