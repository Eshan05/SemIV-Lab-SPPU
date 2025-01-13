// 20. B+ Tree
// Demo: integer B+ Tree with order m (max children per internal node).
// - Internal node: up to m children and m-1 keys (routing keys)
// - Leaf node: up to m-1 keys (data keys), leaves are linked for sorted traversal
// Supports: insert, delete, search, display structure, display leaf-chain, height, #nodes.

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

struct Node {
  bool leaf;
  int keyCount;
  int *keys;   // capacity = order (one extra for overflow)
  Node **ptrs; // capacity = order+1 (one extra for overflow)
  Node *parent;
  Node *next; // leaf link
  int order;

  Node(int ord, bool isLeaf) : leaf(isLeaf), keyCount(0), parent(NULL), next(NULL), order(ord) {
    keys = new int[order];
    ptrs = new Node *[order + 1];
    for (int i = 0; i < order + 1; i++) ptrs[i] = NULL;
  }

  ~Node() {
    delete[] keys;
    delete[] ptrs;
  }
};

struct BPlusTree {
  int order; // max children
  Node *root;

  BPlusTree() : order(0), root(NULL) {}
  ~BPlusTree() { clear(); }

  int maxKeys() const { return order - 1; }
  int minKeysLeaf() const { return order / 2; }               // ceil((order-1)/2)
  int minChildrenInternal() const { return (order + 1) / 2; } // ceil(order/2)
  int minKeysInternal() const { return minChildrenInternal() - 1; }

  void init(int ord) {
    clear();
    order = ord;
    if (order < 3) order = 3;
  }

  bool ready() const { return order >= 3; }

  void clearRec(Node *node) {
    if (!node) return;
    if (!node->leaf) {
      for (int i = 0; i <= node->keyCount; i++) clearRec(node->ptrs[i]);
    }
    delete node;
  }

  void clear() {
    clearRec(root);
    root = NULL;
  }

  int heightLevels() const {
    if (!root) return 0;
    int h = 1;
    Node *cur = root;
    while (cur && !cur->leaf) {
      cur = cur->ptrs[0];
      h++;
    }
    return h;
  }

  int countNodesRec(Node *node) const {
    if (!node) return 0;
    int cnt = 1;
    if (!node->leaf) {
      for (int i = 0; i <= node->keyCount; i++) cnt += countNodesRec(node->ptrs[i]);
    }
    return cnt;
  }

  int countNodes() const { return countNodesRec(root); }

  int childIndex(Node *parent, Node *child) const {
    if (!parent) return -1;
    for (int i = 0; i <= parent->keyCount; i++)
      if (parent->ptrs[i] == child) return i;
    return -1;
  }

  int firstKey(Node *node) const {
    Node *cur = node;
    while (cur && !cur->leaf) cur = cur->ptrs[0];
    if (!cur || cur->keyCount == 0) return 0;
    return cur->keys[0];
  }

  void refreshSeparators(Node *internal) {
    if (!internal || internal->leaf) return;
    // keys[i] = first key in subtree of ptrs[i+1]
    for (int i = 0; i < internal->keyCount; i++) {
      if (internal->ptrs[i + 1]) internal->keys[i] = firstKey(internal->ptrs[i + 1]);
    }
  }

  Node *findLeaf(int key) const {
    Node *cur = root;
    if (!cur) return NULL;
    while (!cur->leaf) {
      int i = 0;
      while (i < cur->keyCount && key >= cur->keys[i]) i++;
      cout << "At internal node, key " << key << " follows child " << i << "\n";
      cur = cur->ptrs[i];
    }
    cout << "Reached leaf for key " << key << "\n";
    return cur;
  }

  bool containsInLeaf(Node *leaf, int key) const {
    if (!leaf) return false;
    for (int i = 0; i < leaf->keyCount; i++)
      if (leaf->keys[i] == key) return true;
    return false;
  }

