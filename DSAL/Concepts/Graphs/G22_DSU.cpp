// 17a. Disjoint sets: Union by size keeps the trees shallow and path
// compression flattens what depth is left, so a find walks a few links
// instead of a whole chain.
// Demo: Two wide groups merge, a deeper group shows what compression is for,
// and a chain demo runs with the size rule off to show the depth it prevents.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXN = 15;

static int n = 0;
static int parentOf[MAXN];
static int sizeOf[MAXN];
static int groupCount = 0;
static int unionRule = 1;

static int readInt(const string &prompt) {
  int x;
  while (true) {
    cout << prompt;
    if (cin >> x) {
      cin.ignore(1024, '\n');
      return x;
    }
    if (cin.eof()) {
      cout << "\nInput closed.\n";
      exit(0);
    }
    cout << "Invalid number. Try again.\n";
    cin.clear();
    cin.ignore(1024, '\n');
  }
}

static int readRange(const string &prompt, int lo, int hi) {
  while (true) {
    int x = readInt(prompt);
    if (x >= lo && x <= hi) return x;
    cout << "  Number must be in " << lo << ".." << hi << ".\n";
  }
}

static void resetSets(int count) {
  n = count;
  for (int i = 0; i < MAXN; i++) {
    parentOf[i] = i;
    sizeOf[i] = (i < n) ? 1 : 0;
  }
  groupCount = n;
}

static int rootOf(int x) {
  while (parentOf[x] != x) x = parentOf[x];
  return x;
}

static int depthOf(int x) {
  int links = 0;
  while (parentOf[x] != x) {
    x = parentOf[x];
    links++;
  }
  return links;
}

static string pathText(int x) {
  string s = to_string(x);
  while (parentOf[x] != x) {
    x = parentOf[x];
    s += " -> " + to_string(x);
  }
  return s;
}

static int betterRoot(int ra, int rb) {
  if (sizeOf[ra] > sizeOf[rb]) return ra;
  if (sizeOf[rb] > sizeOf[ra]) return rb;
  return (ra < rb) ? ra : rb;
}

static string ruleText() {
  return unionRule ? "By size" : "Off, the second root wins";
}

static void unionQuiet(int a, int b) {
  int ra = rootOf(a);
  int rb = rootOf(b);
  if (ra == rb) return;

  if (!unionRule) {
    parentOf[ra] = rb;
    sizeOf[rb] += sizeOf[ra];
    groupCount--;
    return;
  }

  int top = betterRoot(ra, rb);
  int under = (top == ra) ? rb : ra;
  parentOf[under] = top;
  sizeOf[top] += sizeOf[under];
  groupCount--;
}

static int deepestElement() {
  int who = 0;
  int deepest = 0;
  for (int i = 0; i < n; i++) {
    int d = depthOf(i);
    if (d > deepest) {
      deepest = d;
      who = i;
    }
  }
  return who;
}

static int deepestDepth() { return depthOf(deepestElement()); }

static void buildSets() {
  int count = readRange("Number of elements (0.." + to_string(MAXN) + "): ", 0,
                        MAXN);
  resetSets(count);
  unionRule = 1;

  if (n == 0) {
    cout << "Built 0 elements. Nothing to union yet.\n";
    return;
  }
  cout << "Built " << n << (n == 1 ? " singleton" : " singletons");
  cout << ": Each element starts as its own root.\n";
}

static void loadMergingDemo() {
  resetSets(8);
  unionRule = 1;
  cout << "Merging demo: 8 elements.\n";
  cout << "  Four pairs form first, then each pair of pairs joins.\n";
  cout << "  Union by size keeps both new trees at depth 2.\n\n";

  unionQuiet(0, 1);
  unionQuiet(2, 3);
  unionQuiet(4, 5);
  unionQuiet(6, 7);
  unionQuiet(0, 2);
  unionQuiet(4, 6);

  cout << "Demo loaded: " << groupCount
       << " groups, each one 4 elements wide.\n";
  cout << "  Deepest path is " << deepestDepth() << " links.\n";
}

static void loadDeepDemo() {
  resetSets(10);
  unionRule = 1;
  cout << "Deep demo: 10 elements.\n";
  cout << "  Groups of 4 and 5 form first, then the 4 group hangs under the\n";
  cout << "  5 group. Element 3 ends 3 links below the root of 4.\n\n";

  unionQuiet(0, 1);
  unionQuiet(2, 3);
  unionQuiet(0, 2);
  unionQuiet(4, 5);
  unionQuiet(6, 7);
  unionQuiet(4, 6);
  unionQuiet(4, 8);
  unionQuiet(0, 4);

  cout << "Demo loaded: " << groupCount << " groups, one of them 9 elements.\n";
  cout << "  Deepest path is " << deepestDepth() << " links, so a find has\n";
  cout << "  something to flatten.\n";
}

