// 34d. Matching as a flow problem: Add a source S and a sink T. S sends one
// unit into each left vertex, every allowed pair has capacity one, and each
// right vertex sends one unit into T. A unit crosses exactly one pair, so the
// largest flow is exactly the largest matching.
// Demo: The reroute graph from 34a. The flow reaches the same two pairs, and
// the cut that stops it also names a vertex cover of size two.

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

const int MAXV = 15;
const int NET_MAX = 13;
const int TRACE_LIMIT = 70;

static int leftN = 0;
static int rightN = 0;
static int edgeCount = 0;
static bool edgeLR[MAXV][MAXV];

static int resM[MAXV][MAXV];
static int matchL[MAXV];
static int matchR[MAXV];
static bool coverL[MAXV];
static bool coverR[MAXV];
static int coverSize = 0;
static int lastFlow = 0;
static bool flowDone = false;
static int traceShown = 0;

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

static string plural(int count, const string &one, const string &many) {
  return to_string(count) + " " + (count == 1 ? one : many);
}

static string edgeText(int u, int v) {
  return "L" + to_string(u) + " - R" + to_string(v);
}

static void say(const string &line) {
  if (traceShown >= TRACE_LIMIT) return;
  traceShown++;
  cout << line << "\n";
}

static void clearFlow() {
  for (int i = 0; i < MAXV; i++) {
    for (int j = 0; j < MAXV; j++) resM[i][j] = 0;
    matchL[i] = -1;
    matchR[i] = -1;
    coverL[i] = false;
    coverR[i] = false;
  }
  coverSize = 0;
  lastFlow = 0;
  flowDone = false;
}

static void resetBipartite() {
  for (int i = 0; i < MAXV; i++) {
    for (int j = 0; j < MAXV; j++) edgeLR[i][j] = false;
  }
  leftN = 0;
  rightN = 0;
  edgeCount = 0;
  clearFlow();
}

static void addEdge(int u, int v) {
  edgeLR[u][v] = true;
  edgeCount++;
}

static int leftDegree(int u) {
  int total = 0;
  for (int v = 0; v < rightN; v++) {
    if (edgeLR[u][v]) total++;
  }
  return total;
}

static int rightDegree(int v) {
  int total = 0;
  for (int u = 0; u < leftN; u++) {
    if (edgeLR[u][v]) total++;
  }
  return total;
}

static int netNodes() { return leftN + rightN + 2; }

static int leftNode(int u) { return 1 + u; }

static int rightNode(int v) { return 1 + leftN + v; }

static int sinkNode() { return leftN + rightN + 1; }

static string nodeName(int x) {
  if (x == 0) return "S";
  if (x == sinkNode()) return "T";
  if (x <= leftN) return "L" + to_string(x - 1);
  return "R" + to_string(x - 1 - leftN);
}

static bool isForwardArc(int u, int v) {
  if (u == 0 && v >= 1 && v <= leftN) return true;
  if (u >= 1 && u <= leftN && v > leftN && v < sinkNode()) {
    int leftIndex = u - 1;
    int rightIndex = v - 1 - leftN;
    return edgeLR[leftIndex][rightIndex];
  }
  return v == sinkNode() && u > leftN && u < sinkNode();
}

static void buildNetwork() {
  int total = netNodes();
  for (int i = 0; i < total; i++) {
    for (int j = 0; j < total; j++) resM[i][j] = 0;
  }
  for (int u = 0; u < leftN; u++) resM[0][leftNode(u)] = 1;
  for (int u = 0; u < leftN; u++) {
    for (int v = 0; v < rightN; v++) {
      if (edgeLR[u][v]) resM[leftNode(u)][rightNode(v)] = 1;
    }
  }
  for (int v = 0; v < rightN; v++) resM[rightNode(v)][sinkNode()] = 1;
}

static bool bfsPath(int parent[], int level[]) {
  int total = netNodes();
  for (int i = 0; i < total; i++) {
    parent[i] = -2;
    level[i] = -1;
  }
  int queue[MAXV];
  int head = 0;
  int tail = 0;
  parent[0] = -1;
  level[0] = 0;
  queue[tail++] = 0;

  while (head < tail) {
    int u = queue[head++];
    for (int v = 0; v < total; v++) {
      if (resM[u][v] <= 0) continue;
      if (parent[v] != -2) continue;
      parent[v] = u;
      level[v] = level[u] + 1;
      queue[tail++] = v;
    }
  }
  return parent[sinkNode()] != -2;
}

