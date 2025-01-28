// 34b. Bipartite matching, found by Hopcroft-Karp: One breadth-first pass
// layers the left vertices by how many matched edges an augmenting path has
// used, and one depth-first pass flips every disjoint shortest path it can
// find. Demo: The same rerouting graph finishes in one phase, while the
// deficient graph needs two.

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;
const int TRACE_LIMIT = 70;

static int leftN = 0;
static int rightN = 0;
static int edgeCount = 0;
static bool edgeLR[MAXV][MAXV];

static int matchL[MAXV];
static int matchR[MAXV];
static int distL[MAXV];
static int walkL[MAXV];
static int walkR[MAXV];
static int shortest = 0;
static bool matchDone = false;
static int phases = 0;
static int traceShown = 0;

static const int INF = 1000000;

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

static void resetBipartite() {
  for (int i = 0; i < MAXV; i++) {
    for (int j = 0; j < MAXV; j++) edgeLR[i][j] = false;
  }
  leftN = 0;
  rightN = 0;
  edgeCount = 0;
  matchDone = false;
}

static void clearMatching() {
  for (int i = 0; i < MAXV; i++) {
    matchL[i] = -1;
    matchR[i] = -1;
  }
  matchDone = false;
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

static bool isPerfect() {
  return leftN == rightN && leftN > 0;
}

static bool bfsLayers() {
  int queue[MAXV];
  int head = 0;
  int tail = 0;

  for (int u = 0; u < leftN; u++) {
    if (matchL[u] == -1) {
      distL[u] = 0;
      queue[tail++] = u;
    } else {
      distL[u] = INF;
    }
  }

  shortest = INF;
  while (head < tail) {
    int u = queue[head++];
    if (distL[u] >= shortest) continue;
    for (int v = 0; v < rightN; v++) {
      if (!edgeLR[u][v]) continue;
      int w = matchR[v];
      if (w == -1) {
        if (distL[u] < shortest) shortest = distL[u];
      } else if (distL[w] == INF) {
        distL[w] = distL[u] + 1;
        queue[tail++] = w;
      }
    }
  }

  string text;
  for (int u = 0; u < leftN; u++) {
    if (text.size() > 0) text += ", ";
    text += "L" + to_string(u);
    if (distL[u] == INF) text += " unreached";
    else text += " at " + to_string(distL[u]);
  }
  say("  Layers: " + text + ".");
  if (shortest == INF) {
    say("  No free right vertex is reachable, so no path is left.");
    return false;
  }
  say("  The shortest path leaves a left vertex at layer " + to_string(shortest) +
      ".");
  return true;
}

static string walkText(int depth) {
  string text = "L" + to_string(walkL[0]);
  for (int i = 0; i <= depth; i++) {
    text += " - R" + to_string(walkR[i]);
    if (i < depth) text += " - L" + to_string(walkL[i + 1]);
  }
  return text;
}

static bool dfsHopcroft(int u, int depth) {
  walkL[depth] = u;
  for (int v = 0; v < rightN; v++) {
    if (!edgeLR[u][v]) continue;
    int w = matchR[v];
    if (w == -1) {
      if (distL[u] != shortest) continue;
      walkR[depth] = v;
      matchL[u] = v;
      matchR[v] = u;
      say("    Path " + walkText(depth) + ".");
      return true;
    }
    if (distL[w] != distL[u] + 1) continue;
    walkR[depth] = v;
    if (dfsHopcroft(w, depth + 1)) {
      matchL[u] = v;
      matchR[v] = u;
      return true;
    }
  }
  distL[u] = INF;
  return false;
}

static bool kuhnSilent(int u, bool seen[], int mL[], int mR[]) {
  for (int v = 0; v < rightN; v++) {
    if (!edgeLR[u][v] || seen[v]) continue;
    seen[v] = true;
    if (mR[v] == -1 || kuhnSilent(mR[v], seen, mL, mR)) {
      mL[u] = v;
      mR[v] = u;
      return true;
    }
  }
  return false;
}

static int countKuhn() {
  int mL[MAXV];
  int mR[MAXV];
  for (int i = 0; i < MAXV; i++) {
    mL[i] = -1;
    mR[i] = -1;
  }
  int found = 0;
  for (int u = 0; u < leftN; u++) {
    bool seen[MAXV];
    for (int v = 0; v < MAXV; v++) seen[v] = false;
    if (kuhnSilent(u, seen, mL, mR)) found++;
  }
  return found;
}

static void runHopcroftKarp() {
  if (leftN == 0 || rightN == 0) {
    cout << "Build the bipartite graph first.\n";
    return;
  }

  clearMatching();
  phases = 0;
  traceShown = 0;

  cout << "\n--- Hopcroft-Karp ---\n";
  cout << "A phase has two passes. The breadth-first pass gives every free\n";
  cout << "left vertex layer 0, follows matched edges to layer 1, then follows\n";
  cout << "the next matched edge to layer 2, and so on. The depth-first pass\n";
  cout << "flips only paths that start at layer 0 and end at the shortest\n";
  cout << "reachable free right vertex, so all paths in one phase are disjoint.\n";

  int matched = 0;
  int paths = 0;
  while (true) {
    say("BFS pass.");
    if (!bfsLayers()) {
      cout << "No phase is left: The last BFS found no free right vertex.\n";
      break;
    }
    phases++;
    int before = matched;

    for (int u = 0; u < leftN; u++) {
      if (matchL[u] != -1) continue;
      if (distL[u] != 0) continue;
      if (dfsHopcroft(u, 0)) {
        matched++;
        paths++;
      }
    }

    cout << "Phase " << phases << ": "
         << plural(matched - before, "more pair", "more pairs") << ".\n";
    if (matched == before) break;
  }

  cout << "\n--- Matching ---\n";
  for (int u = 0; u < leftN; u++) {
    if (matchL[u] == -1) {
      cout << "  L" << u << ": (Free)\n";
    } else {
      cout << "  L" << u << " - R" << matchL[u] << "\n";
    }
  }

  cout << "\nPairs: " << matched << ".\n";
  if (isPerfect() && matched == leftN) {
    cout << "Every vertex is paired, so this is a perfect matching.\n";
  } else {
    cout << "This is a maximum matching. No phase found another free right\n";
    cout << "vertex, so no larger matching exists.\n";
    cout << "It is not perfect: " << leftN << " left and " << rightN
         << " right, but only " << matched << " pairs.\n";
  }

  int kuhnSearches = countKuhn();
  cout << "\nKuhn needs " << plural(kuhnSearches, "separate search",
                                    "separate searches")
       << " to find the same " << plural(paths, "pair", "pairs") << ".\n";
  cout << "Hopcroft-Karp used " << plural(phases, "phase", "phases") << " for "
       << plural(paths, "path", "paths") << ".\n";
  cout << "A phase can settle several pairs with one BFS, and the shortest\n";
  cout << "paths keep the number of phases near the square root of the graph.\n";
  if (traceShown >= TRACE_LIMIT) {
    cout << "(Trace stops after " << TRACE_LIMIT
         << " lines. The search itself ran to the end.)\n";
  }
  matchDone = true;
}

static void showEdges() {
  cout << "\n--- Bipartite edges ---\n";
  if (leftN == 0 || rightN == 0) {
    cout << "(Empty)\n";
    return;
  }
  if (edgeCount == 0) {
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
  cout << "\n--- Current matching ---\n";
  if (!matchDone) {
    cout << "(No matching yet)\n";
    cout << "Run Hopcroft-Karp first, or the listing would show stale pairs.\n";
    return;
  }
  for (int u = 0; u < leftN; u++) {
    if (matchL[u] == -1) {
      cout << "  L" << u << ": (Free)\n";
    } else {
      cout << "  L" << u << " - R" << matchL[u] << "\n";
    }
  }
}

static void buildBipartite() {
  int freshL = readRange("Number of left vertices (0.." +
                             to_string(MAXV) + "): ",
                         0, MAXV);
  int freshR = readRange("Number of right vertices (0.." +
                             to_string(MAXV - freshL) + "): ",
                         0, MAXV - freshL);
  resetBipartite();
  leftN = freshL;
  rightN = freshR;

  if (leftN == 0 || rightN == 0) {
    cout << "Build complete: " << leftN << " left, " << rightN
         << " right, 0 edges.\n";
    cout << "A matching needs at least one vertex on each side.\n";
    return;
  }

  int limit = leftN * rightN;
  cout << "Edges join a left vertex to a right vertex, never two on one side.\n";

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
    matchDone = false;
    cout << "  Added E" << (edgeCount - 1) << " " << edgeText(u, v) << "\n";
    cout << "    edgeLR[" << u << "][" << v << "] = 1\n";
    cout << "  Edges = " << edgeCount << " now. L" << u << " has degree "
         << leftDegree(u) << ", R" << v << " has degree " << rightDegree(v)
         << ".\n";
  }

  cout << "\nBuild complete: " << leftN << " left, " << rightN << " right, "
       << edgeCount << (edgeCount == 1 ? " edge.\n" : " edges.\n");
}

static void loadRerouteDemo() {
  resetBipartite();
  leftN = 2;
  rightN = 2;
  cout << "Reroute demo: 2 left, 2 right, 3 edges.\n";
  cout << "  L0 - R0, L0 - R1, L1 - R0.\n";
  cout << "  L0 takes R0 in phase 1. Phase 2 moves L0 to R1 and gives R0 to\n";
  cout << "  L1, so both phases stay short.\n\n";

  addEdge(0, 0);
  addEdge(0, 1);
  addEdge(1, 0);
  clearMatching();

  cout << "Demo loaded: " << leftN << " left, " << rightN << " right, "
       << edgeCount << " edges.\n";
}

static void loadDeficientDemo() {
  resetBipartite();
  leftN = 3;
  rightN = 3;
  cout << "Deficient demo: 3 left, 3 right, 3 edges.\n";
  cout << "  L0 - R0, L1 - R0, L2 - R1.\n";
  cout << "  L0 and L1 both reach only R0, so one of them must stay free. One\n";
  cout << "  phase settles the matching, and the next BFS proves no path is\n";
  cout << "  left.\n\n";

  addEdge(0, 0);
  addEdge(1, 0);
  addEdge(2, 1);
  clearMatching();

  cout << "Demo loaded: " << leftN << " left, " << rightN << " right, "
       << edgeCount << " edges.\n";
}

static void loadFullDemo() {
  resetBipartite();
  leftN = 3;
  rightN = 3;
  cout << "Full demo: 3 left, 3 right, 9 edges.\n";
  cout << "  Every left vertex reaches every right vertex.\n";
  cout << "  One phase gives all three pairs, because every free right vertex\n";
  cout << "  sits at layer 0 and the depth-first pass never has to reroute.\n\n";

  for (int u = 0; u < leftN; u++) {
    for (int v = 0; v < rightN; v++) addEdge(u, v);
  }
  clearMatching();

  cout << "Demo loaded: " << leftN << " left, " << rightN << " right, "
       << edgeCount << " edges.\n";
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
    matchDone = false;
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
    clearMatching();
    cout << "  Removed " << edgeText(u, v) << "\n";
    cout << "    edgeLR[" << u << "][" << v << "] = 0\n";
    cout << "  Edges = " << edgeCount << " now. Degree of L" << u << " is "
         << leftDegree(u) << ", degree of R" << v << " is " << rightDegree(v)
         << ".\n";
    return;
  }
}

