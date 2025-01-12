// 3. Binary tree: operations + array representation
// Demo: general binary tree (not BST) built using level-order insertion.
// Includes: insert, search, delete-by-value (swap with deepest), display level-order,
// and show array representation (index mapping).

#include <iostream>
using namespace std;

struct Node {
  int key;
  Node *left;
  Node *right;
  Node(int k) : key(k), left(NULL), right(NULL) {}
};

struct NodeQueue {
  Node **a;
  int cap;
  int head;
  int tail;
  int n;

  NodeQueue(int c) {
    cap = (c < 8) ? 8 : c;
    a = new Node *[cap];
    head = tail = n = 0;
  }
  ~NodeQueue() { delete[] a; }

  bool empty() const { return n == 0; }

  void push(Node *x) {
    if (n == cap) {
      int newCap = cap * 2;
      Node **b = new Node *[newCap];
      for (int i = 0; i < n; i++) b[i] = a[(head + i) % cap];
      delete[] a;
      a = b;
      cap = newCap;
      head = 0;
      tail = n;
    }
    a[tail] = x;
    tail = (tail + 1) % cap;
    n++;
  }

  Node *pop() {
    if (n == 0) return NULL;
    Node *x = a[head];
    head = (head + 1) % cap;
    n--;
    return x;
  }
};

static void clearInput() {
  cin.clear();
  cin.ignore(1024, '\n');
}

static int readInt(const string &prompt) {
  int x;
  while (true) {
    cout << prompt;
    if (cin >> x) return x;
    cout << "Invalid number. Try again.\n";
    clearInput();
  }
}

static void freeTree(Node *root) {
  if (!root) return;
  freeTree(root->left);
  freeTree(root->right);
  delete root;
}

static void displayLevelOrder(Node *root);

static void insertLevelOrder(Node *&root, int key) {
  Node *nn = new Node(key);
  if (!root) {
    root = nn;
    cout << "Inserted as root.\n";
    displayLevelOrder(root);
    return;
  }
  NodeQueue q(32);
  q.push(root);
  while (!q.empty()) {
    Node *cur = q.pop();
    if (!cur->left) {
      cur->left = nn;
      cout << "Inserted " << key << " as LEFT child of " << cur->key << "\n";
      cout << "First empty slot found during level-order scan.\n";
      displayLevelOrder(root);
      return;
    }
    if (!cur->right) {
      cur->right = nn;
      cout << "Inserted " << key << " as RIGHT child of " << cur->key << "\n";
      cout << "Left was full, so the first empty level-order slot was right.\n";
      displayLevelOrder(root);
      return;
    }
    q.push(cur->left);
    q.push(cur->right);
  }
}

static bool searchBFS(Node *root, int key) {
  if (!root) {
    cout << "Tree empty.\n";
    return false;
  }
  NodeQueue q(32);
  q.push(root);
  cout << "Search path (level-order):\n";
  int idx = 0;
  int found = 0;
  while (!q.empty()) {
    Node *cur = q.pop();
    cout << "  visit index " << idx << " value " << cur->key;
    if (cur->key == key) {
      cout << " <- match";
      found++;
    }
    cout << "\n";
    if (cur->left) q.push(cur->left);
    if (cur->right) q.push(cur->right);
    idx++;
  }
  if (found == 0) cout << "No node with value " << key << ".\n";
  else cout << "Found " << found << " node(s) with value " << key << ".\n";
  return found > 0;
}

static void displayLevelOrder(Node *root) {
  if (!root) {
    cout << "(empty)\n";
    return;
  }
  NodeQueue q(32);
  q.push(root);
  cout << "Level-order: ";
  while (!q.empty()) {
    Node *cur = q.pop();
    cout << cur->key << " ";
    if (cur->left) q.push(cur->left);
    if (cur->right) q.push(cur->right);
  }
  cout << "\n";
}

