// 6. DFS and BFS: recursive + iterative
// Demo: use a binary tree as the structure to traverse.
// - DFS recursive: preorder
// - DFS iterative: stack
// - BFS iterative: queue
// - BFS "recursive": print each level using recursion (uses height)

#include <iostream>
using namespace std;

struct Node {
  int key;
  Node* left;
  Node* right;
  Node(int k) : key(k), left(NULL), right(NULL) {}
};

struct Stack {
  Node** a;
  int n;
  int cap;
  Stack() : a(NULL), n(0), cap(0) {}
  ~Stack() { delete[] a; }
  bool empty() const { return n == 0; }
  void push(Node* x) {
    if (n >= cap) {
      int nc = (cap == 0) ? 8 : cap * 2;
      Node** b = new Node*[nc];
      for (int i = 0; i < n; i++) b[i] = a[i];
      delete[] a;
      a = b;
      cap = nc;
    }
    a[n++] = x;
  }
  Node* pop() {
    if (n == 0) return NULL;
    return a[--n];
  }
};

struct Queue {
  Node** a;
  int cap;
  int head;
  int tail;
  int n;
  Queue(int c) {
    cap = (c < 8) ? 8 : c;
    a = new Node*[cap];
    head = tail = n = 0;
  }
  ~Queue() { delete[] a; }
  bool empty() const { return n == 0; }
  void push(Node* x) {
    if (n == cap) {
      int nc = cap * 2;
      Node** b = new Node*[nc];
      for (int i = 0; i < n; i++) b[i] = a[(head + i) % cap];
      delete[] a;
      a = b;
      cap = nc;
      head = 0;
      tail = n;
    }
    a[tail] = x;
    tail = (tail + 1) % cap;
    n++;
  }
  Node* pop() {
    if (n == 0) return NULL;
    Node* x = a[head];
    head = (head + 1) % cap;
    n--;
    return x;
  }
};

static void clearInput() {
  cin.clear();
  cin.ignore(1024, '\n');
}

static int readInt(const string& prompt) {
  int x;
  while (true) {
    cout << prompt;
    if (cin >> x) return x;
    cout << "Invalid.\n";
    clearInput();
  }
}

static void freeTree(Node* root) {
  if (!root) return;
  freeTree(root->left);
  freeTree(root->right);
  delete root;
}

static void insertLevelOrder(Node*& root, int k) {
  Node* nn = new Node(k);
  if (!root) {
    root = nn;
    return;
  }
  Queue q(32);
  q.push(root);
  while (!q.empty()) {
    Node* cur = q.pop();
    if (!cur->left) {
      cur->left = nn;
      return;
    }
    if (!cur->right) {
      cur->right = nn;
      return;
    }
    q.push(cur->left);
    q.push(cur->right);
  }
}

static void displayLevelOrder(Node* root) {
  if (!root) {
    cout << "(empty)\n";
    return;
  }
  Queue q(32);
  q.push(root);
  cout << "Level-order: ";
  while (!q.empty()) {
    Node* cur = q.pop();
    cout << cur->key << " ";
    if (cur->left) q.push(cur->left);
    if (cur->right) q.push(cur->right);
  }
  cout << "\n";
}

static void dfsRec(Node* root) {
  if (!root) return;
  cout << root->key << " ";
  dfsRec(root->left);
  dfsRec(root->right);
}

static void dfsIter(Node* root) {
  if (!root) return;
  Stack st;
  st.push(root);
  while (!st.empty()) {
    Node* cur = st.pop();
    cout << cur->key << " ";
    if (cur->right) st.push(cur->right);
    if (cur->left) st.push(cur->left);
  }
}

static void bfsIter(Node* root) {
  if (!root) return;
  Queue q(32);
  q.push(root);
  while (!q.empty()) {
    Node* cur = q.pop();
    cout << cur->key << " ";
    if (cur->left) q.push(cur->left);
    if (cur->right) q.push(cur->right);
  }
}

static int height(Node* root) {
  if (!root) return 0;
  int hl = height(root->left);
  int hr = height(root->right);
  return (hl > hr ? hl : hr) + 1;
}

static void printLevel(Node* root, int level) {
  if (!root) return;
  if (level == 1) {
    cout << root->key << " ";
    return;
  }
  printLevel(root->left, level - 1);
  printLevel(root->right, level - 1);
}

static void bfsRec(Node* root) {
  int h = height(root);
  for (int lvl = 1; lvl <= h; lvl++) {
    printLevel(root, lvl);
  }
}

int main() {
  Node* root = NULL;
  int choice = 0;
  while (choice != 7) {
    cout << "\n=== T08: DFS/BFS (Recursive + Iterative) on Binary Tree ===\n";
    cout << "1. Insert (level-order)\n";
    cout << "2. Display level-order\n";
    cout << "3. DFS recursive (preorder)\n";
    cout << "4. DFS iterative (stack)\n";
    cout << "5. BFS iterative (queue)\n";
    cout << "6. BFS recursive (by levels)\n";
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
      displayLevelOrder(root);
    } else if (choice == 3) {
      cout << "DFS(rec): ";
      dfsRec(root);
      cout << "\n";
    } else if (choice == 4) {
      cout << "DFS(iter): ";
      dfsIter(root);
      cout << "\n";
    } else if (choice == 5) {
      cout << "BFS(iter): ";
      bfsIter(root);
      cout << "\n";
    } else if (choice == 6) {
      cout << "BFS(rec): ";
      bfsRec(root);
      cout << "\n";
    } else if (choice == 7) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  freeTree(root);
  return 0;
}
