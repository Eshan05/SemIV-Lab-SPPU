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

#include <iostream>
#include <string>
using namespace std;

class SetIterator {
 private:
  int *arr;
  int count;
  int idx;

 public:
  SetIterator(int *data, int n) {
    arr = data;
    count = n;
    idx = 0;
  }

  bool hasNext() const { return idx < count; }

  int next() {
    if (!hasNext()) return 0;
    return arr[idx++];
  }
};

class SetADT {
 private:
  int *data;
  int count;
  int capacity;

  void ensureCapacity() {
    if (count < capacity) return;

    int newCap = capacity * 2;
    if (newCap < 4) newCap = 4;

    int *newData = new int[newCap];
    for (int i = 0; i < count; ++i) {
      newData[i] = data[i];
    }
    delete[] data;
    data = newData;
    capacity = newCap;
  }

  int binarySearch(int key) const {
    int low = 0;
    int high = count - 1;
    while (low <= high) {
      int mid = low + (high - low) / 2;
      if (data[mid] == key) return mid;
      if (data[mid] < key) low = mid + 1;
      else high = mid - 1;
    }
    return -1;
  }

  int findInsertPosition(int key) const {
    int low = 0;
    int high = count;
    while (low < high) {
      int mid = low + (high - low) / 2;
      if (data[mid] < key) low = mid + 1;
      else high = mid;
    }
    return low;
  }

 public:
  SetADT(int cap = 8) {
    if (cap < 1) cap = 8;
    capacity = cap;
    count = 0;
    data = new int[capacity];
  }

  SetADT(const SetADT &other) {
    capacity = other.capacity;
    count = other.count;
    data = new int[capacity];
    for (int i = 0; i < count; ++i) {
      data[i] = other.data[i];
    }
  }

  SetADT &operator=(const SetADT &other) {
    if (this == &other) return *this;
    delete[] data;
    capacity = other.capacity;
    count = other.count;
    data = new int[capacity];
    for (int i = 0; i < count; ++i) {
      data[i] = other.data[i];
    }
    return *this;
  }

  ~SetADT() { delete[] data; }

  bool add(int element) {
    if (contains(element)) return false;
    ensureCapacity();
    int pos = findInsertPosition(element);
    for (int i = count; i > pos; --i) {
      data[i] = data[i - 1];
    }
    data[pos] = element;
    ++count;
    return true;
  }

  bool remove(int element) {
    int idx = binarySearch(element);
    if (idx == -1) return false;
    for (int i = idx; i < count - 1; ++i) {
      data[i] = data[i + 1];
    }
    --count;
    return true;
  }

  bool contains(int element) const { return binarySearch(element) != -1; }

  int size() const { return count; }

  SetIterator iterator() { return SetIterator(data, count); }

  void display() const {
    cout << "{ ";
    for (int i = 0; i < count; ++i) {
      cout << data[i] << " ";
    }
    cout << "}\n";
  }

  SetADT unionSet(const SetADT &other) const {
    SetADT result(count + other.count + 2);
    int i = 0, j = 0;
    while (i < count && j < other.count) {
      if (data[i] < other.data[j]) result.data[result.count++] = data[i++];
      else if (data[i] > other.data[j]) result.data[result.count++] = other.data[j++];
      else {
        result.data[result.count++] = data[i];
        ++i;
        ++j;
      }
    }
    while (i < count) result.data[result.count++] = data[i++];
    while (j < other.count) result.data[result.count++] = other.data[j++];
    return result;
  }

  SetADT intersection(const SetADT &other) const {
    SetADT result((count < other.count ? count : other.count) + 2);
    int i = 0, j = 0;
    while (i < count && j < other.count) {
      if (data[i] < other.data[j]) ++i;
      else if (data[i] > other.data[j]) ++j;
      else {
        result.data[result.count++] = data[i];
        ++i;
        ++j;
      }
    }
    return result;
  }

  SetADT difference(const SetADT &other) const {
    SetADT result(count + 2);
    int i = 0, j = 0;
    while (i < count && j < other.count) {
      if (data[i] < other.data[j]) result.data[result.count++] = data[i++];
      else if (data[i] > other.data[j]) ++j;
      else {
        ++i;
        ++j;
      }
    }
    while (i < count) result.data[result.count++] = data[i++];
    return result;
  }

  bool isSubsetOf(const SetADT &other) const {
    int i = 0, j = 0;
    while (i < count && j < other.count) {
      if (data[i] < other.data[j]) return false;
      if (data[i] > other.data[j]) ++j;
      else {
        ++i;
        ++j;
      }
    }
    return i == count;
  }
};

void setMenu(SetADT &s, const string &name) {
  while (true) {
    cout << "\n"
         << name << " Menu\n";
    cout << "1. Add element\n";
    cout << "2. Remove element\n";
    cout << "3. Contains element\n";
    cout << "4. Size\n";
    cout << "5. Display\n";
    cout << "6. Iterate\n";
    cout << "7. Back\n";
    cout << "Enter choice: ";

    int choice;
    cin >> choice;

    if (choice == 1) {
      int x;
      cout << "Enter element: ";
      cin >> x;
      cout << (s.add(x) ? "Added\n" : "Duplicate element\n");
    } else if (choice == 2) {
      int x;
      cout << "Enter element: ";
      cin >> x;
      cout << (s.remove(x) ? "Removed\n" : "Not found\n");
    } else if (choice == 3) {
      int x;
      cout << "Enter element: ";
      cin >> x;
      cout << (s.contains(x) ? "Present\n" : "Absent\n");
    } else if (choice == 4) {
      cout << "Size: " << s.size() << "\n";
    } else if (choice == 5) {
      s.display();
    } else if (choice == 6) {
      SetIterator it = s.iterator();
      cout << "Iterator output: ";
      while (it.hasNext()) cout << it.next() << " ";
      cout << "\n";
    } else if (choice == 7) {
      break;
    } else {
      cout << "Invalid choice\n";
    }
  }
}

int main() {
  SetADT setA, setB;

  while (true) {
    cout << "\nMain Menu\n";
    cout << "1. Operate on Set A\n";
    cout << "2. Operate on Set B\n";
    cout << "3. Union (A U B)\n";
    cout << "4. Intersection (A n B)\n";
    cout << "5. Difference (A - B)\n";
    cout << "6. Difference (B - A)\n";
    cout << "7. Is A subset of B?\n";
    cout << "8. Display both sets\n";
    cout << "9. Exit\n";
    cout << "Enter choice: ";

    int choice;
    cin >> choice;

    if (choice == 1) {
      setMenu(setA, "Set A");
    } else if (choice == 2) {
      setMenu(setB, "Set B");
    } else if (choice == 3) {
      SetADT r = setA.unionSet(setB);
      cout << "Union: ";
      r.display();
    } else if (choice == 4) {
      SetADT r = setA.intersection(setB);
      cout << "Intersection: ";
      r.display();
    } else if (choice == 5) {
      SetADT r = setA.difference(setB);
      cout << "Difference (A-B): ";
      r.display();
    } else if (choice == 6) {
      SetADT r = setB.difference(setA);
      cout << "Difference (B-A): ";
      r.display();
    } else if (choice == 7) {
      if (setA.isSubsetOf(setB)) cout << "Set A is a subset of Set B\n";
      else cout << "Set A is NOT a subset of Set B\n";
    } else if (choice == 8) {
      cout << "Set A: ";
      setA.display();
      cout << "Set B: ";
      setB.display();
    } else if (choice == 9) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid choice\n";
    }
  }

  return 0;
}