  bool search(int key, int &comps) const {
    comps = 0;
    Node *leaf = findLeaf(key);
    if (!leaf) return false;
    for (int i = 0; i < leaf->keyCount; i++) {
      comps++;
      cout << "Compare " << key << " with leaf key " << leaf->keys[i] << ": ";
      if (leaf->keys[i] == key) {
        cout << "match\n";
        return true;
      }
      cout << "no\n";
    }
    cout << "End of leaf; key not present.\n";
    return false;
  }

  void insertIntoLeaf(Node *leaf, int key) {
    int i = leaf->keyCount - 1;
    while (i >= 0 && leaf->keys[i] > key) {
      cout << "Shift leaf key " << leaf->keys[i] << " right\n";
      leaf->keys[i + 1] = leaf->keys[i];
      i--;
    }
    leaf->keys[i + 1] = key;
    cout << "Place key " << key << " in leaf slot " << i + 1 << "\n";
    leaf->keyCount++;
  }

  void insertIntoParent(Node *left, int key, Node *right) {
    if (!left->parent) {
      Node *newRoot = new Node(order, false);
      newRoot->keys[0] = key;
      newRoot->keyCount = 1;
      newRoot->ptrs[0] = left;
      newRoot->ptrs[1] = right;
      left->parent = newRoot;
      right->parent = newRoot;
      root = newRoot;
      return;
    }

    Node *parent = left->parent;
    int idx = childIndex(parent, left);
    // insert key at keys[idx], and right at ptrs[idx+1]
    for (int i = parent->keyCount - 1; i >= idx; i--) parent->keys[i + 1] = parent->keys[i];
    for (int i = parent->keyCount; i >= idx + 1; i--) parent->ptrs[i + 1] = parent->ptrs[i];
    parent->keys[idx] = key;
    parent->ptrs[idx + 1] = right;
    right->parent = parent;
    parent->keyCount++;

    if (parent->keyCount > maxKeys()) splitInternal(parent);
    else refreshSeparators(parent);
  }

  void splitLeaf(Node *leaf) {
    Node *newLeaf = new Node(order, true);
    newLeaf->parent = leaf->parent;
    newLeaf->next = leaf->next;
    leaf->next = newLeaf;

    int leftSize = (order + 1) / 2; // ceil(order/2) where overflow keys count==order
    int rightSize = leaf->keyCount - leftSize;
    newLeaf->keyCount = rightSize;
    for (int i = 0; i < rightSize; i++) newLeaf->keys[i] = leaf->keys[leftSize + i];
    leaf->keyCount = leftSize;

    int sep = newLeaf->keys[0];
    cout << "Leaf split, promote separator " << sep << "\n";
    insertIntoParent(leaf, sep, newLeaf);
  }

  void splitInternal(Node *node) {
    // node has keyCount == order (overflow)
    int totalKeys = node->keyCount;
    int totalPtrs = totalKeys + 1;
    int leftPtrCount = (totalPtrs + 1) / 2; // ceil
    int leftKeyCount = leftPtrCount - 1;

    int promote = node->keys[leftKeyCount];
    Node *right = new Node(order, false);
    right->parent = node->parent;

    int rightKeyCount = totalKeys - leftKeyCount - 1;
    right->keyCount = rightKeyCount;
    for (int i = 0; i < rightKeyCount; i++) right->keys[i] = node->keys[leftKeyCount + 1 + i];

    int rightPtrCount = totalPtrs - leftPtrCount;
    for (int i = 0; i < rightPtrCount; i++) {
      right->ptrs[i] = node->ptrs[leftPtrCount + i];
      node->ptrs[leftPtrCount + i] = NULL;
      if (right->ptrs[i]) right->ptrs[i]->parent = right;
    }

    node->keyCount = leftKeyCount;
    for (int i = node->keyCount; i < order; i++) {
      // keys beyond keyCount are considered garbage; no need to clear
    }

    cout << "Internal split, promote key " << promote << "\n";
    insertIntoParent(node, promote, right);
    refreshSeparators(node);
    refreshSeparators(right);
  }

