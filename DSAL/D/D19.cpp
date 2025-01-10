// 19. A Dictionary stores keywords and its meanings. Provide facility for adding new keywords, deleting keywords, updating values of any entry. Provide facility to display whole data sorted in ascending/ Descending order. Also find how many maximum comparisons may require for finding any keyword. Use Height balance tree and find the complexity for finding a keyword

#include <iostream>
#include <string>
using namespace std;

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

string readToken(const string &prompt) {
  cout << prompt;
  string s;
  cin >> s;
  return s;
}

string readLine(const string &prompt) {
  cout << prompt;
  string s;
  getline(cin >> ws, s);
  return s;
}

int maxInt(int a, int b) {
  return (a > b) ? a : b;
}

class AvlDictionary {
  struct Node {
    string key;
    string meaning;
    int height;
    Node *left;
    Node *right;
    Node(const string &k, const string &m)
        : key(k), meaning(m), height(1), left(NULL), right(NULL) {}
  };

  Node *root;
  int nodeCount;

  int height(Node *n) const {
    return n ? n->height : 0;
  }

  int balanceFactor(Node *n) const {
    return n ? height(n->left) - height(n->right) : 0;
  }

  void updateHeight(Node *n) {
    if (!n) return;
    n->height = 1 + maxInt(height(n->left), height(n->right));
  }

  Node *rotateRight(Node *y) {
    Node *x = y->left;
    Node *t2 = x->right;

    x->right = y;
    y->left = t2;

    updateHeight(y);
    updateHeight(x);
    return x;
  }

  Node *rotateLeft(Node *x) {
    Node *y = x->right;
    Node *t2 = y->left;

    y->left = x;
    x->right = t2;

    updateHeight(x);
    updateHeight(y);
    return y;
  }

  Node *rebalance(Node *n) {
    if (!n) return n;
    updateHeight(n);
    int bf = balanceFactor(n);

    // Left heavy
    if (bf > 1) {
      if (balanceFactor(n->left) < 0) {
        n->left = rotateLeft(n->left);
      }
      return rotateRight(n);
    }

    // Right heavy
    if (bf < -1) {
      if (balanceFactor(n->right) > 0) {
        n->right = rotateRight(n->right);
      }
      return rotateLeft(n);
    }

    return n;
  }

  Node *insertRec(Node *n, const string &key, const string &meaning, bool &inserted) {
    if (!n) {
      inserted = true;
      return new Node(key, meaning);
    }

    if (key < n->key) {
      n->left = insertRec(n->left, key, meaning, inserted);
    } else if (key > n->key) {
      n->right = insertRec(n->right, key, meaning, inserted);
    } else {
      inserted = false; // duplicate key
      return n;
    }

    return rebalance(n);
  }

  Node *minNode(Node *n) const {
    Node *cur = n;
    while (cur && cur->left) cur = cur->left;
    return cur;
  }

  Node *removeRec(Node *n, const string &key, bool &removed) {
    if (!n) return NULL;

    if (key < n->key) {
      n->left = removeRec(n->left, key, removed);
    } else if (key > n->key) {
      n->right = removeRec(n->right, key, removed);
    } else {
      removed = true;
      // node with 0 or 1 child
      if (!n->left || !n->right) {
        Node *child = n->left ? n->left : n->right;
        delete n;
        return child;
      }

      // node with 2 children: copy successor data and delete successor
      Node *succ = minNode(n->right);
      n->key = succ->key;
      n->meaning = succ->meaning;
      bool removedDummy = false;
      n->right = removeRec(n->right, succ->key, removedDummy);
    }

    return rebalance(n);
  }

  Node *findRec(Node *n, const string &key, int &comparisons) const {
    while (n) {
      comparisons++;
      if (key == n->key) return n;
      if (key < n->key) n = n->left;
      else n = n->right;
    }
    return NULL;
  }

  void displayAscRec(Node *n) const {
    if (!n) return;
    displayAscRec(n->left);
    cout << n->key << " : " << n->meaning << "\n";
    displayAscRec(n->right);
  }

  void displayDescRec(Node *n) const {
    if (!n) return;
    displayDescRec(n->right);
    cout << n->key << " : " << n->meaning << "\n";
    displayDescRec(n->left);
  }

