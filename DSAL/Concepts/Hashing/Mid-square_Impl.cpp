#include <cmath>
#include <iostream>
#include <vector>
using namespace std;

const int sizeT = 10;

int hFn(int k) {
  long long sq = (long long)k * k;

  int numDigits = 0;
  int tempSizeT = sizeT;
  while (tempSizeT != 0) {
    tempSizeT /= 10;
    numDigits++;
  }

  // (int)(log10(sq) + 1)depicts the number of digits in the square
  // Then we divide by 2 to get the middle digit position
  int shift = (numDigits > 0) ? ((int)(log10(sq) + 1) / 2) - (numDigits / 2)
                              : 0;
  if (shift < 0) shift = 0;
  long long divisor = (long long)pow(10, shift);
  int mid = (sq / divisor) % sizeT;
  return mid;

  // EX: k = 1234, sizeT = 100
  // sq = 1522756, numDigits = 2
  // shift = (log10(1522756) + 1) / 2) - (2 / 2)
  //       = (6 + 1) / 2 - 1 = 3 - 1 = 2
  // divisor = 10^2 = 100, mid = (1522756 / 100) % 100 = 15227 % 100 = 27
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
