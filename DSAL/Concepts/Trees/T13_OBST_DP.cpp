// 11. Optimal Binary Search Tree (OBST)
// Demo: Given sorted keys and successful-search probabilities p[i], build OBST with minimum expected cost.
// Uses classic DP: cost(i,j) = min_k cost(i,k-1)+cost(k+1,j)+sum(p[i..j])

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

static double readDouble(const string &prompt) {
  double x;
  while (true) {
    cout << prompt;
    if (cin >> x) return x;
    cout << "Invalid number.\n";
    clearInput();
  }
}

static bool sortKeyProbPairs(int keys[], double p[], int n) {
  bool moved = false;
  for (int i = 2; i <= n; i++) {
    int key = keys[i];
    double prob = p[i];
    int j = i - 1;
    while (j >= 1 && keys[j] > key) {
      if (!moved) cout << "Keys are not sorted; sort key/probability pairs before DP.\n";
      moved = true;
      cout << "  move (" << keys[j] << "," << p[j] << ") right\n";
      keys[j + 1] = keys[j];
      p[j + 1] = p[j];
      j--;
    }
    if (j + 1 != i) {
      cout << "  place (" << key << "," << prob << ") at position " << j + 1 << "\n";
    }
    keys[j + 1] = key;
    p[j + 1] = prob;
  }

  for (int i = 2; i <= n; i++) {
    if (keys[i] == keys[i - 1]) {
      cout << "Duplicate key " << keys[i] << " found after sorting. Reload unique keys.\n";
      return false;
    }
  }

  if (moved) {
    cout << "Sorted pairs used by OBST:\n";
    for (int i = 1; i <= n; i++) {
      cout << "  " << i << ": key=" << keys[i] << ", p=" << p[i] << "\n";
    }
  }
  return true;
}

struct Node {
  int key;
  Node *left;
  Node *right;
  Node(int k) : key(k), left(NULL), right(NULL) {}
};

static void freeTree(Node *root) {
  if (!root) return;
  freeTree(root->left);
  freeTree(root->right);
  delete root;
}

static void preorder(Node *root) {
  if (!root) return;
  cout << root->key << " ";
  preorder(root->left);
  preorder(root->right);
}

static void inorder(Node *root) {
  if (!root) return;
  inorder(root->left);
  cout << root->key << " ";
  inorder(root->right);
}

static Node *buildFromRootTable(int keys[], int rootK[25][25], int i, int j) {
  if (i > j) return NULL;
  int k = rootK[i][j];
  if (k < i || k > j) return NULL;
  Node *r = new Node(keys[k]);
  r->left = buildFromRootTable(keys, rootK, i, k - 1);
  r->right = buildFromRootTable(keys, rootK, k + 1, j);
  return r;
}

static bool searchCount(Node *root, int key, int &comps) {
  comps = 0;
  Node *cur = root;
  while (cur) {
    comps++;
    cout << "Compare " << key << " with " << cur->key << ": ";
    if (key == cur->key) return true;
    if (key < cur->key) {
      cout << "go left\n";
      cur = cur->left;
    } else {
      cout << "go right\n";
      cur = cur->right;
    }
  }
  cout << "Reached NULL.\n";
  return false;
}

int main() {
  int keys[25];
  double p[25];
  int n = 0;
  double cost[25][25];
  int rootK[25][25];
  double sumP[25];
  bool computed = false;
  Node *root = NULL;

  int choice = 0;
  while (choice != 6) {
    cout << "\n=== T13: OBST (DP) ===\n";
    cout << "1. Enter keys + probabilities/frequencies\n";
    cout << "2. Compute OBST (DP) + show cost\n";
    cout << "3. Display constructed tree\n";
    cout << "4. Search in OBST (count comparisons)\n";
    cout << "5. Clear\n";
    cout << "6. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      freeTree(root);
      root = NULL;
      computed = false;

      int requestedN = readInt("n (<=20): ");
      if (requestedN < 1) {
        cout << "n must be at least 1; using 1.\n";
        requestedN = 1;
      }
      if (requestedN > 20) {
        cout << "n capped at 20 for the fixed demo arrays.\n";
        requestedN = 20;
      }
      n = requestedN;
      cout << "Enter keys:\n";
      for (int i = 1; i <= n; i++) {
        keys[i] = readInt(string("key[") + to_string(i) + "]: ");
      }
      cout << "Enter probabilities/frequencies p[i] for the matching keys:\n";
      for (int i = 1; i <= n; i++) {
        p[i] = readDouble(string("p[") + to_string(i) + "]: ");
        if (p[i] < 0) {
          cout << "p[" << i << "] is negative; use 0.\n";
          p[i] = 0;
        }
      }
      if (!sortKeyProbPairs(keys, p, n)) {
        n = 0;
        continue;
      }
      sumP[0] = 0;
      for (int i = 1; i <= n; i++) sumP[i] = sumP[i - 1] + p[i];
      cout << "Loaded " << n << " sorted key/probability pairs. Total weight=" << sumP[n] << "\n";
    } else if (choice == 2) {
      if (n == 0) {
        cout << "Enter data first.\n";
        continue;
      }
      freeTree(root);
      root = NULL;

      // init
      for (int i = 1; i <= n + 1; i++) {
        cost[i][i - 1] = 0;
        rootK[i][i - 1] = 0;
      }
      for (int len = 1; len <= n; len++) {
        for (int i = 1; i + len - 1 <= n; i++) {
          int j = i + len - 1;
          double w = sumP[j] - sumP[i - 1];
          double best = 1e100;
          int bestK = i;
          cout << "\nInterval [" << i << "," << j << "] keys "
               << keys[i] << ".." << keys[j] << ", weight sum=" << w << "\n";
          for (int k = i; k <= j; k++) {
            double c = cost[i][k - 1] + cost[k + 1][j] + w;
            cout << "  try root key " << keys[k] << ": left=" << cost[i][k - 1]
                 << " + right=" << cost[k + 1][j] << " + weight=" << w
                 << " => " << c << "\n";
            if (c < best) {
              best = c;
              bestK = k;
            }
          }
          cost[i][j] = best;
          rootK[i][j] = bestK;
          cout << "range [" << i << "," << j << "] -> root key " << keys[bestK]
               << ", cost=" << best << "\n";
        }
      }
      cout << "Optimal cost=" << cost[1][n] << "\n";
      cout << "Root index=" << rootK[1][n] << " (key=" << keys[rootK[1][n]] << ")\n";
      root = buildFromRootTable(keys, rootK, 1, n);
      computed = true;
    } else if (choice == 3) {
      if (!computed || !root) {
        cout << "Compute OBST first.\n";
        continue;
      }
      cout << "Preorder: ";
      preorder(root);
      cout << "\n";
      cout << "Inorder: ";
      inorder(root);
      cout << "\n";
    } else if (choice == 4) {
      if (!computed || !root) {
        cout << "Compute OBST first.\n";
        continue;
      }
      int k = readInt("Search key: ");
      int comps = 0;
      bool ok = searchCount(root, k, comps);
      cout << (ok ? "Found" : "Not found") << ", comparisons=" << comps << "\n";
    } else if (choice == 5) {
      freeTree(root);
      root = NULL;
      computed = false;
      n = 0;
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
