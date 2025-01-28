// 34a. Bipartite matching, found by Kuhn's algorithm: Take each left vertex
// in turn and look for an augmenting path, which may ask an already matched
// left vertex to move to another right vertex. Demo: A two-by-two graph where
// the second left vertex must push the first one aside to reach a full
// matching.

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
static bool matchDone = false;
static int tries = 0;
static int calls = 0;
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

static string pad(int depth) { return string(depth * 2, ' '); }

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

static bool tryKuhn(int u, int depth, bool seen[]) {
  calls++;
  for (int v = 0; v < rightN; v++) {
    if (!edgeLR[u][v]) continue;
    if (seen[v]) {
      say(pad(depth) + "Edge " + edgeText(u, v) + ": R" + to_string(v) +
          " was already tried this round.");
      continue;
    }
    seen[v] = true;
    tries++;

    say(pad(depth) + "Try " + edgeText(u, v) + ".");
    if (matchR[v] == -1) {
      matchL[u] = v;
      matchR[v] = u;
      say(pad(depth) + "R" + to_string(v) + " is free, so L" + to_string(u) +
          " takes it.");
      return true;
    }

    int other = matchR[v];
    say(pad(depth) + "R" + to_string(v) + " is held by L" + to_string(other) +
        ", so ask it to move.");
    if (tryKuhn(other, depth + 1, seen)) {
      matchL[u] = v;
      matchR[v] = u;
      say(pad(depth) + "L" + to_string(other) + " moved, so L" +
          to_string(u) + " takes R" + to_string(v) + ".");
      return true;
    }
    say(pad(depth) + "L" + to_string(other) + " cannot move.");
  }
  return false;
}

static bool isPerfect() {
  return leftN == rightN && leftN > 0;
}

static void runKuhn() {
  if (leftN == 0 || rightN == 0) {
    cout << "Build the bipartite graph first.\n";
    return;
  }

  clearMatching();
  tries = 0;
  calls = 0;
  traceShown = 0;

  cout << "\n--- Kuhn from the left side ---\n";
  cout << "Each left vertex is handled once. It looks for a right vertex that\n";
  cout << "is free, or one whose partner can move elsewhere, and that chain is\n";
  cout << "an augmenting path. If the chain ends at a free right vertex, every\n";
  cout << "pair along it flips and the matching grows by one.\n";

  int matched = 0;
  int paths = 0;
  for (int u = 0; u < leftN; u++) {
    bool seen[MAXV];
    for (int v = 0; v < MAXV; v++) seen[v] = false;
    say("Start from L" + to_string(u) + ".");
    if (tryKuhn(u, 0, seen)) {
      matched++;
      paths++;
      say("L" + to_string(u) + " joined the matching.");
    } else {
      say("L" + to_string(u) + " stays free: No augmenting path starts here.");
    }
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
    cout << "This is a maximum matching. Kuhn tried every left vertex and never\n";
    cout << "found another free right vertex, so no larger matching exists.\n";
    cout << "It is not perfect: " << leftN << " left and " << rightN
         << " right, but only " << matched << " pairs.\n";
  }

  vector<int> freeL;
  vector<int> freeR;
  for (int u = 0; u < leftN; u++) {
    if (matchL[u] == -1) freeL.push_back(u);
  }
  for (int v = 0; v < rightN; v++) {
    if (matchR[v] == -1) freeR.push_back(v);
  }
  if (freeL.size() > 0) {
    cout << "Free left:";
    for (int u : freeL) cout << " L" << u;
    cout << ".\n";
  }
  if (freeR.size() > 0) {
    cout << "Free right:";
    for (int v : freeR) cout << " R" << v;
    cout << ".\n";
  }

  cout << "Edge attempts: " << tries << ". Recursive calls: " << calls
       << ".\n";
  cout << "Augmenting paths used: " << plural(paths, "path", "paths") << ".\n";
  cout << "A failed search repeats work, because each left vertex starts a new\n";
  cout << "walk. The next file removes that repetition with a level graph.\n";
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
    cout << "Run Kuhn first, or the listing would show stale pairs.\n";
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
  cout << "  L0 takes R0 first. L1 then asks L0 to move to R1, and the\n";
  cout << "  matching reaches two pairs without starting over.\n\n";

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
  cout << "  L0 and L1 both reach only R0, so one of them must stay free. No\n";
  cout << "  matching can pair all three left vertices.\n\n";

  addEdge(0, 0);
  addEdge(1, 0);
  addEdge(2, 1);
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
    cout << "\n=== G48: Bipartite Matching (Kuhn) ===\n";
    cout << "Graph: Bipartite | L = " << leftN << ", R = " << rightN
         << ", edges = " << edgeCount << "\n";
    cout << "1. Build a bipartite graph\n";
    cout << "2. Load the reroute demo (2 + 2 vertices)\n";
    cout << "3. Load the deficient demo (3 + 3 vertices)\n";
    cout << "4. Bipartite operations\n";
    cout << "5. Run Kuhn\n";
    cout << "6. Show the edge list\n";
    cout << "7. Show the current matching\n";
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
      runKuhn();
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
