#include <iostream>
using namespace std;

class HT {
 private:
  int size;
  int *hashT;

 public:
  HT(int size) {
    this->size = size;
    hashT = new int[size];
    for (int i = 0; i < size; i++) {
      hashT[i] = -1;
    }
  }

  void insert(int k) {
    int idx = hFn(k);
    while (hashT[idx] != -1) {
      idx = (idx + 1) % size;
    }
    hashT[idx] = k;
    cout << "Inserted " << k << " at index " << idx << endl;
  }

  int hFn(int k) { return k % size; }

  void find(int k) {
    int idx = hFn(k);
    while (hashT[idx] != -1) {
      if (hashT[idx] == k) {
        cout << "Found " << k << " at index " << idx << endl;
        return;
      }
      idx = (idx + 1) % size;
    }
    cout << "Key not found" << endl;
  }

  void deleteK(int k) {
    int idx = hFn(k);
    while (hashT[idx] != -1) {
      if (hashT[idx] == k) {
        hashT[idx] = -1;
        cout << "Deleted " << k << " at index " << idx << endl;
        return;
      }
      idx = (idx + 1) % size;
    }
    cout << "Key not found" << endl;
  }

  void displayHashTable() {
    cout << "Hash Table:" << endl;
    for (int i = 0; i < size; i++) {
      if (hashT[i] == -1) {
        cout << i << ": Empty" << endl;
      } else {
        cout << i << ": " << hashT[i] << endl;
      }
    }
  }
};

int main() {
  HT ht(10);
  int choice, key;
  do {
    cout << "\n-> HT Modulo Array <-: " << endl;
    cout << "1. Insert a key" << endl;
    cout << "2. Find a key" << endl;
    cout << "3. Delete a key" << endl;
    cout << "4. Display hash table" << endl;
    cout << "5. Exit" << endl;
    cout << "Enter your choice: ";
    cin >> choice;

    switch (choice) {
      case 1:
        cout << "Enter key to insert: ";
        cin >> key;
        ht.insert(key);
        break;

      case 2:
        cout << "Enter key to find: ";
        cin >> key;
        ht.find(key);
        break;

      case 3:
        cout << "Enter key to delete: ";
        cin >> key;
        ht.deleteK(key);
        break;

      case 4:
        ht.displayHashTable();
        break;

      case 5:
        cout << "Exiting..." << endl;
        return 0;

      default:
        cout << "Invalid choice. Please try again." << endl;
    }
  } while (true);
  return 0;
}