static void addLeftInteractive() {
  if (leftN + rightN >= MAXV) {
    cout << "Vertex limit reached: MAXV = " << MAXV << " in total.\n";
    return;
  }
  int fresh = leftN;
  for (int v = 0; v < MAXV; v++) edgeLR[fresh][v] = false;
  leftN++;
  clearMatching();
  cout << "\nAdded L" << fresh << " with no edges.\n";
  cout << "  Left = " << leftN << ", right = " << rightN << ", edges = "
       << edgeCount << ".\n";
  cout << "  A new left vertex starts free, and the next run can pair it.\n";
}

static void addRightInteractive() {
  if (leftN + rightN >= MAXV) {
    cout << "Vertex limit reached: MAXV = " << MAXV << " in total.\n";
    return;
  }
  int fresh = rightN;
  for (int u = 0; u < MAXV; u++) edgeLR[u][fresh] = false;
  rightN++;
  clearMatching();
  cout << "\nAdded R" << fresh << " with no edges.\n";
  cout << "  Left = " << leftN << ", right = " << rightN << ", edges = "
       << edgeCount << ".\n";
  cout << "  A new right vertex starts free, and the next run can pair it.\n";
}

static void removeLastLeft() {
  if (leftN == 0) {
    cout << "There is no left vertex to remove.\n";
    return;
  }
  int lost = 0;
  for (int v = 0; v < rightN; v++) {
    if (edgeLR[leftN - 1][v]) lost++;
  }
  leftN--;
  edgeCount -= lost;
  clearMatching();
  cout << "\nRemoved L" << leftN << ", the last left vertex.\n";
  cout << "  Edges lost: " << lost << ". Left = " << leftN << ", right = "
       << rightN << ", edges = " << edgeCount << ".\n";
}

