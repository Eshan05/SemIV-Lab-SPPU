// 18. Given sequence $k = k_1 < k_2 < \dots <k_n$ of $n$ sorted keys, with a search probability $\pi$ for each key $k_i$ . Build the Binary search tree that has the least search cost given the access probability for each key

#include <iostream>
#include <string>
using namespace std;

const int MAXN = 60; // DP tables are (n+1) x (n+1)

int n = 0;
int keys[MAXN + 1];
double p[MAXN + 1];
double q[MAXN + 1];

double w[MAXN + 1][MAXN + 1];
double cst[MAXN + 1][MAXN + 1];
int r[MAXN + 1][MAXN + 1];

bool hasInput = false;
bool built = false;

void clearLine() {
  char c;
  while (cin.get(c)) {
    if (c == '\n') break;
  }
}

int readInt(const string &prompt) {
  int x;
  while (true) {
    cout << prompt;
    if (cin >> x) return x;
    cout << "Invalid number. Try again.\n";
    cin.clear();
    clearLine();
  }
}

double readDouble(const string &prompt) {
  double x;
  while (true) {
    cout << prompt;
    if (cin >> x) return x;
    cout << "Invalid number. Try again.\n";
    cin.clear();
    clearLine();
  }
}

void resetAll() {
  n = 0;
  hasInput = false;
  built = false;
}

void inputData() {
  resetAll();
  cout << "\n===== Optimal BST Input (D18) =====\n";
  n = readInt("Enter number of keys (1-60): ");
  if (n < 1) {
    cout << "Need at least 1 key.\n";
    n = 0;
    return;
  }
  if (n > MAXN) {
    cout << "Clamping to MAXN=" << MAXN << "\n";
    n = MAXN;
  }

  cout << "Enter the keys in strictly increasing order.\n";
  for (int i = 1; i <= n; ++i) {
    while (true) {
      keys[i] = readInt("k[" + to_string(i) + "] = ");
      if (i == 1 || keys[i] > keys[i - 1]) break;
      cout << "Key must be > previous key (" << keys[i - 1] << "). Re-enter.\n";
    }
  }

  cout << "\nEnter success/search probabilities p[i] for each key k[i].\n";
  cout << "(They can be weights or probabilities; non-negative numbers.)\n";
  for (int i = 1; i <= n; ++i) {
    p[i] = readDouble("p[" + to_string(i) + "] = ");
    if (p[i] < 0) {
      cout << "Negative not allowed; using 0.\n";
      p[i] = 0;
    }
  }

  int mode = readInt("\nEnter 1 to also input failure probabilities q[0..n] (full OBST), or 2 to assume all q=0: ");
  if (mode == 1) {
    cout << "\nEnter failure probabilities q[0..n].\n";
    for (int i = 0; i <= n; ++i) {
      q[i] = readDouble("q[" + to_string(i) + "] = ");
      if (q[i] < 0) {
        cout << "Negative not allowed; using 0.\n";
        q[i] = 0;
      }
    }
  } else {
    for (int i = 0; i <= n; ++i) q[i] = 0;
  }

  hasInput = true;
  built = false;
  cout << "\nInput loaded. Now choose 'Build OBST'.\n";
}

int minValue(int i, int j) {
  int start = r[i][j - 1];
  int end = r[i + 1][j];
  if (start == 0) start = i + 1;
  if (end == 0) end = j;

  int bestK = start;
  double best = 1e100;
  for (int k = start; k <= end; ++k) {
    double val = cst[i][k - 1] + cst[k][j];
    if (val < best) {
      best = val;
      bestK = k;
    }
  }
  return bestK;
}

