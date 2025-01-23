// 26. Floyd-Warshall: One table, filled from the edges outwards. Round k asks
// every pair whether going through Vk is cheaper, so after round k all routes
// whose middle vertices come from {V0..Vk} are final. Nothing is settled, so a
// negative weight needs no special care, and a negative cycle announces itself
// on the diagonal. Demo: Six vertices where a -5 edge turns a cost of 20 into 6,
// and only the round that reaches V4 finds it.

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

const int MAXV = 15;
const int MAXE = 120;
const int INF = 1000000;

static int n = 0;
static int edgeCount = 0;
static int wG[MAXV][MAXV];
static bool hasArc[MAXV][MAXV];
static int fromSide[MAXE];
static int toSide[MAXE];
static int wEdge[MAXE];
static vector<pair<int, int>> adj[MAXV];

static int d[MAXV][MAXV];
static int nextHop[MAXV][MAXV];
static bool tableValid = false;
static bool negCycle = false;

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
    for (int j = 0; j < MAXV; j++) {
      wG[i][j] = 0;
      hasArc[i][j] = false;
    }
  }
  n = 0;
  edgeCount = 0;
}

static int addEdge(int u, int v, int w) {
  int id = edgeCount;
  fromSide[id] = u;
  toSide[id] = v;
  wEdge[id] = w;
  wG[u][v] = w;
  hasArc[u][v] = true;
  adj[u].push_back(make_pair(v, w));
  edgeCount++;
  return id;
}

static string edgeLabelOf(int id) {
  return "E" + to_string(id) + " (" + to_string(fromSide[id]) + " -> " +
         to_string(toSide[id]) + ", weight " + to_string(wEdge[id]) + ")";
}

static void dropEdgeId(int removed) {
  for (int id = removed; id < edgeCount - 1; id++) {
    fromSide[id] = fromSide[id + 1];
    toSide[id] = toSide[id + 1];
    wEdge[id] = wEdge[id + 1];
  }
  edgeCount--;
}

static const char *tableState() {
  if (!tableValid) return "Out of date";
  if (negCycle) return "Meaningless";
  return "Up to date";
}

static void printDistGrid() {
  int w = 6;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (d[i][j] >= INF) continue;
      int len = (int)to_string(d[i][j]).size();
      if (len + 2 > w) w = len + 2;
    }
  }
  cout << "      ";
  for (int j = 0; j < n; j++) cout << setw(w) << j;
  cout << "\n";
  for (int i = 0; i < n; i++) {
    cout << setw(4) << i << "  ";
    for (int j = 0; j < n; j++) {
      if (d[i][j] >= INF) cout << setw(w) << "-";
      else cout << setw(w) << d[i][j];
    }
    cout << "\n";
  }
}

static void printRoute(int u, int v) {
  if (d[u][v] >= INF) {
    cout << "(None)";
    return;
  }
  cout << u;
  int cur = u;
  int guard = 0;
  while (cur != v && guard <= n) {
    int nx = nextHop[cur][v];
    if (nx == -1) {
      cout << " ?";
      return;
    }
    cout << " - " << nx;
    cur = nx;
    guard++;
  }
}

static void printTable() {
  cout << "\n--- Distance matrix ---\n";
  printDistGrid();

  if (negCycle) {
    cout << "\nNegative diagonal entries:";
    for (int v = 0; v < n; v++) {
      if (d[v][v] < 0) cout << " d[" << v << "][" << v << "] = " << d[v][v];
    }
    cout << ".\n";
    cout << "A vertex that reaches itself for less than nothing sits on a cycle that\n";
    cout << "keeps getting cheaper. Every cell on that cycle keeps dropping with each\n";
    cout << "round, so none of these numbers is a shortest path, and the routes are\n";
    cout << "not printed at all.\n";
    return;
  }

  cout << "\n--- Routes ---\n";
  int pairs = 0;
  int missing = 0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (i == j) continue;
      if (d[i][j] >= INF) {
        missing++;
        continue;
      }
      pairs++;
      cout << "  V" << i << " -> V" << j << " at " << setw(4) << d[i][j] << "   ";
      printRoute(i, j);
      cout << "\n";
    }
  }
  if (pairs == 0) cout << "  (None)\n";
  if (missing > 0) {
    cout << "  Ordered pairs with no route at all: " << missing << ".\n";
  }
}

