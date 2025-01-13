// 21. Suffix Tree (Ukkonen)
// Demo: Build suffix tree in O(n) (Ukkonen's algorithm) for a single input string.
// Features:
// - Display edges (compressed substrings)
// - Search pattern
// - Longest repeated substring (deepest internal node with >=2 leaves)
// Notes:
// - For simplicity, this demo expects ASCII input without spaces.
// - A sentinel '$' is appended (must not appear in input).

#include <iostream>
#include <string>
using namespace std;

static void clearInput() {
  cin.clear();
  cin.ignore(1024, '\n');
}

static string readToken(const string &prompt) {
  cout << prompt;
  string s;
  cin >> s;
  return s;
}

struct SuffixTree {
  static const int ALPH = 128; // ASCII

  struct Node {
    Node *children[ALPH];
    Node *suffixLink;
    int start;
    int *end;        // inclusive end index
    int suffixIndex; // >=0 for leaves after finalization
    bool ownsEnd;

    Node(int s, int *e, bool owns) : suffixLink(NULL), start(s), end(e), suffixIndex(-1), ownsEnd(owns) {
      for (int i = 0; i < ALPH; i++) children[i] = NULL;
    }
  };

  string text;
  Node *root;

  Node *lastNewNode;
  Node *activeNode;
  int activeEdge;
  int activeLength;
  int remainingSuffixCount;

  int leafEnd;
  int *rootEnd;

  int size;

  SuffixTree() : root(NULL), lastNewNode(NULL), activeNode(NULL), activeEdge(-1), activeLength(0), remainingSuffixCount(0), leafEnd(-1), rootEnd(NULL), size(0) {}

  ~SuffixTree() { clear(); }

  void clear() {
    freeNode(root);
    root = NULL;
    text.clear();
    lastNewNode = NULL;
    activeNode = NULL;
    activeEdge = -1;
    activeLength = 0;
    remainingSuffixCount = 0;
    leafEnd = -1;
    rootEnd = NULL;
    size = 0;
  }

  int edgeLength(Node *n) const {
    if (!n) return 0;
    return *(n->end) - n->start + 1;
  }

  Node *newNode(int start, int *end, bool ownsEnd) {
    Node *n = new Node(start, end, ownsEnd);
    n->suffixLink = root;
    return n;
  }

  bool walkDown(Node *next) {
    if (!next) return false;
    int len = edgeLength(next);
    if (activeLength >= len) {
      activeEdge += len;
      activeLength -= len;
      activeNode = next;
      return true;
    }
    return false;
  }

  bool buildFrom(const string &s) {
    clear();
    if (s.size() == 0) return false;
    for (size_t i = 0; i < s.size(); i++) {
      unsigned char c = (unsigned char)s[i];
      if (c >= ALPH) {
        cout << "Only ASCII characters supported.\n";
        return false;
      }
      if (s[i] == '$') {
        cout << "Input must not contain '$' (sentinel).\n";
        return false;
      }
    }

    text = s + "$";
    size = (int)text.size();
    leafEnd = -1;
    rootEnd = new int(-1);
    root = new Node(-1, rootEnd, true);
    root->suffixLink = root;
    activeNode = root;
    activeEdge = -1;
    activeLength = 0;
    remainingSuffixCount = 0;
    lastNewNode = NULL;

    for (int i = 0; i < size; i++) {
      cout << "Phase " << i + 1 << ": add '" << text[i] << "'\n";
      extend(i);
    }
    setSuffixIndexByDFS(root, 0);
    return true;
  }

