// 23. Huffman tree (again) - Two-Queue construction
// Alternative to min-heap: sort leaves once, then repeatedly pick two minimums from:
// - a sorted list of leaves
// - a queue of internal nodes created so far
// This demonstrates the classic O(n) merge-like Huffman build.

#include <iostream>
#include <string>
using namespace std;

struct HNode {
  char ch;
  int freq;
  HNode *left;
  HNode *right;
  HNode(char c, int f) : ch(c), freq(f), left(NULL), right(NULL) {}
  HNode(int f, HNode *l, HNode *r) : ch('\0'), freq(f), left(l), right(r) {}
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
    cout << "Invalid number.\n";
    clearInput();
  }
}

static string readToken(const string &prompt) {
  cout << prompt;
  string s;
  cin >> s;
  return s;
}

static void freeTree(HNode *root) {
  if (!root) return;
  freeTree(root->left);
  freeTree(root->right);
  delete root;
}

static void insertionSortLeaves(HNode **leaves, int n) {
  for (int i = 1; i < n; i++) {
    HNode *key = leaves[i];
    int j = i - 1;
    while (j >= 0 && leaves[j]->freq > key->freq) {
      leaves[j + 1] = leaves[j];
      j--;
    }
    leaves[j + 1] = key;
  }
}

static HNode *popMin(HNode **leaves, int nLeaves, int &iLeaf, HNode **q, int &qh, int &qt) {
  // leaves: sorted array, iLeaf is current index
  // q: internal nodes queue with non-decreasing freq
  bool haveLeaf = (iLeaf < nLeaves);
  bool haveQ = (qh < qt);
  if (!haveLeaf && !haveQ) return NULL;
  if (!haveLeaf) return q[qh++];
  if (!haveQ) return leaves[iLeaf++];
  if (leaves[iLeaf]->freq <= q[qh]->freq) return leaves[iLeaf++];
  return q[qh++];
}

static void buildCodes(HNode *root, string prefix, string codeMap[256]) {
  if (!root) return;
  if (!root->left && !root->right) {
    if (prefix.size() == 0) prefix = "0";
    codeMap[(unsigned char)root->ch] = prefix;
    return;
  }
  buildCodes(root->left, prefix + "0", codeMap);
  buildCodes(root->right, prefix + "1", codeMap);
}

static void displayCodes(const string codeMap[256]) {
  cout << "\nSymbol -> Code\n";
  for (int i = 0; i < 256; i++) {
    if (codeMap[i].size() > 0) {
      cout << (char)i << " -> " << codeMap[i] << "\n";
    }
  }
}

static bool symbolExists(char sym[], int n, char c) {
  for (int i = 0; i < n; i++)
    if (sym[i] == c) return true;
  return false;
}

int main() {
  char sym[256];
  int fr[256];
  int n = 0;

  HNode *root = NULL;
  string codeMap[256];

  int choice = 0;
  while (choice != 7) {
    cout << "\n=== T26: Huffman (Two-Queue Build) ===\n";
    cout << "1. Add symbol + frequency\n";
    cout << "2. Load small demo set\n";
    cout << "3. Show current inputs\n";
    cout << "4. Build Huffman tree (two-queue steps)\n";
    cout << "5. Show codes\n";
    cout << "6. Encode message\n";
    cout << "7. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      if (n >= 256) {
        cout << "Max symbols reached.\n";
        continue;
      }
      string s = readToken("Symbol (single char): ");
      if (s.size() != 1) {
        cout << "Enter exactly one character.\n";
        continue;
      }
      char c = s[0];
      if (symbolExists(sym, n, c)) {
        cout << "Symbol already exists; clear/reload before using a different frequency.\n";
        continue;
      }
      int f = readInt("Frequency (>0): ");
      if (f <= 0) {
        cout << "Frequency must be > 0.\n";
        continue;
      }
      sym[n] = c;
      fr[n] = f;
      n++;
      cout << "Added '" << c << "' freq=" << f << "\n";
    } else if (choice == 2) {
      freeTree(root);
      root = NULL;
      for (int i = 0; i < 256; i++) codeMap[i].clear();
      n = 0;
      sym[n] = 'a';
      fr[n++] = 5;
      sym[n] = 'b';
      fr[n++] = 9;
      sym[n] = 'c';
      fr[n++] = 12;
      sym[n] = 'd';
      fr[n++] = 13;
      sym[n] = 'e';
      fr[n++] = 16;
      sym[n] = 'f';
      fr[n++] = 45;
      cout << "Loaded demo: a5 b9 c12 d13 e16 f45\n";
    } else if (choice == 3) {
      cout << "\nInputs (" << n << "):\n";
      for (int i = 0; i < n; i++) cout << i + 1 << ") '" << sym[i] << "' -> " << fr[i] << "\n";
    } else if (choice == 4) {
      freeTree(root);
      root = NULL;
      for (int i = 0; i < 256; i++) codeMap[i].clear();
      if (n == 0) {
        cout << "Add symbols first.\n";
        continue;
      }

      HNode **leaves = new HNode *[n];
      for (int i = 0; i < n; i++) leaves[i] = new HNode(sym[i], fr[i]);
      insertionSortLeaves(leaves, n);

      HNode **q = new HNode *[2 * n];
      int qh = 0, qt = 0;
      int iLeaf = 0;

      cout << "\nBuild steps (two-queue min picks):\n";
      while (true) {
        HNode *x = popMin(leaves, n, iLeaf, q, qh, qt);
        HNode *y = popMin(leaves, n, iLeaf, q, qh, qt);
        if (!x) break;
        if (!y) {
          root = x;
          break;
        }
        cout << "Pick (" << (x->ch ? x->ch : '*') << "," << x->freq << ") and (";
        cout << (y->ch ? y->ch : '*') << "," << y->freq << ") => " << (x->freq + y->freq) << "\n";
        q[qt++] = new HNode(x->freq + y->freq, x, y);
      }

      delete[] q;
      delete[] leaves;

      buildCodes(root, "", codeMap);
      cout << "Built Huffman tree. Root frequency=" << (root ? root->freq : 0) << "\n";
    } else if (choice == 5) {
      if (!root) {
        cout << "Build tree first.\n";
        continue;
      }
      displayCodes(codeMap);
    } else if (choice == 6) {
      if (!root) {
        cout << "Build tree first.\n";
        continue;
      }
      cout << "Message (no spaces): ";
      string msg;
      cin >> msg;
      string bits;
      bool ok = true;
      for (size_t i = 0; i < msg.size(); i++) {
        unsigned char uc = (unsigned char)msg[i];
        if (codeMap[uc].size() == 0) {
          cout << "No code for '" << msg[i] << "'.\n";
          ok = false;
          break;
        }
        cout << "'" << msg[i] << "' -> " << codeMap[uc] << "\n";
        bits += codeMap[uc];
      }
      if (ok) cout << "Encoded bits: " << bits << "\n";
    } else if (choice == 7) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }

  freeTree(root);
  return 0;
}
