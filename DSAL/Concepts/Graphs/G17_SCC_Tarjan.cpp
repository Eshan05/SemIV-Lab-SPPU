// 13b. Strongly connected components: Tarjan's single pass.
// Demo: The same graph as the Kosaraju file, the same components. One pass
// finds them, because a vertex closes a component exactly when nothing in its
// subtree could climb above it.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;

static int n = 0;
static int edgeCount = 0;
static bool hasArc[MAXV][MAXV];
static vector<int> adj[MAXV];

static int disc[MAXV];
static int low[MAXV];
static bool onStack[MAXV];
static vector<int> stk;
static int timerStep = 0;
static int compOf[MAXV];
static int compCount = 0;

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

static void resetGraph() {
  for (int i = 0; i < MAXV; i++) {
    adj[i].clear();
    for (int j = 0; j < MAXV; j++) hasArc[i][j] = false;
  }
  n = 0;
  edgeCount = 0;
}

static void addArc(int u, int v) {
  hasArc[u][v] = true;
  adj[u].push_back(v);
  edgeCount++;
}

static void buildGraph() {
  int newN = readRange("Number of vertices (0.." + to_string(MAXV) + "): ",
                       0, MAXV);

  resetGraph();
  n = newN;

  if (n == 0) {
    cout << "Build complete: 0 vertices, 0 arcs.\n";
    return;
  }
  cout << "Directed arcs. Enter u v with vertices in 0.." << (n - 1) << ".\n";
  int e = 0;
  while (true) {
    e++;
    cout << "\nArc " << e << "\n";
    int u = readInt("  From vertex (-1 to stop): ");
    if (u == -1) break;
    int v = readInt("  To vertex (-1 to stop): ");
    if (v == -1) break;
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      e--;
      continue;
    }
    if (hasArc[u][v]) {
      cout << "  Rejected: That arc already exists.\n";
      e--;
      continue;
    }
    addArc(u, v);
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << edgeCount
       << (edgeCount == 1 ? " arc.\n" : " arcs.\n");
}

static void loadDemo() {
  resetGraph();
  n = 8;

  cout << "SCC demo: 8 vertices, the same graph as the Kosaraju file.\n";
  cout << "  Pair 0 - 1 with arcs both ways, then 1 - 2 and 1 - 3, then a\n";
  cout << "  pair 3 - 4, then 2 - 5 and 4 - 5, then the cycle 5 - 6 - 7 - 5.\n";
  cout << "  V2 joins the first pair to the last cycle without a way back.\n\n";

  addArc(0, 1);
  addArc(1, 0);
  addArc(1, 2);
  addArc(1, 3);
  addArc(2, 5);
  addArc(3, 4);
  addArc(4, 3);
  addArc(4, 5);
  addArc(5, 6);
  addArc(6, 7);
  addArc(7, 5);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " arcs.\n";
}

static void showAdjacency() {
  cout << "\n--- Adjacency List ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  for (int i = 0; i < n; i++) {
    cout << setw(3) << i << ":";
    if (adj[i].empty()) cout << " (None)";
    else
      for (int v : adj[i]) cout << " " << v;
    cout << "\n";
  }
  cout << "Each line lists out-neighbours in insertion order.\n";
  cout << "There is no reversed list here: One pass never needs one.\n";
}

static void resetPass() {
  for (int i = 0; i < MAXV; i++) {
    disc[i] = -1;
    low[i] = -1;
    compOf[i] = 0;
    onStack[i] = false;
  }
  stk.clear();
  timerStep = 0;
  compCount = 0;
}

static void printStack(const string &pad) {
  cout << pad << "  Stack now:";
  if (stk.empty()) {
    cout << " (Empty)\n";
    return;
  }
  for (int i = 0; i < (int)stk.size(); i++) {
    cout << (i == 0 ? " [" : ", ") << "V" << stk[i];
  }
  cout << "]\n";
}

