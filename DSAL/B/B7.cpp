// 7. Construct an expression tree from the given prefix expression [Ex: `+--a*bc/def`] and traverse it using post order traversal (Non-recursive) and then delete the entire tree.

#include <iostream>
#include <string>
using namespace std;

struct Node {
  char data;
  Node *left;
  Node *right;
};

Node *createNode(char c) {
  Node *n = new Node;
  n->data = c;
  n->left = 0;
  n->right = 0;
  return n;
}

bool isOperator(char c) {
  return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

void printStack(Node **st, int top) {
  cout << "Stack(top->bottom): ";
  for (int i = top; i >= 0; --i) cout << st[i]->data << " ";
  if (top == -1) cout << "(empty)";
  cout << "\n";
}

Node *buildFromPrefixWithSteps(const string &pre) {
  int n = static_cast<int>(pre.size());
  if (n == 0) return 0;

  Node **st = new Node *[n + 5];
  int top = -1;

  cout << "\n--- Construction Steps (scan right to left) ---\n";
  for (int i = n - 1; i >= 0; --i) {
    char c = pre[i];
    if (c == ' ') continue;

    if (!isOperator(c)) {
      Node *nd = createNode(c);
      st[++top] = nd;
      cout << "Read operand '" << c << "' -> push node\n";
      printStack(st, top);
    } else {
      cout << "Read operator '" << c << "' -> need 2 operands from stack\n";
      if (top < 1) {
        cout << "Error: insufficient operands\n";
        delete[] st;
        return 0;
      }
      Node *left = st[top--];
      Node *right = st[top--];
      Node *op = createNode(c);
      op->left = left;
      op->right = right;
      st[++top] = op;
      cout << "Create subtree: (" << c << " with left=" << left->data << ", right=" << right->data << ")\n";
      printStack(st, top);
    }
  }

  Node *root = 0;
  if (top == 0) root = st[top];
  else cout << "Error: invalid expression, final stack size != 1\n";

  delete[] st;
  return root;
}

void postorderNonRecursiveWithSteps(Node *root) {
  if (root == 0) return;

  Node *s1[600];
  Node *s2[600];
  int t1 = -1, t2 = -1;

  cout << "\n--- Non-Recursive Postorder Steps ---\n";
  s1[++t1] = root;
  cout << "Push root " << root->data << " to S1\n";

  while (t1 != -1) {
    Node *cur = s1[t1--];
    s2[++t2] = cur;
    cout << "Pop " << cur->data << " from S1, push to S2\n";
    if (cur->left != 0) {
      s1[++t1] = cur->left;
      cout << "  Push left child " << cur->left->data << " to S1\n";
    }
    if (cur->right != 0) {
      s1[++t1] = cur->right;
      cout << "  Push right child " << cur->right->data << " to S1\n";
    }
  }

  cout << "Postorder: ";
  while (t2 != -1) cout << s2[t2--]->data;
  cout << "\n";
}

void deleteTree(Node *root) {
  if (root == 0) return;
  deleteTree(root->left);
  deleteTree(root->right);
  delete root;
}

int main() {
  while (true) {
    cout << "\n1. Build expression tree from prefix and show steps\n";
    cout << "2. Exit\n";
    cout << "Enter choice: ";
    int ch;
    if (!(cin >> ch)) {
      cin.clear();
      char c;
      while (cin.get(c) && c != '\n') {}
      cout << "Invalid choice\n";
      continue;
    }

    if (ch == 1) {
      cout << "Enter prefix expression: ";
      string pre;
      getline(cin >> ws, pre);

      Node *root = buildFromPrefixWithSteps(pre);
      if (root == 0) {
        cout << "Invalid prefix expression\n";
      } else {
        postorderNonRecursiveWithSteps(root);
        deleteTree(root);
        cout << "Tree deleted successfully\n";
      }
    } else if (ch == 2) {
      break;
    } else {
      cout << "Invalid choice\n";
    }
  }

  return 0;
}
