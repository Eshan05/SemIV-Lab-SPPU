// 11. A Dictionary stores keywords and its meanings. Provide facility for adding new keywords, deleting keywords, updating values of any entry. Provide facility to display whole data sorted in ascending/ Descending order. Also find how many maximum comparisons may require for finding any keyword. Use Binary Search Tree for implementation.

#include <iostream>
#include <string>
using namespace std;

struct Node {
  string key;
  string meaning;
  Node *left;
  Node *right;
};

Node *createNode(const string &k, const string &m) {
  Node *n = new Node;
  n->key = k;
  n->meaning = m;
  n->left = 0;
  n->right = 0;
  return n;
}

Node *insertOrUpdate(Node *root, const string &k, const string &m, bool updateOnly, bool &success) {
  if (root == 0) {
    if (updateOnly) {
      success = false;
      return 0;
    }
    success = true;
    return createNode(k, m);
  }

  if (k < root->key) {
    root->left = insertOrUpdate(root->left, k, m, updateOnly, success);
  } else if (k > root->key) {
    root->right = insertOrUpdate(root->right, k, m, updateOnly, success);
  } else {
    root->meaning = m;
    success = true;
  }
  return root;
}

Node *minNode(Node *root) {
  Node *cur = root;
  while (cur != 0 && cur->left != 0) cur = cur->left;
  return cur;
}

Node *deleteKey(Node *root, const string &k, bool &deleted) {
  if (root == 0) return 0;

  if (k < root->key) {
    root->left = deleteKey(root->left, k, deleted);
  } else if (k > root->key) {
    root->right = deleteKey(root->right, k, deleted);
  } else {
    deleted = true;
    if (root->left == 0) {
      Node *t = root->right;
      delete root;
      return t;
    }
    if (root->right == 0) {
      Node *t = root->left;
      delete root;
      return t;
    }

    Node *succ = minNode(root->right);
    root->key = succ->key;
    root->meaning = succ->meaning;
    bool dummy = false;
    root->right = deleteKey(root->right, succ->key, dummy);
  }

  return root;
}

bool searchKey(Node *root, const string &k, string &meaning, int &comparisons) {
  comparisons = 0;
  Node *cur = root;
  while (cur != 0) {
    ++comparisons;
    if (k == cur->key) {
      meaning = cur->meaning;
      return true;
    }
    if (k < cur->key) cur = cur->left;
    else cur = cur->right;
  }
  return false;
}

void inorder(Node *root) {
  if (root == 0) return;
  inorder(root->left);
  cout << root->key << " : " << root->meaning << "\n";
  inorder(root->right);
}

void reverseInorder(Node *root) {
  if (root == 0) return;
  reverseInorder(root->right);
  cout << root->key << " : " << root->meaning << "\n";
  reverseInorder(root->left);
}

int height(Node *root) {
  if (root == 0) return 0;
  int l = height(root->left);
  int r = height(root->right);
  return (l > r ? l : r) + 1;
}

void deleteTree(Node *root) {
  if (root == 0) return;
  deleteTree(root->left);
  deleteTree(root->right);
  delete root;
}

int main() {
  Node *root = 0;

  while (true) {
    cout << "\n1. Add keyword\n";
    cout << "2. Delete keyword\n";
    cout << "3. Update meaning\n";
    cout << "4. Search keyword\n";
    cout << "5. Display ascending\n";
    cout << "6. Display descending\n";
    cout << "7. Maximum comparisons needed\n";
    cout << "8. Exit\n";
    cout << "Enter choice: ";

    int ch;
    cin >> ch;

    if (ch == 1) {
      string k, m;
      bool ok = false;
      cout << "Enter keyword: ";
      cin >> k;
      cout << "Enter meaning (single_word): ";
      cin >> m;
      root = insertOrUpdate(root, k, m, false, ok);
      cout << (ok ? "Added/Updated\n" : "Failed\n");

    } else if (ch == 2) {
      string k;
      bool del = false;
      cout << "Enter keyword to delete: ";
      cin >> k;
      root = deleteKey(root, k, del);
      cout << (del ? "Deleted\n" : "Keyword not found\n");

    } else if (ch == 3) {
      string k, m;
      bool ok = false;
      cout << "Enter keyword to update: ";
      cin >> k;
      cout << "Enter new meaning (single_word): ";
      cin >> m;
      root = insertOrUpdate(root, k, m, true, ok);
      cout << (ok ? "Updated\n" : "Keyword not found\n");

    } else if (ch == 4) {
      string k, m;
      int c = 0;
      cout << "Enter keyword to search: ";
      cin >> k;
      if (searchKey(root, k, m, c)) {
        cout << "Found. Meaning: " << m << "\n";
      } else {
        cout << "Not found\n";
      }
      cout << "Comparisons used: " << c << "\n";

    } else if (ch == 5) {
      cout << "\nDictionary (Ascending):\n";
      inorder(root);

    } else if (ch == 6) {
      cout << "\nDictionary (Descending):\n";
      reverseInorder(root);

    } else if (ch == 7) {
      cout << "Maximum comparisons in worst case: " << height(root) << "\n";

    } else if (ch == 8) {
      break;

    } else {
      cout << "Invalid choice\n";
    }
  }

  deleteTree(root);
  return 0;
}
