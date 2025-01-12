// 5b. Binary tree traversal: INORDER (recursive + iterative)

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

static void inorderRec(Node* root) {
  if (!root) return;
  inorderRec(root->left);
  cout << root->key << " ";
  inorderRec(root->right);
}

static void inorderIter(Node* root) {
  Stack st;
  Node* cur = root;
  while (cur || !st.empty()) {
    while (cur) {
      st.push(cur);
      cur = cur->left;
    }
    cur = st.pop();
    cout << cur->key << " ";
    cur = cur->right;
  }
}

int main() {
  Node* root = NULL;
  int choice = 0;
  while (choice != 6) {
    cout << "\n=== T06: Binary Tree INORDER Traversal ===\n";
    cout << "1. Insert (level-order)\n";
    cout << "2. Display level-order\n";
    cout << "3. Inorder (recursive)\n";
    cout << "4. Inorder (iterative stack)\n";
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
      cout << "Inorder(rec): ";
      inorderRec(root);
      cout << "\n";
    } else if (choice == 4) {
      cout << "Inorder(iter): ";
      inorderIter(root);
      cout << "\n";
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
