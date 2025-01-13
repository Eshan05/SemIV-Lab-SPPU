// 19. B-Tree
// Demo: integer B-Tree of minimum degree t (t>=2)
// Supports: insertion, deletion, searching, display structure, inorder traversal, height, #nodes.
// Notes:
// - Max keys per node = 2t-1
// - Min keys per non-root node = t-1

#include <iostream>
using namespace std;

static void clearInput() {
  cin.clear();
  cin.ignore(1024, '\n');
}

static int readInt(const string &prompt) {
  int x;
  while (true) {
    cout << prompt;
    if (cin >> x) return x;
    cout << "Invalid number.\n";
    clearInput();
  }
}

struct BNode {
  int t;
  bool leaf;
  int n;     // current #keys
  int *keys; // size 2t-1
  BNode **C; // children size 2t

  BNode(int _t, bool _leaf) : t(_t), leaf(_leaf), n(0) {
    keys = new int[2 * t - 1];
    C = new BNode *[2 * t];
    for (int i = 0; i < 2 * t; i++) C[i] = NULL;
  }

  ~BNode() {
    delete[] keys;
    delete[] C;
  }

  void traverse() const {
    int i;
    for (i = 0; i < n; i++) {
      if (!leaf && C[i]) C[i]->traverse();
      cout << keys[i] << " ";
    }
    if (!leaf && C[i]) C[i]->traverse();
  }

  BNode *search(int k, int &comps) {
    int i = 0;
    cout << "Search node [";
    for (int j = 0; j < n; j++) {
      cout << keys[j];
      if (j + 1 < n) cout << ",";
    }
    cout << "]\n";
    while (i < n && k > keys[i]) {
      comps++;
      cout << "  compare " << k << " > " << keys[i] << " -> move to next key\n";
      i++;
    }
    if (i < n) {
      comps++;
      cout << "  compare " << k << " with " << keys[i] << ": ";
      if (keys[i] == k) {
        cout << "match\n";
        return this;
      }
      cout << "smaller, use child " << i << "\n";
    } else {
      cout << "  key is larger than all keys here, use child " << i << "\n";
    }
    if (leaf) {
      cout << "  reached leaf; key not here\n";
      return NULL;
    }
    return C[i] ? C[i]->search(k, comps) : NULL;
  }

  void display(int depth) const {
    for (int i = 0; i < depth; i++) cout << "    ";
    cout << "[";
    for (int i = 0; i < n; i++) {
      cout << keys[i];
      if (i + 1 < n) cout << ",";
    }
    cout << "]" << (leaf ? " (leaf)" : "") << "\n";
    if (!leaf) {
      for (int i = 0; i <= n; i++) {
        if (C[i]) C[i]->display(depth + 1);
      }
    }
  }

  int heightLevels() const {
    if (leaf) return 1;
    if (!C[0]) return 1;
    return 1 + C[0]->heightLevels();
  }

  int countNodes() const {
    int cnt = 1;
    if (!leaf) {
      for (int i = 0; i <= n; i++)
        if (C[i]) cnt += C[i]->countNodes();
    }
    return cnt;
  }

  void splitChild(int i, BNode *y) {
    // y is full; split into y and z, move middle key up.
    BNode *z = new BNode(t, y->leaf);
    z->n = t - 1;
    for (int j = 0; j < t - 1; j++) z->keys[j] = y->keys[j + t];
    if (!y->leaf) {
      for (int j = 0; j < t; j++) {
        z->C[j] = y->C[j + t];
        y->C[j + t] = NULL;
      }
    }
    y->n = t - 1;

    for (int j = n; j >= i + 1; j--) C[j + 1] = C[j];
    C[i + 1] = z;
    for (int j = n - 1; j >= i; j--) keys[j + 1] = keys[j];
    keys[i] = y->keys[t - 1];
    n++;
  }

  void insertNonFull(int k) {
    int i = n - 1;
    if (leaf) {
      while (i >= 0 && keys[i] > k) {
        keys[i + 1] = keys[i];
        i--;
      }
      keys[i + 1] = k;
      n++;
      return;
    }
    while (i >= 0 && keys[i] > k) i--;
    i++;
    if (C[i] && C[i]->n == 2 * t - 1) {
      cout << "Split child at index " << i << "\n";
      splitChild(i, C[i]);
      if (keys[i] < k) i++;
    }
    if (!C[i]) C[i] = new BNode(t, true);
    C[i]->insertNonFull(k);
  }

  int findKey(int k) const {
    int idx = 0;
    while (idx < n && keys[idx] < k) idx++;
    return idx;
  }

