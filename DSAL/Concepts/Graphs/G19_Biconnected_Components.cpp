// 15a. Biconnected components (Blocks): Stack every edge, and pop a block the
// moment Low[v] >= disc[u], because then nothing below v climbs over u.
// Demo: A barbell of two triangles joined by a bridge falls into three blocks.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;
const int MAXE = 120;

static int n = 0;
static int edgeCount = 0;
static bool matrixG[MAXV][MAXV];
static int aSide[MAXE];
static int bSide[MAXE];
static vector<int> incident[MAXV];

static int disc[MAXV];
static int low[MAXV];
static bool seen[MAXV];
static int timerStep = 0;

static vector<int> edgeStack;
static vector<int> blockEdges[MAXE + 1];
static int blockCount = 0;

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
    incident[i].clear();
    for (int j = 0; j < MAXV; j++) matrixG[i][j] = false;
  }
  n = 0;
  edgeCount = 0;
  edgeStack.clear();
  blockCount = 0;
  for (int i = 0; i <= MAXE; i++) blockEdges[i].clear();
}

static int otherEnd(int id, int u) {
  return (aSide[id] == u) ? bSide[id] : aSide[id];
}

static int addEdge(int u, int v) {
  int id = edgeCount;
  aSide[id] = u;
  bSide[id] = v;
  incident[u].push_back(id);
  incident[v].push_back(id);
  matrixG[u][v] = true;
  matrixG[v][u] = true;
  edgeCount++;
  return id;
}

static string edgeLabelOf(int id) {
  return "E" + to_string(id) + " (" + to_string(aSide[id]) + " - " +
         to_string(bSide[id]) + ")";
}

// Ids stay a 0..E-1 run, so removing one shifts every later id down and the
// incidence lists are rebuilt rather than patched in place.
static void dropEdgeId(int removed) {
  for (int id = removed; id < edgeCount - 1; id++) {
    aSide[id] = aSide[id + 1];
    bSide[id] = bSide[id + 1];
  }
  edgeCount--;

  for (int a = 0; a < MAXV; a++) {
    vector<int> fresh;
    for (int oldId : incident[a]) {
      if (oldId == removed) continue;
      fresh.push_back(oldId > removed ? oldId - 1 : oldId);
    }
    incident[a] = fresh;
  }
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
  cout << "Undirected edges only. Enter two vertex numbers in 0.." << (n - 1)
       << ".\n";
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
      cout << "  Rejected: A self-loop is not a second route between two ends.\n";
      e--;
      continue;
    }
    if (matrixG[u][v]) {
      cout << "  Rejected: That edge already exists.\n";
      e--;
      continue;
    }
    addEdge(u, v);
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << edgeCount
       << (edgeCount == 1 ? " edge.\n" : " edges.\n");
}

static void loadDemo(int which) {
  resetGraph();

  if (which == 1) {
    n = 6;
    cout << "Barbell demo: 6 vertices.\n";
    cout << "  Triangle 0 - 1 - 2 - 0, bridge 2 - 3, triangle 3 - 4 - 5 - 3.\n";
    cout << "  Expect three blocks: Two triangles and the bridge on its own.\n\n";
    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(2, 0);
    addEdge(2, 3);
    addEdge(3, 4);
    addEdge(4, 5);
    addEdge(5, 3);
  } else if (which == 2) {
    n = 5;
    cout << "Two-triangles demo: 5 vertices.\n";
    cout << "  Triangles 0 - 1 - 2 - 0 and 0 - 3 - 4 - 0 share only V0.\n";
    cout << "  Expect two blocks that meet at the shared vertex.\n\n";
    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(2, 0);
    addEdge(0, 3);
    addEdge(3, 4);
    addEdge(4, 0);
  } else {
    n = 5;
    cout << "Path demo: 5 vertices.\n";
    cout << "  Path 0 - 1 - 2 - 3 - 4, no cycle at all.\n";
    cout << "  Every edge is its own block, because every edge is a bridge.\n\n";
    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(2, 3);
    addEdge(3, 4);
  }

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void showAdjacency() {
  cout << "\n--- Adjacency List (Edge ids) ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  for (int i = 0; i < n; i++) {
    cout << setw(3) << i << ":";
    if (incident[i].empty()) cout << " (None)";
    else
      for (int id : incident[i]) cout << " E" << id;
    cout << "\n";
  }
  cout << "Every edge id appears twice, once at each of its ends.\n";
}

static void resetSearch() {
  for (int i = 0; i < MAXV; i++) {
    disc[i] = -1;
    low[i] = -1;
    seen[i] = false;
  }
  timerStep = 0;
  edgeStack.clear();
  blockCount = 0;
  for (int i = 0; i <= MAXE; i++) blockEdges[i].clear();
}