static void traceLevels(const int level[]) {
  int total = netNodes();
  int deepest = 0;
  for (int i = 0; i < total; i++) {
    if (level[i] > deepest) deepest = level[i];
  }
  for (int d = 1; d <= deepest; d++) {
    string text;
    for (int i = 0; i < total; i++) {
      if (level[i] != d) continue;
      if (!text.empty()) text += ", ";
      text += nodeName(i);
    }
    if (!text.empty()) say("  Layer " + to_string(d) + ": " + text + ".");
  }
}

static void augmentPath(const int parent[]) {
  int path[MAXV];
  int len = 0;
  int cur = sinkNode();
  while (true) {
    path[len++] = cur;
    if (cur == 0) break;
    cur = parent[cur];
  }
  reverse(path, path + len);

  string text;
  for (int i = 0; i < len; i++) {
    if (i > 0) text += " -> ";
    text += nodeName(path[i]);
  }
  say("  Path: " + text + ".");

  int reverseCount = 0;
  for (int i = 0; i + 1 < len; i++) {
    if (!isForwardArc(path[i], path[i + 1])) reverseCount++;
    resM[path[i]][path[i + 1]]--;
    resM[path[i + 1]][path[i]]++;
  }

  if (reverseCount == 0) {
    say("  Every arc is forward, so one more pair joins the matching.");
  } else {
    say("  " + plural(reverseCount, "arc is", "arcs are") +
        " reverse, so " + plural(reverseCount, "pair is", "pairs are") +
        " rearranged.");
  }
}

static void readMatching() {
  for (int u = 0; u < leftN; u++) matchL[u] = -1;
  for (int v = 0; v < rightN; v++) matchR[v] = -1;
  for (int u = 0; u < leftN; u++) {
    for (int v = 0; v < rightN; v++) {
      if (!edgeLR[u][v]) continue;
      if (resM[leftNode(u)][rightNode(v)] != 0) continue;
      matchL[u] = v;
      matchR[v] = u;
    }
  }
}