  bool insert(int key) {
    if (!ready()) return false;
    if (!root) {
      root = new Node(order, true);
      root->keys[0] = key;
      root->keyCount = 1;
      return true;
    }
    Node *leaf = findLeaf(key);
    if (containsInLeaf(leaf, key)) return false;
    insertIntoLeaf(leaf, key);
    if (leaf->keyCount > maxKeys()) splitLeaf(leaf);
    else {
      // if leaf is not first child, its first key might be used in parent
      if (leaf->parent) refreshSeparators(leaf->parent);
    }
    return true;
  }

  void removeEntryFromInternal(Node *parent, int ptrIndex) {
    // remove child pointer at ptrIndex, and remove separator key just before it (ptrIndex-1)
    // if ptrIndex==0, remove key 0 (since keys map to child i+1)
    int keyIndex = (ptrIndex == 0) ? 0 : (ptrIndex - 1);
    if (parent->keyCount == 0) return;

    // shift keys left from keyIndex
    for (int i = keyIndex + 1; i < parent->keyCount; i++) parent->keys[i - 1] = parent->keys[i];
    // shift ptrs left from ptrIndex
    for (int i = ptrIndex + 1; i <= parent->keyCount; i++) parent->ptrs[i - 1] = parent->ptrs[i];
    parent->ptrs[parent->keyCount] = NULL;
    parent->keyCount--;
    refreshSeparators(parent);
  }

  void rebalanceInternal(Node *node) {
    if (!node || node == root) {
      if (node == root && node && !node->leaf && node->keyCount == 0) {
        Node *newRoot = node->ptrs[0];
        node->ptrs[0] = NULL;
        if (newRoot) newRoot->parent = NULL;
        delete node;
        root = newRoot;
      }
      return;
    }

    if (node->keyCount >= minKeysInternal()) {
      if (node->parent) refreshSeparators(node->parent);
      return;
    }

    Node *parent = node->parent;
    int idx = childIndex(parent, node);
    Node *left = (idx > 0) ? parent->ptrs[idx - 1] : NULL;
    Node *right = (idx < parent->keyCount) ? parent->ptrs[idx + 1] : NULL;

    // borrow
    if (left && left->keyCount > minKeysInternal()) {
      cout << "Internal borrow from left\n";
      // shift node keys/ptrs right by 1
      for (int i = node->keyCount - 1; i >= 0; i--) node->keys[i + 1] = node->keys[i];
      for (int i = node->keyCount; i >= 0; i--) node->ptrs[i + 1] = node->ptrs[i];
      // bring separator down
      node->keys[0] = parent->keys[idx - 1];
      // bring left's last child
      node->ptrs[0] = left->ptrs[left->keyCount];
      left->ptrs[left->keyCount] = NULL;
      if (node->ptrs[0]) node->ptrs[0]->parent = node;
      // move left's last key up
      parent->keys[idx - 1] = left->keys[left->keyCount - 1];
      left->keyCount--;
      node->keyCount++;
      refreshSeparators(left);
      refreshSeparators(node);
      refreshSeparators(parent);
      return;
    }
    if (right && right->keyCount > minKeysInternal()) {
      cout << "Internal borrow from right\n";
      // bring separator down
      node->keys[node->keyCount] = parent->keys[idx];
      // bring right's first child
      node->ptrs[node->keyCount + 1] = right->ptrs[0];
      if (node->ptrs[node->keyCount + 1]) node->ptrs[node->keyCount + 1]->parent = node;
      // move right's first key up
      parent->keys[idx] = right->keys[0];
      // shift right keys/ptrs left
      for (int i = 1; i < right->keyCount; i++) right->keys[i - 1] = right->keys[i];
      for (int i = 1; i <= right->keyCount; i++) right->ptrs[i - 1] = right->ptrs[i];
      right->ptrs[right->keyCount] = NULL;
      right->keyCount--;
      node->keyCount++;
      refreshSeparators(right);
      refreshSeparators(node);
      refreshSeparators(parent);
      return;
    }

    // merge
    if (left) {
      cout << "Internal merge into left\n";
      // left absorbs separator + node
      left->keys[left->keyCount] = parent->keys[idx - 1];
      for (int i = 0; i < node->keyCount; i++) left->keys[left->keyCount + 1 + i] = node->keys[i];
      for (int i = 0; i <= node->keyCount; i++) {
        left->ptrs[left->keyCount + 1 + i] = node->ptrs[i];
        node->ptrs[i] = NULL;
        if (left->ptrs[left->keyCount + 1 + i]) left->ptrs[left->keyCount + 1 + i]->parent = left;
      }
      left->keyCount += node->keyCount + 1;
      delete node;
      refreshSeparators(left);
      removeEntryFromInternal(parent, idx);
      rebalanceInternal(parent);
    } else if (right) {
      cout << "Internal merge with right\n";
      // node absorbs separator + right
      node->keys[node->keyCount] = parent->keys[idx];
      for (int i = 0; i < right->keyCount; i++) node->keys[node->keyCount + 1 + i] = right->keys[i];
      for (int i = 0; i <= right->keyCount; i++) {
        node->ptrs[node->keyCount + 1 + i] = right->ptrs[i];
        right->ptrs[i] = NULL;
        if (node->ptrs[node->keyCount + 1 + i]) node->ptrs[node->keyCount + 1 + i]->parent = node;
      }
      node->keyCount += right->keyCount + 1;
      delete right;
      refreshSeparators(node);
      removeEntryFromInternal(parent, idx + 1);
      rebalanceInternal(parent);
    }
  }