static int buildArrayRep(Node *root, int *arr, int cap) {
  // Fill arr with BFS order. Return count.
  if (!root) return 0;
  NodeQueue q(32);
  q.push(root);
  int n = 0;
  while (!q.empty() && n < cap) {
    Node *cur = q.pop();
    arr[n++] = cur->key;
    if (cur->left) q.push(cur->left);
    if (cur->right) q.push(cur->right);
  }
  return n;
}

static bool deleteByValue(Node *&root, int key) {
  if (!root) return false;

  NodeQueue q(32);
  q.push(root);
  Node *target = NULL;
  Node *last = NULL;
  Node *parentOfLast = NULL;
  int idx = 0;
  int targetIdx = -1;
  int lastIdx = -1;
  int matches = 0;

  cout << "Delete scan (level-order):\n";
  while (!q.empty()) {
    Node *cur = q.pop();
    cout << "  visit index " << idx << " value " << cur->key;
    if (cur->key == key) {
      cout << " <- match";
      matches++;
      if (!target) {
        target = cur;
        targetIdx = idx;
      }
    }
    cout << "\n";
    if (cur->left) {
      parentOfLast = cur;
      q.push(cur->left);
    }
    if (cur->right) {
      parentOfLast = cur;
      q.push(cur->right);
    }
    last = cur;
    lastIdx = idx;
    idx++;
  }

  if (!target) return false;

  if (matches > 1) {
    cout << "Value " << key << " appears " << matches
         << " times; deleting the first BFS occurrence at index " << targetIdx << ".\n";
  }
  cout << "Deepest node is index " << lastIdx << " value " << last->key << ".\n";
  cout << "Copy deepest value " << last->key << " into target index "
       << targetIdx << " (old value " << target->key << ").\n";
  target->key = last->key;

  // Remove deepest node
  if (last == root) {
    cout << "Deepest node was the root; tree becomes empty.\n";
    delete root;
    root = NULL;
    return true;
  }
  if (parentOfLast && parentOfLast->right == last) {
    cout << "Remove deepest node from RIGHT of parent " << parentOfLast->key << ".\n";
    delete parentOfLast->right;
    parentOfLast->right = NULL;
  } else if (parentOfLast && parentOfLast->left == last) {
    cout << "Remove deepest node from LEFT of parent " << parentOfLast->key << ".\n";
    delete parentOfLast->left;
    parentOfLast->left = NULL;
  }
  return true;
}

int main() {
  Node *root = NULL;
  int choice = 0;
  while (choice != 7) {
    cout << "\n=== T03: Binary Tree + Array Representation ===\n";
    cout << "1. Insert (Level-order)\n";
    cout << "2. Search (BFS)\n";
    cout << "3. Delete by value\n";
    cout << "4. Display level-order\n";
    cout << "5. Show array representation (BFS)\n";
    cout << "6. Clear tree\n";
    cout << "7. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      int k = readInt("Key: ");
      insertLevelOrder(root, k);
    } else if (choice == 2) {
      int k = readInt("Search key: ");
      searchBFS(root, k);
    } else if (choice == 3) {
      int k = readInt("Delete key: ");
      if (deleteByValue(root, k)) cout << "Deleted.\n";
      else cout << "Key not found.\n";
    } else if (choice == 4) {
      displayLevelOrder(root);
    } else if (choice == 5) {
      const int CAP = 256;
      int arr[CAP];
      int n = buildArrayRep(root, arr, CAP);
      cout << "(BFS) Array size=" << n << "\n";
      for (int i = 0; i < n; i++) {
        int li = 2 * i + 1;
        int ri = 2 * i + 2;
        cout << "i=" << i << " val=" << arr[i];
        cout << "  left(i=" << li << ")=" << (li < n ? arr[li] : -1);
        cout << "  right(i=" << ri << ")=" << (ri < n ? arr[ri] : -1);
        cout << "\n";
      }
      cout << "Array child indexes shown from i: left=2*i+1, right=2*i+2.\n";
    } else if (choice == 6) {
      freeTree(root);
      root = NULL;
      cout << "Tree cleared.\n";
    } else if (choice == 7) {
      cout << "Exiting...\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  freeTree(root);
  return 0;
}
