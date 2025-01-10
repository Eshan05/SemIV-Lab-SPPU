// 8. Read for the formulas in propositional calculus. Write a function that reads such a formula and creates its binary tree representation. What is the complexity of your function?

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

bool isOperand(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}

bool isOperator(char c) {
  return c == '!' || c == '&' || c == '|' || c == '>' || c == '=';
}

int precedence(char op) {
  if (op == '!') return 5;
  if (op == '&') return 4;
  if (op == '|') return 3;
  if (op == '>') return 2;
  if (op == '=') return 1;
  return 0;
}

bool isRightAssociative(char op) {
  return op == '!';
}

bool toPostfix(const string &infix, string &postfix, string &err) {
  char st[700];
  int top = -1;
  postfix = "";

  for (int i = 0; i < static_cast<int>(infix.size()); ++i) {
    char c = infix[i];
    if (c == ' ' || c == '\t') continue;

    if (isOperand(c)) {
      postfix += c;
    } else if (c == '(') {
      st[++top] = c;
    } else if (c == ')') {
      int found = 0;
      while (top != -1) {
        if (st[top] == '(') {
          found = 1;
          --top;
          break;
        }
        postfix += st[top--];
      }
      if (!found) {
        err = "Mismatched parentheses: ')' has no matching '('";
        return false;
      }
    } else if (isOperator(c)) {
      while (top != -1 && st[top] != '(' &&
             (precedence(st[top]) > precedence(c) ||
              (precedence(st[top]) == precedence(c) && !isRightAssociative(c)))) {
        postfix += st[top--];
      }
      st[++top] = c;
    } else {
      err = string("Invalid character in formula: '") + c + "'";
      return false;
    }
  }

  while (top != -1) {
    if (st[top] == '(') {
      err = "Mismatched parentheses: '(' has no matching ')'";
      return false;
    }
    postfix += st[top--];
  }

  if (postfix == "") {
    err = "Empty/invalid expression";
    return false;
  }
  return true;
}

bool postfixToTree(const string &post, Node *&root, string &err) {
  Node *st[700];
  int top = -1;

  for (int i = 0; i < static_cast<int>(post.size()); ++i) {
    char c = post[i];
    if (isOperand(c)) {
      st[++top] = createNode(c);
    } else if (isOperator(c)) {
      Node *op = createNode(c);
      if (c == '!') {
        if (top < 0) {
          err = "Invalid formula: missing operand for unary operator '!'";
          return false;
        }
        op->right = st[top--];
      } else {
        if (top < 1) {
          err = string("Invalid formula: missing operands for operator '") + c + "'";
          return false;
        }
        Node *r = st[top--];
        Node *l = st[top--];
        op->left = l;
        op->right = r;
      }
      st[++top] = op;
    } else {
      err = "Invalid postfix sequence";
      return false;
    }
  }

  if (top != 0) {
    err = "Invalid formula: too many operands/operators";
    return false;
  }
  root = st[top];
  return true;
}

void printIndented(Node *root, int depth) {
  if (root == 0) return;
  for (int i = 0; i < depth; ++i) cout << "  ";
  cout << root->data << "\n";
  printIndented(root->left, depth + 1);
  printIndented(root->right, depth + 1);
}

void inorderPretty(Node *root) {
  if (root == 0) return;
  if (root->left != 0 || root->right != 0) cout << "(";
  if (root->left != 0) inorderPretty(root->left);
  cout << root->data;
  if (root->right != 0) inorderPretty(root->right);
  if (root->left != 0 || root->right != 0) cout << ")";
}

void preorder(Node *root) {
  if (root == 0) return;
  cout << root->data;
  preorder(root->left);
  preorder(root->right);
}

void postorder(Node *root) {
  if (root == 0) return;
  postorder(root->left);
  postorder(root->right);
  cout << root->data;
}

void deleteTree(Node *root) {
  if (root == 0) return;
  deleteTree(root->left);
  deleteTree(root->right);
  delete root;
}

int main() {
  Node *root = 0;
  string postfix = "";

  while (true) {
    cout << "\n===== Formula Tree TUI =====\n";
    cout << "1. Parse formula and build tree\n";
    cout << "2. Show postfix\n";
    cout << "3. Show tree (indented)\n";
    cout << "4. Show traversals\n";
    cout << "5. Clear current tree\n";
    cout << "6. Exit\n";
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
      string infix;
      cout << "Enter propositional infix formula: ";
      getline(cin >> ws, infix);

      string err;
      string post;
      if (!toPostfix(infix, post, err)) {
        cout << "Parse error: " << err << "\n";
        continue;
      }

      Node *newRoot = 0;
      if (!postfixToTree(post, newRoot, err)) {
        cout << "Tree build error: " << err << "\n";
        continue;
      }

      deleteTree(root);
      root = newRoot;
      postfix = post;
      cout << "Tree built successfully\n";

    } else if (ch == 2) {
      if (root == 0) cout << "No formula loaded\n";
      else cout << "Postfix: " << postfix << "\n";

    } else if (ch == 3) {
      if (root == 0) cout << "No formula loaded\n";
      else {
        cout << "Binary Tree Representation:\n";
        printIndented(root, 0);
      }

    } else if (ch == 4) {
      if (root == 0) cout << "No formula loaded\n";
      else {
        cout << "Inorder (with brackets): ";
        inorderPretty(root);
        cout << "\n";
        cout << "Preorder: ";
        preorder(root);
        cout << "\n";
        cout << "Postorder: ";
        postorder(root);
        cout << "\n";
      }

    } else if (ch == 5) {
      deleteTree(root);
      root = 0;
      postfix = "";
      cout << "Cleared\n";

    } else if (ch == 6) {
      break;

    } else {
      cout << "Invalid choice\n";
    }
  }

  deleteTree(root);
  return 0;
}
