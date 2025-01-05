// 4. To create ADT that implement the "set" concept.
//   1. Add (New Element) Place a value into the set
//   2. Remove (Element) Remove the value
//   3. Contains (Element) Return true if element is in collection
//   4. Size () Return number of values in collection
//   5. Iterator () Return an iterator used to loop over collection
//   6. Intersection of two sets
//   7. Union of two sets
//   8. Difference between two sets
//   9. Subset

#include <algorithm>
#include <iostream>
#include <list>
using namespace std;

int hFn(int k) { return k % 10; }

class Set {
 private:
  list<int> d;

 public:
  void add(int v) {
    auto it = find(d.begin(), d.end(), v);
    if (it == d.end()) d.push_back(v);
  }

  void deleteK(int v) {
    auto it = find(d.begin(), d.end(), v);
    if (!(it == d.end())) d.remove(v);
  }

  bool contains(int v) const {
    auto it = find(d.begin(), d.end(), v);
    return it != d.end();
  }

  int getSize() { return d.size(); }
  void display() {
    cout << "Set: { ";
    for (int v : d) cout << v << ", ";
    cout << " }" << endl;
  }

  list<int>::iterator iterator() { return d.begin(); }

  Set intersection(const Set &other) {
    Set result;
    for (auto it = d.begin(); it != d.end(); ++it) {
      if (other.contains(*it)) {
        result.add(*it);
      }
    }
    return result;
  }

  Set unionSet(const Set &other) {
    Set result = *this;
    for (auto it = other.d.begin(); it != other.d.end(); ++it) {
      result.add(*it);
    }
    return result;
  }

  Set difference(const Set &other) {
    Set result;
    for (auto it = d.begin(); it != d.end(); ++it) {
      if (!other.contains(*it)) {
        result.add(*it);
      }
    }
    return result;
  }

  bool isSubset(const Set &other) {
    if (d.size() < other.d.size()) return false;
    for (auto it = d.begin(); it != d.end(); ++it) {
      if (!other.contains(*it)) return false;
    }
    return true;
  }
};

int main() {
  Set s1, s2;
  int ch1, ch2, val;
  do {
    cout << "\nMenu: " << endl;
    cout << "1. Set A\n2. Set B\n3. Union of both\n4. Intersection of both\n5. Difference of A with B\n6. Difference of B with A\n7. Is A subset of B\n8. Exit\nEnter your choice: ";
    cin >> ch1;
    switch (ch1) {
      case 1:
        do {
          cout << "Set A: " << endl;
          cout << "1. Add element\n2. Delete element\n3. Find element\n4. Number of elements\n5. Display\n6. Exit\nEnter your choice";
          cin >> ch2;
          switch (ch2) {
            case 1:
              cout << "Enter element to add: ";
              cin >> val;
              s1.add(val);
              break;
            case 2:
              cout << "Enter element to delete: ";
              cin >> val;
              s1.deleteK(val);
              break;
            case 3:
              cout << "Enter element to find: ";
              cin >> val;
              if (s1.contains(val)) cout << "Element found\n";
              else cout << "Element not found\n";
              break;
            case 4:
              cout << "Number of elements: " << s1.getSize() << endl;
              break;
            case 5:
              s1.display();
              break;
            case 6:
              cout << "Exiting...\n";
              break;
            default:
              cout << "Invalid choice\n";
          }
        } while (ch2 != 6);
        break;
      case 2:
        do {
          cout << "Set B: " << endl;
          cout << "1. Add element\n2. Delete element\n3. Find element\n4. Number of elements\n5. Display\n6. Exit\nEnter your choice";
          cin >> ch2;
          switch (ch2) {
            case 1:
              cout << "Enter element to add: ";
              cin >> val;
              s2.add(val);
              break;
            case 2:
              cout << "Enter element to delete: ";
              cin >> val;
              s2.deleteK(val);
              break;
            case 3:
              cout << "Enter element to find: ";
              cin >> val;
              if (s2.contains(val)) cout << "Element found\n";
              else cout << "Element not found\n";
              break;
            case 4:
              cout << "Number of elements: " << s2.getSize() << endl;
              break;
            case 5:
              s2.display();
              break;
            case 6:
              cout << "Exiting...\n";
              break;
            default:
              cout << "Invalid choice\n";
          }
        } while (ch2 != 6);
        break;

      case 3: s1.unionSet(s2).display(); break;
      case 4: s1.intersection(s2).display(); break;
      case 5: s1.difference(s2).display(); break;

      case 6: s2.difference(s1).display(); break;
      case 7:
        if (s1.isSubset(s2)) cout << "A is a subset of B\n";
        else cout << "A is not a subset of B\n";
        break;

      case 8:
        cout << "Exiting...\n";
        break;

      default:
        cout << "Invalid choice\n";
    }
  } while (ch1 != 8);
  return 0;
}