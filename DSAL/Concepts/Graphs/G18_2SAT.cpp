// 14. 2-SAT: A clause (a | b) says the same thing as the two implications
// ~a -> b and ~b -> a, so one SCC pass decides the whole formula.
// Demo: A four-variable formula forces a chain of choices, then a four-clause
// formula that no assignment can satisfy.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXVAR = 7;
const int MAXLIT = 14;
const int MAXCLAUSE = 24;

static int varCount = 0;
static int clauseCount = 0;
static int clauseA[MAXCLAUSE];
static int clauseB[MAXCLAUSE];

static bool hasArc[MAXLIT][MAXLIT];
static vector<int> adj[MAXLIT];
static int arcCount = 0;

static int disc[MAXLIT];
static int low[MAXLIT];
static bool onStack[MAXLIT];
static vector<int> stk;
static int timerStep = 0;
static int compOf[MAXLIT];
static int compCount = 0;
static bool value[MAXVAR];

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

static int litOf(int var, int negated) { return var * 2 + negated; }

static int negOf(int lit) { return lit ^ 1; }

static string litName(int lit) {
  string s = (lit % 2 == 1) ? "~" : "";
  return s + "x" + to_string(lit / 2);
}

static void clearArcs() {
  for (int i = 0; i < MAXLIT; i++) {
    adj[i].clear();
    for (int j = 0; j < MAXLIT; j++) hasArc[i][j] = false;
  }
  arcCount = 0;
}

static void clearFormula() {
  clearArcs();
  varCount = 0;
  clauseCount = 0;
}

static bool addOneArc(int from, int to) {
  if (hasArc[from][to]) return false;
  hasArc[from][to] = true;
  adj[from].push_back(to);
  arcCount++;
  return true;
}

static void addBothArcs(int a, int b) {
  addOneArc(negOf(a), b);
  addOneArc(negOf(b), a);
}

static void explainClause(int index, int a, int b) {
  cout << "\nClause " << index << ": (" << litName(a) << " | " << litName(b)
       << ")\n";
  cout << "  If " << litName(a) << " were false, " << litName(b)
       << " would have to be true.\n";
  cout << "  Arc " << litName(negOf(a)) << " -> " << litName(b);
  if (addOneArc(negOf(a), b)) cout << ", added.\n";
  else cout << " is already there.\n";

  cout << "  If " << litName(b) << " were false, " << litName(a)
       << " would have to be true.\n";
  cout << "  Arc " << litName(negOf(b)) << " -> " << litName(a);
  if (addOneArc(negOf(b), a)) cout << ", added.\n";
  else cout << " is already there.\n";
}

static void addClausePair(int a, int b, bool explain) {
  clauseA[clauseCount] = a;
  clauseB[clauseCount] = b;
  clauseCount++;
  if (explain) {
    explainClause(clauseCount, a, b);
  } else {
    addBothArcs(a, b);
  }
}

static bool readClause(int &a, int &b) {
  while (true) {
    int va = readInt("  First literal, variable (-1 to stop): ");
    if (va == -1) return false;
    if (va < 0 || va >= varCount) {
      cout << "  Rejected: Variable must be in 0.." << (varCount - 1) << ".\n";
      cout << "  Please re-enter this clause.\n";
      continue;
    }
    int na = readRange("    Negated? (0 = x" + to_string(va) + ", 1 = ~x" +
                           to_string(va) + "): ",
                       0, 1);

    int vb = readInt("  Second literal, variable (-1 to stop): ");
    if (vb == -1) return false;
    if (vb < 0 || vb >= varCount) {
      cout << "  Rejected: Variable must be in 0.." << (varCount - 1) << ".\n";
      cout << "  Please re-enter this clause.\n";
      continue;
    }
    int nb = readRange("    Negated? (0 = x" + to_string(vb) + ", 1 = ~x" +
                           to_string(vb) + "): ",
                       0, 1);

    int la = litOf(va, na);
    int lb = litOf(vb, nb);
    if (la == negOf(lb)) {
      cout << "  Rejected: (" << litName(la) << " | " << litName(lb)
           << ") holds whatever the values are.\n";
      cout << "    A literal beside its own negation constrains nothing.\n";
      cout << "  Please re-enter this clause.\n";
      continue;
    }

    a = la;
    b = lb;
    return true;
  }
}

