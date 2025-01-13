// 20. Trie
// Demo: lowercase a-z trie with insertion, deletion, searching, display, prefix search, autocomplete.

#include <iostream>
#include <string>
using namespace std;

struct Node {
  bool end;
  Node *next[26];
  Node() : end(false) {
    for (int i = 0; i < 26; i++) next[i] = NULL;
  }
};

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

static bool isLowerWord(const string &s) {
  if (s.size() == 0) return false;
  for (size_t i = 0; i < s.size(); i++) {
    if (s[i] < 'a' || s[i] > 'z') return false;
  }
  return true;
}

static bool insert(Node *root, const string &word) {
  Node *cur = root;
  for (size_t i = 0; i < word.size(); i++) {
    int idx = word[i] - 'a';
    if (!cur->next[idx]) {
      cout << "Create link for '" << word[i] << "' at depth " << i + 1 << "\n";
      cur->next[idx] = new Node();
    } else {
      cout << "Follow existing link for '" << word[i] << "' at depth " << i + 1 << "\n";
    }
    cur = cur->next[idx];
  }
  if (cur->end) {
    cout << "Final node already has end flag set; word already exists.\n";
    return false;
  }
  cur->end = true;
  cout << "Mark end of word at final node.\n";
  return true;
}

static bool search(Node *root, const string &word) {
  Node *cur = root;
  for (size_t i = 0; i < word.size(); i++) {
    int idx = word[i] - 'a';
    cout << "Check '" << word[i] << "' at depth " << i + 1 << "\n";
    if (!cur->next[idx]) {
      cout << "Missing link for '" << word[i] << "'.\n";
      return false;
    }
    cur = cur->next[idx];
  }
  cout << "Reached final node; end flag is " << (cur->end ? "set" : "not set") << ".\n";
  return cur->end;
}

static bool hasChildren(Node *node) {
  for (int i = 0; i < 26; i++)
    if (node->next[i]) return true;
  return false;
}

static bool eraseRec(Node *node, const string &word, int depth) {
  if (!node) return false;
  if (depth == (int)word.size()) {
    if (!node->end) return false;
    node->end = false;
    return !hasChildren(node);
  }
  int idx = word[depth] - 'a';
  if (!node->next[idx]) return false;
  bool shouldDeleteChild = eraseRec(node->next[idx], word, depth + 1);
  if (shouldDeleteChild) {
    cout << "Prune unused link for '" << word[depth] << "' at depth " << depth + 1 << "\n";
    delete node->next[idx];
    node->next[idx] = NULL;
  }
  return (!node->end && !hasChildren(node));
}

static bool eraseWord(Node *root, const string &word) {
  if (!root) return false;
  // We must not delete root itself; ignore returned value.
  bool existedBefore = search(root, word);
  eraseRec(root, word, 0);
  return existedBefore;
}

static void dfsWords(Node *node, string &cur, int &shown, int limit) {
  if (!node) return;
  if (node->end) {
    cout << cur << "\n";
    shown++;
    if (limit > 0 && shown >= limit) return;
  }
  for (int i = 0; i < 26; i++) {
    if (!node->next[i]) continue;
    cur.push_back(char('a' + i));
    dfsWords(node->next[i], cur, shown, limit);
    if (limit > 0 && shown >= limit) {
      cur.pop_back();
      return;
    }
    cur.pop_back();
  }
}

static void displayAll(Node *root) {
  cout << "\nWords in trie:\n";
  string cur;
  int shown = 0;
  dfsWords(root, cur, shown, 0);
  if (shown == 0) cout << "(none)\n";
}

static Node *walkPrefix(Node *root, const string &pref) {
  Node *cur = root;
  for (size_t i = 0; i < pref.size(); i++) {
    int idx = pref[i] - 'a';
    if (!cur->next[idx]) return NULL;
    cur = cur->next[idx];
  }
  return cur;
}

static void autocomplete(Node *root, const string &pref, int limit) {
  Node *node = walkPrefix(root, pref);
  if (!node) {
    cout << "Prefix not found.\n";
    return;
  }
  cout << "\nCompletions for '" << pref << "':\n";
  string cur = pref;
  int shown = 0;
  dfsWords(node, cur, shown, limit);
  if (shown == 0) cout << "(none)\n";
}

static void freeTrie(Node *node) {
  if (!node) return;
  for (int i = 0; i < 26; i++) freeTrie(node->next[i]);
  delete node;
}

int main() {
  Node *root = new Node();
  int choice = 0;
  while (choice != 8) {
    cout << "\n=== T23: Trie (a-z) ===\n";
    cout << "1. Insert word\n";
    cout << "2. Search word\n";
    cout << "3. Delete word\n";
    cout << "4. Display all words\n";
    cout << "5. Prefix exists?\n";
    cout << "6. Autocomplete prefix\n";
    cout << "7. Clear\n";
    cout << "8. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      string w = readToken("Word (lowercase a-z): ");
      if (!isLowerWord(w)) {
        cout << "Only lowercase a-z allowed.\n";
        continue;
      }
      cout << (insert(root, w) ? "Inserted.\n" : "Duplicate ignored.\n");
    } else if (choice == 2) {
      string w = readToken("Word: ");
      if (!isLowerWord(w)) {
        cout << "Only lowercase a-z allowed.\n";
        continue;
      }
      cout << (search(root, w) ? "Found\n" : "Not found\n");
    } else if (choice == 3) {
      string w = readToken("Word: ");
      if (!isLowerWord(w)) {
        cout << "Only lowercase a-z allowed.\n";
        continue;
      }
      cout << (eraseWord(root, w) ? "Deleted\n" : "Not found\n");
    } else if (choice == 4) {
      displayAll(root);
    } else if (choice == 5) {
      string p = readToken("Prefix: ");
      if (!isLowerWord(p)) {
        cout << "Only lowercase a-z allowed.\n";
        continue;
      }
      cout << (walkPrefix(root, p) ? "Prefix exists\n" : "Prefix not found\n");
    } else if (choice == 6) {
      string p = readToken("Prefix: ");
      if (!isLowerWord(p)) {
        cout << "Only lowercase a-z allowed.\n";
        continue;
      }
      int limit = 0;
      cout << "Max suggestions (0=all): ";
      cin >> limit;
      if (!cin) {
        clearInput();
        limit = 0;
      }
      if (limit < 0) {
        cout << "Negative limit acts like 0 (show all).\n";
        limit = 0;
      }
      autocomplete(root, p, limit);
    } else if (choice == 7) {
      freeTrie(root);
      root = new Node();
      cout << "Cleared.\n";
    } else if (choice == 8) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  freeTrie(root);
  return 0;
}
