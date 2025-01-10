// 1. Consider telephone book database of N clients. Make use of a hash table
// implementation to quickly look up client‘s telephone number. Make use of two
// collision handling techniques and compare them using number of comparisons
// required to find a set of telephone numbers

#include <iostream>
#include <string>
using namespace std;

class LinearProbingHash {
 private:
  int size;
  string *keys;
  string *values;
  int *state; // 0 = empty, 1 = occupied

  int hashFunction(const string &key) const {
    unsigned long long h = 0;
    for (int i = 0; i < static_cast<int>(key.size()); ++i) {
      h = h * 31 + static_cast<unsigned char>(key[i]);
    }
    return static_cast<int>(h % size);
  }

 public:
  explicit LinearProbingHash(int tableSize) {
    size = tableSize;
    keys = new string[size];
    values = new string[size];
    state = new int[size];
    for (int i = 0; i < size; ++i) {
      state[i] = 0;
    }
  }

  ~LinearProbingHash() {
    delete[] keys;
    delete[] values;
    delete[] state;
  }

  bool insert(const string &key, const string &value, int &probes, string &msg) {
    int start = hashFunction(key);
    probes = 0;

    for (int i = 0; i < size; ++i) {
      int idx = (start + i) % size;
      ++probes;

      if (state[idx] == 1 && keys[idx] == key) {
        values[idx] = value;
        msg = "Updated existing key";
        return true;
      }

      if (state[idx] == 0) {
        state[idx] = 1;
        keys[idx] = key;
        values[idx] = value;
        msg = "Inserted";
        return true;
      }
    }

    msg = "Hash table full";
    return false;
  }

  bool search(const string &key, string &outValue, int &comparisons) const {
    int start = hashFunction(key);
    comparisons = 0;

    for (int i = 0; i < size; ++i) {
      int idx = (start + i) % size;

      if (state[idx] == 0) {
        return false;
      }

      ++comparisons;
      if (keys[idx] == key) {
        outValue = values[idx];
        return true;
      }
    }

    return false;
  }

  void display() const {
    cout << "\nLinear Probing Table\n";
    for (int i = 0; i < size; ++i) {
      cout << i << " : ";
      if (state[i] == 1) {
        cout << keys[i] << " -> " << values[i] << "\n";
      } else {
        cout << "Empty\n";
      }
    }
  }
};

class ChainingHash {
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
  explicit ChainingHash(int tableSize) {
    size = tableSize;
    table = new Node *[size];
    for (int i = 0; i < size; ++i) {
      table[i] = 0;
    }
  }

  ~ChainingHash() {
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

  bool insert(const string &key, const string &value, int &comparisons, string &msg) {
    int idx = hashFunction(key);
    comparisons = 0;

    Node *cur = table[idx];
    while (cur != 0) {
      ++comparisons;
      if (cur->key == key) {
        cur->value = value;
        msg = "Updated existing key";
        return true;
      }
      cur = cur->next;
    }

    Node *node = new Node(key, value);
    node->next = table[idx];
    table[idx] = node;
    msg = "Inserted";
    return true;
  }

  bool search(const string &key, string &outValue, int &comparisons) const {
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

  void display() const {
    cout << "\nSeparate Chaining Table\n";
    for (int i = 0; i < size; ++i) {
      cout << i << " : ";
      Node *cur = table[i];
      if (cur == 0) {
        cout << "Empty\n";
      } else {
        while (cur != 0) {
          cout << cur->key << " -> " << cur->value;
          cur = cur->next;
          if (cur != 0) {
            cout << " | ";
          }
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

  LinearProbingHash linear(tableSize);
  ChainingHash chaining(tableSize);

  while (true) {
    cout << "\n1. Insert\n";
    cout << "2. Search\n";
    cout << "3. Display Tables\n";
    cout << "4. Compare Searches for N keys\n";
    cout << "5. Exit\n";
    cout << "Enter choice: ";

    int choice;
    cin >> choice;

    if (choice == 1) {
      string name, phone;
      cout << "Enter client name: ";
      cin >> name;
      cout << "Enter phone number: ";
      cin >> phone;

      int p1 = 0, p2 = 0;
      string m1, m2;

      linear.insert(name, phone, p1, m1);
      chaining.insert(name, phone, p2, m2);

      cout << "\nLinear Probing\n";
      cout << "Status: " << m1 << "\n";
      cout << "Probes during insertion: " << p1 << "\n";

      cout << "\nSeparate Chaining\n";
      cout << "Status: " << m2 << "\n";
      cout << "Node comparisons during insertion: " << p2 << "\n";

    } else if (choice == 2) {
      string name;
      cout << "Enter client name to search: ";
      cin >> name;

      string value;
      int c1 = 0, c2 = 0;
      bool f1 = linear.search(name, value, c1);
      bool f2 = chaining.search(name, value, c2);

      cout << "\nLinear Probing\n";
      if (f1) cout << "Found. Phone: " << value << "\n";
      else cout << "Not found\n";
      cout << "Comparisons: " << c1 << "\n";

      cout << "\nSeparate Chaining\n";
      if (f2) cout << "Found. Phone: " << value << "\n";
      else cout << "Not found\n";
      cout << "Comparisons: " << c2 << "\n";

    } else if (choice == 3) {
      linear.display();
      chaining.display();

    } else if (choice == 4) {
      int n;
      cout << "How many names to search? ";
      cin >> n;

      int totalLinear = 0;
      int totalChain = 0;

      for (int i = 0; i < n; ++i) {
        string name, value;
        int c1 = 0, c2 = 0;
        cout << "Name " << (i + 1) << ": ";
        cin >> name;
        linear.search(name, value, c1);
        chaining.search(name, value, c2);
        totalLinear += c1;
        totalChain += c2;
      }

      cout << "\nComparison Summary for " << n << " searches\n";
      cout << "Linear Probing total comparisons: " << totalLinear << "\n";
      cout << "Separate Chaining total comparisons: " << totalChain << "\n";

    } else if (choice == 5) {
      cout << "Exiting...\n";
      break;

    } else {
      cout << "Invalid choice\n";
    }
  }

  return 0;
}
