#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int sizeT = 10;

int foldShiftHash(int key) {
  string keyStr = to_string(key);
  int chunk_size = to_string(sizeT).length();
  int hashValue = 0;

  for (int i = 0; i < keyStr.length(); i += chunk_size) {
    int end = min((int)keyStr.length(), i + chunk_size);
    string chunk = keyStr.substr(i, end - i); // Extract
    // Padding
    while (chunk.length() < chunk_size)
      chunk = "0" + chunk;
    hashValue += stoi(chunk);
  }

  return hashValue % sizeT;
}

int foldBoundaryHash(int key) {
  string keyStr = to_string(key);
  int chunk_size = to_string(sizeT).length();
  int hashValue = 0;
  int chunk_count = 0;

  for (int i = 0; i < keyStr.length(); i += chunk_size) {
    int end = min((int)keyStr.length(), i + chunk_size);
    string chunk = keyStr.substr(i, end - i);

    while (chunk.length() < chunk_size) {
      chunk = "0" + chunk;
    }

    if (chunk_count % 2 == 1)
      reverse(chunk.begin(), chunk.end()); // Reverse the chunk if it's an odd-numbered chunk
    hashValue += stoi(chunk);
    chunk_count++;
  }

  return hashValue % sizeT;
}

int hFn(int k, int hashType) {
  switch (hashType) {
    case 1:
      return foldShiftHash(k);
    case 2:
      return foldBoundaryHash(k);
    default:
      return k % sizeT;
  }
}

void createHT(vector<int> hT[]) {
  for (int i = 0; i < sizeT; i++) {
    hT[i].clear();
  }
}

void insert(vector<int> hT[], int k, int hashType) {
  int idx = hFn(k, hashType);
  hT[idx].push_back(k);
  cout << "Inserted " << k << " at index " << idx << endl;
}

void find(vector<int> hT[], int k, int hashType) {
  int idx = hFn(k, hashType);
  for (auto it = hT[idx].begin(); it != hT[idx].end(); it++) {
    if (*it == k) {
      cout << "Found " << k << " at index " << idx << endl;
      return;
    }
  }
  cout << "Key not found" << endl;
}

void deleteK(vector<int> hT[], int key, int hashType) {
  int idx = hFn(key, hashType);
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
  vector<int> hashT[sizeT]; // Not vector of arrays
  int ch, k, hashType;
  createHT(hashT);

  cout << "Choose a hash function:" << endl;
  cout << "1. Fold Shift" << endl;
  cout << "2. Fold Boundary" << endl;
  cout << "Enter your choice: ";
  cin >> hashType;

  do {
    cout << "\n-> HT Implementation <-: " << endl;
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
        insert(hashT, k, hashType);
        break;

      case 2:
        cout << "Enter key to find: ";
        cin >> k;
        find(hashT, k, hashType);
        break;

      case 3:
        cout << "Enter key to delete: ";
        cin >> k;
        deleteK(hashT, k, hashType);
        break;

      case 4: displayHashTable(hashT); break;
      case 5: cout << "Exiting..." << endl; break;
      default: cout << "Invalid choice" << endl;
    }
  } while (ch != 5);
  return 0;
}