  void rebalanceLeaf(Node *leaf) {
    if (!leaf || leaf == root) {
      if (leaf == root && leaf && leaf->leaf && leaf->keyCount == 0) {
        delete leaf;
        root = NULL;
      }
      return;
    }

    if (leaf->keyCount >= minKeysLeaf()) {
      if (leaf->parent) refreshSeparators(leaf->parent);
      return;
    }

    Node *parent = leaf->parent;
    int idx = childIndex(parent, leaf);
    Node *left = (idx > 0) ? parent->ptrs[idx - 1] : NULL;
    Node *right = (idx < parent->keyCount) ? parent->ptrs[idx + 1] : NULL;

    if (left && left->leaf && left->keyCount > minKeysLeaf()) {
      cout << "Leaf borrow from left\n";
      // shift leaf right
      for (int i = leaf->keyCount - 1; i >= 0; i--) leaf->keys[i + 1] = leaf->keys[i];
      leaf->keys[0] = left->keys[left->keyCount - 1];
      left->keyCount--;
      leaf->keyCount++;
      refreshSeparators(parent);
      return;
    }
    if (right && right->leaf && right->keyCount > minKeysLeaf()) {
      cout << "Leaf borrow from right\n";
      leaf->keys[leaf->keyCount] = right->keys[0];
      leaf->keyCount++;
      for (int i = 1; i < right->keyCount; i++) right->keys[i - 1] = right->keys[i];
      right->keyCount--;
      refreshSeparators(parent);
      return;
    }

    if (left && left->leaf) {
      cout << "Leaf merge into left\n";
      for (int i = 0; i < leaf->keyCount; i++) left->keys[left->keyCount + i] = leaf->keys[i];
      left->keyCount += leaf->keyCount;
      left->next = leaf->next;
      delete leaf;
      removeEntryFromInternal(parent, idx);
      rebalanceInternal(parent);
      return;
    }
    if (right && right->leaf) {
      cout << "Leaf merge with right\n";
      for (int i = 0; i < right->keyCount; i++) leaf->keys[leaf->keyCount + i] = right->keys[i];
      leaf->keyCount += right->keyCount;
      leaf->next = right->next;
      delete right;
      removeEntryFromInternal(parent, idx + 1);
      rebalanceInternal(parent);
      return;
    }
  }