static void runFloydWarshall() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      d[i][j] = (i == j) ? 0 : INF;
      nextHop[i][j] = -1;
    }
  }
  for (int id = 0; id < edgeCount; id++) {
    int u = fromSide[id];
    int v = toSide[id];
    d[u][v] = wEdge[id];
    nextHop[u][v] = v;
  }

  cout << "\n--- Floyd-Warshall from the edge list ---\n";
  cout << "The table starts as the edge weights, so d[i][j] holds the direct\n";
  cout << "arc, 0 on the diagonal, and no route anywhere else. Round k allows Vk as\n";
  cout << "the only middle vertex, so after round k every route whose middle\n";
  cout << "vertices come from {V0..Vk} holds its final cost.\n";
  cout << "Each cell costs one addition and one test: Compare d[i][j] against\n";
  cout << "d[i][k] + d[k][j].\n";
  cout << "Row k and column k do not move during round k, which is what makes the\n";
  cout << "update safe in place. Nothing is settled, so a negative weight needs no\n";
  cout << "special handling at all.\n";

  long long tests = 0;
  int total = 0;

  for (int k = 0; k < n; k++) {
    cout << "\n--- k = " << k << ": V" << k << " as the only middle vertex ---\n";

    int improvements = 0;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (d[i][k] >= INF || d[k][j] >= INF) continue;
        tests++;
        int cand = d[i][k] + d[k][j];
        if (cand >= d[i][j]) continue;
        if (d[i][j] >= INF) {
          cout << "    d[" << i << "][" << j << "] = " << cand << " through V" << k
               << ", which was no route at all before.\n";
        } else {
          cout << "    d[" << i << "][" << j << "] = " << cand << " through V" << k
               << ", down from " << d[i][j] << ".\n";
        }
        nextHop[i][j] = (nextHop[i][k] == -1) ? nextHop[k][j] : nextHop[i][k];
        d[i][j] = cand;
        improvements++;
        total++;
      }
    }

    if (improvements == 0) {
      cout << "  Nothing improves through V" << k << ".\n";
    }

    if (n <= 8) {
      cout << "  Table now:\n";
      cout << "  ";
      printDistGrid();
    }
  }

  tableValid = true;
  negCycle = false;
  for (int v = 0; v < n; v++) {
    if (d[v][v] < 0) negCycle = true;
  }

  printTable();

  if (!negCycle) {
    int pairs = 0;
    int missing = 0;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (i == j) continue;
        if (d[i][j] >= INF) missing++;
        else pairs++;
      }
    }
    cout << "\nPairs with a route: " << pairs << " of " << n * (n - 1)
         << " ordered pairs, diagonal left out.\n";
  }
  cout << "Cells improved over the run: " << total << ", from " << tests
       << (tests == 1 ? " addition and test.\n" : " additions and tests.\n");
  cout << "Rounds: " << n << ", so at most " << n * n * n
       << (n * n * n == 1 ? " test in all.\n" : " tests in all.\n");

  if (negCycle) {
    cout << "\nFloyd found this without meaning to. The diagonal starts at 0, so a\n";
    cout << "negative entry there can only come from a route that leaves Vi and\n";
    cout << "returns for less than it cost to start. One such entry is enough to\n";
    cout << "know that no shortest path exists anywhere on that cycle.\n";
    return;
  }

  cout << "\nFloyd traded the queue for a table. For every pair and every possible\n";
  cout << "middle vertex it asks one question, and it asks it V times over, so the\n";
  cout << "cost is O(V^3) whether the graph is dense or nearly empty. That is more\n";
  cout << "than V runs of Dijkstra on a sparse graph, and far less than V runs of\n";
  cout << "Bellman-Ford on any graph. What it buys is every pair at once, routes\n";
  cout << "included, and a negative weight that needs no special case.\n";
}

static void buildGraph() {
  int fresh = readRange("Number of vertices (0.." + to_string(MAXV) + "): ", 0,
                        MAXV);
  resetGraph();
  n = fresh;
  tableValid = false;

  if (n == 0) {
    cout << "Build complete: 0 vertices, 0 edges.\n";
    return;
  }
  cout << "Directed edges with weights -99..99. Negative weights are welcome\n";
  cout << "here, since Floyd never settles a vertex early.\n";
  int e = 0;
  while (true) {
    e++;
    cout << "\nEdge " << e << "\n";
    int u = readInt("  From vertex (-1 to stop): ");
    if (u == -1) break;
    int v = readInt("  To vertex (-1 to stop): ");
    if (v == -1) break;
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      e--;
      continue;
    }
    if (u == v) {
      cout << "  Rejected: A self-loop is a one-vertex cycle, so it never helps.\n";
      e--;
      continue;
    }
    int w = readInt("  Weight (-99..99): ");
    if (w < -99 || w > 99) {
      cout << "  Rejected: Weights must be in -99..99.\n";
      e--;
      continue;
    }
    if (hasArc[u][v]) {
      cout << "  Rejected: That edge already exists.\n";
      e--;
      continue;
    }
    addEdge(u, v, w);
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << edgeCount
       << (edgeCount == 1 ? " edge.\n" : " edges.\n");
}