  int getPred(int idx) {
    BNode *cur = C[idx];
    while (!cur->leaf) cur = cur->C[cur->n];
    return cur->keys[cur->n - 1];
  }

  int getSucc(int idx) {
    BNode *cur = C[idx + 1];
    while (!cur->leaf) cur = cur->C[0];
    return cur->keys[0];
  }

  void removeFromLeaf(int idx) {
    cout << "Remove key " << keys[idx] << " from leaf position " << idx << "\n";
    for (int i = idx + 1; i < n; i++) keys[i - 1] = keys[i];
    n--;
  }

  void removeFromNonLeaf(int idx) {
    int k = keys[idx];
    if (C[idx]->n >= t) {
      int pred = getPred(idx);
      cout << "Delete internal key: replace with pred " << pred << "\n";
      keys[idx] = pred;
      C[idx]->remove(pred);
    } else if (C[idx + 1]->n >= t) {
      int succ = getSucc(idx);
      cout << "Delete internal key: replace with succ " << succ << "\n";
      keys[idx] = succ;
      C[idx + 1]->remove(succ);
    } else {
      cout << "Merge children around key " << k << "\n";
      merge(idx);
      C[idx]->remove(k);
    }
  }

  void borrowFromPrev(int idx) {
    BNode *child = C[idx];
    BNode *sib = C[idx - 1];

    for (int i = child->n - 1; i >= 0; i--) child->keys[i + 1] = child->keys[i];
    if (!child->leaf) {
      for (int i = child->n; i >= 0; i--) child->C[i + 1] = child->C[i];
    }

    child->keys[0] = keys[idx - 1];
    if (!child->leaf) {
      child->C[0] = sib->C[sib->n];
      sib->C[sib->n] = NULL;
    }

    keys[idx - 1] = sib->keys[sib->n - 1];
    child->n++;
    sib->n--;
  }

  void borrowFromNext(int idx) {
    BNode *child = C[idx];
    BNode *sib = C[idx + 1];

    child->keys[child->n] = keys[idx];
    if (!child->leaf) {
      child->C[child->n + 1] = sib->C[0];
    }
    keys[idx] = sib->keys[0];

    for (int i = 1; i < sib->n; i++) sib->keys[i - 1] = sib->keys[i];
    if (!sib->leaf) {
      for (int i = 1; i <= sib->n; i++) sib->C[i - 1] = sib->C[i];
      sib->C[sib->n] = NULL;
    }

    child->n++;
    sib->n--;
  }

  void merge(int idx) {
    BNode *child = C[idx];
    BNode *sib = C[idx + 1];

    child->keys[t - 1] = keys[idx];
    for (int i = 0; i < sib->n; i++) child->keys[i + t] = sib->keys[i];
    if (!child->leaf) {
      for (int i = 0; i <= sib->n; i++) {
        child->C[i + t] = sib->C[i];
        sib->C[i] = NULL;
      }
    }
    for (int i = idx + 1; i < n; i++) keys[i - 1] = keys[i];
    for (int i = idx + 2; i <= n; i++) C[i - 1] = C[i];
    C[n] = NULL;
    child->n += sib->n + 1;
    n--;
    delete sib;
  }

  void fill(int idx) {
    if (idx != 0 && C[idx - 1]->n >= t) {
      cout << "Borrow from previous sibling\n";
      borrowFromPrev(idx);
    } else if (idx != n && C[idx + 1]->n >= t) {
      cout << "Borrow from next sibling\n";
      borrowFromNext(idx);
    } else {
      cout << "Merge for underflow\n";
      if (idx != n) merge(idx);
      else merge(idx - 1);
    }
  }

  void remove(int k) {
    int idx = findKey(k);
    if (idx < n && keys[idx] == k) {
      if (leaf) removeFromLeaf(idx);
      else removeFromNonLeaf(idx);
      return;
    }
    if (leaf) return;

    bool flag = (idx == n);
    if (C[idx] && C[idx]->n < t) fill(idx);
    if (flag && idx > n) {
      if (C[idx - 1]) C[idx - 1]->remove(k);
    } else {
      if (C[idx]) C[idx]->remove(k);
    }
  }
};

struct BTree {
  int t;
  BNode *root;

  BTree() : t(0), root(NULL) {}

  ~BTree() { clear(); }

  void init(int minDegree) {
    clear();
    t = minDegree;
    if (t < 2) t = 2;
  }

