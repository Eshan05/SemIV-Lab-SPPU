// 23. Dijkstra from an Array: Settle the nearest unsettled vertex, then offer
// its edges to the rest. Finding that nearest vertex is a scan of the whole
// distance array, which is simple and costs O(V^2). Demo: Eight vertices where
// two offers get beaten before they are settled, and one vertex waits at 23
// until a detour brings 13.

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <queue>
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

static int dist[MAXV];
static int parentOf[MAXV];
static bool settled[MAXV];

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

static string distText(int v) {
  if (dist[v] == INF) return "-";
  return to_string(dist[v]);
}

static void printDistRow() {
  cout << "  Vertex :";
  for (int v = 0; v < n; v++) cout << setw(4) << v;
  cout << "\n  Dist   :";
  for (int v = 0; v < n; v++) cout << setw(4) << distText(v);
  cout << "\n  Parent :";
  for (int v = 0; v < n; v++) {
    if (parentOf[v] == -1) cout << setw(4) << "-";
    else cout << setw(4) << parentOf[v];
  }
  cout << "\n";
}

static void printPath(int v, int start) {
  vector<int> path;
  int cur = v;
  while (cur != -1) {
    path.push_back(cur);
    cur = parentOf[cur];
  }
  for (int i = (int)path.size() - 1; i >= 0; i--) {
    cout << (i == (int)path.size() - 1 ? "" : " - ") << path[i];
  }
  if (path.empty() || path.back() != start) cout << "?";
}

static void printSettledRow() {
  cout << "  Settled  : {";
  bool first = true;
  for (int v = 0; v < n; v++) {
    if (!settled[v]) continue;
    if (!first) cout << " ";
    cout << v;
    first = false;
  }
  cout << "} | Unsettled: {";
  first = true;
  for (int v = 0; v < n; v++) {
    if (settled[v]) continue;
    if (!first) cout << " ";
    cout << v;
    first = false;
  }
  cout << "}\n";
}

static void relaxOut(int u) {
  if (adj[u].empty()) {
    cout << "  (No edges leave V" << u << ", so nothing is offered.)\n";
    return;
  }
  for (int i = 0; i < (int)adj[u].size(); i++) {
    int v = adj[u][i].first;
    int w = adj[u][i].second;
    int offer = dist[u] + w;
    if (settled[v]) {
      cout << "    V" << v << " is settled already, so the offer of " << offer
           << " is dropped.\n";
      continue;
    }
    if (offer < dist[v]) {
      cout << "    V" << v << ": ";
      if (dist[v] == INF) cout << "No offer yet, so " << offer;
      else cout << distText(v) << " -> " << offer;
      cout << " through V" << u << ".\n";
      dist[v] = offer;
      parentOf[v] = u;
    } else {
      cout << "    V" << v << ": " << distText(v) << " stays, since " << offer
           << " is no better.\n";
    }
  }
}

static void runDijkstra() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nStart from which vertex?\n";
  int start = readRange("  Source vertex: ", 0, n - 1);

  for (int v = 0; v < n; v++) {
    dist[v] = INF;
    parentOf[v] = -1;
    settled[v] = false;
  }
  dist[start] = 0;

  cout << "\n--- Dijkstra from V" << start << " (Array) ---\n";
  cout << "Every pass takes the nearest unsettled vertex and settles it. That is\n";
  cout << "safe only because no weight is negative: A detour would have to cost\n";
  cout << "something, so it could never come out cheaper after settling.\n";

  settled[start] = true;
  cout << "  Start:\n";
  printDistRow();
  printSettledRow();
  cout << "  Offers from V" << start << ":\n";
  relaxOut(start);

  int pass = 0;
  int settledCount = 1;

  while (settledCount < n) {
    int best = -1;
    for (int v = 0; v < n; v++) {
      if (settled[v] || dist[v] == INF) continue;
      if (best == -1 || dist[v] < dist[best]) best = v;
    }

    if (best == -1) {
      cout << "\nNo unsettled vertex can be reached, so the run stops there.\n";
      break;
    }

    pass++;
    cout << "\nPass " << pass << "\n";
    printDistRow();

    cout << "  Take V" << best << " at " << dist[best]
         << ", the nearest unsettled vertex.\n";
    settled[best] = true;
    settledCount++;
    if (pass == 1) {
      cout << "  Settle it: Any other route would have to leave the unsettled set\n";
      cout << "  and come back, and every edge on the way costs at least one, so\n";
      cout << "  nothing can beat " << dist[best] << ".\n";
      cout << "  Every later pass is the same argument on a smaller set.\n";
    }
    cout << "  Offers from V" << best << ":\n";
    relaxOut(best);
    printSettledRow();
  }

  cout << "\n--- Distances from V" << start << " ---\n";
  printDistRow();

  int reached = 0;
  cout << "\nPaths:\n";
  for (int v = 0; v < n; v++) {
    if (dist[v] == INF) continue;
    reached++;
    cout << "  V" << v << " at " << dist[v] << "   ";
    printPath(v, start);
    cout << "\n";
  }
  for (int v = 0; v < n; v++) {
    if (dist[v] != INF) continue;
    cout << "  V" << v << " at (Unreachable)\n";
  }

  cout << "\nReached " << reached << " of " << n << " vertices. Passes: " << pass
       << ".\n";
  cout << "Each pass scans every vertex to find the nearest one, so the nearest\n";
  cout << "search alone costs about " << n << " x " << n << " = " << n * n
       << " steps.\n";
  cout << "\nThe settled set only grows, and every distance in it is final. That\n";
  cout << "is the whole argument, and it is exactly where a negative edge would\n";
  cout << "break the method: A detour with one negative edge can still turn out\n";
  cout << "cheaper after a vertex has been settled.\n";
}