static void closeComponent(int u, int depth, bool verbose) {
  string pad(depth * 2, ' ');
  compCount++;
  if (verbose) {
    cout << pad << "Test V" << u << ": Low and disc are both " << disc[u]
         << ", so a component closes here.\n";
    cout << pad << "  Pop until V" << u << " comes off:";
  }

  int popped = 0;
  while (true) {
    int w = stk.back();
    stk.pop_back();
    onStack[w] = false;
    compOf[w] = compCount;
    popped++;
    if (verbose) cout << " V" << w;
    if (w == u) break;
  }

  if (verbose) {
    cout << "\n";
    cout << pad << "  That is component " << compCount << " of "
         << popped << (popped == 1 ? " vertex" : " vertices") << ".\n";
    printStack(pad);
  }
}

static void strongconnect(int u, int depth, bool verbose) {
  string pad(depth * 2, ' ');
  disc[u] = timerStep;
  low[u] = disc[u];
  timerStep++;
  stk.push_back(u);
  onStack[u] = true;
  if (verbose) {
    cout << pad << "Enter V" << u << " with disc " << disc[u] << " and low "
         << low[u] << ", then push it on the stack.\n";
  }

  for (int v : adj[u]) {
    if (disc[v] == -1) {
      if (verbose) {
        cout << pad << "  Arc " << u << " -> " << v << ": Unvisited, descend.\n";
      }
      strongconnect(v, depth + 1, verbose);

      if (low[v] < low[u]) low[u] = low[v];
      if (verbose) {
        cout << pad << "    Return from V" << v << ", so low[" << u
             << "] = min(" << low[u] << ", " << low[v] << ") = " << low[u]
             << ".\n";
      }
    } else if (onStack[v]) {
      if (disc[v] < low[u]) low[u] = disc[v];
      if (verbose) {
        cout << pad << "  Arc " << u << " -> " << v << ": V" << v
             << " is still open on the stack, so low[" << u << "] = "
             << low[u] << ".\n";
      }
    } else {
      if (verbose) {
        cout << pad << "  Arc " << u << " -> " << v << ": V" << v
             << " sits in a finished component, so the arc is ignored.\n";
      }
    }
  }

  if (low[u] == disc[u]) {
    closeComponent(u, depth, verbose);
  } else if (verbose) {
    cout << pad << "Test V" << u << ": Low " << low[u] << " < disc " << disc[u]
         << ", so V" << u << " climbs out and no component closes.\n";
    cout << pad << "Leave V" << u << ".\n";
  }
}

static void relabelComponents() {
  int smallest[MAXV + 1];
  for (int c = 0; c <= compCount; c++) smallest[c] = MAXV + 1;
  for (int v = 0; v < n; v++) {
    int c = compOf[v];
    if (c >= 1 && v < smallest[c]) smallest[c] = v;
  }

  int order[MAXV + 1];
  for (int c = 1; c <= compCount; c++) order[c] = c;
  for (int i = 1; i <= compCount; i++) {
    for (int j = i + 1; j <= compCount; j++) {
      if (smallest[order[j]] < smallest[order[i]]) {
        int t = order[i];
        order[i] = order[j];
        order[j] = t;
      }
    }
  }

  int fresh[MAXV + 1];
  for (int i = 1; i <= compCount; i++) fresh[order[i]] = i;
  for (int v = 0; v < n; v++) compOf[v] = fresh[compOf[v]];
}

static int sizeOfComp(int id) {
  int size = 0;
  for (int v = 0; v < n; v++) {
    if (compOf[v] == id) size++;
  }
  return size;
}

static void printMemberList(int id) {
  for (int v = 0; v < n; v++) {
    if (compOf[v] == id) cout << " V" << v;
  }
}

static void reportComponents() {
  cout << "\n--- Components ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }

  for (int c = 1; c <= compCount; c++) {
    int size = sizeOfComp(c);
    cout << "Component " << c << " (" << size
         << (size == 1 ? " vertex):" : " vertices):");
    printMemberList(c);
    cout << "\n";
  }

  cout << "\nVertex     :";
  for (int v = 0; v < n; v++) cout << " " << v;
  cout << "\nComponent  :";
  for (int v = 0; v < n; v++) cout << " " << compOf[v];
  cout << "\n";

  cout << "\nComponents are numbered by their smallest vertex, so the numbers\n";
  cout << "are the same in the Kosaraju and the Tarjan file.\n";
}