static void loadChainDemo() {
  resetSets(8);
  unionRule = 0;
  cout << "Chain demo: 8 elements, size rule switched off.\n";
  cout << "  Unions come as 0-1, 1-2, 2-3 and so on up to 6-7.\n";
  cout << "  Each one hangs the first root under the second, so the set becomes\n";
  cout << "  a chain instead of a wide tree.\n\n";

  for (int i = 0; i + 1 < 8; i++) unionQuiet(i, i + 1);

  cout << "Demo loaded: " << groupCount << " group, deepest path "
       << deepestDepth() << " links.\n";
  cout << "  A find from element 0 now walks the whole chain.\n";
  cout << "  Run it from the operations menu with compression off to see that.\n";
}

static void traceFind(int x, bool compress) {
  int r = rootOf(x);
  int links = depthOf(x);

  cout << "\n--- Find " << x << " ---\n";
  cout << "Path: " << pathText(x) << "\n";
  cout << "Root is " << r << ", so the walk took " << links
       << (links == 1 ? " link.\n" : " links.\n");

  if (links == 0) {
    cout << x << " is a root itself, so the walk stops where it starts.\n";
    return;
  }
  if (links == 1) {
    cout << "One link, and it points straight at the root.\n";
    return;
  }

  if (!compress) {
    cout << "Compression is off, so every parent stays as it was.\n";
    cout << "The next find from " << x << " walks the same " << links
         << " links.\n";
    return;
  }

  cout << "Compressing:\n";
  int cur = x;
  while (parentOf[cur] != r) {
    int next = parentOf[cur];
    cout << "  parent[" << cur << "] = " << r << "\n";
    parentOf[cur] = r;
    cur = next;
  }
  cout << "Now: " << x << " -> " << r << ", one link.\n";
  cout << "The element and everything on its old path skip the middle.\n";
}

static void unionElements(int a, int b) {
  int ra = rootOf(a);
  int rb = rootOf(b);

  cout << "  Root of " << a << " is " << ra << ", root of " << b << " is "
       << rb << ".\n";
  if (ra == rb) {
    cout << "  They already share a group, so nothing changes.\n";
    cout << "  Groups: " << groupCount << ".\n";
    return;
  }

  if (!unionRule) {
    cout << "  The size rule is off, so root " << ra << " goes under root "
         << rb << ".\n";
    parentOf[ra] = rb;
    sizeOf[rb] += sizeOf[ra];
    groupCount--;
    cout << "  parent[" << ra << "] = " << rb << ", size[" << rb
         << "] = " << sizeOf[rb] << ".\n";
    cout << "  Groups: " << groupCount << ". The chain just got longer.\n";
    return;
  }

  int top = betterRoot(ra, rb);
  int under = (top == ra) ? rb : ra;

  cout << "  Sizes: " << sizeOf[ra] << " and " << sizeOf[rb] << ".\n";
  if (sizeOf[ra] == sizeOf[rb]) {
    cout << "  Sizes are equal, so the lower root " << top
         << " stays on top.\n";
  } else {
    cout << "  The smaller tree goes under the larger, so root " << top
         << " stays on top.\n";
  }
  parentOf[under] = top;
  sizeOf[top] += sizeOf[under];
  groupCount--;

  cout << "  parent[" << under << "] = " << top << ", size[" << top
       << "] = " << sizeOf[top] << ".\n";
  cout << "  Both trees answer to root " << top << " from now on.\n";
  cout << "  Groups: " << groupCount << ".\n";
}