static void printEdgeStack(const string &pad) {
  cout << pad << "  Edge stack now:";
  if (edgeStack.empty()) {
    cout << " (Empty)\n";
    return;
  }
  for (int i = 0; i < (int)edgeStack.size(); i++) {
    cout << (i == 0 ? " [" : ", ") << "E" << edgeStack[i];
  }
  cout << "]\n";
}

static void closeBlock(int stopEdge, int depth, bool verbose) {
  string pad(depth * 2, ' ');
  vector<int> fresh;
  while (true) {
    int id = edgeStack.back();
    edgeStack.pop_back();
    fresh.push_back(id);
    if (id == stopEdge) break;
  }

  for (int i = 0; i < (int)fresh.size(); i++) {
    for (int j = i + 1; j < (int)fresh.size(); j++) {
      if (fresh[j] >= fresh[i]) continue;
      int t = fresh[i];
      fresh[i] = fresh[j];
      fresh[j] = t;
    }
  }

  blockEdges[blockCount] = fresh;
  blockCount++;

  if (verbose) {
    cout << pad << "    Pop until E" << stopEdge << " comes off.\n";
    cout << pad << "    Block " << blockCount << " holds " << fresh.size()
         << (fresh.size() == 1 ? " edge:" : " edges:");
    for (int id : fresh) cout << " E" << id;
    cout << "\n";
    printEdgeStack(pad);
  }
}

static void dfs(int u, int parentEdge, int depth, bool verbose) {
  string pad(depth * 2, ' ');
  seen[u] = true;
  disc[u] = timerStep;
  low[u] = disc[u];
  timerStep++;
  if (verbose) {
    cout << pad << "Enter V" << u << " with disc " << disc[u] << ".\n";
  }

  for (int id : incident[u]) {
    int v = otherEnd(id, u);

    if (id == parentEdge) {
      if (verbose) {
        cout << pad << "  " << edgeLabelOf(id) << ": The edge back to the"
             << " parent, skip it.\n";
      }
      continue;
    }

    if (!seen[v]) {
      edgeStack.push_back(id);
      if (verbose) {
        cout << pad << "  " << edgeLabelOf(id) << ": Tree edge, push it and"
             << " descend.\n";
      }
      dfs(v, id, depth + 1, verbose);

      int before = low[u];
      if (low[v] < low[u]) low[u] = low[v];
      if (verbose) {
        cout << pad << "    Return from V" << v << ", so low[" << u
             << "] = min(" << before << ", " << low[v] << ") = " << low[u]
             << ".\n";
      }

      if (low[v] >= disc[u]) {
        if (verbose) {
          cout << pad << "    Low[" << v << "] = " << low[v] << " >= disc[" << u
               << "] = " << disc[u] << ", so no route below V" << v
               << " climbs over V" << u << ".\n";
        }
        closeBlock(id, depth, verbose);
      } else if (verbose) {
        cout << pad << "    Low[" << v << "] = " << low[v] << " < disc[" << u
             << "] = " << disc[u] << ", so the block runs on past V" << u
             << ".\n";
      }
    } else if (disc[v] < disc[u]) {
      edgeStack.push_back(id);
      int before = low[u];
      if (disc[v] < low[u]) low[u] = disc[v];
      if (verbose) {
        cout << pad << "  " << edgeLabelOf(id) << ": Back edge to an ancestor,"
             << " push it, and low[" << u << "] = min(" << before << ", "
             << disc[v] << ") = " << low[u] << ".\n";
      }
    } else {
      if (verbose) {
        cout << pad << "  " << edgeLabelOf(id) << ": Already pushed from the"
             << " other side, ignore it.\n";
      }
    }
  }
}

static void runSearch(bool verbose) {
  resetSearch();
  if (verbose) cout << "\n--- Lowlink search with an edge stack ---\n";
  for (int start = 0; start < n; start++) {
    if (seen[start]) continue;
    if (verbose) cout << "\nNew search at V" << start << ".\n";
    dfs(start, -1, 0, verbose);
  }
}

static bool vertexInBlock(int b, int v) {
  for (int id : blockEdges[b]) {
    if (aSide[id] == v || bSide[id] == v) return true;
  }
  return false;
}

static int blocksHolding(int v) {
  int count = 0;
  for (int b = 0; b < blockCount; b++) {
    if (vertexInBlock(b, v)) count++;
  }
  return count;
}

