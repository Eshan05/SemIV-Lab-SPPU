// 7. Huffman tree construction (classic)
// Demo: build Huffman codes from symbol-frequency pairs using a custom min-heap.
// Provides: build steps, display codes, encode message, decode bitstring.

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

struct MinHeap {
  HNode **a;
  int n;
  int cap;

  MinHeap() : a(NULL), n(0), cap(0) {}
  ~MinHeap() { delete[] a; }

  void ensure(int need) {
    if (need <= cap) return;
    int nc = (cap == 0) ? 16 : cap;
    while (nc < need) nc *= 2;
    HNode **b = new HNode *[nc];
    for (int i = 0; i < n; i++) b[i] = a[i];
    delete[] a;
    a = b;
    cap = nc;
  }

  bool empty() const { return n == 0; }

  static bool lessNode(HNode *x, HNode *y) {
    if (x->freq != y->freq) return x->freq < y->freq;
    // tie-break (stable-ish): leaf before internal, then by char
    bool xl = (x->ch != '\0');
    bool yl = (y->ch != '\0');
    if (xl != yl) return xl;
    return x->ch < y->ch;
  }

  void heapUp(int i) {
    while (i > 0) {
      int p = (i - 1) / 2;
      if (!lessNode(a[i], a[p])) break;
      HNode *tmp = a[i];
      a[i] = a[p];
      a[p] = tmp;
      i = p;
    }
  }

  void heapDown(int i) {
    while (true) {
      int l = 2 * i + 1;
      int r = 2 * i + 2;
      int m = i;
      if (l < n && lessNode(a[l], a[m])) m = l;
      if (r < n && lessNode(a[r], a[m])) m = r;
      if (m == i) break;
      HNode *tmp = a[i];
      a[i] = a[m];
      a[m] = tmp;
      i = m;
    }
  }

  void push(HNode *x) {
    ensure(n + 1);
    a[n] = x;
    heapUp(n);
    n++;
  }

  HNode *popMin() {
    if (n == 0) return NULL;
    HNode *out = a[0];
    n--;
    if (n > 0) {
      a[0] = a[n];
      heapDown(0);
    }
    return out;
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

static void buildCodes(HNode *root, string prefix, string codeMap[256]) {
  if (!root) return;
  if (!root->left && !root->right) {
    // Single-symbol corner case: ensure non-empty code.
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
      char c = (char)i;
      if (c == ' ') cout << "[space]";
      else cout << c;
      cout << " -> " << codeMap[i] << "\n";
    }
  }
}

static void displayTree(HNode *root, int depth, const string &edge) {
  if (!root) return;
  for (int i = 0; i < depth; i++) cout << "  ";
  if (!root->left && !root->right) {
    cout << edge << "'" << root->ch << "'" << " (" << root->freq << ")\n";
  } else {
    cout << edge << "*" << " (" << root->freq << ")\n";
  }
  displayTree(root->left, depth + 1, "0-> ");
  displayTree(root->right, depth + 1, "1-> ");
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
  while (choice != 8) {
    cout << "\n=== T09: Huffman Coding ===\n";
    cout << "1. Add symbol + frequency\n";
    cout << "2. Load small demo set\n";
    cout << "3. Show current inputs\n";
    cout << "4. Build Huffman tree\n";
    cout << "5. Show codes\n";
    cout << "6. Encode message\n";
    cout << "7. Decode bitstring\n";
    cout << "8. Exit\n";
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
      // Classic sample
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
      MinHeap heap;
      for (int i = 0; i < n; i++) heap.push(new HNode(sym[i], fr[i]));

      cout << "\nBuild steps (pop two minimums each time):\n";
      while (heap.n > 1) {
        HNode *x = heap.popMin();
        HNode *y = heap.popMin();
        cout << "Combine (";
        if (x->ch) cout << x->ch;
        else cout << '*';
        cout << "," << x->freq << ") + (";
        if (y->ch) cout << y->ch;
        else cout << '*';
        cout << "," << y->freq << ") => " << (x->freq + y->freq) << "\n";
        heap.push(new HNode(x->freq + y->freq, x, y));
      }
      root = heap.popMin();
      buildCodes(root, "", codeMap);
      cout << "\nHuffman tree built.\n";
      cout << "Tree (0=left, 1=right):\n";
      displayTree(root, 0, "ROOT-> ");
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
          cout << "No code for character '" << msg[i] << "'.\n";
          ok = false;
          break;
        }
        cout << "'" << msg[i] << "' -> " << codeMap[uc] << "\n";
        bits += codeMap[uc];
      }
      if (ok) {
        cout << "Encoded bits: " << bits << "\n";
      }
    } else if (choice == 7) {
      if (!root) {
        cout << "Build tree first.\n";
        continue;
      }
      cout << "Bitstring (0/1): ";
      string bits;
      cin >> bits;
      string out;
      HNode *cur = root;
      bool ok = true;
      for (size_t i = 0; i < bits.size(); i++) {
        char b = bits[i];
        if (b != '0' && b != '1') {
          ok = false;
          break;
        }
        cout << "bit " << b << ": ";
        if (b == '0') {
          cout << "go left";
          cur = cur->left;
        } else {
          cout << "go right";
          cur = cur->right;
        }
        if (!cur) {
          cout << " -> null\n";
          ok = false;
          break;
        }
        if (!cur->left && !cur->right) {
          cout << " -> leaf '" << cur->ch << "'\n";
          out.push_back(cur->ch);
          cur = root;
        } else {
          cout << "\n";
        }
      }
      if (!ok || cur != root) cout << "Invalid / incomplete bitstring for this tree.\n";
      else {
        cout << "Decoded message: " << out << "\n";
      }
    } else if (choice == 8) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }

  freeTree(root);
  return 0;
}