static int arcCountBetween(int a, int b) {
  for (int u = 0; u < n; u++) {
    if (compOf[u] != a) continue;
    for (int v : adj[u]) {
      if (compOf[v] == b) return 1;
    }
  }
  return 0;
}

static void showCondensation() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (compCount == 0) {
    cout << "Find the components first.\n";
    return;
  }

  cout << "\n--- Condensation Graph ---\n";
  for (int c = 1; c <= compCount; c++) {
    cout << "C" << c << ":";
    printMemberList(c);
    cout << "\n";
  }

  cout << "Arcs:";
  int arcs = 0;
  for (int a = 1; a <= compCount; a++) {
    for (int b = 1; b <= compCount; b++) {
      if (a == b) continue;
      if (!arcCountBetween(a, b)) continue;
      cout << (arcs == 0 ? " " : ", ") << "C" << a << " -> C" << b;
      arcs++;
    }
  }
  if (arcs == 0) cout << " (None)";
  cout << "\n";

  cout << "One arc per component pair, no matter how many original arcs\n";
  cout << "crossed that way. Collapsing every cycle leaves no cycle behind,\n";
  cout << "so the condensation is always a DAG.\n";
}

static void runTrace() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  resetPass();
  cout << "\n--- Tarjan: One Pass ---\n";
  for (int start = 0; start < n; start++) {
    if (disc[start] != -1) continue;
    cout << "\nNew search at V" << start << ".\n";
    strongconnect(start, 0, true);
  }

  cout << "\nDisc :";
  for (int i = 0; i < n; i++) cout << " " << disc[i];
  cout << "\nLow  :";
  for (int i = 0; i < n; i++) cout << " " << low[i];
  cout << "\n";
  cout << "The search closed " << compCount
       << (compCount == 1 ? " component" : " components")
       << " and each one came off the stack in a single burst.\n";
  cout << "The far end of the condensation closes first, so this order is a\n";
  cout << "reverse topological order. Kosaraju reaches the same components\n";
  cout << "from the other side, starting at the sources.\n";

  relabelComponents();
  reportComponents();
}

static void runComponentsOnly() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  resetPass();
  for (int start = 0; start < n; start++) {
    if (disc[start] != -1) continue;
    strongconnect(start, 0, false);
  }

  relabelComponents();
  reportComponents();
}

static string arcLabel(int u, int v) {
  return "(" + to_string(u) + " -> " + to_string(v) + ")";
}

static void addArcInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nAdd arc (V = " << n << ", so vertices 0.." << (n - 1) << ")\n";
  while (true) {
    int u = readInt("  From vertex (-1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To vertex (-1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (hasArc[u][v]) {
      cout << "  Rejected: " << arcLabel(u, v) << " already exists.\n";
      cout << "    Arcs are ordered: " << v << " -> " << u
           << " is a different arc.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }

    addArc(u, v);
    cout << "  Added " << arcLabel(u, v) << "\n";
    cout << "    adj[" << u << "] gains " << v << "\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " arc" : " arcs")
         << " now.\n";
    return;
  }
}

static void removeNeighbour(vector<int> &list, int target) {
  for (int i = 0; i < (int)list.size(); i++) {
    if (list[i] != target) continue;
    list.erase(list.begin() + i);
    return;
  }
}

static void recountEdges() {
  edgeCount = 0;
  for (int u = 0; u < n; u++) {
    for (int v = 0; v < n; v++) {
      if (hasArc[u][v]) edgeCount++;
    }
  }
}