static void buildGraph() {
  int fresh = readRange("Number of vertices (0.." + to_string(MAXV) + "): ", 0,
                        MAXV);
  resetGraph();
  n = fresh;

  if (n == 0) {
    cout << "Build complete: 0 vertices, 0 edges.\n";
    return;
  }
  cout << "Directed edges with weights 1..99. Dijkstra cannot use a negative\n";
  cout << "weight, since the nearest vertex has to stay final.\n";
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
      cout << "  Rejected: A self-loop never shortens a path.\n";
      e--;
      continue;
    }
    int w = readInt("  Weight (1..99): ");
    if (w < 1 || w > 99) {
      cout << "  Rejected: Dijkstra needs non-negative weights, so 1..99.\n";
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
  n = 8;
  cout << "Demo: 8 vertices, 13 edges, and V7 cannot be reached at all.\n";
  cout << "  0 -> 1 (4), 0 -> 2 (1), 2 -> 1 (2), 1 -> 3 (5), 2 -> 3 (8),\n";
  cout << "  2 -> 4 (9), 3 -> 4 (2), 4 -> 5 (3), 1 -> 5 (20), 5 -> 6 (1),\n";
  cout << "  3 -> 6 (9), 6 -> 4 (1), 7 -> 0 (2).\n";
  cout << "  Two offers get beaten before they are settled: V1 drops from 4 to 3\n";
  cout << "  and V3 from 9 to 8, and V5 waits at 23 until V4 turns up with 13.\n\n";

  addEdge(0, 1, 4);
  addEdge(0, 2, 1);
  addEdge(2, 1, 2);
  addEdge(1, 3, 5);
  addEdge(2, 3, 8);
  addEdge(2, 4, 9);
  addEdge(3, 4, 2);
  addEdge(4, 5, 3);
  addEdge(1, 5, 20);
  addEdge(5, 6, 1);
  addEdge(3, 6, 9);
  addEdge(6, 4, 1);
  addEdge(7, 0, 2);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadTieDemo() {
  resetGraph();
  n = 5;
  cout << "Tie demo: 5 vertices where V1 and V2 sit at the same distance.\n";
  cout << "  0 -> 1 (2), 0 -> 2 (2), 1 -> 3 (1), 2 -> 3 (1), 4 -> 0 (1).\n";
  cout << "  The tie goes to the lower index, and either order gives the same\n";
  cout << "  distances. V4 cannot be reached from V0.\n\n";

  addEdge(0, 1, 2);
  addEdge(0, 2, 2);
  addEdge(1, 3, 1);
  addEdge(2, 3, 1);
  addEdge(4, 0, 1);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
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
      cout << "  Rejected: A self-loop never shortens a path.\n";
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

    int w = readInt("  Weight (1..99): ");
    if (w < 1 || w > 99) {
      cout << "  Rejected: Dijkstra needs non-negative weights, so 1..99.\n";
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
  cout << "  With no edges, only the source can be reached.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Directed, weighted 1..99 | V = " << n << ", E = "
         << edgeCount << "\n";
    cout << "1. Add an edge to the current graph\n";
    cout << "2. Remove an edge from the current graph\n";
    cout << "3. Add a vertex to the current graph\n";
    cout << "4. Remove a vertex from the current graph\n";
    cout << "5. Clear all edges\n";
    cout << "0. Back to the main menu\n";

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
  }
}

int main() {
  resetGraph();

  while (true) {
    cout << "\n=== G33: Dijkstra from an Array ===\n";
    cout << "Graph: Directed, weighted 1..99 | V = " << n << ", E = " << edgeCount
         << "\n";
    cout << "1. Build a graph with weights 1..99\n";
    cout << "2. Load demo graph (8 vertices)\n";
    cout << "3. Load tie demo (5 vertices)\n";
    cout << "4. Graph operations\n";
    cout << "5. Run Dijkstra from a vertex\n";
    cout << "6. Show the adjacency matrix\n";
    cout << "7. Show the edge list\n";
    cout << "8. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemo();
    } else if (choice == 3) {
      loadTieDemo();
    } else if (choice == 4) {
      operationsMenu();
    } else if (choice == 5) {
      runDijkstra();
    } else if (choice == 6) {
      showMatrix();
    } else if (choice == 7) {
      showEdges();
    } else if (choice == 8) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