static void runMatchingByFlow() {
  if (leftN == 0 || rightN == 0) {
    cout << "Build the bipartite graph first.\n";
    return;
  }

  clearFlow();
  buildNetwork();
  traceShown = 0;

  cout << "\n--- A flow network built from the bipartite graph ---\n";
  cout << "Network: S = node 0, L0..L" << (leftN - 1) << " = 1.." << leftN
       << ", R0..R" << (rightN - 1) << " = " << (leftN + 1) << ".."
       << (leftN + rightN) << ", T = " << sinkNode() << ".\n";
  cout << "Arcs: S to each left vertex, every allowed pair, each right to T.\n";
  cout << "Every arc has capacity one, so a left vertex passes one unit and a\n";
  cout << "right vertex accepts one unit.\n";

  cout << "\n--- Augmenting rounds (Edmonds-Karp) ---\n";
  cout << "Each BFS uses the fewest arcs left, and every unit that reaches T\n";
  cout << "grows the flow by one.\n";

  int units = 0;
  int round = 1;
  while (true) {
    int parent[MAXV];
    int level[MAXV];
    say("Round " + to_string(round) + ": BFS from S.");
    bool found = bfsPath(parent, level);
    traceLevels(level);
    if (!found) {
      say("  T is not reachable, so no augmenting path is left.");
      break;
    }
    augmentPath(parent);
    units++;
    say("  One unit arrived at T. Flow = " + to_string(units) + ".");
    round++;
  }

  lastFlow = units;
  readMatching();

  cout << "\n--- Matching taken from the flow ---\n";
  cout << "A pair carries flow 1 exactly when its arc is used.\n";
  for (int u = 0; u < leftN; u++) {
    if (matchL[u] == -1) {
      cout << "  L" << u << ": (Free)\n";
    } else {
      cout << "  L" << u << " - R" << matchL[u] << "\n";
    }
  }
  cout << "Pairs: " << units << ".\n";

  int total = netNodes();
  bool inS[MAXV] = {false};
  int queue[MAXV];
  int head = 0;
  int tail = 0;
  inS[0] = true;
  queue[tail++] = 0;
  while (head < tail) {
    int u = queue[head++];
    for (int v = 0; v < total; v++) {
      if (resM[u][v] <= 0) continue;
      if (inS[v]) continue;
      inS[v] = true;
      queue[tail++] = v;
    }
  }

  cout << "\n--- Minimum cut, read from the final residual ---\n";
  string reached;
  for (int i = 0; i < total; i++) {
    if (!inS[i]) continue;
    if (!reached.empty()) reached += ", ";
    reached += nodeName(i);
  }
  cout << "Reachable from S: " << reached << ".\n";
  cout << "No residual arc leaves that set, so it is a cut.\n";

  int cutCapacity = 0;
  string cutText;
  for (int u = 0; u < total; u++) {
    if (!inS[u]) continue;
    for (int v = 0; v < total; v++) {
      if (inS[v]) continue;
      if (!isForwardArc(u, v)) continue;
      cutCapacity++;
      if (!cutText.empty()) cutText += ", ";
      cutText += nodeName(u) + " -> " + nodeName(v);
    }
  }
  if (cutText.empty()) cutText = "(None)";
  cout << "Cut arcs: " << cutText << ". Capacity " << cutCapacity << ".\n";

  cout << "\n--- A vertex cover read from the same cut ---\n";
  cout << "The cover keeps every left vertex outside S and every right vertex\n";
  cout << "inside S. A matched pair crosses the cut, so one of its two ends\n";
  cout << "always lands in the cover.\n";

  for (int i = 0; i < MAXV; i++) {
    coverL[i] = false;
    coverR[i] = false;
  }
  coverSize = 0;
  string coverText;
  for (int u = 0; u < leftN; u++) {
    if (inS[leftNode(u)]) continue;
    coverL[u] = true;
    if (!coverText.empty()) coverText += ", ";
    coverText += "L" + to_string(u);
    coverSize++;
  }
  for (int v = 0; v < rightN; v++) {
    if (!inS[rightNode(v)]) continue;
    coverR[v] = true;
    if (!coverText.empty()) coverText += ", ";
    coverText += "R" + to_string(v);
    coverSize++;
  }
  if (coverText.empty()) coverText = "(Empty)";
  cout << "Cover: " << coverText << ".\n";

  bool covered = true;
  for (int u = 0; u < leftN; u++) {
    for (int v = 0; v < rightN; v++) {
      if (!edgeLR[u][v]) continue;
      if (coverL[u] || coverR[v]) continue;
      covered = false;
    }
  }
  if (covered) {
    cout << "Every edge touches the cover, so no edge was missed.\n";
  } else {
    cout << "An edge is uncovered, which would contradict the cut.\n";
  }

  cout << "\n--- Result ---\n";
  cout << "Max flow = " << units << ". Matching = "
       << plural(units, "pair", "pairs") << ". Cut = " << cutCapacity
       << ". Cover = " << coverSize << ".\n";
  if (cutCapacity == units && coverSize == units) {
    cout << "Max flow, matching, cut and cover all agree at " << units
         << ".\n";
  } else {
    cout << "The numbers disagree, which would mean a bug in the reduction.\n";
  }
  cout << "A unit travels S, one left vertex, one right vertex, T, so every\n";
  cout << "unit is one pair and no vertex appears in two units.\n";
  cout << "The cut and the cover match the flow in size, which is Konig's\n";
  cout << "theorem seen from the flow side.\n";
  if (traceShown >= TRACE_LIMIT) {
    cout << "(Trace stops after " << TRACE_LIMIT
         << " lines. The flow itself ran to the end.)\n";
  }
  flowDone = true;
}