static void buildFormula() {
  int fresh = readRange("Number of variables (0.." + to_string(MAXVAR) + "): ", 0,
                        MAXVAR);
  clearFormula();
  varCount = fresh;

  if (varCount == 0) {
    cout << "Build complete: 0 variables, 0 clauses.\n";
    return;
  }

  cout << "Each clause is two literals joined by or. A literal is a variable\n";
  cout << "plus 0 for plain or 1 for negated.\n";
  while (clauseCount < MAXCLAUSE) {
    cout << "\nClause " << (clauseCount + 1) << " of at most " << MAXCLAUSE
         << "\n";
    int a, b;
    if (!readClause(a, b)) break;
    addClausePair(a, b, true);
  }

  cout << "\nBuild complete: " << varCount
       << (varCount == 1 ? " variable, " : " variables, ") << clauseCount
       << (clauseCount == 1 ? " clause, " : " clauses, ") << arcCount
       << (arcCount == 1 ? " arc.\n" : " arcs.\n");
}

static void loadDemo(bool satisfiableDemo) {
  clearFormula();

  if (satisfiableDemo) {
    varCount = 4;
    cout << "Satisfiable demo: 4 variables, 4 clauses.\n";
    cout << "  (x0 | x1), (~x1 | x2), (~x2 | x3), (~x3 | ~x0).\n";
    cout << "  Each clause passes a requirement along the chain, and the last\n";
    cout << "  one points back at x0.\n\n";
    addClausePair(litOf(0, 0), litOf(1, 0), false);
    addClausePair(litOf(1, 1), litOf(2, 0), false);
    addClausePair(litOf(2, 1), litOf(3, 0), false);
    addClausePair(litOf(3, 1), litOf(0, 1), false);
  } else {
    varCount = 2;
    cout << "Unsatisfiable demo: 2 variables, 4 clauses.\n";
    cout << "  (x0 | x1), (~x0 | x1), (x0 | ~x1), (~x0 | ~x1).\n";
    cout << "  Every combination of signs appears once, so each of the four\n";
    cout << "  possible assignments kills one clause.\n\n";
    addClausePair(litOf(0, 0), litOf(1, 0), false);
    addClausePair(litOf(0, 1), litOf(1, 0), false);
    addClausePair(litOf(0, 0), litOf(1, 1), false);
    addClausePair(litOf(0, 1), litOf(1, 1), false);
  }

  cout << "Demo loaded: " << varCount << " variables, " << clauseCount
       << " clauses, " << arcCount << " arcs.\n";
}

static void showFormula() {
  cout << "\n--- Formula ---\n";
  if (varCount == 0) {
    cout << "(Empty)\n";
    return;
  }

  cout << varCount << (varCount == 1 ? " variable, " : " variables, ")
       << clauseCount << (clauseCount == 1 ? " clause, " : " clauses, ")
       << arcCount << (arcCount == 1 ? " arc.\n" : " arcs.\n");
  if (clauseCount == 0) {
    cout << "Clauses: (None)\n";
    return;
  }
  for (int i = 0; i < clauseCount; i++) {
    cout << "  " << (i + 1) << ". (" << litName(clauseA[i]) << " | "
         << litName(clauseB[i]) << ")\n";
  }

  cout << "\n--- Implication Graph ---\n";
  for (int lit = 0; lit < 2 * varCount; lit++) {
    cout << setw(3) << lit << " " << setw(4) << litName(lit) << ":";
    if (adj[lit].empty()) cout << " (None)";
    else
      for (int v : adj[lit]) cout << " " << litName(v);
    cout << "\n";
  }
  cout << "Node 2v holds x_v and node 2v + 1 holds ~x_v, so each variable owns\n";
  cout << "two nodes and every arc crosses between them.\n";
}

static void resetPass() {
  for (int i = 0; i < MAXLIT; i++) {
    disc[i] = -1;
    low[i] = -1;
    compOf[i] = 0;
    onStack[i] = false;
  }
  stk.clear();
  timerStep = 0;
  compCount = 0;
}

static void printStack(const string &pad) {
  cout << pad << "  Stack now:";
  if (stk.empty()) {
    cout << " (Empty)\n";
    return;
  }
  for (int i = 0; i < (int)stk.size(); i++) {
    cout << (i == 0 ? " [" : ", ") << litName(stk[i]);
  }
  cout << "]\n";
}

static void closeComponent(int u, int depth, bool verbose) {
  string pad(depth * 2, ' ');
  vector<int> fresh;
  while (true) {
    int w = stk.back();
    stk.pop_back();
    onStack[w] = false;
    fresh.push_back(w);
    if (w == u) break;
  }

  compCount++;
  for (int w : fresh) compOf[w] = compCount;

  if (verbose) {
    cout << pad << "Low and disc are both " << disc[u] << ", so a component"
         << " closes here.\n";
    cout << pad << "  Pop until " << litName(u) << " comes off:";
    for (int w : fresh) cout << " " << litName(w);
    cout << "\n";
    cout << pad << "  That is component " << compCount << " with "
         << fresh.size() << (fresh.size() == 1 ? " literal.\n" : " literals.\n");
    printStack(pad);
  }
}

