// 25. Transitive Closure: Warshall's algorithm never leaves the matrix. Round
// k lets Vk be the only middle vertex, and every row that can reach Vk simply
// borrows row k. Demo: A 6-vertex graph with one 3-cycle, where the rows fill
// in one round at a time and the finished diagonal names the cycle.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
using namespace std;

const int MAXV = 15;

static int n = 0;
static int edgeCount = 0;
static int adj[MAXV][MAXV];
static int reach[MAXV][MAXV];
static bool closureValid = false;

static void clearInput() {
  cin.clear();
  cin.ignore(1024, '\n');
}

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
    clearInput();
  }
}

static int readRange(const string &prompt, int lo, int hi) {
  while (true) {
    int x = readInt(prompt);
    if (x >= lo && x <= hi) return x;
    cout << "  Number must be in " << lo << ".." << hi << ".\n";
  }
}

static int readBit() {
  int x;
  while (true) {
    if (cin >> x) {
      if (x == 0 || x == 1) return x;
      cout << "  Each entry must be 0 or 1, so try that one again.\n";
      continue;
    }
    if (cin.eof()) {
      cout << "\nInput closed.\n";
      exit(0);
    }
    cout << "Invalid number. Try again.\n";
    clearInput();
  }
}

static const char *reachState() {
  return closureValid ? "Up to date" : "Out of date";
}

static void resetGraph() {
  for (int i = 0; i < MAXV; i++) {
    for (int j = 0; j < MAXV; j++) {
      adj[i][j] = 0;
      reach[i][j] = 0;
    }
  }
  n = 0;
  edgeCount = 0;
  closureValid = false;
}

static void setArc(int u, int v, int val) {
  if (adj[u][v] == val) return;
  adj[u][v] = val;
  if (val) edgeCount++;
  else edgeCount--;
  closureValid = false;
}

static void printGrid(int g[MAXV][MAXV], const string &pad) {
  cout << pad << "     ";
  for (int j = 0; j < n; j++) cout << setw(3) << j;
  cout << "\n";
  for (int i = 0; i < n; i++) {
    cout << pad << setw(3) << i << "  ";
    for (int j = 0; j < n; j++) cout << setw(3) << g[i][j];
    cout << "\n";
  }
}

static void showAdjacency() {
  cout << "\n--- Adjacency Matrix ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  printGrid(adj, "  ");
  cout << "Row i lists the edges leaving Vi, so a 1 at column j is the arc\n";
  cout << "Vi -> Vj and nothing more. The matrix is not symmetric.\n";
}

static void showReachability() {
  cout << "\n--- Reachability Matrix ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  if (!closureValid) {
    cout << "Run Warshall's algorithm first, so this table is up to date.\n";
    return;
  }
  printGrid(reach, "  ");
  cout << "A 1 at row i, column j means some route leads from Vi to Vj, with no\n";
  cout << "promise about its length. The diagonal is the interesting part, since\n";
  cout << "a 1 there says Vi sits on a cycle.\n";
}

static void buildFromMatrix() {
  int fresh = readRange("Number of vertices (0.." + to_string(MAXV) + "): ", 0,
                        MAXV);
  resetGraph();
  n = fresh;

  if (n == 0) {
    cout << "Build complete: 0 vertices, 0 edges.\n";
    return;
  }

  cout << "Type each row of the adjacency matrix as " << n
       << " values of 0 or 1.\n";
  cout << "A 1 at column j means an arc from Vi to Vj. Nothing is weighted here,\n";
  cout << "because the only question is whether a route exists at all.\n";

  for (int i = 0; i < n; i++) {
    while (true) {
      cout << "\nRow " << i << ": ";
      bool selfLoop = false;
      for (int j = 0; j < n; j++) {
        adj[i][j] = readBit();
        if (j == i && adj[i][j] == 1) selfLoop = true;
      }
      if (!selfLoop) break;
      cout << "  Rejected: A 1 on the diagonal is a self-loop, and it would put\n";
      cout << "  V" << i << " on a cycle for free. Please re-enter the row.\n";
    }
  }

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (adj[i][j]) edgeCount++;
    }
  }

  cout << "\nBuild complete: " << n << (n == 1 ? " vertex, " : " vertices, ")
       << edgeCount << (edgeCount == 1 ? " edge.\n" : " edges.\n");
}