  bool erase(int key) {
    if (!root) return false;
    Node *leaf = findLeaf(key);
    if (!leaf) return false;
    int pos = -1;
    for (int i = 0; i < leaf->keyCount; i++)
      if (leaf->keys[i] == key) {
        pos = i;
        break;
      }
    if (pos < 0) return false;
    for (int i = pos + 1; i < leaf->keyCount; i++) leaf->keys[i - 1] = leaf->keys[i];
    leaf->keyCount--;

    if (leaf == root) {
      if (leaf->keyCount == 0) {
        delete leaf;
        root = NULL;
      }
      return true;
    }

    if (leaf->keyCount >= minKeysLeaf()) {
      refreshSeparators(leaf->parent);
      return true;
    }
    rebalanceLeaf(leaf);
    return true;
  }

  void displayRec(Node *node, int depth) const {
    if (!node) return;
    for (int i = 0; i < depth; i++) cout << "    ";
    cout << (node->leaf ? "Leaf " : "Int  ");
    cout << "[";
    for (int i = 0; i < node->keyCount; i++) {
      cout << node->keys[i];
      if (i + 1 < node->keyCount) cout << ",";
    }
    cout << "]\n";
    if (!node->leaf) {
      for (int i = 0; i <= node->keyCount; i++) displayRec(node->ptrs[i], depth + 1);
    }
  }

  void display() const {
    if (!root) {
      cout << "(empty)\n";
      return;
    }
    displayRec(root, 0);
  }

  void displayLeaves() const {
    if (!root) {
      cout << "(empty)\n";
      return;
    }
    Node *cur = root;
    while (cur && !cur->leaf) cur = cur->ptrs[0];
    cout << "Leaves (sorted): ";
    while (cur) {
      for (int i = 0; i < cur->keyCount; i++) cout << cur->keys[i] << " ";
      cur = cur->next;
    }
    cout << "\n";
  }
};

int main() {
  BPlusTree t;
  int choice = 0;
  while (choice != 8) {
    cout << "\n=== T22: B+ Tree (order m) ===\n";
    cout << "1. Create/Reset with order m\n";
    cout << "2. Insert key\n";
    cout << "3. Delete key\n";
    cout << "4. Search key\n";
    cout << "5. Display structure\n";
    cout << "6. Display leaf-chain\n";
    cout << "7. Height/#Nodes/Limits\n";
    cout << "8. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      int m = readInt("Order m (max children, >=3): ");
      if (m < 3) {
        cout << "order must be at least 3; using 3.\n";
        m = 3;
      }
      t.init(m);
      cout << "Created B+ tree with order=" << t.order << "\n";
    } else if (choice == 2) {
      if (!t.ready()) {
        cout << "Create tree first (option 1).\n";
        continue;
      }
      int k = readInt("Key: ");
      bool ok = t.insert(k);
      cout << (ok ? "Inserted.\n" : "Duplicate ignored.\n");
    } else if (choice == 3) {
      if (!t.ready()) {
        cout << "Create tree first (option 1).\n";
        continue;
      }
      int k = readInt("Delete key: ");
      bool ok = t.erase(k);
      cout << (ok ? "Deleted.\n" : "Not found.\n");
    } else if (choice == 4) {
      if (!t.ready()) {
        cout << "Create tree first (option 1).\n";
        continue;
      }
      int k = readInt("Search key: ");
      int comps = 0;
      bool ok = t.search(k, comps);
      cout << (ok ? "Found" : "Not found") << ", comparisons=" << comps << "\n";
    } else if (choice == 5) {
      if (!t.ready()) {
        cout << "Create tree first (option 1).\n";
        continue;
      }
      t.display();
    } else if (choice == 6) {
      if (!t.ready()) {
        cout << "Create tree first (option 1).\n";
        continue;
      }
      t.displayLeaves();
    } else if (choice == 7) {
      if (!t.ready()) {
        cout << "Create tree first (option 1).\n";
        continue;
      }
      cout << "order=" << t.order << ", maxKeys=" << (t.ready() ? t.maxKeys() : 0) << "\n";
      cout << "minLeafKeys=" << (t.ready() ? t.minKeysLeaf() : 0) << ", minInternalKeys=" << (t.ready() ? t.minKeysInternal() : 0) << "\n";
      cout << "Height(levels)=" << t.heightLevels() << ", Nodes=" << t.countNodes() << "\n";
    } else if (choice == 8) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  return 0;
}
