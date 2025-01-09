#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
using namespace std;

class UH {
 private:
  int Tsize;
  vector<int> *hT;
  int a, b, p;

  int hFn(int k) {
    return ((a * k + b) % p + p) % p % Tsize;
  }

  void generateRandomParams() {
    a = rand() % p;
    b = rand() % p;
  }

 public:
  UH(int s) : Tsize(s) {
    hT = new vector<int>[Tsize];
    p = 1000000007;
    srand(time(0));
    generateRandomParams();
  }

  ~UH() { delete[] hT; }

  void insert(int k) {
    int idx = hFn(k);
    if (hT[idx].size() > 5) {
      idx = (idx + 1) % Tsize;
    }
    hT[idx].push_back(k);
    cout << "Inserted key " << k << " at index " << idx << endl;
  }

  void deleteK(int k) {
    int idx = hFn(k);
    vector<int> &chain = hT[idx];
    chain.erase(remove(chain.begin(), chain.end(), k), chain.end());
    cout << "Deleted key " << k << " from index " << idx << endl;
  }

  bool search(int k) {
    int idx = hFn(k);
    // for (int k : hashTable[index])
    for (auto it = hT[idx].begin(); it != hT[idx].end(); it++) {
      if (*it == k) {
        cout << "Found key " << k << " at index " << idx << endl;
        return true;
      }
    }
    cout << "Key not found" << endl;
    return false;
  }

  void displayHT() {
    cout << "Hash Table:" << endl;
    for (int i = 0; i < Tsize; i++) {
      cout << i << ": ";
      if (hT[i].empty()) {
        cout << "Empty" << endl;
      } else {
        for (int value : hT[i]) {
          cout << value << " ";
        }
        cout << endl;
      }
    }
  }

  void newHFn() {
    generateRandomParams();
    cout << "New hash function parameters: a = " << a << ", b = " << b << endl;
  }
};

int main() {
  int Tsize, ch, k;
  cout << "Enter the size of the hash table: ";
  cin >> Tsize;
  UH uHash(Tsize);
  do {
    cout << "\nUniversal Hash Table Menu:\n";
    cout << "1. Insert Key\n";
    cout << "2. Delete Key\n";
    cout << "3. Search Key\n";
    cout << "4. Display Table\n";
    cout << "5. New Hash Function\n";
    cout << "6. Exit\n";
    cout << "Enter your choice: ";
    cin >> ch;

    switch (ch) {
      case 1:
        cout << "Enter key to insert: ";
        cin >> k;
        uHash.insert(k);
        break;

      case 2:
        cout << "Enter key to delete: ";
        cin >> k;
        uHash.deleteK(k);
        break;

      case 3:
        cout << "Enter key to search: ";
        cin >> k;
        uHash.search(k);
        break;

      case 4: uHash.displayHT(); break;
      case 5: uHash.newHFn(); break;
      case 6: cout << "Exiting..." << endl; break;
      default: cout << "Invalid choice." << endl; break;
    }
  } while (ch != 6);
  return 0;
}