static void unionInteractive() {
  if (n == 0) {
    cout << "Build the sets first.\n";
    return;
  }

  cout << "\nUnion two elements (Elements are 0.." << (n - 1) << ")\n";
  while (true) {
    int a = readInt("  First element (-1 to cancel): ");
    if (a == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int b = readInt("  Second element (-1 to cancel): ");
    if (b == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (a < 0 || a >= n || b < 0 || b >= n) {
      cout << "  Rejected: Elements must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this union.\n";
      continue;
    }
    if (a == b) {
      cout << "  Rejected: Element " << a << " is already its own group.\n";
      cout << "  Please re-enter this union.\n";
      continue;
    }

    cout << "Union " << a << " and " << b << "\n";
    unionElements(a, b);
    return;
  }
}

static void findInteractive(bool compress, bool askCompress) {
  if (n == 0) {
    cout << "Build the sets first.\n";
    return;
  }

  int x;
  while (true) {
    cout << "\nFind the root (Elements are 0.." << (n - 1) << ")\n";
    x = readInt("  Element (-1 to cancel): ");
    if (x == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (x >= 0 && x < n) break;
    cout << "  Rejected: Element must be in 0.." << (n - 1) << ".\n";
  }

  bool useCompression = compress;
  if (askCompress) {
    useCompression = readRange("  Compress the path too? (1 = yes, 0 = no): ", 0,
                               1) == 1;
  }

  traceFind(x, useCompression);
}

static void compressAll() {
  if (n == 0) {
    cout << "Build the sets first.\n";
    return;
  }

  cout << "\n--- Compress Every Path ---\n";
  int changes = 0;
  for (int x = 0; x < n; x++) {
    int r = rootOf(x);
    if (parentOf[x] == r) continue;
    cout << "  parent[" << x << "] = " << r << "\n";
    parentOf[x] = r;
    changes++;
  }

  if (changes == 0) {
    cout << "Every element already points at its root.\n";
    return;
  }
  cout << "Links changed: " << changes << ".\n";
  cout << "Every element now sits one link below its root, so the next\n";
  cout << "find is a single step.\n";
}

static void resetInteractive() {
  if (n == 0) {
    cout << "There is nothing to reset. Build the sets first.\n";
    return;
  }
  int before = groupCount;
  resetSets(n);

  cout << "\nReset to " << n << (n == 1 ? " singleton" : " singletons")
       << ": Each element is its own root again.\n";
  cout << "  Groups: " << before << " -> " << groupCount << ".\n";
}

static void showEvery() {
  cout << "\n--- Every Element ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }

  cout << "Element:";
  for (int i = 0; i < n; i++) cout << setw(3) << i;
  cout << "\nParent :";
  for (int i = 0; i < n; i++) cout << setw(3) << parentOf[i];
  cout << "\nRoot   :";
  for (int i = 0; i < n; i++) cout << setw(3) << rootOf(i);
  cout << "\n";

  int who = deepestElement();
  int links = depthOf(who);
  cout << "Deepest path: " << links << (links == 1 ? " link" : " links")
       << ", ending at element " << who << ".\n";

  cout << "Root sizes:";
  bool first = true;
  for (int i = 0; i < n; i++) {
    if (parentOf[i] != i) continue;
    if (!first) cout << ",";
    cout << " " << i << " has " << sizeOf[i]
         << (sizeOf[i] == 1 ? " element" : " elements");
    first = false;
  }
  cout << ".\n";
  cout << "Groups: " << groupCount << ".\n";
  cout << "Union rule: " << ruleText() << ".\n";
  if (!unionRule) {
    cout << "With the size rule off, depth grows with every union, so a find\n";
    cout << "pays for the whole chain.\n";
  }
}

static void showGroups() {
  cout << "\n--- Groups ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }

  int index = 0;
  for (int r = 0; r < n; r++) {
    if (parentOf[r] != r) continue;
    index++;
    cout << "Group " << index << " (Root " << r << ", " << sizeOf[r]
         << (sizeOf[r] == 1 ? " member):" : " members):");
    for (int x = 0; x < n; x++) {
      if (rootOf(x) == r) cout << " " << x;
    }
    cout << "\n";
  }
  cout << "Count: " << groupCount << ".\n";
  cout << "Two elements answer to the same root exactly when they share a\n";
  cout << "group, and the root is the one element that points at itself.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Disjoint-set operations ---\n";
    cout << "Sets: " << n << (n == 1 ? " element" : " elements")
         << ", " << groupCount << (groupCount == 1 ? " group" : " groups")
         << "\n";
    cout << "Union rule: " << ruleText() << "\n";
    cout << "1. Union two elements\n";
    cout << "2. Find the root, with compression\n";
    cout << "3. Find the root, without compression\n";
    cout << "4. Compress every path\n";
    cout << "5. Reset to singletons\n";
    cout << "0. Back to the main menu\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      unionInteractive();
    } else if (choice == 2) {
      findInteractive(true, false);
    } else if (choice == 3) {
      findInteractive(false, false);
    } else if (choice == 4) {
      compressAll();
    } else if (choice == 5) {
      resetInteractive();
    } else if (choice == 0) {
      return;
    } else {
      cout << "Invalid option.\n";
    }
  }
}

int main() {
  resetSets(0);

  while (true) {
    cout << "\n=== G22: Disjoint Sets (Union-Find) ===\n";
    cout << "Sets: " << n << (n == 1 ? " element" : " elements") << ", "
         << groupCount << (groupCount == 1 ? " group" : " groups")
         << " | Union rule: " << ruleText() << "\n";
    cout << "1. Build n elements\n";
    cout << "2. Load merging demo (8 elements)\n";
    cout << "3. Load deep demo (10 elements)\n";
    cout << "4. Load chain demo (8 elements, size rule off)\n";
    cout << "5. Disjoint-set operations\n";
    cout << "6. Show every element\n";
    cout << "7. Show the groups\n";
    cout << "8. Trace a find\n";
    cout << "9. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildSets();
    } else if (choice == 2) {
      loadMergingDemo();
    } else if (choice == 3) {
      loadDeepDemo();
    } else if (choice == 4) {
      loadChainDemo();
    } else if (choice == 5) {
      operationsMenu();
    } else if (choice == 6) {
      showEvery();
    } else if (choice == 7) {
      showGroups();
    } else if (choice == 8) {
      findInteractive(false, true);
    } else if (choice == 9) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
