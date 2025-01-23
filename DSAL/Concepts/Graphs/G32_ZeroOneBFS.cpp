// 22. 0-1 BFS: A deque keeps the frontier sorted by distance, because a
// 0-weight edge leaves the distance alone and goes on the front, while a
// 1-weight edge adds exactly one and goes on the back. Demo: Eight vertices
// where a chain of 0-weight edges beats one weight-1 edge, and a plain BFS
// would report the wrong cost.

#include <algorithm>
#include <cstdlib>
#include <deque>
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

static void printDeque(const deque<int> &dq) {
  cout << "  Deque, front on the left: [";
  bool first = true;
  for (int i = 0; i < (int)dq.size(); i++) {
    if (!first) cout << " ";
    cout << dq[i];
    first = false;
  }
  if (first) cout << "Empty";
  cout << "]\n";
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

static void buildGraph() {
  int fresh = readRange("Number of vertices (0.." + to_string(MAXV) + "): ", 0,
                        MAXV);
  resetGraph();
  n = fresh;

  if (n == 0) {
    cout << "Build complete: 0 vertices, 0 edges.\n";
    return;
  }
  cout << "Directed edges with weights 0 or 1. That is what a deque can sort.\n";
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
    int w = readInt("  Weight (0 or 1): ");
    if (w < 0 || w > 1) {
      cout << "  Rejected: A 0-1 BFS only handles weights 0 and 1.\n";
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
  cout << "Demo: 8 vertices, 12 edges, and V7 cannot be reached at all.\n";
  cout << "  0 -> 1 (1), 0 -> 2 (0), 2 -> 1 (0), 1 -> 3 (1), 2 -> 3 (1),\n";
  cout << "  3 -> 4 (0), 1 -> 4 (1), 2 -> 5 (1), 4 -> 5 (1), 5 -> 6 (0),\n";
  cout << "  4 -> 6 (1), 7 -> 0 (1).\n";
  cout << "  The chain 0 -> 2 -> 1 costs nothing, so V1 is cheaper than its own\n";
  cout << "  single weight-1 edge from V0 would suggest.\n\n";

  addEdge(0, 1, 1);
  addEdge(0, 2, 0);
  addEdge(2, 1, 0);
  addEdge(1, 3, 1);
  addEdge(2, 3, 1);
  addEdge(3, 4, 0);
  addEdge(1, 4, 1);
  addEdge(2, 5, 1);
  addEdge(4, 5, 1);
  addEdge(5, 6, 0);
  addEdge(4, 6, 1);
  addEdge(7, 0, 1);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadOnesDemo() {
  resetGraph();
  n = 6;
  cout << "All-ones demo: 6 vertices, every weight 1, so the deque turns into a\n";
  cout << "plain BFS queue and the distance counts edges.\n";
  cout << "  0 -> 1 (1), 0 -> 2 (1), 1 -> 3 (1), 2 -> 3 (1), 1 -> 4 (1),\n";
  cout << "  3 -> 5 (1), 4 -> 5 (1).\n\n";

  addEdge(0, 1, 1);
  addEdge(0, 2, 1);
  addEdge(1, 3, 1);
  addEdge(2, 3, 1);
  addEdge(1, 4, 1);
  addEdge(3, 5, 1);
  addEdge(4, 5, 1);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void runZeroOne() {
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

  deque<int> dq;
  dq.push_back(start);

  cout << "\n--- 0-1 BFS from V" << start << " ---\n";
  cout << "A 0-weight edge keeps the distance, so the vertex goes on the front.\n";
  cout << "A 1-weight edge adds one, so it goes on the back. The deque therefore\n";
  cout << "stays in distance order, which is all BFS needs to handle weights.\n";
  cout << "Push V" << start << " with distance 0, since the source costs nothing.\n";

  int step = 0;
  int pushes = 1;
  int stale = 0;
  int settles = 0;

  while (!dq.empty()) {
    int u = dq.front();
    dq.pop_front();
    step++;

    cout << "\nStep " << step << "\n";
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

    if (settled[u]) {
      stale++;
      cout << "  Pop V" << u << ", but it was settled at " << dist[u]
           << " already, so this\n";
      cout << "  second copy is ignored.\n";
      printDeque(dq);
      continue;
    }

    settled[u] = true;
    cout << "  Pop V" << u << " from the front and settle it at " << dist[u]
         << ".\n";
    if (settles == 0) {
      cout << "  Nothing in the deque is nearer, so no later route can beat a\n";
      cout << "  settled distance. Every step repeats that argument.\n";
    }
    settles++;

    if (adj[u].empty()) {
      cout << "  Edges out of V" << u << ": (None)\n";
      printDeque(dq);
      continue;
    }

    cout << "  Edges out of V" << u << ":\n";
    for (int i = 0; i < (int)adj[u].size(); i++) {
      int v = adj[u][i].first;
      int w = adj[u][i].second;
      int offer = dist[u] + w;
      cout << "    To V" << v << ", weight " << w << ": " << dist[u] << " + "
           << w << " = " << offer << ", ";
      if (offer < dist[v]) {
        cout << "so dist[" << v << "] = " << offer << " and it goes on the "
             << (w == 0 ? "front" : "back") << ".\n";
        dist[v] = offer;
        parentOf[v] = u;
        pushes++;
        if (w == 0) dq.push_front(v);
        else dq.push_back(v);
      } else {
        cout << "but dist[" << v << "] is already " << distText(v)
             << ".\n";
      }
    }
    printDeque(dq);
  }

  cout << "\nThe deque is empty, so every reachable vertex is settled.\n";
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

  cout << "\nReached " << reached << " of " << n << " vertices. Pushes: "
       << pushes << ".\n";
  if (stale > 0) {
    cout << "Stale copies ignored: " << stale << ", which happens when a vertex\n";
    cout << "got a better distance after it was already in the deque.\n";
  } else {
    cout << "No copy ever had to be thrown away.\n";
  }

  bool anyZero = false;
  for (int id = 0; id < edgeCount; id++) {
    if (wEdge[id] == 0) anyZero = true;
  }

  if (anyZero) {
    cout << "\nEvery 0-weight edge kept the distance and went on the front, and\n";
    cout << "every 1-weight edge raised it by one and went on the back. That is\n";
    cout << "why the deque is always sorted, and why a vertex is final the moment\n";
    cout << "it leaves the front. Plain BFS counts edges instead of weight, so it\n";
    cout << "would charge a 0-weight chain as if each link cost 1.\n";
  } else {
    cout << "\nEvery edge weighs 1, so every push went on the back and the deque\n";
    cout << "behaved exactly like a plain BFS queue. That is the case 0-1 BFS was\n";
    cout << "built to generalise, and it costs no more than BFS does.\n";
  }
  cout << "\nEach edge causes at most one push, so the whole run costs O(V + E).\n";
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

    int w = readInt("  Weight (0 or 1): ");
    if (w < 0 || w > 1) {
      cout << "  Rejected: A 0-1 BFS only handles weights 0 and 1.\n";
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
    cout << "Graph: Directed, weighted 0 or 1 | V = " << n << ", E = "
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
    cout << "\n=== G32: 0-1 BFS with a Deque ===\n";
    cout << "Graph: Directed, weighted 0 or 1 | V = " << n << ", E = "
         << edgeCount << "\n";
    cout << "1. Build a graph with weights 0 or 1\n";
    cout << "2. Load demo graph (8 vertices)\n";
    cout << "3. Load all-ones demo (6 vertices)\n";
    cout << "4. Graph operations\n";
    cout << "5. Run 0-1 BFS from a vertex\n";
    cout << "6. Show the adjacency matrix\n";
    cout << "7. Show the edge list\n";
    cout << "8. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemo();
    } else if (choice == 3) {
      loadOnesDemo();
    } else if (choice == 4) {
      operationsMenu();
    } else if (choice == 5) {
      runZeroOne();
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
