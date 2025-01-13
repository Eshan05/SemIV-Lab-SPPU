// 17. KD Tree (2D)
// Demo: construction via insertion, deletion, and nearest-neighbor search.
// Points are (x,y). Split dimension alternates: depth%2 (x then y).

#include <iostream>
using namespace std;

struct Node {
  int x;
  int y;
  Node *left;
  Node *right;
  Node(int xx, int yy) : x(xx), y(yy), left(NULL), right(NULL) {}
};

static void clearInput() {
  cin.clear();
  cin.ignore(1024, '\n');
}

static int readInt(const string &prompt) {
  int v;
  while (true) {
    cout << prompt;
    if (cin >> v) return v;
    cout << "Invalid number.\n";
    clearInput();
  }
}

static bool samePoint(Node *n, int x, int y) {
  return n && n->x == x && n->y == y;
}

static Node *insert(Node *root, int x, int y, int depth, bool &inserted) {
  if (!root) {
    cout << "Insert point here at depth " << depth << "\n";
    inserted = true;
    return new Node(x, y);
  }
  int cd = depth % 2;
  if (samePoint(root, x, y)) {
    cout << "Point already exists at depth " << depth << "; no new node.\n";
    return root;
  }
  cout << "At (" << root->x << "," << root->y << "), compare "
       << (cd == 0 ? 'x' : 'y') << "\n";
  if ((cd == 0 && x < root->x) || (cd == 1 && y < root->y)) {
    root->left = insert(root->left, x, y, depth + 1, inserted);
  } else {
    root->right = insert(root->right, x, y, depth + 1, inserted);
  }
  return root;
}

static bool search(Node *root, int x, int y, int depth, int &comps) {
  comps++;
  if (!root) {
    cout << "Reached NULL at depth " << depth << "\n";
    return false;
  }
  int cd = depth % 2;
  cout << "Compare point (" << x << "," << y << ") with (" << root->x << "," << root->y
       << ") on " << (cd == 0 ? 'x' : 'y') << ": ";
  if (samePoint(root, x, y)) return true;
  if ((cd == 0 && x < root->x) || (cd == 1 && y < root->y)) {
    cout << "go left\n";
    return search(root->left, x, y, depth + 1, comps);
  }
  cout << "go right\n";
  return search(root->right, x, y, depth + 1, comps);
}

static int coord(Node *n, int dim) {
  return dim == 0 ? n->x : n->y;
}

static Node *findMin(Node *root, int dim, int depth) {
  if (!root) return NULL;
  int cd = depth % 2;
  if (cd == dim) {
    if (!root->left) return root;
    return findMin(root->left, dim, depth + 1);
  }
  Node *leftMin = findMin(root->left, dim, depth + 1);
  Node *rightMin = findMin(root->right, dim, depth + 1);
  Node *best = root;
  if (leftMin && coord(leftMin, dim) < coord(best, dim)) best = leftMin;
  if (rightMin && coord(rightMin, dim) < coord(best, dim)) best = rightMin;
  return best;
}

static Node *erase(Node *root, int x, int y, int depth) {
  if (!root) return NULL;
  int cd = depth % 2;
  if (samePoint(root, x, y)) {
    if (root->right) {
      Node *mn = findMin(root->right, cd, depth + 1);
      cout << "Delete match: replace with min "
           << (cd == 0 ? 'x' : 'y') << " from right subtree ("
           << mn->x << "," << mn->y << ")\n";
      root->x = mn->x;
      root->y = mn->y;
      root->right = erase(root->right, mn->x, mn->y, depth + 1);
    } else if (root->left) {
      Node *mn = findMin(root->left, cd, depth + 1);
      cout << "Delete match: no right subtree, use min "
           << (cd == 0 ? 'x' : 'y') << " from left subtree ("
           << mn->x << "," << mn->y << ")\n";
      root->x = mn->x;
      root->y = mn->y;
      // Delete the moved point from left subtree, then move left subtree to right
      root->left = erase(root->left, mn->x, mn->y, depth + 1);
      root->right = root->left;
      root->left = NULL;
    } else {
      delete root;
      return NULL;
    }
    return root;
  }
  if ((cd == 0 && x < root->x) || (cd == 1 && y < root->y)) {
    root->left = erase(root->left, x, y, depth + 1);
  } else {
    root->right = erase(root->right, x, y, depth + 1);
  }
  return root;
}