static void removeArcInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove an arc (V = " << n << ", so vertices 0.." << (n - 1)
       << ")\n";
  while (true) {
    int u = readInt("  From vertex (-1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To vertex (-1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (!hasArc[u][v]) {
      cout << "  Rejected: " << arcLabel(u, v) << " is not in the graph.\n";
      cout << "    Arcs are ordered: " << v << " -> " << u
           << " is a different arc.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }

    hasArc[u][v] = false;
    removeNeighbour(adj[u], v);
    edgeCount--;

    cout << "  Removed " << arcLabel(u, v) << "\n";
    cout << "    adj[" << u << "] drops " << v << "\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " arc" : " arcs")
         << " now.\n";
    return;
  }
}

static void addVertexInteractive() {
  if (n >= MAXV) {
    cout << "Vertex limit reached: MAXV = " << MAXV << ".\n";
    return;
  }

  int fresh = n;
  for (int i = 0; i <= fresh; i++) {
    hasArc[fresh][i] = false;
    hasArc[i][fresh] = false;
  }
  adj[fresh].clear();
  n++;

  cout << "\nAdded V" << fresh << " with no arcs.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
  cout << "  Nothing reaches it and it reaches nothing, so it is a component\n";
  cout << "  of one vertex.\n";
}

static void removeVertexInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  int v;
  while (true) {
    v = readInt("  Vertex to remove (-1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (v >= 0 && v < n) break;
    cout << "  Rejected: Vertex must be in 0.." << (n - 1) << ".\n";
  }

  bool kept[MAXV][MAXV];
  for (int a = 0; a < n; a++) {
    for (int b = 0; b < n; b++) kept[a][b] = false;
  }
  for (int a = 0; a < n; a++) {
    if (a == v) continue;
    for (int b = 0; b < n; b++) {
      if (b == v) continue;
      int na = (a < v) ? a : a - 1;
      int nb = (b < v) ? b : b - 1;
      kept[na][nb] = hasArc[a][b];
    }
  }

  vector<int> keptAdj[MAXV];
  for (int a = 0; a < n; a++) {
    if (a == v) continue;
    int na = (a < v) ? a : a - 1;
    for (int w : adj[a]) {
      if (w == v) continue;
      keptAdj[na].push_back((w < v) ? w : w - 1);
    }
  }

  int before = edgeCount;
  n--;
  for (int a = 0; a < MAXV; a++) {
    adj[a].clear();
    for (int b = 0; b < MAXV; b++) hasArc[a][b] = false;
  }
  for (int a = 0; a < n; a++) {
    adj[a] = keptAdj[a];
    for (int b = 0; b < n; b++) hasArc[a][b] = kept[a][b];
  }
  recountEdges();

  cout << "\nRemoved V" << v << ".\n";
  if (v < n) cout << "  Vertices above V" << v << " shift down by one.\n";
  cout << "  Arcs lost: " << before - edgeCount;
  cout << " (Recounted from the matrix.)\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
}

static void clearAllArcs() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nThe graph already has no arcs.\n";
    return;
  }

  int before = edgeCount;
  for (int u = 0; u < n; u++) {
    adj[u].clear();
    for (int w = 0; w < n; w++) hasArc[u][w] = false;
  }
  edgeCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " arc.\n" : " arcs.\n");
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
  cout << "  With no arcs every vertex is a component of its own.\n";
}

static void graphOperationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Directed | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Add an arc to the current graph\n";
    cout << "2. Remove an arc from the current graph\n";
    cout << "3. Add a vertex to the current graph\n";
    cout << "4. Remove a vertex from the current graph\n";
    cout << "5. Clear all arcs\n";
    cout << "0. Back to the main menu\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      addArcInteractive();
    } else if (choice == 2) {
      removeArcInteractive();
    } else if (choice == 3) {
      addVertexInteractive();
    } else if (choice == 4) {
      removeVertexInteractive();
    } else if (choice == 5) {
      clearAllArcs();
    } else if (choice == 0) {
      return;
    } else {
      cout << "Invalid option.\n";
    }
  }
}

int main() {
  resetGraph();

  while (true) {
    cout << "\n=== G17: Strongly Connected Components (Tarjan) ===\n";
    cout << "Graph: Directed | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build a directed graph\n";
    cout << "2. Load SCC demo (8 vertices)\n";
    cout << "3. Graph operations\n";
    cout << "4. Trace the single pass\n";
    cout << "5. Report components only\n";
    cout << "6. Show the condensation graph\n";
    cout << "7. Show adjacency list\n";
    cout << "8. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemo();
    } else if (choice == 3) {
      graphOperationsMenu();
    } else if (choice == 4) {
      runTrace();
    } else if (choice == 5) {
      runComponentsOnly();
    } else if (choice == 6) {
      showCondensation();
    } else if (choice == 7) {
      showAdjacency();
    } else if (choice == 8) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
