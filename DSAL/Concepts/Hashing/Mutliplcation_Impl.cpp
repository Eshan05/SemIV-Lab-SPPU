#include <iostream>
#include <vector>
using namespace std;

const int sizeT = 10;

const double A = 0.6180339887;
int hFn(int k) {
  double fractionalPart = (k * A) - int(k * A);
  return int(sizeT * fractionalPart);
}

void createHT(vector<int> hT[]) {
  for (int i = 0; i < sizeT; i++) {
    hT[i].clear();
  }
}

void insert(vector<int> hT[], int k) {
  int idx = hFn(k);
  hT[idx].push_back(k);
  cout << "Inserted " << k << " at index " << idx << endl;
}

void find(vector<int> hT[], int k) {
  int idx = hFn(k);
  for (auto it = hT[idx].begin(); it != hT[idx].end(); it++) {
    if (*it == k) {
      cout << "Found " << k << " at index " << idx << endl;
      return;
    }
  }
  cout << "Key not found" << endl;
}

void deleteK(vector<int> hT[], int key) {
  int idx = hFn(key);
  for (auto it = hT[idx].begin(); it != hT[idx].end(); it++) {
    if (*it == key) {
      hT[idx].erase(it);
      cout << "Deleted " << key << " at index " << idx << endl;
      return;
    }
  }
  cout << "Key not found" << endl;
}

void displayHashTable(vector<int> hashTable[]) {
  cout << "Hash Table:" << endl;
  for (int i = 0; i < sizeT; i++) {
    cout << i << ": ";
    if (hashTable[i].empty()) {
      cout << "Empty" << endl;
    } else {
      for (int value : hashTable[i]) {
        cout << value << " ";
      }
      cout << endl;
    }
  }
}

int main() {
  vector<int> hashT[sizeT];
  int ch, k;
  createHT(hashT);
  do {
    cout << "\n-> HT Modulo Vector <-: " << endl;
    cout << "1. Insert a key" << endl;
    cout << "2. Find a key" << endl;
    cout << "3. Delete a key" << endl;
    cout << "4. Display hash table" << endl;
    cout << "5. Exit" << endl;
    cout << "Enter your choice: ";
    cin >> ch;

    switch (ch) {
      case 1:
        cout << "Enter key to insert: ";
        cin >> k;
        insert(hashT, k);
        break;

      case 2:
        cout << "Enter key to find: ";
        cin >> k;
        find(hashT, k);
        break;

      case 3:
        cout << "Enter key to delete: ";
        cin >> k;
        deleteK(hashT, k);
        break;

      case 4: displayHashTable(hashT); break;
      case 5: cout << "Exiting..." << endl; break;
      default: cout << "Invalid choice" << endl;
    }
  } while (ch != 5);
  return 0;
}
