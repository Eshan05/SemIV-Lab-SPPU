// 5a. Binary tree traversal: PREORDER (recursive + iterative)

#include <iostream>
#include <string>
using namespace std;

struct Node {
  int key;
  Node *left;
  Node *right;
  Node(int k) : key(k), left(NULL), right(NULL) {}
};

struct Stack {
  Node **a;
  int n;
  int cap;
  Stack() : a(NULL), n(0), cap(0) {}
  ~Stack() { delete[] a; }
  bool empty() const { return n == 0; }
  void push(Node *x) {
    if (n >= cap) {
      int nc = (cap == 0) ? 8 : cap * 2;
      Node **b = new Node *[nc];
      for (int i = 0; i < n; i++) b[i] = a[i];
      delete[] a;
      a = b;
      cap = nc;
    }
    a[n++] = x;
  }
  Node *pop() {
    if (n == 0) return NULL;
    return a[--n];
  }
};

struct Queue {
  Node **a;
  int cap;
  int head;
  int tail;
  int n;
  Queue(int c) {
    cap = (c < 8) ? 8 : c;
    a = new Node *[cap];
    head = tail = n = 0;
  }
  ~Queue() { delete[] a; }
  bool empty() const { return n == 0; }
  void push(Node *x) {
    if (n == cap) {
      int nc = cap * 2;
      Node **b = new Node *[nc];
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
    cout << "Invalid.\n";
    clearInput();
  }
}

static void freeTree(Node *root) {
  if (!root) return;
  freeTree(root->left);
  freeTree(root->right);
  delete root;
}

static void insertLevelOrder(Node *&root, int k) {
  Node *nn = new Node(k);
  if (!root) {
    root = nn;
    cout << "Tree empty -> new node becomes ROOT.\n";
    return;
  }
  Queue q(32);
  q.push(root);
  cout << "Visited nodes (level-order dequeue): ";
  while (!q.empty()) {
    Node *cur = q.pop();
    cout << cur->key << " ";
    if (!cur->left) {
      cur->left = nn;
      cout << "\nInserted " << k << " as LEFT child of " << cur->key << ".\n";
      return;
    }
    if (!cur->right) {
      cur->right = nn;
      cout << "\nInserted " << k << " as RIGHT child of " << cur->key << ".\n";
      return;
    }
    q.push(cur->left);
    q.push(cur->right);
  }
}

static void displayLevelOrder(Node *root) {
  if (!root) {
    cout << "(empty)\n";
    return;
  }
  Queue q(32);
  q.push(root);
  cout << "Level-order (BFS using queue). Dequeue order == visit order:\n";
  int step = 0;
  while (!q.empty()) {
    Node *cur = q.pop();
    step++;
    cout << "  step " << step << ": dequeue " << cur->key << " -> visit " << cur->key;
    if (cur->left) {
      cout << ", enqueue L=" << cur->left->key;
      q.push(cur->left);
    }
    if (cur->right) {
      cout << ", enqueue R=" << cur->right->key;
      q.push(cur->right);
    }
    cout << "\n";
  }
}

static void preorderRec(Node *root) {
  if (!root) return;
  cout << root->key << " ";
  preorderRec(root->left);
  preorderRec(root->right);
}

static void preorderIter(Node *root) {
  if (!root) {
    cout << "(empty)\n";
    return;
  }
  cout << "Stack order: pop -> visit -> push RIGHT -> push LEFT\n";
  Stack st;
  st.push(root);
  cout << "  step 0: push root " << root->key << "\n";
  int step = 0;
  while (!st.empty()) {
    Node *cur = st.pop();
    step++;
    cout << "  step " << step << ": pop " << cur->key << " -> visit " << cur->key;
    // push right first so left is processed first
    if (cur->right) {
      cout << ", push R=" << cur->right->key;
      st.push(cur->right);
    }
    if (cur->left) {
      cout << ", push L=" << cur->left->key;
      st.push(cur->left);
    }
    if (!cur->left && !cur->right) cout << ", (leaf)";
    cout << "\n";
  }
}

int main() {
  Node *root = NULL;
  int choice = 0;
  while (choice != 6) {
    cout << "\n=== T05: Binary Tree PREORDER Traversal ===\n";
    cout << "1. Insert (Level-order)\n";
    cout << "2. Display Level-order\n";
    cout << "3. Preorder (Recursive)\n";
    cout << "4. Preorder (Iterative)\n";
    cout << "5. Clear\n";
    cout << "6. Exit\n";
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
      cout << "Recursive preorder rule: visit -> left -> right\n";
      cout << "Order: ";
      preorderRec(root);
      cout << "\n";
    } else if (choice == 4) {
      cout << "\n";
      preorderIter(root);
    } else if (choice == 5) {
      freeTree(root);
      root = NULL;
      cout << "Cleared.\n";
    } else if (choice == 6) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  freeTree(root);
  return 0;
}