  void extend(int pos) {
    leafEnd = pos;
    remainingSuffixCount++;
    lastNewNode = NULL;

    while (remainingSuffixCount > 0) {
      if (activeLength == 0) activeEdge = pos;
      int edgeChar = (unsigned char)text[activeEdge];

      if (!activeNode->children[edgeChar]) {
        // Rule 2: new leaf
        cout << "  new leaf for suffix starting with '" << text[activeEdge] << "'\n";
        activeNode->children[edgeChar] = newNode(pos, &leafEnd, false);
        if (lastNewNode) {
          lastNewNode->suffixLink = activeNode;
          lastNewNode = NULL;
        }
      } else {
        Node *next = activeNode->children[edgeChar];
        if (walkDown(next)) continue;

        // next character on the edge
        if (text[next->start + activeLength] == text[pos]) {
          // Rule 3: no new node
          cout << "  edge already has '" << text[pos] << "', activeLength -> "
               << activeLength + 1 << "\n";
          if (lastNewNode && activeNode != root) {
            lastNewNode->suffixLink = activeNode;
            lastNewNode = NULL;
          }
          activeLength++;
          break;
        }

        // Rule 2: split
        cout << "  split edge at length " << activeLength << ", add '" << text[pos] << "'\n";
        int *splitEnd = new int(next->start + activeLength - 1);
        Node *split = newNode(next->start, splitEnd, true);
        activeNode->children[edgeChar] = split;

        split->children[(unsigned char)text[pos]] = newNode(pos, &leafEnd, false);
        next->start += activeLength;
        split->children[(unsigned char)text[next->start]] = next;

        if (lastNewNode) lastNewNode->suffixLink = split;
        lastNewNode = split;
      }

      remainingSuffixCount--;
      if (activeNode == root && activeLength > 0) {
        activeLength--;
        activeEdge = pos - remainingSuffixCount + 1;
      } else if (activeNode != root) {
        activeNode = activeNode->suffixLink;
      }
    }
  }

  void setSuffixIndexByDFS(Node *n, int labelHeight) {
    if (!n) return;
    bool leaf = true;
    for (int i = 0; i < ALPH; i++) {
      if (n->children[i]) {
        leaf = false;
        setSuffixIndexByDFS(n->children[i], labelHeight + edgeLength(n->children[i]));
      }
    }
    if (leaf) {
      n->suffixIndex = size - labelHeight;
    }
  }

  void freeNode(Node *n) {
    if (!n) return;
    for (int i = 0; i < ALPH; i++) freeNode(n->children[i]);
    if (n->ownsEnd && n->end) delete n->end;
    delete n;
  }

  void printSubstring(int start, int end) const {
    for (int i = start; i <= end && i < size; i++) cout << text[i];
  }

  void displayEdges() const {
    if (!root) {
      cout << "(build tree first)\n";
      return;
    }
    cout << "\nEdges (u -> label -> v). Leaf shows suffixIndex.\n";
    displayRec(root, 0);
  }

  void displayRec(Node *n, int depth) const {
    if (!n) return;
    for (int c = 0; c < ALPH; c++) {
      Node *child = n->children[c];
      if (!child) continue;
      for (int i = 0; i < depth; i++) cout << "  ";
      cout << "- ";
      printSubstring(child->start, *(child->end));
      if (child->suffixIndex >= 0) cout << "  (leaf, suffixIndex=" << child->suffixIndex << ")";
      cout << "\n";
      displayRec(child, depth + 1);
    }
  }

  bool searchPattern(const string &pat) const {
    if (!root) {
      cout << "Build the tree first.\n";
      return false;
    }
    if (pat.size() == 0) {
      cout << "Empty pattern matches at root.\n";
      return true;
    }
    for (size_t i = 0; i < pat.size(); i++) {
      unsigned char c = (unsigned char)pat[i];
      if (c >= ALPH || pat[i] == '$') {
        cout << "Pattern has unsupported character at index " << i << "\n";
        return false;
      }
    }

    Node *cur = root;
    size_t i = 0;
    while (i < pat.size()) {
      unsigned char c = (unsigned char)pat[i];
      Node *next = cur->children[c];
      cout << "At node, look for edge starting with '" << pat[i] << "': ";
      if (!next) {
        cout << "missing\n";
        return false;
      }
      cout << "found edge \"";
      printSubstring(next->start, *(next->end));
      cout << "\"\n";
      int j = next->start;
      int e = *(next->end);
      while (j <= e && i < pat.size()) {
        cout << "  compare pattern[" << i << "]='" << pat[i]
             << "' with text[" << j << "]='" << text[j] << "': ";
        if (text[j] != pat[i]) {
          cout << "mismatch\n";
          return false;
        }
        cout << "match\n";
        j++;
        i++;
      }
      cur = next;
    }
    return true;
  }