void buildOBST() {
  if (!hasInput) {
    cout << "Load input first.\n";
    return;
  }

  for (int i = 0; i <= n; ++i) {
    for (int j = 0; j <= n; ++j) {
      w[i][j] = 0;
      cst[i][j] = 0;
      r[i][j] = 0;
    }
  }

  for (int i = 0; i < n; ++i) {
    w[i][i] = q[i];
    cst[i][i] = 0;
    r[i][i] = 0;

    w[i][i + 1] = q[i] + q[i + 1] + p[i + 1];
    cst[i][i + 1] = w[i][i + 1];
    r[i][i + 1] = i + 1;
  }
  w[n][n] = q[n];
  cst[n][n] = 0;
  r[n][n] = 0;

  for (int m = 2; m <= n; ++m) {
    for (int i = 0; i <= n - m; ++i) {
      int j = i + m;
      w[i][j] = w[i][j - 1] + p[j] + q[j];
      int k = minValue(i, j);
      cst[i][j] = w[i][j] + cst[i][k - 1] + cst[k][j];
      r[i][j] = k;
    }
  }

  built = true;
  cout.setf(ios::fixed);
  cout.precision(4);
  cout << "\nOBST built successfully. Minimum expected cost = " << cst[0][n] << "\n";
}

void displayTables() {
  if (!built) {
    cout << "Build the OBST first.\n";
    return;
  }

  cout.setf(ios::fixed);
  cout.precision(4);

  cout << "\nWeight (w) Table\n";
  cout << "i/j\t";
  for (int j = 0; j <= n; ++j) cout << j << "\t";
  cout << "\n";
  for (int i = 0; i <= n; ++i) {
    cout << i << "\t";
    for (int j = 0; j <= n; ++j) cout << w[i][j] << "\t";
    cout << "\n";
  }

  cout << "\nCost (c) Table\n";
  cout << "i/j\t";
  for (int j = 0; j <= n; ++j) cout << j << "\t";
  cout << "\n";
  for (int i = 0; i <= n; ++i) {
    cout << i << "\t";
    for (int j = 0; j <= n; ++j) cout << cst[i][j] << "\t";
    cout << "\n";
  }

  cout.unsetf(ios::floatfield);
  cout << "\nRoot (r) Table\n";
  cout << "i/j\t";
  for (int j = 0; j <= n; ++j) cout << j << "\t";
  cout << "\n";
  for (int i = 0; i <= n; ++i) {
    cout << i << "\t";
    for (int j = 0; j <= n; ++j) cout << r[i][j] << "\t";
    cout << "\n";
  }
}

void printEdges(int i, int j) {
  if (i >= j) return;
  int k = r[i][j];
  if (k == 0) return;

  int leftK = r[i][k - 1];
  int rightK = r[k][j];

  cout << "\t" << keys[k] << "\t\t";
  if (leftK != 0) cout << keys[leftK];
  else cout << "-";
  cout << "\t\t";
  if (rightK != 0) cout << keys[rightK];
  else cout << "-";
  cout << "\n";

  printEdges(i, k - 1);
  printEdges(k, j);
}

void displayTree() {
  if (!built) {
    cout << "Build the OBST first.\n";
    return;
  }
  if (n == 0) {
    cout << "No keys.\n";
    return;
  }

  int rootK = r[0][n];
  cout.setf(ios::fixed);
  cout.precision(4);
  cout << "\nOptimal BST Root: " << keys[rootK] << "\n";
  cout << "Minimum expected search cost: " << cst[0][n] << "\n";
  cout << "\n\tNODE\t\tLEFT\t\tRIGHT\n";
  printEdges(0, n);
}

int main() {
  while (true) {
    cout << "\n===== Group D18 (Optimal BST) =====\n";
    cout << "1. Input keys and probabilities\n";
    cout << "2. Build OBST (DP tables)\n";
    cout << "3. Display W/C/R tables\n";
    cout << "4. Display Optimal BST (Node relationships)\n";
    cout << "5. Reset\n";
    cout << "6. Exit\n";

    int ch = readInt("Enter choice: ");
    if (ch == 1) {
      inputData();
    } else if (ch == 2) {
      buildOBST();
    } else if (ch == 3) {
      displayTables();
    } else if (ch == 4) {
      displayTree();
    } else if (ch == 5) {
      resetAll();
      cout << "Reset done.\n";
    } else if (ch == 6) {
      break;
    } else {
      cout << "Invalid choice.\n";
    }
  }
  return 0;
}