static void showEdges() {
  cout << "\n--- Bipartite edges ---\n";
  if (leftN == 0 || rightN == 0 || edgeCount == 0) {
    cout << "(Empty)\n";
  } else {
    int shown = 0;
    for (int u = 0; u < leftN; u++) {
      for (int v = 0; v < rightN; v++) {
        if (!edgeLR[u][v]) continue;
        shown++;
        cout << "  E" << (shown - 1) << ": " << edgeText(u, v) << "\n";
      }
    }
  }
  cout << "Left degrees:";
  for (int u = 0; u < leftN; u++) cout << " L" << u << "=" << leftDegree(u);
  cout << ".\n";
  cout << "Right degrees:";
  for (int v = 0; v < rightN; v++) cout << " R" << v << "=" << rightDegree(v);
  cout << ".\n";
}

static void showMatching() {
  cout << "\n--- Current matching and cover ---\n";
  if (!flowDone) {
    cout << "(No flow yet)\n";
    cout << "Run the flow first, or the listing would show stale pairs.\n";
    return;
  }
  for (int u = 0; u < leftN; u++) {
    if (matchL[u] == -1) {
      cout << "  L" << u << ": (Free)\n";
    } else {
      cout << "  L" << u << " - R" << matchL[u] << "\n";
    }
  }
  string coverText;
  for (int u = 0; u < leftN; u++) {
    if (!coverL[u]) continue;
    if (!coverText.empty()) coverText += ", ";
    coverText += "L" + to_string(u);
  }
  for (int v = 0; v < rightN; v++) {
    if (!coverR[v]) continue;
    if (!coverText.empty()) coverText += ", ";
    coverText += "R" + to_string(v);
  }
  if (coverText.empty()) coverText = "(Empty)";
  cout << "Cover: " << coverText << ".\n";
  cout << "Max flow = " << lastFlow << ".\n";
}

static void buildBipartite() {
  int freshL = readRange("Number of left vertices (0.." +
                             to_string(NET_MAX) + "): ",
                         0, NET_MAX);
  int freshR = readRange("Number of right vertices (0.." +
                             to_string(NET_MAX - freshL) + "): ",
                         0, NET_MAX - freshL);
  resetBipartite();
  leftN = freshL;
  rightN = freshR;

  if (leftN == 0 || rightN == 0) {
    cout << "Build complete: " << leftN << " left, " << rightN
         << " right, 0 edges.\n";
    cout << "The flow network needs at least one vertex on each side.\n";
    return;
  }

  int limit = leftN * rightN;
  cout << "Edges join a left vertex to a right vertex, never two on one side.\n";
  cout << "A pair needs at least one vertex on each side to carry a unit.\n";

  while (true) {
    cout << "\nEdge " << (edgeCount + 1) << " (Limit " << limit
         << ", -1 to stop)\n";
    int u = readInt("  Left vertex (0.." + to_string(leftN - 1) +
                    ", -1 to stop): ");
    if (u == -1) break;
    int v = readInt("  Right vertex (0.." + to_string(rightN - 1) +
                    ", -1 to stop): ");
    if (v == -1) break;
    if (u < 0 || u >= leftN || v < 0 || v >= rightN) {
      cout << "  Rejected: Left must be in 0.." << (leftN - 1)
           << " and right in 0.." << (rightN - 1) << ".\n";
      continue;
    }
    if (edgeLR[u][v]) {
      cout << "  Rejected: " << edgeText(u, v) << " already exists.\n";
      continue;
    }
    addEdge(u, v);
    clearFlow();
    cout << "  Added E" << (edgeCount - 1) << " " << edgeText(u, v) << "\n";
    cout << "    edgeLR[" << u << "][" << v << "] = 1\n";
    cout << "  Edges = " << edgeCount << " now. L" << u << " has degree "
         << leftDegree(u) << ", R" << v << " has degree " << rightDegree(v)
         << ".\n";
  }

  cout << "\nBuild complete: " << leftN << " left, " << rightN << " right, "
       << plural(edgeCount, "edge", "edges") << ".\n";
}

static void loadRerouteDemo() {
  resetBipartite();
  leftN = 2;
  rightN = 2;
  cout << "Reroute demo: 2 left, 2 right, 3 edges.\n";
  cout << "  L0 - R0, L0 - R1, L1 - R0.\n";
  cout << "  The first unit goes L0 - R0. The second starts at L1, pushes the\n";
  cout << "  first unit back along a reverse arc, and reaches R1, so the flow\n";
  cout << "  rises to two without starting over.\n\n";

  addEdge(0, 0);
  addEdge(0, 1);
  addEdge(1, 0);
  clearFlow();

  cout << "Demo loaded: " << leftN << " left, " << rightN << " right, "
       << plural(edgeCount, "edge", "edges") << ".\n";
}