static void listBlocks() {
  cout << "\n--- Biconnected Components ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "No blocks. A block needs at least one edge.\n";
    return;
  }
  if (blockCount == 0) {
    cout << "Run the search first.\n";
    return;
  }

  for (int b = 0; b < blockCount; b++) {
    int m = (int)blockEdges[b].size();
    cout << "Block " << (b + 1) << ": " << m << (m == 1 ? " edge" : " edges")
         << ", vertices";
    for (int v = 0; v < n; v++) {
      if (vertexInBlock(b, v)) cout << " " << v;
    }
    cout << "\n";
    for (int id : blockEdges[b]) {
      cout << "  " << edgeLabelOf(id) << "\n";
    }
  }

  cout << "\nBlocks: " << blockCount << ", one per stack burst.\n";
  cout << "Every edge sits in exactly one block, and the numbering follows the\n";
  cout << "closing order, so the deepest end of the graph comes first.\n";

  cout << "Cut vertices (In two or more blocks):";
  bool any = false;
  for (int v = 0; v < n; v++) {
    if (blocksHolding(v) < 2) continue;
    cout << " " << v;
    any = true;
  }
  if (!any) cout << " (None)";
  cout << "\n";
  cout << "A vertex shared by two blocks is a cut vertex, and a block made of a\n";
  cout << "single edge is a bridge. Both facts fall out of the same stack.\n";
}

static void runTrace() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  runSearch(true);

  cout << "\nDisc:";
  for (int i = 0; i < n; i++) cout << " " << disc[i];
  cout << "\nLow :";
  for (int i = 0; i < n; i++) cout << " " << low[i];
  cout << "\n";

  listBlocks();
}

static void runListOnly() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  runSearch(false);
  listBlocks();
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
      cout << "  Rejected: A self-loop is not a second route between two ends.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (matrixG[u][v]) {
      cout << "  Rejected: " << u << " - " << v << " already exists.\n";
      cout << "    Undirected edges are unordered: " << v << " - " << u
           << " is the same edge as " << u << " - " << v << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int id = addEdge(u, v);
    cout << "  Added " << edgeLabelOf(id) << "\n";
    cout << "    incident[" << u << "] gains E" << id << ", incident[" << v
         << "] gains E" << id << "\n";
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
    if (!matrixG[u][v]) {
      cout << "  Rejected: " << u << " - " << v << " is not in the graph.\n";
      cout << "    Undirected edges are unordered: " << v << " - " << u
           << " is the same edge as " << u << " - " << v << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int found = -1;
    for (int id : incident[u]) {
      if (otherEnd(id, u) == v) {
        found = id;
        break;
      }
    }

    matrixG[u][v] = false;
    matrixG[v][u] = false;
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
    matrixG[fresh][i] = false;
    matrixG[i][fresh] = false;
  }
  incident[fresh].clear();
  n++;

  cout << "\nAdded V" << fresh << " with no edges.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
  cout << "  A lone vertex carries no edge, so it belongs to no block.\n";
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

  int kept[MAXV][MAXV];
  for (int a = 0; a < n; a++) {
    for (int b = 0; b < n; b++) kept[a][b] = 0;
  }
  for (int a = 0; a < n; a++) {
    if (a == v) continue;
    for (int b = 0; b < n; b++) {
      if (b == v) continue;
      kept[(a < v) ? a : a - 1][(b < v) ? b : b - 1] = matrixG[a][b];
    }
  }

  int keptCount = 0;
  for (int id = 0; id < edgeCount; id++) {
    if (aSide[id] == v || bSide[id] == v) continue;
    aSide[keptCount] = (aSide[id] < v) ? aSide[id] : aSide[id] - 1;
    bSide[keptCount] = (bSide[id] < v) ? bSide[id] : bSide[id] - 1;
    keptCount++;
  }
  int before = edgeCount;
  edgeCount = keptCount;

  n--;
  for (int a = 0; a < MAXV; a++) {
    incident[a].clear();
    for (int b = 0; b < MAXV; b++) matrixG[a][b] = false;
  }
  for (int a = 0; a < n; a++) {
    for (int b = 0; b < n; b++) matrixG[a][b] = kept[a][b];
  }
  for (int id = 0; id < edgeCount; id++) {
    incident[aSide[id]].push_back(id);
    incident[bSide[id]].push_back(id);
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
    incident[u].clear();
    for (int w = 0; w < n; w++) matrixG[u][w] = false;
  }
  edgeCount = 0;
  edgeStack.clear();
  blockCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
  cout << "  With no edges there are no blocks at all.\n";
}

static void graphOperationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Undirected | V = " << n << ", E = " << edgeCount << "\n";
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
    cout << "\n=== G19: Biconnected Components (Blocks) ===\n";
    cout << "Graph: Undirected | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build an undirected graph\n";
    cout << "2. Load barbell demo (6 vertices)\n";
    cout << "3. Load two-triangles demo (5 vertices)\n";
    cout << "4. Load path demo (5 vertices)\n";
    cout << "5. Graph operations\n";
    cout << "6. Trace the search with the edge stack\n";
    cout << "7. List the blocks\n";
    cout << "8. Show adjacency list\n";
    cout << "9. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemo(1);
    } else if (choice == 3) {
      loadDemo(2);
    } else if (choice == 4) {
      loadDemo(3);
    } else if (choice == 5) {
      graphOperationsMenu();
    } else if (choice == 6) {
      runTrace();
    } else if (choice == 7) {
      runListOnly();
    } else if (choice == 8) {
      showAdjacency();
    } else if (choice == 9) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
