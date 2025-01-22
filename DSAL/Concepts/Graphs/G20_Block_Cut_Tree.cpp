// 15b. Block-cut tree: Shrink every block to one node and every cut vertex to
// another, and the blocks of a connected graph fall into a tree.
// Demo: A barbell turns into a path of block, cut vertex, block, cut vertex,
// block, so removing a cut vertex really does break the tree in two.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;
const int MAXE = 120;
const int MAXNODE = MAXE + 1 + MAXV;

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

static bool isCut[MAXV];
static vector<int> bctAdj[MAXNODE];
static bool bctSeen[MAXNODE];
static int bctNodes = 0;

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
  bctNodes = 0;
  for (int i = 0; i <= MAXE; i++) blockEdges[i].clear();
  for (int i = 0; i < MAXNODE; i++) bctAdj[i].clear();
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
    cout << "  Expect a path: Block, V2, Block, V3, Block.\n\n";
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
    cout << "  Expect a tiny tree: Block, V0, Block.\n\n";
    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(2, 0);
    addEdge(0, 3);
    addEdge(3, 4);
    addEdge(4, 0);
  } else if (which == 3) {
    n = 5;
    cout << "Ring demo: 5 vertices.\n";
    cout << "  One cycle 0 - 1 - 2 - 3 - 4 - 0 and nothing else.\n";
    cout << "  One block, no cut vertex, so the tree is a single node.\n\n";
    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(2, 3);
    addEdge(3, 4);
    addEdge(4, 0);
  } else {
    n = 4;
    cout << "Star demo: 4 vertices.\n";
    cout << "  Centre V0 with leaves 1, 2 and 3.\n";
    cout << "  Expect a star tree: One block per leaf, all hanging off V0.\n\n";
    addEdge(0, 1);
    addEdge(0, 2);
    addEdge(0, 3);
  }

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
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

static void resetSearch() {
  for (int i = 0; i < MAXV; i++) {
    disc[i] = -1;
    low[i] = -1;
    seen[i] = false;
    isCut[i] = false;
  }
  timerStep = 0;
  edgeStack.clear();
  blockCount = 0;
  for (int i = 0; i <= MAXE; i++) blockEdges[i].clear();
}

static void closeBlock(int stopEdge) {
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
}

static void dfs(int u, int parentEdge) {
  seen[u] = true;
  disc[u] = timerStep;
  low[u] = disc[u];
  timerStep++;

  for (int id : incident[u]) {
    int v = otherEnd(id, u);
    if (id == parentEdge) continue;

    if (!seen[v]) {
      edgeStack.push_back(id);
      dfs(v, id);
      if (low[v] < low[u]) low[u] = low[v];
      if (low[v] >= disc[u]) closeBlock(id);
    } else if (disc[v] < disc[u]) {
      edgeStack.push_back(id);
      if (disc[v] < low[u]) low[u] = disc[v];
    }
  }
}

static void extractBlocks() {
  resetSearch();
  for (int start = 0; start < n; start++) {
    if (seen[start]) continue;
    dfs(start, -1);
  }
  for (int v = 0; v < n; v++) {
    isCut[v] = (blocksHolding(v) >= 2);
  }
}

static void listBlocks() {
  cout << "\n--- Blocks ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "No blocks. A block needs at least one edge.\n";
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
  }

  cout << "Cut vertices (In two or more blocks):";
  bool any = false;
  for (int v = 0; v < n; v++) {
    if (!isCut[v]) continue;
    cout << " " << v;
    any = true;
  }
  if (!any) cout << " (None)";
  cout << "\n";
  cout << "Blocks: " << blockCount << ", one per stack burst.\n";
  cout << "The numbering follows the closing order, so the deepest end of the\n";
  cout << "graph comes first.\n";
}

static bool isBlockNode(int id) { return id < blockCount; }

static string nodeName(int id) {
  if (isBlockNode(id)) return "B" + to_string(id + 1);
  return "V" + to_string(id - blockCount);
}

static void link(int a, int b) {
  bctAdj[a].push_back(b);
  bctAdj[b].push_back(a);
}

