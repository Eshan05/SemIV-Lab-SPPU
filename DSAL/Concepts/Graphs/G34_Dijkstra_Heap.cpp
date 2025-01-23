// 23. Dijkstra from a Heap: The array version spent a whole scan on every pass
// just to find the nearest unsettled vertex. A min-heap answers that in
// O(log E) instead, and the settling argument does not change at all: Pop the
// smallest offer and nothing left in the heap can beat it. The price is stale
// copies, because an improvement leaves the old entry behind. Demo: The same
// eight vertices as the array version, so the two runs can be compared step by
// step.

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

typedef priority_queue<pair<int, int>, vector<pair<int, int>>,
                       greater<pair<int, int>>>
    MinHeap;

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

static void printHeap(MinHeap heap) {
  vector<pair<int, int>> items;
  while (!heap.empty()) {
    items.push_back(heap.top());
    heap.pop();
  }
  if (items.empty()) {
    cout << "  Heap, smallest first: (Empty)\n";
    return;
  }
  cout << "  Heap, smallest first: ";
  for (int i = 0; i < (int)items.size(); i++) {
    if (i > 0 && i % 4 == 0) cout << "\n      ";
    else if (i > 0) cout << " ";
    cout << "(" << items[i].first << ", " << items[i].second << ")";
  }
  cout << "\n";
}

static void relaxOut(int u, MinHeap &heap, int &pushes) {
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
      cout << " through V" << u << ", so push (" << offer << ", " << v << ").\n";
      dist[v] = offer;
      parentOf[v] = u;
      heap.push(make_pair(offer, v));
      pushes++;
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

  cout << "\n--- Dijkstra from V" << start << " (Heap) ---\n";
  cout << "The array version hunted for the nearest unsettled vertex on every\n";
  cout << "pass. A heap does that hunting for us: Push each improved distance,\n";
  cout << "and the smallest one always pops first. Same rule, cheaper search.\n";

  MinHeap heap;
  heap.push(make_pair(0, start));
  int pushes = 1;
  int pops = 0;
  int stale = 0;

  cout << "\nBefore any pop:\n";
  printDistRow();
  cout << "Nothing is settled yet, and only V" << start << " has a distance.\n";
  cout << "Push (0, " << start << ") for the source, since it costs nothing.\n";
  printHeap(heap);

  int step = 0;
  while (!heap.empty()) {
    int d = heap.top().first;
    int u = heap.top().second;
    heap.pop();
    pops++;
    step++;

    cout << "\nStep " << step << "\n";
    printDistRow();

    if (settled[u]) {
      stale++;
      cout << "  Pop V" << u << " at " << d << ", but V" << u << " was settled at "
           << dist[u] << " already,\n";
      cout << "  so this copy is dropped.\n";
      if (stale == 1) {
        cout << "  A heap cannot delete or lower an entry, so the old copy had to\n";
        cout << "  sit there until it came up. Every stale pop after this looks the\n";
        cout << "  same, so they are announced without the explanation.\n";
      }
      printHeap(heap);
      continue;
    }

    settled[u] = true;
    cout << "  Pop V" << u << " at " << d << " and settle it.\n";
    if (step == 1) {
      cout << "  Nothing in the heap is nearer, so nothing can beat 0.\n";
      cout << "  Every later pop repeats that same argument.\n";
    }
    cout << "  Offers from V" << u << ":\n";
    relaxOut(u, heap, pushes);
    printSettledRow();
    printHeap(heap);
  }

  cout << "\nThe heap is empty, so every reachable vertex is settled.\n";
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

  cout << "\nReached " << reached << " of " << n << " vertices. Pushes: " << pushes
       << ", pops: " << pops << ".\n";
  if (stale > 0) {
    cout << "Stale copies: " << stale << ", and each one was dropped the moment it\n";
    cout << "surfaced.\n";
  } else {
    cout << "No copy was ever stale, because no distance was improved twice.\n";
  }

  cout << "\nA vertex is pushed once per improvement, so the heap holds at most E\n";
  cout << "entries, and each push or pop costs a trip of height log E. That is\n";
  cout << "the whole gain over the array, which paid a full V scan per pass. On a\n";
  cout << "dense graph the heap earns little, since E is close to V x V and the\n";
  cout << "scan was already cheap; on a sparse one it earns a lot.\n";
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
  cout << "  The interesting part for a heap is V5: It is pushed once at 23 and\n";
  cout << "  again at 13, so two copies sit in the heap and the 23 copy is only\n";
  cout << "  discovered to be useless when it finally surfaces.\n\n";

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
  cout << "  Both (2, 1) and (2, 2) are pushed, and the heap breaks the tie by\n";
  cout << "comparing the vertex number second. Either order gives the same\n";
  cout << "distances. V4 cannot be reached from V0.\n\n";

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
    cout << "\n=== G34: Dijkstra from a Heap ===\n";
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