static void loadDeficientDemo() {
  resetBipartite();
  leftN = 3;
  rightN = 3;
  cout << "Deficient demo: 3 left, 3 right, 3 edges.\n";
  cout << "  L0 - R0, L1 - R0, L2 - R1.\n";
  cout << "  L0 and L1 both reach only R0, so the flow can use only one of\n";
  cout << "  them. L2 reaches R1, so the flow stops at two units.\n\n";

  addEdge(0, 0);
  addEdge(1, 0);
  addEdge(2, 1);
  clearFlow();

  cout << "Demo loaded: " << leftN << " left, " << rightN << " right, "
       << plural(edgeCount, "edge", "edges") << ".\n";
}

static bool noSides() { return leftN == 0 || rightN == 0; }

static void addEdgeInteractive() {
  if (noSides()) {
    cout << "Add at least one left and one right vertex first.\n";
    return;
  }
  if (edgeCount == leftN * rightN) {
    cout << "Every left-right pair already has an edge.\n";
    return;
  }

  cout << "\nAdd an edge (L = " << leftN << ", R = " << rightN << ")\n";
  while (true) {
    int u = readInt("  Left vertex (0.." + to_string(leftN - 1) +
                    ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  Right vertex (0.." + to_string(rightN - 1) +
                    ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (u < 0 || u >= leftN || v < 0 || v >= rightN) {
      cout << "  Rejected: Left must be in 0.." << (leftN - 1)
           << " and right in 0.." << (rightN - 1) << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (edgeLR[u][v]) {
      cout << "  Rejected: " << edgeText(u, v) << " already exists.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    addEdge(u, v);
    clearFlow();
    cout << "  Added E" << (edgeCount - 1) << " " << edgeText(u, v) << "\n";
    cout << "    edgeLR[" << u << "][" << v << "] = 1\n";
    cout << "  Edges = " << edgeCount << " now. Degree of L" << u << " is "
         << leftDegree(u) << ", degree of R" << v << " is " << rightDegree(v)
         << ".\n";
    return;
  }
}

static void removeEdgeInteractive() {
  if (noSides()) {
    cout << "Build the bipartite graph first.\n";
    return;
  }

  cout << "\nRemove an edge (L = " << leftN << ", R = " << rightN << ")\n";
  while (true) {
    int u = readInt("  Left vertex (0.." + to_string(leftN - 1) +
                    ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  Right vertex (0.." + to_string(rightN - 1) +
                    ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (u < 0 || u >= leftN || v < 0 || v >= rightN) {
      cout << "  Rejected: Left must be in 0.." << (leftN - 1)
           << " and right in 0.." << (rightN - 1) << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (!edgeLR[u][v]) {
      cout << "  Rejected: " << edgeText(u, v) << " is not in the graph.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    edgeLR[u][v] = false;
    edgeCount--;
    clearFlow();
    cout << "  Removed " << edgeText(u, v) << "\n";
    cout << "  Edges = " << edgeCount << " now. Degree of L" << u << " is "
         << leftDegree(u) << ", degree of R" << v << " is " << rightDegree(v)
         << ".\n";
    return;
  }
}

static void addLeftInteractive() {
  if (leftN + rightN >= NET_MAX) {
    cout << "Vertex limit reached: The network needs S and T as well, so the\n";
    cout << "two sides together hold at most " << NET_MAX
         << " vertices.\n";
    return;
  }

  int fresh = leftN;
  for (int v = 0; v < rightN; v++) edgeLR[fresh][v] = false;
  leftN++;
  clearFlow();

  cout << "\nAdded L" << fresh << " with no edges.\n";
  cout << "  L = " << leftN << ", R = " << rightN << ", edges = " << edgeCount
       << ".\n";
  cout << "  Without an edge this left vertex cannot send a unit anywhere.\n";
}

static void addRightInteractive() {
  if (leftN + rightN >= NET_MAX) {
    cout << "Vertex limit reached: The network needs S and T as well, so the\n";
    cout << "two sides together hold at most " << NET_MAX
         << " vertices.\n";
    return;
  }

  int fresh = rightN;
  for (int u = 0; u < leftN; u++) edgeLR[u][fresh] = false;
  rightN++;
  clearFlow();

  cout << "\nAdded R" << fresh << " with no edges.\n";
  cout << "  L = " << leftN << ", R = " << rightN << ", edges = " << edgeCount
       << ".\n";
  cout << "  Without an edge this right vertex accepts no unit.\n";
}

static void removeLastLeft() {
  if (leftN == 0) {
    cout << "There is no left vertex to remove.\n";
    return;
  }

  int gone = leftN - 1;
  for (int v = 0; v < rightN; v++) {
    if (edgeLR[gone][v]) edgeCount--;
    edgeLR[gone][v] = false;
  }
  leftN--;
  clearFlow();

  cout << "\nRemoved L" << gone << " and every edge from it.\n";
  cout << "  L = " << leftN << ", R = " << rightN << ", edges = " << edgeCount
       << ".\n";
}

static void removeLastRight() {
  if (rightN == 0) {
    cout << "There is no right vertex to remove.\n";
    return;
  }

  int gone = rightN - 1;
  for (int u = 0; u < leftN; u++) {
    if (edgeLR[u][gone]) edgeCount--;
    edgeLR[u][gone] = false;
  }
  rightN--;
  clearFlow();

  cout << "\nRemoved R" << gone << " and every edge into it.\n";
  cout << "  L = " << leftN << ", R = " << rightN << ", edges = " << edgeCount
       << ".\n";
}

static void clearEdges() {
  if (noSides()) {
    cout << "Build the bipartite graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nThe graph already has no edges.\n";
    return;
  }

  int before = edgeCount;
  for (int u = 0; u < leftN; u++) {
    for (int v = 0; v < rightN; v++) edgeLR[u][v] = false;
  }
  edgeCount = 0;
  clearFlow();

  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  L = " << leftN << ", R = " << rightN
       << ". The vertices are still there.\n";
  cout << "  With no edges the source reaches no right vertex, so the flow\n";
  cout << "  and the matching are both zero.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Bipartite operations ---\n";
    cout << "Graph: Bipartite | L = " << leftN << ", R = " << rightN
         << ", E = " << edgeCount << "\n";
    cout << "1. Add an edge\n";
    cout << "2. Remove an edge\n";
    cout << "3. Add a left vertex\n";
    cout << "4. Add a right vertex\n";
    cout << "5. Remove the last left vertex\n";
    cout << "6. Remove the last right vertex\n";
    cout << "7. Clear all edges\n";
    cout << "0. Back to the main menu\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      addEdgeInteractive();
    } else if (choice == 2) {
      removeEdgeInteractive();
    } else if (choice == 3) {
      addLeftInteractive();
    } else if (choice == 4) {
      addRightInteractive();
    } else if (choice == 5) {
      removeLastLeft();
    } else if (choice == 6) {
      removeLastRight();
    } else if (choice == 7) {
      clearEdges();
    } else if (choice == 0) {
      return;
    } else {
      cout << "Invalid option.\n";
    }
  }
}

int main() {
  resetBipartite();

  while (true) {
    cout << "\n=== G53: Matching by Maximum Flow ===\n";
    cout << "Graph: Bipartite | L = " << leftN << ", R = " << rightN
         << ", E = " << edgeCount << "\n";
    cout << "1. Build a bipartite graph\n";
    cout << "2. Load the reroute demo (2 left, 2 right)\n";
    cout << "3. Load the deficient demo (3 left, 3 right)\n";
    cout << "4. Graph operations\n";
    cout << "5. Run the flow and read the matching\n";
    cout << "6. Show the bipartite edges\n";
    cout << "7. Show the current matching and cover\n";
    cout << "8. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildBipartite();
    } else if (choice == 2) {
      loadRerouteDemo();
    } else if (choice == 3) {
      loadDeficientDemo();
    } else if (choice == 4) {
      operationsMenu();
    } else if (choice == 5) {
      runMatchingByFlow();
    } else if (choice == 6) {
      showEdges();
    } else if (choice == 7) {
      showMatching();
    } else if (choice == 8) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
