// 2. Implement all the functions of a dictionary (ADT) using hashing and handle
// collisions using chaining with / without replacement. Data: Set of (key,
// value) pairs, Keys are mapped to values, Keys must be comparable, Keys must
// be unique. Standard Operations: Insert(key, value), Find(key), Delete(key)

#include <iostream>
#include <string>
using namespace std;

class Dictionary {
 private:
  struct Node {
    string key;
    string value;
    Node *next;
    Node(const string &k, const string &v) : key(k), value(v), next(0) {}
  };

  int size;
  Node **table;

  int hashFunction(const string &key) const {
    unsigned long long h = 0;
    for (int i = 0; i < static_cast<int>(key.size()); ++i) {
      h = h * 31 + static_cast<unsigned char>(key[i]);
    }
    return static_cast<int>(h % size);
  }

 public:
  explicit Dictionary(int tableSize) {
    size = tableSize;
    table = new Node *[size];
    for (int i = 0; i < size; ++i) {
      table[i] = 0;
    }
  }

  ~Dictionary() {
    for (int i = 0; i < size; ++i) {
      Node *cur = table[i];
      while (cur != 0) {
        Node *nxt = cur->next;
        delete cur;
        cur = nxt;
      }
    }
    delete[] table;
  }

  string insertWithReplacement(const string &key, const string &value) {
    int idx = hashFunction(key);
    Node *cur = table[idx];
    while (cur != 0) {
      if (cur->key == key) {
        cur->value = value;
        return "Updated existing key";
      }
      cur = cur->next;
    }

    Node *node = new Node(key, value);
    node->next = table[idx];
    table[idx] = node;
    return "Inserted new key";
  }

  string insertWithoutReplacement(const string &key, const string &value) {
    int idx = hashFunction(key);
    Node *cur = table[idx];
    while (cur != 0) {
      if (cur->key == key) {
        return "Key already exists (No update)";
      }
      cur = cur->next;
    }

    Node *node = new Node(key, value);
    node->next = table[idx];
    table[idx] = node;
    return "Inserted new key";
  }

  bool find(const string &key, string &outValue, int &comparisons) const {
    int idx = hashFunction(key);
    comparisons = 0;

    Node *cur = table[idx];
    while (cur != 0) {
      ++comparisons;
      if (cur->key == key) {
        outValue = cur->value;
        return true;
      }
      cur = cur->next;
    }

    return false;
  }

  bool removeKey(const string &key) {
    int idx = hashFunction(key);
    Node *cur = table[idx];
    Node *prev = 0;

    while (cur != 0) {
      if (cur->key == key) {
        if (prev == 0) {
          table[idx] = cur->next;
        } else {
          prev->next = cur->next;
        }
        delete cur;
        return true;
      }
      prev = cur;
      cur = cur->next;
    }
    return false;
  }

  void display() const {
    cout << "\nDictionary State\n";
    for (int i = 0; i < size; ++i) {
      cout << i << " : ";
      Node *cur = table[i];
      if (cur == 0) {
        cout << "Empty\n";
      } else {
        while (cur != 0) {
          cout << "[" << cur->key << ":" << cur->value << "]";
          cur = cur->next;
          if (cur != 0) cout << " -> ";
        }
        cout << "\n";
      }
    }
  }
};

int main() {
  int tableSize;
  cout << "Enter hash table size: ";
  cin >> tableSize;

  if (tableSize <= 0) {
    cout << "Invalid table size\n";
    return 0;
  }

  Dictionary dict(tableSize);

  while (true) {
    cout << "\n1. Insert (With Replacement)\n";
    cout << "2. Insert (Without Replacement)\n";
    cout << "3. Find\n";
    cout << "4. Delete\n";
    cout << "5. Display\n";
    cout << "6. Exit\n";
    cout << "Enter choice: ";

    int choice;
    cin >> choice;

    if (choice == 1) {
      string key, value;
      cout << "Enter key: ";
      cin >> key;
      cout << "Enter value: ";
      cin >> value;
      cout << dict.insertWithReplacement(key, value) << "\n";

    } else if (choice == 2) {
      string key, value;
      cout << "Enter key: ";
      cin >> key;
      cout << "Enter value: ";
      cin >> value;
      cout << dict.insertWithoutReplacement(key, value) << "\n";

    } else if (choice == 3) {
      string key, value;
      int comparisons = 0;
      cout << "Enter key to find: ";
      cin >> key;
      if (dict.find(key, value, comparisons)) {
        cout << "Found. Value: " << value << "\n";
      } else {
        cout << "Not found\n";
      }
      cout << "Comparisons: " << comparisons << "\n";

    } else if (choice == 4) {
      string key;
      cout << "Enter key to delete: ";
      cin >> key;
      if (dict.removeKey(key)) cout << "Deleted successfully\n";
      else cout << "Key not found\n";

    } else if (choice == 5) {
      dict.display();

    } else if (choice == 6) {
      cout << "Exiting...\n";
      break;

    } else {
      cout << "Invalid choice\n";
    }
  }

  return 0;
}
