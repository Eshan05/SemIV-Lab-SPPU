// 3. For given set of elements create skip list. Find the element in the set
// that is closest to some given value. (Note: Decide the level of element in
// the list Randomly with some upper limit)

#include <iostream>
#include <string>
using namespace std;

class SimpleRandom {
 private:
  unsigned long long state;

 public:
  explicit SimpleRandom(unsigned long long seed) {
    if (seed == 0) seed = 1;
    state = seed;
  }

  unsigned int next() {
    state = (state * 48271ULL) % 2147483647ULL;
    return static_cast<unsigned int>(state);
  }

  int nextBit() { return static_cast<int>(next() % 2U); }
};

class SkipList {
 private:
  struct Node {
    int key;
    int level;
    Node **forward;

    Node(int k, int lvl) {
      key = k;
      level = lvl;
      forward = new Node *[lvl + 1];
      for (int i = 0; i <= lvl; ++i) {
        forward[i] = 0;
      }
    }

    ~Node() { delete[] forward; }
  };

  int maxLevel;
  int currentLevel;
  Node *header;
  SimpleRandom rng;

  int absInt(int x) const { return (x < 0) ? -x : x; }

  int randomLevel() {
    int lvl = 0;
    while (lvl < maxLevel && rng.nextBit() == 1) {
      ++lvl;
    }
    return lvl;
  }

 public:
  SkipList(int maxLvl, unsigned long long seed)
      : maxLevel(maxLvl), currentLevel(0), header(new Node(0, maxLvl)), rng(seed) {}

  ~SkipList() {
    Node *cur = header;
    while (cur != 0) {
      Node *nxt = cur->forward[0];
      delete cur;
      cur = nxt;
    }
  }

  bool insert(int key) {
    Node **update = new Node *[maxLevel + 1];
    Node *cur = header;

    for (int i = currentLevel; i >= 0; --i) {
      while (cur->forward[i] != 0 && cur->forward[i]->key < key) {
        cur = cur->forward[i];
      }
      update[i] = cur;
    }

    cur = cur->forward[0];
    if (cur != 0 && cur->key == key) {
      delete[] update;
      return false;
    }

    int nodeLevel = randomLevel();
    if (nodeLevel > currentLevel) {
      for (int i = currentLevel + 1; i <= nodeLevel; ++i) {
        update[i] = header;
      }
      currentLevel = nodeLevel;
    }

    Node *node = new Node(key, nodeLevel);
    for (int i = 0; i <= nodeLevel; ++i) {
      node->forward[i] = update[i]->forward[i];
      update[i]->forward[i] = node;
    }

    delete[] update;
    return true;
  }

  bool findClosest(int key, int &closest) const {
    Node *cur = header;

    for (int i = currentLevel; i >= 0; --i) {
      while (cur->forward[i] != 0 && cur->forward[i]->key < key) {
        cur = cur->forward[i];
      }
    }

    Node *left = (cur == header) ? 0 : cur;
    Node *right = cur->forward[0];

    if (left == 0 && right == 0) return false;
    if (left == 0) {
      closest = right->key;
      return true;
    }
    if (right == 0) {
      closest = left->key;
      return true;
    }

    int dl = absInt(left->key - key);
    int dr = absInt(right->key - key);
    closest = (dl <= dr) ? left->key : right->key;
    return true;
  }

  void display() const {
    cout << "\nSkip List Structure\n";
    for (int lvl = currentLevel; lvl >= 0; --lvl) {
      cout << "Level " << lvl << " : ";
      Node *cur = header->forward[lvl];
      while (cur != 0) {
        cout << cur->key << " ";
        cur = cur->forward[lvl];
      }
      cout << "\n";
    }
  }
};

int main() {
  int maxLvl;
  unsigned long long seed;

  cout << "Enter max level for skip list: ";
  cin >> maxLvl;
  if (maxLvl < 1) {
    cout << "Max level must be at least 1\n";
    return 0;
  }

  cout << "Enter random seed (any positive integer): ";
  cin >> seed;
  if (seed == 0) seed = 1;

  SkipList sl(maxLvl, seed);

  while (true) {
    cout << "\n1. Insert Element\n";
    cout << "2. Find Closest Element\n";
    cout << "3. Display List\n";
    cout << "4. Exit\n";
    cout << "Enter choice: ";

    int choice;
    cin >> choice;

    if (choice == 1) {
      int value;
      cout << "Enter integer element: ";
      cin >> value;
      if (sl.insert(value)) cout << "Inserted " << value << "\n";
      else cout << "Element already exists\n";

    } else if (choice == 2) {
      int value, closest;
      cout << "Enter value to find closest: ";
      cin >> value;
      if (sl.findClosest(value, closest)) {
        cout << "Closest element to " << value << " is " << closest << "\n";
      } else {
        cout << "Skip list is empty\n";
      }

    } else if (choice == 3) {
      sl.display();

    } else if (choice == 4) {
      cout << "Exiting...\n";
      break;

    } else {
      cout << "Invalid choice\n";
    }
  }

  return 0;
}