  void destroy(Node *n) {
    if (!n) return;
    destroy(n->left);
    destroy(n->right);
    delete n;
  }

 public:
  AvlDictionary() : root(NULL), nodeCount(0) {}

  ~AvlDictionary() {
    destroy(root);
    root = NULL;
  }

  bool insert(const string &key, const string &meaning) {
    bool inserted = false;
    root = insertRec(root, key, meaning, inserted);
    if (inserted) nodeCount++;
    return inserted;
  }

  bool removeKey(const string &key) {
    bool removed = false;
    root = removeRec(root, key, removed);
    if (removed) nodeCount--;
    return removed;
  }

  bool updateMeaning(const string &key, const string &meaning) {
    int cmp = 0;
    Node *n = findRec(root, key, cmp);
    if (!n) return false;
    n->meaning = meaning;
    return true;
  }

  bool search(const string &key, string &meaningOut, int &comparisonsOut) const {
    comparisonsOut = 0;
    Node *n = findRec(root, key, comparisonsOut);
    if (!n) return false;
    meaningOut = n->meaning;
    return true;
  }

  void displayAscending() const {
    if (!root) {
      cout << "Dictionary is empty.\n";
      return;
    }
    displayAscRec(root);
  }

  void displayDescending() const {
    if (!root) {
      cout << "Dictionary is empty.\n";
      return;
    }
    displayDescRec(root);
  }

  int size() const {
    return nodeCount;
  }

  int treeHeight() const {
    return height(root);
  }

  int maxComparisonsSuccessful() const {
    // Worst-case successful search visits at most one node per level.
    return treeHeight();
  }

  int maxComparisonsUnsuccessful() const {
    // An unsuccessful search can go one step past a leaf (to NULL).
    return treeHeight() + (root ? 1 : 0);
  }
};

int main() {
  AvlDictionary dict;
  while (true) {
    cout << "\n===== Group D19 (AVL Dictionary) =====\n";
    cout << "1. Add new keyword\n";
    cout << "2. Delete keyword\n";
    cout << "3. Update meaning\n";
    cout << "4. Search keyword\n";
    cout << "5. Display ascending\n";
    cout << "6. Display descending\n";
    cout << "7. Max comparisons + complexity\n";
    cout << "8. Exit\n";

    int ch = readInt("Enter choice: ");
    if (ch == 1) {
      string key = readToken("Keyword: ");
      string meaning = readLine("Meaning: ");
      if (dict.insert(key, meaning)) cout << "Inserted.\n";
      else cout << "Keyword already exists. Use update instead.\n";
    } else if (ch == 2) {
      string key = readToken("Keyword to delete: ");
      if (dict.removeKey(key)) cout << "Deleted.\n";
      else cout << "Keyword not found.\n";
    } else if (ch == 3) {
      string key = readToken("Keyword to update: ");
      string meaning = readLine("New meaning: ");
      if (dict.updateMeaning(key, meaning)) cout << "Updated.\n";
      else cout << "Keyword not found.\n";
    } else if (ch == 4) {
      string key = readToken("Keyword to search: ");
      string meaning;
      int comps = 0;
      if (dict.search(key, meaning, comps)) {
        cout << "Found: " << key << " : " << meaning << "\n";
        cout << "Comparisons: " << comps << "\n";
      } else {
        cout << "Not found.\n";
        cout << "Comparisons: " << comps << "\n";
      }
    } else if (ch == 5) {
      dict.displayAscending();
    } else if (ch == 6) {
      dict.displayDescending();
    } else if (ch == 7) {
      cout << "\nEntries: " << dict.size() << "\n";
      cout << "AVL height: " << dict.treeHeight() << "\n";
      cout << "Max comparisons (successful search): " << dict.maxComparisonsSuccessful() << "\n";
      cout << "Max comparisons (unsuccessful search): " << dict.maxComparisonsUnsuccessful() << "\n";
      cout << "Complexity of finding a keyword in AVL: O(log n)\n";
    } else if (ch == 8) {
      break;
    } else {
      cout << "Invalid choice.\n";
    }
  }
  return 0;
}
