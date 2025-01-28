// 34c. Konig's theorem, seen through a minimum vertex cover: Find a maximum
// matching, then grow Z from the free left vertices along unmatched edges and
// back along matched ones. The cover is the left vertices outside Z together
// with the right vertices inside Z. Demo: A deficient graph where the cover
// has two vertices while the matching has two pairs.

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
static bool inZL[MAXV];
static bool inZR[MAXV];
static bool resultDone = false;
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
  resultDone = false;
}

static void clearResult() {
  for (int i = 0; i < MAXV; i++) {
    matchL[i] = -1;
    matchR[i] = -1;
    inZL[i] = false;
    inZR[i] = false;
  }
  resultDone = false;
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

static bool tryKuhn(int u, bool seen[]) {
  for (int v = 0; v < rightN; v++) {
    if (!edgeLR[u][v] || seen[v]) continue;
    seen[v] = true;
    if (matchR[v] == -1 || tryKuhn(matchR[v], seen)) {
      matchL[u] = v;
      matchR[v] = u;
      return true;
    }
  }
  return false;
}

static int findMatching() {
  for (int i = 0; i < MAXV; i++) {
    matchL[i] = -1;
    matchR[i] = -1;
  }
  int matched = 0;
  for (int u = 0; u < leftN; u++) {
    bool seen[MAXV];
    for (int v = 0; v < MAXV; v++) seen[v] = false;
    if (tryKuhn(u, seen)) matched++;
  }
  return matched;
}

static void runKonig() {
  if (leftN == 0 || rightN == 0) {
    cout << "Build the bipartite graph first.\n";
    return;
  }

  clearResult();
  traceShown = 0;

  cout << "\n--- Konig from a maximum matching ---\n";
  cout << "A matching and a vertex cover are two different jobs. The matching\n";
  cout << "takes disjoint edges, and the cover takes vertices so that every\n";
  cout << "edge touches at least one of them. In a bipartite graph their best\n";
  cout << "sizes are always equal, and the proof builds one from the other.\n";

  int matched = findMatching();
  cout << "\n--- Maximum matching ---\n";
  for (int u = 0; u < leftN; u++) {
    if (matchL[u] == -1) {
      cout << "  L" << u << ": (Free)\n";
    } else {
      cout << "  L" << u << " - R" << matchL[u] << "\n";
    }
  }

  for (int i = 0; i < MAXV; i++) {
    inZL[i] = false;
    inZR[i] = false;
  }

  int queueL[MAXV];
  int queueR[MAXV];
  int headL = 0;
  int headR = 0;
  int tailL = 0;
  int tailR = 0;

  cout << "\n--- Alternating reachability (Z) ---\n";
  for (int u = 0; u < leftN; u++) {
    if (matchL[u] != -1) continue;
    inZL[u] = true;
    queueL[tailL++] = u;
    say("Start at free L" + to_string(u) + ".");
  }
  if (tailL == 0) {
    say("No left vertex is free, so Z starts empty.");
  }

  while (headL < tailL || headR < tailR) {
    if (headL < tailL) {
      int u = queueL[headL++];
      for (int v = 0; v < rightN; v++) {
        if (!edgeLR[u][v]) continue;
        if (matchL[u] == v) {
          say("L" + to_string(u) + " - R" + to_string(v) +
              " is in the matching, so skip it.");
          continue;
        }
        if (inZR[v]) continue;
        inZR[v] = true;
        queueR[tailR++] = v;
        say("L" + to_string(u) + " - R" + to_string(v) +
            " is not in the matching, so R" + to_string(v) + " enters Z.");
      }
    }
    if (headR < tailR) {
      int v = queueR[headR++];
      int u = matchR[v];
      if (u == -1) {
        say("R" + to_string(v) + " is free, so the walk stops there.");
        continue;
      }
      if (inZL[u]) continue;
      inZL[u] = true;
      queueL[tailL++] = u;
      say("R" + to_string(v) + " is matched to L" + to_string(u) +
          ", so L" + to_string(u) + " enters Z.");
    }
  }

  cout << "\n--- Minimum vertex cover ---\n";
  string coverText;
  int coverSize = 0;
  for (int u = 0; u < leftN; u++) {
    if (inZL[u]) continue;
    if (coverSize > 0) coverText += ", ";
    coverText += "L" + to_string(u);
    coverSize++;
  }
  for (int v = 0; v < rightN; v++) {
    if (!inZR[v]) continue;
    if (coverSize > 0) coverText += ", ";
    coverText += "R" + to_string(v);
    coverSize++;
  }
  if (coverSize == 0) coverText = "(Empty)";
  cout << "Cover: " << coverText << ".\n";
  cout << "Left outside Z, plus right inside Z.\n";

  bool covered = true;
  for (int u = 0; u < leftN; u++) {
    for (int v = 0; v < rightN; v++) {
      if (!edgeLR[u][v]) continue;
      bool leftIn = !inZL[u];
      bool rightIn = inZR[v];
      if (!leftIn && !rightIn) covered = false;
    }
  }
  if (covered) {
    cout << "Every edge touches the cover, so no edge was missed.\n";
  } else {
    cout << "An edge is uncovered, which would contradict the theorem.\n";
  }

  cout << "\nMatching size = " << matched << ". Cover size = " << coverSize
       << ".\n";
  cout << "The two numbers agree, which is Konig's theorem: In a bipartite\n";
  cout << "graph, the largest matching and the smallest vertex cover have the\n";
  cout << "same size. A cover must use one vertex from every matching edge, so\n";
  cout << "it can never be smaller, and this construction reaches that bound.\n";
  if (traceShown >= TRACE_LIMIT) {
    cout << "(Trace stops after " << TRACE_LIMIT
         << " lines. The search itself ran to the end.)\n";
  }
  resultDone = true;
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

static void showResult() {
  cout << "\n--- Current matching and cover ---\n";
  if (!resultDone) {
    cout << "(No result yet)\n";
    cout << "Run Konig first, or the listing would show stale pairs.\n";
    return;
  }
  for (int u = 0; u < leftN; u++) {
    if (matchL[u] == -1) {
      cout << "  L" << u << ": (Free)\n";
    } else {
      cout << "  L" << u << " - R" << matchL[u] << "\n";
    }
  }
  cout << "Cover:";
  bool any = false;
  for (int u = 0; u < leftN; u++) {
    if (inZL[u]) continue;
    cout << " L" << u;
    any = true;
  }
  for (int v = 0; v < rightN; v++) {
    if (!inZR[v]) continue;
    cout << " R" << v;
    any = true;
  }
  if (!any) cout << " (Empty)";
  cout << ".\n";
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
    resultDone = false;
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
  cout << "  The matching has two pairs and no left vertex is free, so Z is\n";
  cout << "  empty and the cover is the whole left side.\n\n";

  addEdge(0, 0);
  addEdge(0, 1);
  addEdge(1, 0);
  clearResult();

  cout << "Demo loaded: " << leftN << " left, " << rightN << " right, "
       << edgeCount << " edges.\n";
}

static void loadDeficientDemo() {
  resetBipartite();
  leftN = 3;
  rightN = 3;
  cout << "Deficient demo: 3 left, 3 right, 3 edges.\n";
  cout << "  L0 - R0, L1 - R0, L2 - R1.\n";
  cout << "  L1 is free, so Z walks L1 - R0 - L0. The cover drops L0 and L1\n";
  cout << "  and keeps L2 and R0, which covers every edge.\n\n";

  addEdge(0, 0);
  addEdge(1, 0);
  addEdge(2, 1);
  clearResult();

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
    resultDone = false;
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
    clearResult();
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
  clearResult();
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
  clearResult();
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
  clearResult();
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
  clearResult();
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
  clearResult();
  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  Left = " << leftN << ", right = " << rightN
       << ", edges = 0. The vertices are still there.\n";
  cout << "  With no edges the matching and the cover are both empty.\n";
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
    cout << "\n=== G50: Konig's Theorem (Minimum Vertex Cover) ===\n";
    cout << "Graph: Bipartite | L = " << leftN << ", R = " << rightN
         << ", edges = " << edgeCount << "\n";
    cout << "1. Build a bipartite graph\n";
    cout << "2. Load the reroute demo (2 + 2 vertices)\n";
    cout << "3. Load the deficient demo (3 + 3 vertices)\n";
    cout << "4. Bipartite operations\n";
    cout << "5. Run Konig\n";
    cout << "6. Show the edge list\n";
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
      runKonig();
    } else if (choice == 6) {
      showEdges();
    } else if (choice == 7) {
      showResult();
    } else if (choice == 8) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