static void removeLastRight() {
  if (rightN == 0) {
    cout << "There is no right vertex to remove.\n";
    return;
  }
  int lost = 0;
  for (int u = 0; u < leftN; u++) {
    if (edgeLR[u][rightN - 1]) lost++;
  }
  rightN--;
  edgeCount -= lost;
  clearMatching();
  cout << "\nRemoved R" << rightN << ", the last right vertex.\n";
  cout << "  Edges lost: " << lost << ". Left = " << leftN << ", right = "
       << rightN << ", edges = " << edgeCount << ".\n";
}

static void clearEdges() {
  if (leftN == 0 && rightN == 0) {
    cout << "Build the bipartite graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nThe graph already has no edges.\n";
    return;
  }
  int before = edgeCount;
  for (int u = 0; u < MAXV; u++) {
    for (int v = 0; v < MAXV; v++) edgeLR[u][v] = false;
  }
  edgeCount = 0;
  clearMatching();
  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  Left = " << leftN << ", right = " << rightN
       << ", edges = 0. The vertices are still there.\n";
  cout << "  With no edges every vertex is free.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Bipartite operations ---\n";
    cout << "Graph: Bipartite | L = " << leftN << ", R = " << rightN
         << ", edges = " << edgeCount << "\n";
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
    cout << "\n=== G49: Bipartite Matching (Hopcroft-Karp) ===\n";
    cout << "Graph: Bipartite | L = " << leftN << ", R = " << rightN
         << ", edges = " << edgeCount << "\n";
    cout << "1. Build a bipartite graph\n";
    cout << "2. Load the reroute demo (2 + 2 vertices)\n";
    cout << "3. Load the deficient demo (3 + 3 vertices)\n";
    cout << "4. Load the full demo (3 + 3 vertices)\n";
    cout << "5. Bipartite operations\n";
    cout << "6. Run Hopcroft-Karp\n";
    cout << "7. Show the edge list\n";
    cout << "8. Show the current matching\n";
    cout << "9. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildBipartite();
    } else if (choice == 2) {
      loadRerouteDemo();
    } else if (choice == 3) {
      loadDeficientDemo();
    } else if (choice == 4) {
      loadFullDemo();
    } else if (choice == 5) {
      operationsMenu();
    } else if (choice == 6) {
      runHopcroftKarp();
    } else if (choice == 7) {
      showEdges();
    } else if (choice == 8) {
      showMatching();
    } else if (choice == 9) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
