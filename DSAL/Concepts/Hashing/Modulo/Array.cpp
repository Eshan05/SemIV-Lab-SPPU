// Typically in HT, we have KV pairs so instead of index then hash value then
// key we can treat hash value as index. So basically we have any number of rows
// and 2 columns, but instead of using Table[i][0] and Table[i][1] we can use 1D
// array too with Table[index] = key

#include <iostream>
using namespace std;

const int Tsize = 10;

void createHT(int hashT[]) {
  for (int i = 0; i < Tsize; i++) {
    hashT[i] = -1;
  }
}

int hFn(int k) { return k % Tsize; }

void insert(int hT[], int k) {
  int idx = hFn(k);
  while (hT[idx] != -1) {
    idx = (idx + 1) % Tsize;
  }
  hT[idx] = k;
  cout << "Inserted " << k << " at index " << idx << endl;
}

void deleteK(int hT[], int k) {
  int idx = hFn(k);
  while (hT[idx] != -1) {  // LP
    if (hT[idx] == k) {
      hT[idx] = -1;
      cout << "Deleted " << k << " at index " << idx << endl;
      return;
    }
    idx = (idx + 1) % Tsize;
  }
  cout << "Key not found" << endl;
}

void find(int hT[], int k) {
  int idx = hFn(k);
  while (hT[idx] != -1) {
    if (hT[idx] = k) {
      cout << "Found " << k << " at index " << idx << endl;
      return;
    }
    idx = (idx + 1) % Tsize;
  }
  cout << "Key not found" << endl;
}

void displayHashTable(int hashTable[]) {
  cout << "Hash Table:" << endl;
  for (int i = 0; i < Tsize; i++) {
    if (hashTable[i] == -1) {
      cout << i << ": Empty" << endl;
    } else {
      cout << i << ": " << hashTable[i] << endl;
    }
  }
}

int main() {
  int hashT[Tsize];
  int choice, key;
  createHT(hashT);
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
        insert(hashT, key);
        break;

      case 2:
        cout << "Enter key to find: ";
        cin >> key;
        find(hashT, key);
        break;

      case 3:
        cout << "Enter key to delete: ";
        cin >> key;
        deleteK(hashT, key);
        break;

      case 4:
        displayHashTable(hashT);
        break;

      case 5:
        cout << "Exiting..." << endl;
        break;

      default:
        cout << "Invalid choice" << endl;
    }
  } while (choice != 5);
  return 0;
}