static void loadDemo() {
  resetGraph();
  n = 6;
  cout << "Demo: 6 vertices, 6 edges, and one 3-cycle in the middle.\n";
  cout << "  0 -> 1, 1 -> 2, 2 -> 0, 2 -> 4, 3 -> 4, 4 -> 5.\n";
  cout << "  V0, V1 and V2 reach each other, so their rows should fill in and\n";
  cout << "  their diagonal entries should light up. V3 reaches V4 and V5 and\n";
  cout << "  nothing else, and V5 reaches nothing at all.\n\n";

  setArc(0, 1, 1);
  setArc(1, 2, 1);
  setArc(2, 0, 1);
  setArc(2, 4, 1);
  setArc(3, 4, 1);
  setArc(4, 5, 1);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void runWarshall() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) reach[i][j] = adj[i][j];
  }

  cout << "\n--- Warshall from the adjacency matrix ---\n";
  cout << "The adjacency matrix is copied into the reachability matrix first.\n";
  cout << "Round k allows Vk as the only middle vertex, so after round k every\n";
  cout << "route whose middle vertices all come from {V0..Vk} is present.\n";
  cout << "A row i with a 1 in column k can borrow row k: Whatever Vk reaches,\n";
  cout << "Vi reaches too, by going to Vk first.\n";
  cout << "Row k itself never changes during round k, since it would only borrow\n";
  cout << "from itself. That is what makes the in-place update safe.\n";

  int changes = 0;

  for (int k = 0; k < n; k++) {
    cout << "\n--- k = " << k << ": V" << k << " as the only middle vertex ---\n";

    int anyIn = 0;
    for (int i = 0; i < n; i++) {
      if (reach[i][k]) anyIn++;
    }

    bool rowKEmpty = true;
    for (int j = 0; j < n; j++) {
      if (reach[k][j]) rowKEmpty = false;
    }

    if (anyIn == 0) {
      cout << "  Column " << k << " is all zeros, so no row can borrow row " << k
           << ".\n";
    } else if (rowKEmpty) {
      cout << "  Row " << k << " is all zeros, so borrowing it gives nothing.\n";
    } else {
      if (reach[k][k]) {
        cout << "  Row " << k << " has a 1 in column " << k << ", so V" << k
             << " is on a cycle.\n";
        cout << "  Borrowing its own row changes nothing, so it is skipped.\n";
      }

      for (int i = 0; i < n; i++) {
        if (i == k) continue;
        if (!reach[i][k]) continue;

        string cols;
        int gained = 0;
        for (int j = 0; j < n; j++) {
          if (!reach[k][j] || reach[i][j]) continue;
          reach[i][j] = 1;
          gained++;
          if (gained > 1) cols += " ";
          cols += to_string(j);
        }

        if (gained == 0) {
          cout << "  Row " << i << " borrows row " << k
               << " but already holds every column in it.\n";
        } else {
          cout << "  Row " << i << " has a 1 in column " << k
               << ", so it borrows row " << k << "\n";
          cout << "  and gains " << (gained == 1 ? "column " : "columns ") << cols
               << ".\n";
          changes += gained;
        }
      }
    }

    if (n <= 8) {
      cout << "  Reachability now:\n";
      printGrid(reach, "  ");
    }
  }

  closureValid = true;

  cout << "\n--- Reachability matrix ---\n";
  printGrid(reach, "  ");

  string onCycle;
  string offCycle;
  int cyclic = 0;
  for (int v = 0; v < n; v++) {
    if (!reach[v][v]) {
      if (!offCycle.empty()) offCycle += " ";
      offCycle += to_string(v);
      continue;
    }
    cyclic++;
    if (!onCycle.empty()) onCycle += " ";
    onCycle += to_string(v);
  }

  int pairs = 0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (reach[i][j]) pairs++;
    }
  }

  cout << "\nThe diagonal answers one more question per vertex, whether it can\n";
  cout << "return to itself:\n";
  cout << "  On a cycle  : "
       << (onCycle.empty() ? "(None)" : "{" + onCycle + "}") << "\n";
  cout << "  Off a cycle : "
       << (offCycle.empty() ? "(None)" : "{" + offCycle + "}") << "\n";
  if (cyclic == 0) {
    cout << "  Nobody returns to itself, so the graph is acyclic.\n";
  } else if (cyclic == n) {
    cout << "  Every vertex returns to itself, so the whole graph sits on cycles.\n";
  } else {
    cout << "  " << cyclic << " of " << n
         << " vertices return to themselves, so those are the ones on a cycle.\n";
  }

  cout << "\nPairs that are connected: " << pairs << " of " << n * n
       << ", diagonal included.\n";
  cout << "Cells changed by the run: " << changes << ".\n";

  cout << "\nWarshall asked one question per cell: Is there any route from Vi to\n";
  cout << "Vj at all? It never looked at weights, so it cannot tell a cheap route\n";
  cout << "from an expensive one, and it never needed a queue or a frontier. The\n";
  cout << "triple loop is the whole algorithm, and O(V^3) is the whole cost,\n";
  cout << "however few edges there are.\n";
  cout << "\nFor a single source a BFS answers the same question in O(V + E), so\n";
  cout << "the matrix form earns its keep only when every pair is wanted at once,\n";
  cout << "or when the matrix is your input already.\n";
}

