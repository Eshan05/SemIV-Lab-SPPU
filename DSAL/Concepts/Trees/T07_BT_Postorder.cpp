// 5c. Binary tree traversal: POSTORDER (recursive + iterative)

#include <iostream>
#include <string>
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
  Node* top() const {
    if (n == 0) return NULL;
    return a[n - 1];
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
    cout << "Tree empty -> new node becomes ROOT.\n";
    return;
  }
  Queue q(32);
  q.push(root);
  cout << "Visited nodes (level-order dequeue): ";
  while (!q.empty()) {
    Node* cur = q.pop();
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

static void displayLevelOrder(Node* root) {
  if (!root) {
    cout << "(empty)\n";
    return;
  }
  Queue q(32);
  q.push(root);
  cout << "Level-order (BFS using queue). Dequeue order == visit order:\n";
  int step = 0;
  while (!q.empty()) {
    Node* cur = q.pop();
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

static void postorderRec(Node* root) {
  if (!root) return;
  postorderRec(root->left);
  postorderRec(root->right);
  cout << root->key << " ";
}

static void postorderIter(Node* root) {
  // One-stack method using a previous pointer.
  if (!root) {
    cout << "(empty)\n";
    return;
  }
  cout << "Iterative postorder (one stack + prev pointer):\n";
  cout << "Idea: push while going LEFT. Peek stack-top. If its RIGHT exists and not processed -> go RIGHT. Else visit+pop.\n";
  Stack st;
  Node* cur = root;
  Node* prev = NULL;
  int visitStep = 0;
  while (cur || !st.empty()) {
    if (cur) {
      cout << "  push " << cur->key << " (go left)\n";
      st.push(cur);
      cur = cur->left;
    } else {
      Node* peek = st.top();
      if (peek->right && prev != peek->right) {
        cout << "  peek " << peek->key << ": right child " << peek->right->key << " not done -> go right\n";
        cur = peek->right;
      } else {
        visitStep++;
        cout << "  visit-step " << visitStep << ": visit " << peek->key << " -> pop\n";
        prev = peek;
        st.pop();
      }
    }
  }
}

int main() {
  Node* root = NULL;
  int choice = 0;
  while (choice != 6) {
    cout << "\n=== T07: Binary Tree POSTORDER Traversal ===\n";
    cout << "1. Insert (level-order)\n";
    cout << "2. Display level-order\n";
    cout << "3. Postorder (recursive)\n";
    cout << "4. Postorder (iterative 1-stack)\n";
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
      cout << "Recursive postorder rule: left -> right -> visit\n";
      cout << "Order: ";
      postorderRec(root);
      cout << "\n";
    } else if (choice == 4) {
      cout << "\n";
      postorderIter(root);
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