  struct LRInfo {
    int leaves;
    int anySuffix;
    LRInfo(int l = 0, int s = -1) : leaves(l), anySuffix(s) {}
  };

  LRInfo lrsDfs(Node *n, int labelHeight, int &bestHeight, int &bestStart) const {
    if (!n) return LRInfo(0, -1);
    if (n->suffixIndex >= 0) {
      return LRInfo(1, n->suffixIndex);
    }
    int leafCount = 0;
    int any = -1;
    for (int c = 0; c < ALPH; c++) {
      Node *child = n->children[c];
      if (!child) continue;
      LRInfo info = lrsDfs(child, labelHeight + edgeLength(child), bestHeight, bestStart);
      leafCount += info.leaves;
      if (any < 0 && info.anySuffix >= 0) any = info.anySuffix;
    }
    if (n != root && leafCount >= 2) {
      if (labelHeight > bestHeight && any >= 0) {
        bestHeight = labelHeight;
        bestStart = any;
      }
    }
    return LRInfo(leafCount, any);
  }

  string longestRepeatedSubstring() const {
    if (!root) return "";
    int bestHeight = 0;
    int bestStart = -1;
    lrsDfs(root, 0, bestHeight, bestStart);
    if (bestHeight <= 0 || bestStart < 0) return "";
    // Ensure we don't include sentinel
    if (bestStart + bestHeight > size) bestHeight = size - bestStart;
    if (bestHeight > 0 && text[bestStart + bestHeight - 1] == '$') bestHeight--;
    if (bestHeight <= 0) return "";
    return text.substr((size_t)bestStart, (size_t)bestHeight);
  }
};

int main() {
  SuffixTree st;
  int choice = 0;
  while (choice != 8) {
    cout << "\n=== T24: Suffix Tree (Ukkonen) ===\n";
    cout << "1. Build from string\n";
    cout << "2. Display edges\n";
    cout << "3. Search pattern\n";
    cout << "4. Longest repeated substring\n";
    cout << "5. Clear\n";
    cout << "6. Show current text\n";
    cout << "7. Extend current text\n";
    cout << "8. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      string s = readToken("String (ASCII, no spaces, no '$'): ");
      bool ok = st.buildFrom(s);
      cout << (ok ? "Built.\n" : "Build failed.\n");
    } else if (choice == 2) {
      st.displayEdges();
    } else if (choice == 3) {
      string p = readToken("Pattern: ");
      cout << (st.searchPattern(p) ? "Found\n" : "Not found\n");
    } else if (choice == 4) {
      if (st.text.size() == 0) {
        cout << "Build the tree first.\n";
        continue;
      }
      string lrs = st.longestRepeatedSubstring();
      if (lrs.size() == 0) cout << "No repeated substring found.\n";
      else cout << "Longest repeated substring: " << lrs << " (len=" << lrs.size() << ")\n";
    } else if (choice == 5) {
      st.clear();
      cout << "Cleared.\n";
    } else if (choice == 6) {
      if (st.text.size() == 0) cout << "(none)\n";
      else cout << "Text used: " << st.text << "\n";
    } else if (choice == 7) {
      string extra = readToken("Append string (ASCII, no spaces, no '$'): ");
      string base = "";
      if (st.text.size() > 0) {
        base = st.text;
        if (base[base.size() - 1] == '$') base.erase(base.size() - 1);
      } else {
        cout << "No current text; build from the append string.\n";
      }
      cout << "Remove old '$', append new text, then add one '$' during rebuild: \""
           << base << "\" + \"" << extra << "\"\n";
      bool ok = st.buildFrom(base + extra);
      cout << (ok ? "Extended/rebuilt.\n" : "Extend failed.\n");
    } else if (choice == 8) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  return 0;
}