static int bctEdgeCount() {
  int total = 0;
  for (int i = 0; i < bctNodes; i++) total += (int)bctAdj[i].size();
  return total / 2;
}

static void buildBlockCutTree() {
  bctNodes = blockCount + n;
  for (int i = 0; i < MAXNODE; i++) bctAdj[i].clear();

  cout << "\n--- Building the tree ---\n";
  for (int b = 0; b < blockCount; b++) {
    bool met = false;
    for (int v = 0; v < n; v++) {
      if (!vertexInBlock(b, v)) continue;
      if (!isCut[v]) continue;
      link(b, blockCount + v);
      if (!met) {
        cout << "Block B" << (b + 1) << " meets cut vertex";
        met = true;
      } else {
        cout << ",";
      }
      cout << " V" << v;
    }
    if (met) cout << ".\n";
    else cout << "Block B" << (b + 1) << " holds no cut vertex.\n";
  }
  cout << "A vertex in two or more blocks is the only kind that gets a node,\n";
  cout << "because the blocks already carry every other vertex.\n";
}

static void printTreeNode(int u, int parent, int depth) {
  string pad(depth * 2, ' ');
  bctSeen[u] = true;
  cout << pad << nodeName(u);
  if (isBlockNode(u)) {
    int m = (int)blockEdges[u].size();
    cout << " (" << m << (m == 1 ? " edge)" : " edges)");
  } else {
    cout << " (In " << blocksHolding(u - blockCount) << " blocks)";
  }
  cout << "\n";

  for (int w : bctAdj[u]) {
    if (w == parent) continue;
    printTreeNode(w, u, depth + 1);
  }
}

static void showBlockCutTree() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "No blocks, so there is no tree to build.\n";
    return;
  }

  extractBlocks();
  listBlocks();
  buildBlockCutTree();

  int edges = bctEdgeCount();
  int cuts = 0;
  for (int v = 0; v < n; v++) {
    if (isCut[v]) cuts++;
  }

  cout << "\n--- Block-Cut Tree ---\n";
  cout << "Nodes: " << blockCount
       << (blockCount == 1 ? " block + " : " blocks + ") << cuts
       << (cuts == 1 ? " cut vertex = " : " cut vertices = ")
       << (blockCount + cuts) << ".\n";
  cout << "Edges: " << edges << ".\n";
  if (edges == blockCount + cuts - 1) {
    cout << "Edges = nodes - 1, so the whole graph became one tree.\n";
  } else {
    cout << "Edges < nodes - 1, so the graph became a forest of "
         << (blockCount + cuts - edges) << " trees, one per component.\n";
  }
  cout << "\n";

  for (int i = 0; i < bctNodes; i++) bctSeen[i] = false;
  int trees = 0;
  for (int b = 0; b < blockCount; b++) {
    if (bctSeen[b]) continue;
    if (trees > 0) cout << "\n";
    printTreeNode(b, -1, 0);
    trees++;
  }

  cout << "\nBlocks hang at the leaves and cut vertices sit between them, so\n";
  cout << "deleting a cut vertex splits the tree into its own branches.\n";
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
  cout << "  A lone vertex belongs to no block, so it adds no tree node.\n";
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
  bctNodes = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
  cout << "  With no edges there are no blocks, so the tree is empty too.\n";
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
    cout << "\n=== G20: Block-Cut Tree ===\n";
    cout << "Graph: Undirected | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build an undirected graph\n";
    cout << "2. Load barbell demo (6 vertices)\n";
    cout << "3. Load two-triangles demo (5 vertices)\n";
    cout << "4. Load ring demo (5 vertices)\n";
    cout << "5. Load star demo (4 vertices)\n";
    cout << "6. Graph operations\n";
    cout << "7. List the blocks\n";
    cout << "8. Build and show the block-cut tree\n";
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
      loadDemo(4);
    } else if (choice == 6) {
      graphOperationsMenu();
    } else if (choice == 7) {
      if (n == 0) {
        cout << "Build the graph first.\n";
      } else {
        extractBlocks();
        listBlocks();
      }
    } else if (choice == 8) {
      showBlockCutTree();
    } else if (choice == 9) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