static void strongconnect(int u, int depth, bool verbose) {
  string pad(depth * 2, ' ');
  disc[u] = timerStep;
  low[u] = disc[u];
  timerStep++;
  stk.push_back(u);
  onStack[u] = true;
  if (verbose) {
    cout << pad << "Enter " << litName(u) << " with disc " << disc[u]
         << " and push it on the stack.\n";
  }

  for (int v : adj[u]) {
    if (disc[v] == -1) {
      if (verbose) {
        cout << pad << "  Arc " << litName(u) << " -> " << litName(v)
             << ": Unvisited, descend.\n";
      }
      strongconnect(v, depth + 1, verbose);
      int before = low[u];
      if (low[v] < low[u]) low[u] = low[v];
      if (verbose) {
        cout << pad << "    Return from " << litName(v) << ", so low["
             << litName(u) << "] = min(" << before << ", " << low[v] << ") = "
             << low[u] << ".\n";
      }
    } else if (onStack[v]) {
      int before = low[u];
      if (disc[v] < low[u]) low[u] = disc[v];
      if (verbose) {
        cout << pad << "  Arc " << litName(u) << " -> " << litName(v) << ": "
             << litName(v) << " is still open, so low[" << litName(u)
             << "] = min(" << before << ", " << disc[v] << ") = " << low[u]
             << ".\n";
      }
    } else {
      if (verbose) {
        cout << pad << "  Arc " << litName(u) << " -> " << litName(v) << ": "
             << litName(v) << " sits in a finished component, ignore it.\n";
      }
    }
  }

  if (low[u] == disc[u]) {
    closeComponent(u, depth, verbose);
  } else if (verbose) {
    cout << pad << "Low " << low[u] << " < disc " << disc[u] << ", so "
         << litName(u) << " climbs out and stays on the stack.\n";
  }
}

static void reportComponents() {
  cout << "\n--- Components ---\n";
  if (varCount == 0) {
    cout << "(Empty)\n";
    return;
  }
  for (int c = 1; c <= compCount; c++) {
    int size = 0;
    for (int lit = 0; lit < 2 * varCount; lit++) {
      if (compOf[lit] == c) size++;
    }
    cout << "Component " << c << " (" << size
         << (size == 1 ? " literal):" : " literals):");
    for (int lit = 0; lit < 2 * varCount; lit++) {
      if (compOf[lit] == c) cout << " " << litName(lit);
    }
    cout << "\n";
  }
  cout << "\nComponents are numbered in closing order, so component 1 closed\n";
  cout << "first and sits nearest a sink of the implication graph.\n";
}

static bool literalHolds(int lit) {
  bool plain = value[lit / 2];
  return (lit % 2 == 0) ? plain : !plain;
}

static void checkClauses() {
  cout << "\n--- Clause check ---\n";
  for (int i = 0; i < clauseCount; i++) {
    int a = clauseA[i];
    int b = clauseB[i];
    cout << "  " << (i + 1) << ". (" << litName(a) << " | " << litName(b)
         << "): ";
    if (literalHolds(a)) {
      cout << "Holds, because " << litName(a) << " is true.\n";
    } else if (literalHolds(b)) {
      cout << "Holds, because " << litName(b) << " is true.\n";
    } else {
      cout << "Fails, because both literals are false.\n";
    }
  }
}

static void finishSolve() {
  cout << "\n--- Contradiction check ---\n";
  bool contradiction = false;
  for (int v = 0; v < varCount; v++) {
    int cp = compOf[litOf(v, 0)];
    int cn = compOf[litOf(v, 1)];
    if (cp == cn) {
      contradiction = true;
      cout << "  x" << v << " has " << litName(litOf(v, 0)) << " and "
           << litName(litOf(v, 1)) << " in component " << cp << ".\n";
      cout << "    Each one reaches the other, so x" << v
           << " forces its own negation.\n";
    } else {
      cout << "  x" << v << " has " << litName(litOf(v, 0)) << " in component "
           << cp << " and " << litName(litOf(v, 1)) << " in component " << cn
           << ". Apart, so fine.\n";
    }
  }

  if (contradiction) {
    cout << "\nSatisfiable: No. A variable and its negation share a component,\n";
    cout << "so no assignment can keep both implications true.\n";
    return;
  }

  cout << "\n--- Assignment ---\n";
  for (int v = 0; v < varCount; v++) {
    value[v] = compOf[litOf(v, 0)] < compOf[litOf(v, 1)];
  }

  cout << setw(9) << "Variable" << setw(9) << "Comp(x)" << setw(10) << "Comp(~x)"
       << "  Value\n";
  for (int v = 0; v < varCount; v++) {
    cout << setw(9) << ("x" + to_string(v)) << setw(9) << compOf[litOf(v, 0)]
         << setw(10) << compOf[litOf(v, 1)] << "  "
         << (value[v] ? "true" : "false") << "\n";
  }
  cout << "A component closes before everything that reaches it, so a small\n";
  cout << "number sits near a sink. Nothing forces a sink, so the literal that\n";
  cout << "closes first is the safe one to set true, and its variable follows.\n";

  checkClauses();
  cout << "\nSatisfiable: Yes. Both implications of every clause hold.\n";
}