  void clearRec(BNode *node) {
    if (!node) return;
    if (!node->leaf) {
      for (int i = 0; i <= node->n; i++) clearRec(node->C[i]);
    }
    delete node;
  }

  void clear() {
    clearRec(root);
    root = NULL;
  }

  bool isReady() const { return t >= 2; }

  bool search(int k, int &comps) {
    comps = 0;
    if (!root) return false;
    return root->search(k, comps) != NULL;
  }

  void traverse() const {
    if (!root) {
      cout << "(empty)\n";
      return;
    }
    root->traverse();
    cout << "\n";
  }

  void display() const {
    if (!root) {
      cout << "(empty)\n";
      return;
    }
    root->display(0);
  }

  void insert(int k) {
    if (!root) {
      root = new BNode(t, true);
      root->keys[0] = k;
      root->n = 1;
      return;
    }
    // Prevent duplicates
    int comps = 0;
    if (root->search(k, comps)) {
      cout << "Duplicate ignored.\n";
      return;
    }
    if (root->n == 2 * t - 1) {
      BNode *s = new BNode(t, false);
      s->C[0] = root;
      cout << "Root full -> split root\n";
      s->splitChild(0, root);
      int i = 0;
      if (s->keys[0] < k) i++;
      s->C[i]->insertNonFull(k);
      root = s;
    } else {
      root->insertNonFull(k);
    }
  }

  void remove(int k) {
    if (!root) return;
    root->remove(k);
    if (root->n == 0) {
      BNode *old = root;
      if (root->leaf) root = NULL;
      else root = root->C[0];
      old->C[0] = NULL;
      delete old;
    }
  }

  int heightLevels() const {
    if (!root) return 0;
    return root->heightLevels();
  }

  int countNodes() const {
    if (!root) return 0;
    return root->countNodes();
  }
};

int main() {
  BTree bt;
  int choice = 0;
  while (choice != 8) {
    cout << "\n=== T21: B-Tree (min degree t) ===\n";
    cout << "1. Create/Reset B-Tree with t\n";
    cout << "2. Insert key\n";
    cout << "3. Delete key\n";
    cout << "4. Search key\n";
    cout << "5. Inorder traversal\n";
    cout << "6. Display structure\n";
    cout << "7. Height / #Nodes / Limits\n";
    cout << "8. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      int t = readInt("Minimum degree t (>=2): ");
      if (t < 2) {
        cout << "t must be at least 2; using 2.\n";
        t = 2;
      }
      bt.init(t);
      cout << "Created B-Tree with t=" << bt.t << "\n";
    } else if (choice == 2) {
      if (!bt.isReady()) {
        cout << "Create tree first (option 1).\n";
        continue;
      }
      int k = readInt("Key: ");
      bt.insert(k);
      cout << "Insert done.\n";
    } else if (choice == 3) {
      if (!bt.isReady()) {
        cout << "Create tree first (option 1).\n";
        continue;
      }
      int k = readInt("Delete key: ");
      int comps = 0;
      if (bt.search(k, comps)) {
        bt.remove(k);
        cout << "Deleted.\n";
      } else {
        cout << "Not found; no delete performed.\n";
      }
    } else if (choice == 4) {
      if (!bt.isReady()) {
        cout << "Create tree first (option 1).\n";
        continue;
      }
      int k = readInt("Search key: ");
      int comps = 0;
      bool ok = bt.search(k, comps);
      cout << (ok ? "Found" : "Not found") << ", comparisons=" << comps << "\n";
    } else if (choice == 5) {
      if (!bt.isReady()) {
        cout << "Create tree first (option 1).\n";
        continue;
      }
      bt.traverse();
    } else if (choice == 6) {
      if (!bt.isReady()) {
        cout << "Create tree first (option 1).\n";
        continue;
      }
      bt.display();
    } else if (choice == 7) {
      if (!bt.isReady()) {
        cout << "Create tree first (option 1).\n";
        continue;
      }
      cout << "\nB-Tree limits for this tree:\n";
      cout << "  minimum degree t        : " << bt.t << "\n";
      cout << "  max keys per node       : " << (bt.t > 0 ? 2 * bt.t - 1 : 0) << "\n";
      cout << "  min keys per non-root   : " << (bt.t > 0 ? bt.t - 1 : 0) << "\n";
      cout << "  max children per node   : " << (bt.t > 0 ? 2 * bt.t : 0) << "\n";
      cout << "  height (levels)         : " << bt.heightLevels() << "\n";
      cout << "  allocated B-tree nodes  : " << bt.countNodes() << "\n";
    } else if (choice == 8) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  return 0;
}