static void loadDemo() {
  resetGraph();
  n = 6;
  tableValid = false;
  cout << "Demo: 6 vertices, 8 edges, one negative weight, no negative cycle.\n";
  cout << "  0 -> 1 (3), 0 -> 2 (8), 1 -> 2 (2), 1 -> 3 (1), 2 -> 3 (4),\n";
  cout << "  3 -> 4 (7), 2 -> 5 (12), 4 -> 5 (-5).\n";
  cout << "  Watch the cost from V0 to V5. The obvious routes cost 20 and 17, but\n";
  cout << "  the long way round through V4 pays -5 at the end and lands at 6. No\n";
  cout << "  round before k = 4 can see that, because V4 is the last middle vertex\n";
  cout << "  the table needs.\n\n";

  addEdge(0, 1, 3);
  addEdge(0, 2, 8);
  addEdge(1, 2, 2);
  addEdge(1, 3, 1);
  addEdge(2, 3, 4);
  addEdge(3, 4, 7);
  addEdge(2, 5, 12);
  addEdge(4, 5, -5);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadCycleDemo() {
  resetGraph();
  n = 4;
  tableValid = false;
  cout << "Negative-cycle demo: 4 vertices, and V1 -> V2 -> V1 costs -2.\n";
  cout << "  0 -> 1 (2), 1 -> 2 (-3), 2 -> 1 (1), 2 -> 3 (4).\n";
  cout << "  Round k = 1 lets V1 be a middle vertex, and from there d[1][1] can\n";
  cout << "  only fall. Every later round makes it worse, which is the diagonal\n";
  cout << "  telling you that no shortest path exists.\n\n";

  addEdge(0, 1, 2);
  addEdge(1, 2, -3);
  addEdge(2, 1, 1);
  addEdge(2, 3, 4);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void showTable() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (!tableValid) {
    cout << "Run Floyd-Warshall first, so this table is up to date.\n";
    return;
  }
  printTable();
}

static void showMatrix() {
  cout << "\n--- Adjacency Matrix ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  cout << "      ";
  for (int j = 0; j < n; j++) cout << setw(4) << j;
  cout << "\n";
  for (int i = 0; i < n; i++) {
    cout << setw(4) << i << "  ";
    for (int j = 0; j < n; j++) {
      if (!hasArc[i][j]) cout << setw(4) << "-";
      else cout << setw(4) << wG[i][j];
    }
    cout << "\n";
  }
  cout << "A dash means no edge. The matrix is not symmetric, because row i\n";
  cout << "lists the edges leaving Vi.\n";
}

static void showEdges() {
  cout << "\n--- Edge List ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "(None)\n";
    cout << "Vertices are there, but no edge joins them.\n";
    return;
  }
  for (int id = 0; id < edgeCount; id++) {
    cout << "  " << edgeLabelOf(id) << "\n";
  }
  cout << "Each line reads from left to right, and the arrow only works one\n";
  cout << "way.\n";
}

static void addEdgeInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nAdd edge (V = " << n << ", so vertices 0.." << (n - 1) << ")\n";
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
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (u == v) {
      cout << "  Rejected: A self-loop is a one-vertex cycle, so it never helps.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (hasArc[u][v]) {
      cout << "  Rejected: " << u << " -> " << v << " already exists.\n";
      cout << "    Directed edges are ordered: " << v << " -> " << u
           << " is a different edge.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int w = readInt("  Weight (-99..99): ");
    if (w < -99 || w > 99) {
      cout << "  Rejected: Weights must be in -99..99.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int id = addEdge(u, v, w);
    cout << "  Added " << edgeLabelOf(id) << "\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " edge" : " edges")
         << " now.\n";
    return;
  }
}

static void removeEdgeInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove an edge (V = " << n << ", so vertices 0.." << (n - 1)
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
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (!hasArc[u][v]) {
      cout << "  Rejected: " << u << " -> " << v << " is not in the graph.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int found = -1;
    for (int id = 0; id < edgeCount; id++) {
      if (fromSide[id] == u && toSide[id] == v) {
        found = id;
        break;
      }
    }

    hasArc[u][v] = false;
    wG[u][v] = 0;
    for (int i = 0; i < (int)adj[u].size(); i++) {
      if (adj[u][i].first != v) continue;
      adj[u].erase(adj[u].begin() + i);
      break;
    }

    string label = edgeLabelOf(found);
    dropEdgeId(found);

    cout << "  Removed " << label << "\n";
    if (found < edgeCount) {
      cout << "  Edges above E" << found << " shift down by one.\n";
    }
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " edge" : " edges")
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
    wG[fresh][i] = 0;
    wG[i][fresh] = 0;
    hasArc[fresh][i] = false;
    hasArc[i][fresh] = false;
  }
  adj[fresh].clear();
  n++;

  cout << "\nAdded V" << fresh << " with no edges.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
  cout << "  A lone vertex is unreachable until some edge points at it.\n";
}