static long long dist2(int x1, int y1, int x2, int y2) {
  long long dx = (long long)x1 - x2;
  long long dy = (long long)y1 - y2;
  return dx * dx + dy * dy;
}

static void nnSearch(Node *root, int tx, int ty, int depth, Node *&best, long long &bestD2) {
  if (!root) return;
  long long d2 = dist2(root->x, root->y, tx, ty);
  if (!best || d2 < bestD2) {
    best = root;
    bestD2 = d2;
    cout << "Nearest candidate -> (" << root->x << "," << root->y
         << "), d2=" << d2 << "\n";
  }

  int cd = depth % 2;
  Node *near = NULL;
  Node *far = NULL;
  if ((cd == 0 && tx < root->x) || (cd == 1 && ty < root->y)) {
    near = root->left;
    far = root->right;
  } else {
    near = root->right;
    far = root->left;
  }

  nnSearch(near, tx, ty, depth + 1, best, bestD2);

  long long planeDist2 = 0;
  if (cd == 0) {
    long long dx = (long long)tx - root->x;
    planeDist2 = dx * dx;
  } else {
    long long dy = (long long)ty - root->y;
    planeDist2 = dy * dy;
  }
  if (planeDist2 <= bestD2) {
    cout << "Plane close enough at depth " << depth << " -> also check far branch.\n";
    nnSearch(far, tx, ty, depth + 1, best, bestD2);
  } else {
    cout << "Skip far branch at depth " << depth << " (plane distance too large).\n";
  }
}

static void display(Node *root, int depth) {
  if (!root) return;
  for (int i = 0; i < depth; i++) cout << "  ";
  cout << "(" << root->x << "," << root->y << ")";
  cout << " split=" << (depth % 2 == 0 ? 'x' : 'y') << "\n";
  display(root->left, depth + 1);
  display(root->right, depth + 1);
}

static void freeTree(Node *root) {
  if (!root) return;
  freeTree(root->left);
  freeTree(root->right);
  delete root;
}

int main() {
  Node *root = NULL;
  int choice = 0;
  while (choice != 7) {
    cout << "\n=== T19: KD Tree (2D) ===\n";
    cout << "1. Insert point\n";
    cout << "2. Delete point\n";
    cout << "3. Search point\n";
    cout << "4. Nearest neighbor\n";
    cout << "5. Display (preorder with split dim)\n";
    cout << "6. Clear\n";
    cout << "7. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      int x = readInt("x: ");
      int y = readInt("y: ");
      bool inserted = false;
      root = insert(root, x, y, 0, inserted);
      cout << (inserted ? "Inserted.\n" : "Duplicate ignored.\n");
    } else if (choice == 2) {
      int x = readInt("x: ");
      int y = readInt("y: ");
      int comps = 0;
      if (search(root, x, y, 0, comps)) {
        root = erase(root, x, y, 0);
        cout << "Deleted.\n";
      } else {
        cout << "Not found; no delete performed.\n";
      }
    } else if (choice == 3) {
      int x = readInt("x: ");
      int y = readInt("y: ");
      int comps = 0;
      bool ok = search(root, x, y, 0, comps);
      cout << (ok ? "Found" : "Not found") << ", comparisons=" << comps << "\n";
    } else if (choice == 4) {
      int x = readInt("target x: ");
      int y = readInt("target y: ");
      Node *best = NULL;
      long long bestD2 = 0;
      nnSearch(root, x, y, 0, best, bestD2);
      if (!best) cout << "Tree empty.\n";
      else {
        cout << "Nearest: (" << best->x << "," << best->y << ")";
        cout << " dist^2=" << bestD2 << "\n";
      }
    } else if (choice == 5) {
      if (!root) cout << "(empty)\n";
      else display(root, 0);
    } else if (choice == 6) {
      freeTree(root);
      root = NULL;
      cout << "Cleared.\n";
    } else if (choice == 7) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  freeTree(root);
  return 0;
}