static void runTrace() {
  if (varCount == 0) {
    cout << "Build a formula first.\n";
    return;
  }

  resetPass();
  cout << "\n--- Tarjan on the implication graph ---\n";
  for (int lit = 0; lit < 2 * varCount; lit++) {
    if (disc[lit] != -1) continue;
    cout << "\nNew search at " << litName(lit) << ".\n";
    strongconnect(lit, 0, true);
  }

  cout << "\nDisc:";
  for (int lit = 0; lit < 2 * varCount; lit++) cout << " " << disc[lit];
  cout << "\nLow :";
  for (int lit = 0; lit < 2 * varCount; lit++) cout << " " << low[lit];
  cout << "\n";

  reportComponents();
  finishSolve();
}

static void runSolveOnly() {
  if (varCount == 0) {
    cout << "Build a formula first.\n";
    return;
  }

  resetPass();
  for (int lit = 0; lit < 2 * varCount; lit++) {
    if (disc[lit] != -1) continue;
    strongconnect(lit, 0, false);
  }

  reportComponents();
  finishSolve();
}

static void addClauseInteractive() {
  if (varCount == 0) {
    cout << "Build a formula first.\n";
    return;
  }
  if (clauseCount >= MAXCLAUSE) {
    cout << "Clause limit reached: MAXCLAUSE = " << MAXCLAUSE << ".\n";
    return;
  }

  cout << "\nClause " << (clauseCount + 1) << " of at most " << MAXCLAUSE
       << "\n";
  int a, b;
  if (!readClause(a, b)) {
    cout << "  Stopped. The formula is unchanged.\n";
    return;
  }

  addClausePair(a, b, true);
  cout << "  Clauses: " << clauseCount << ", arcs: " << arcCount << ".\n";
}

static void removeLastClause() {
  if (clauseCount == 0) {
    cout << "\nThe formula has no clauses yet.\n";
    return;
  }

  int a = clauseA[clauseCount - 1];
  int b = clauseB[clauseCount - 1];
  clauseCount--;
  clearArcs();
  for (int i = 0; i < clauseCount; i++) {
    addBothArcs(clauseA[i], clauseB[i]);
  }

  cout << "\nRemoved clause " << (clauseCount + 1) << ": (" << litName(a)
       << " | " << litName(b) << ").\n";
  cout << "  The implication graph is rebuilt from the clauses that are left.\n";
  cout << "  Clauses: " << clauseCount << ", arcs: " << arcCount << ".\n";
}

int main() {
  clearFormula();

  while (true) {
    cout << "\n=== G18: 2-SAT (Implications, SCC) ===\n";
    cout << "Formula: " << varCount
         << (varCount == 1 ? " variable, " : " variables, ") << clauseCount
         << (clauseCount == 1 ? " clause" : " clauses") << "\n";
    cout << "1. Build a formula\n";
    cout << "2. Load satisfiable demo (4 variables)\n";
    cout << "3. Load unsatisfiable demo (2 variables)\n";
    cout << "4. Add a clause\n";
    cout << "5. Remove the last clause\n";
    cout << "6. Show the formula and the implication graph\n";
    cout << "7. Trace the SCC pass, then assign\n";
    cout << "8. Report the assignment only\n";
    cout << "9. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildFormula();
    } else if (choice == 2) {
      loadDemo(true);
    } else if (choice == 3) {
      loadDemo(false);
    } else if (choice == 4) {
      addClauseInteractive();
    } else if (choice == 5) {
      removeLastClause();
    } else if (choice == 6) {
      showFormula();
    } else if (choice == 7) {
      runTrace();
    } else if (choice == 8) {
      runSolveOnly();
    } else if (choice == 9) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