static void removeVertexInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove a vertex (V = " << n << ", so vertices 0.." << (n - 1)
       << ")\n";
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

  vector<int> keptFrom;
  vector<int> keptTo;
  vector<int> keptW;
  for (int id = 0; id < edgeCount; id++) {
    if (fromSide[id] == v || toSide[id] == v) continue;
    keptFrom.push_back(fromSide[id] < v ? fromSide[id] : fromSide[id] - 1);
    keptTo.push_back(toSide[id] < v ? toSide[id] : toSide[id] - 1);
    keptW.push_back(wEdge[id]);
  }

  int before = edgeCount;
  n--;

  for (int a = 0; a < MAXV; a++) {
    adj[a].clear();
    for (int b = 0; b < MAXV; b++) {
      wG[a][b] = 0;
      hasArc[a][b] = false;
    }
  }

  edgeCount = (int)keptFrom.size();
  for (int id = 0; id < edgeCount; id++) {
    fromSide[id] = keptFrom[id];
    toSide[id] = keptTo[id];
    wEdge[id] = keptW[id];
    wG[fromSide[id]][toSide[id]] = wEdge[id];
    hasArc[fromSide[id]][toSide[id]] = true;
    adj[fromSide[id]].push_back(make_pair(toSide[id], wEdge[id]));
  }

  cout << "\nRemoved V" << v << ".\n";
  if (v < n) cout << "  Vertices above V" << v << " shift down by one.\n";
  cout << "  Edges lost: " << before - edgeCount << ".\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
}

static void clearAllEdges() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nThe graph already has no edges.\n";
    return;
  }

  int before = edgeCount;
  for (int u = 0; u < n; u++) {
    adj[u].clear();
    for (int w = 0; w < n; w++) {
      wG[u][w] = 0;
      hasArc[u][w] = false;
    }
  }
  edgeCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
  cout << "  With no edges, no pair has a route, so the table is all dashes\n";
  cout << "  except the diagonal.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Directed, weighted -99..99 | V = " << n << ", E = " << edgeCount
         << " | Table: " << tableState() << "\n";
    cout << "1. Add an edge to the current graph\n";
    cout << "2. Remove an edge from the current graph\n";
    cout << "3. Add a vertex to the current graph\n";
    cout << "4. Remove a vertex from the current graph\n";
    cout << "5. Clear all edges\n";
    cout << "0. Back to the main menu\n";

    int beforeN = n;
    int beforeE = edgeCount;
    int choice = readInt("Select: ");

    if (choice == 1) {
      addEdgeInteractive();
    } else if (choice == 2) {
      removeEdgeInteractive();
    } else if (choice == 3) {
      addVertexInteractive();
    } else if (choice == 4) {
      removeVertexInteractive();
    } else if (choice == 5) {
      clearAllEdges();
    } else if (choice == 0) {
      return;
    } else {
      cout << "Invalid option.\n";
    }

    if (n != beforeN || edgeCount != beforeE) {
      tableValid = false;
      cout << "  The all-pairs table is out of date, so run Floyd-Warshall again.\n";
    }
  }
}

int main() {
  resetGraph();

  while (true) {
    cout << "\n=== G37: Floyd-Warshall (All Pairs) ===\n";
    cout << "Graph: Directed, weighted -99..99 | V = " << n << ", E = "
         << edgeCount << " | Table: " << tableState() << "\n";
    cout << "1. Build a graph from edges\n";
    cout << "2. Load demo graph (6 vertices)\n";
    cout << "3. Load negative-cycle demo (4 vertices)\n";
    cout << "4. Graph operations\n";
    cout << "5. Run Floyd-Warshall\n";
    cout << "6. Show the all-pairs table\n";
    cout << "7. Show the adjacency matrix\n";
    cout << "8. Show the edge list\n";
    cout << "9. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemo();
    } else if (choice == 3) {
      loadCycleDemo();
    } else if (choice == 4) {
      operationsMenu();
    } else if (choice == 5) {
      runFloydWarshall();
    } else if (choice == 6) {
      showTable();
    } else if (choice == 7) {
      showMatrix();
    } else if (choice == 8) {
      showEdges();
    } else if (choice == 9) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