static void askPair() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (!closureValid) {
    cout << "Run Warshall's algorithm first, so the matrix is up to date.\n";
    return;
  }

  cout << "\nAsk about a pair (V = " << n << ", so vertices 0.." << (n - 1) << ")\n";
  int u = readRange("  From vertex: ", 0, n - 1);
  int v = readRange("  To vertex: ", 0, n - 1);

  if (u == v) {
    if (reach[u][u]) {
      cout << "  Yes, but only by going round a cycle. V" << u
           << " returns to itself.\n";
    } else {
      cout << "  No. V" << u << " never returns to itself, so no route leads\n";
      cout << "  from V" << u << " back to V" << u << ".\n";
    }
    return;
  }
  if (!reach[u][v]) {
    cout << "  No. V" << u << " cannot reach V" << v << ".\n";
    return;
  }
  cout << "  Yes. V" << u << " reaches V" << v;
  if (adj[u][v]) {
    cout << ", and the arc V" << u << " -> V" << v << " does it directly.\n";
  } else {
    cout << " through other vertices.\n";
  }
}

static void addArcInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nAdd an arc (V = " << n << ", so vertices 0.." << (n - 1) << ")\n";
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
    if (u == v) {
      cout << "  Rejected: A self-loop would put V" << u << " on a cycle for free.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (adj[u][v]) {
      cout << "  Rejected: " << u << " -> " << v << " already exists.\n";
      cout << "    Arcs are ordered: " << v << " -> " << u
           << " is a different arc.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }

    setArc(u, v, 1);
    cout << "  Added " << u << " -> " << v << ".\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " edge" : " edges")
         << " now.\n";
    cout << "  Reachability is out of date, so run Warshall again.\n";
    return;
  }
}

static void removeArcInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove an arc (V = " << n << ", so vertices 0.." << (n - 1) << ")\n";
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
    if (!adj[u][v]) {
      cout << "  Rejected: " << u << " -> " << v << " is not in the graph.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }

    setArc(u, v, 0);
    cout << "  Removed " << u << " -> " << v << ".\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " edge" : " edges")
         << " now.\n";
    cout << "  Reachability is out of date, so run Warshall again.\n";
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
    adj[fresh][i] = 0;
    adj[i][fresh] = 0;
    reach[fresh][i] = 0;
    reach[i][fresh] = 0;
  }
  n++;
  closureValid = false;

  cout << "\nAdded V" << fresh << " with no arcs.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
  cout << "  Its row and column are all zeros, so it reaches nothing and\n";
  cout << "  nothing reaches it until you add an arc.\n";
}

static void removeVertexInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove a vertex (V = " << n << ", so vertices 0.." << (n - 1) << ")\n";
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

  int lost = 0;
  for (int i = 0; i < n; i++) lost += adj[v][i];
  for (int i = 0; i < n; i++) {
    if (i != v) lost += adj[i][v];
  }

  for (int i = v; i < n - 1; i++) {
    for (int j = 0; j < n; j++) adj[i][j] = adj[i + 1][j];
  }
  for (int i = 0; i < n; i++) {
    for (int j = v; j < n - 1; j++) adj[i][j] = adj[i][j + 1];
  }
  n--;
  edgeCount -= lost;
  closureValid = false;

  cout << "\nRemoved V" << v << ".\n";
  if (v < n) cout << "  Vertices above V" << v << " shift down by one.\n";
  cout << "  Edges lost: " << lost << ".\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
}

static void clearAllArcs() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nThe graph already has no edges.\n";
    return;
  }

  int before = edgeCount;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) adj[i][j] = 0;
  }
  edgeCount = 0;
  closureValid = false;

  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
  cout << "  With no arcs, the closure is the zero matrix, and only the diagonal\n";
  cout << "  could ever light up again.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Directed, unweighted | V = " << n << ", E = " << edgeCount
         << " | Reachability: " << reachState() << "\n";
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
    cout << "\n=== G36: Transitive Closure (Warshall) ===\n";
    cout << "Graph: Directed, unweighted | V = " << n << ", E = " << edgeCount
         << " | Reachability: " << reachState() << "\n";
    cout << "1. Enter the adjacency matrix\n";
    cout << "2. Load demo graph (6 vertices)\n";
    cout << "3. Graph operations\n";
    cout << "4. Run Warshall's algorithm\n";
    cout << "5. Show the adjacency matrix\n";
    cout << "6. Show the reachability matrix\n";
    cout << "7. Ask whether one vertex reaches another\n";
    cout << "8. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildFromMatrix();
    } else if (choice == 2) {
      loadDemo();
    } else if (choice == 3) {
      operationsMenu();
    } else if (choice == 4) {
      runWarshall();
    } else if (choice == 5) {
      showAdjacency();
    } else if (choice == 6) {
      showReachability();
    } else if (choice == 7) {
      askPair();
    } else if (choice == 8) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
