// 25. Implementation of a direct access file -Insertion and deletion of a record from a direct access file

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

static const char *DATA_FILE = "F25_Direct.dat";

const int MAX_SLOTS = 100;
const int NAME_LEN = 30;

struct Record {
  int id;              // 0 means empty
  char status;         // 0=empty, 1=active, 2=deleted
  char name[NAME_LEN]; // fixed
};

void clearLine() {
  char c;
  while (cin.get(c)) {
    if (c == '\n') break;
  }
}

int readInt(const string &prompt) {
  int x;
  while (true) {
    cout << prompt;
    if (cin >> x) return x;
    cout << "Invalid number. Try again.\n";
    cin.clear();
    clearLine();
  }
}

string readLine(const string &prompt) {
  cout << prompt;
  string s;
  getline(cin >> ws, s);
  return s;
}

void makeEmptyRecord(Record &r) {
  r.id = 0;
  r.status = 0;
  memset(r.name, 0, sizeof(r.name));
}

bool createEmptyFile() {
  ofstream out(DATA_FILE, ios::binary | ios::trunc);
  if (!out) return false;
  Record r;
  makeEmptyRecord(r);
  for (int i = 0; i < MAX_SLOTS; ++i) {
    out.write((char *)&r, sizeof(r));
  }
  return true;
}

bool ensureFileReady() {
  ifstream in(DATA_FILE, ios::binary | ios::ate);
  if (!in) return createEmptyFile();
  streamoff sz = in.tellg();
  in.close();
  streamoff expected = (streamoff)MAX_SLOTS * (streamoff)sizeof(Record);
  if (sz != expected) {
    cout << "File size mismatch. Re-initializing direct access file.\n";
    return createEmptyFile();
  }
  return true;
}

long long offsetOfId(int id) {
  return (long long)(id - 1) * (long long)sizeof(Record);
}

bool readRecordAt(fstream &f, int id, Record &out) {
  long long off = offsetOfId(id);
  f.seekg(off, ios::beg);
  if (!f.read((char *)&out, sizeof(out))) return false;
  return true;
}

bool writeRecordAt(fstream &f, int id, const Record &rec) {
  long long off = offsetOfId(id);
  f.seekp(off, ios::beg);
  if (!f.write((const char *)&rec, sizeof(rec))) return false;
  f.flush();
  return true;
}

void showRecord(const Record &r) {
  cout << "ID: " << r.id << "\n";
  cout << "Name: " << r.name << "\n";
}

void initFileMenu() {
  if (createEmptyFile()) cout << "Direct access file initialized.\n";
  else cout << "Failed to initialize file.\n";
}

void insertRecord() {
  if (!ensureFileReady()) {
    cout << "File not ready.\n";
    return;
  }
  int id = readInt("Enter ID (1-100): ");
  if (id < 1 || id > MAX_SLOTS) {
    cout << "ID out of range.\n";
    return;
  }

  fstream f(DATA_FILE, ios::in | ios::out | ios::binary);
  if (!f) {
    cout << "Unable to open file.\n";
    return;
  }

  Record cur;
  if (!readRecordAt(f, id, cur)) {
    cout << "Read error.\n";
    return;
  }
  if (cur.status == 1) {
    cout << "Slot already occupied.\n";
    int ow = readInt("Overwrite? 1.Yes 2.No : ");
    if (ow != 1) return;
  }

  string name = readLine("Enter name: ");
  Record rec;
  makeEmptyRecord(rec);
  rec.id = id;
  rec.status = 1;
  // copy name with truncation
  for (int i = 0; i < NAME_LEN - 1 && i < (int)name.size(); ++i) {
    rec.name[i] = name[i];
  }

  if (writeRecordAt(f, id, rec)) cout << "Record inserted at direct slot " << id << ".\n";
  else cout << "Write error.\n";
}

void deleteRecord() {
  if (!ensureFileReady()) {
    cout << "File not ready.\n";
    return;
  }
  int id = readInt("Enter ID to delete (1-100): ");
  if (id < 1 || id > MAX_SLOTS) {
    cout << "ID out of range.\n";
    return;
  }

  fstream f(DATA_FILE, ios::in | ios::out | ios::binary);
  if (!f) {
    cout << "Unable to open file.\n";
    return;
  }
  Record cur;
  if (!readRecordAt(f, id, cur)) {
    cout << "Read error.\n";
    return;
  }
  if (cur.status != 1) {
    cout << "Record not found (empty/deleted slot).\n";
    return;
  }
  cur.status = 2;
  if (writeRecordAt(f, id, cur)) cout << "Record deleted.\n";
  else cout << "Write error.\n";
}

void searchRecord() {
  if (!ensureFileReady()) {
    cout << "File not ready.\n";
    return;
  }
  int id = readInt("Enter ID to search (1-100): ");
  if (id < 1 || id > MAX_SLOTS) {
    cout << "ID out of range.\n";
    return;
  }
  ifstream f(DATA_FILE, ios::in | ios::binary);
  if (!f) {
    cout << "Unable to open file.\n";
    return;
  }
  Record cur;
  f.seekg(offsetOfId(id), ios::beg);
  if (!f.read((char *)&cur, sizeof(cur))) {
    cout << "Read error.\n";
    return;
  }
  if (cur.status != 1) {
    cout << "Record not found.\n";
    return;
  }
  cout << "Record found (direct access O(1)).\n";
  showRecord(cur);
}

void displayAll() {
  if (!ensureFileReady()) {
    cout << "File not ready.\n";
    return;
  }
  ifstream f(DATA_FILE, ios::in | ios::binary);
  if (!f) {
    cout << "Unable to open file.\n";
    return;
  }
  cout << "\n--- Active Records ---\n";
  cout << "Slot\tID\tName\n";
  Record cur;
  int shown = 0;
  for (int slot = 1; slot <= MAX_SLOTS; ++slot) {
    f.read((char *)&cur, sizeof(cur));
    if (!f) break;
    if (cur.status == 1) {
      cout << slot << "\t" << cur.id << "\t" << cur.name << "\n";
      shown++;
    }
  }
  if (shown == 0) cout << "(no active records)\n";
}

int main() {
  ensureFileReady();
  while (true) {
    cout << "\n===== Group F25 (Direct Access File) =====\n";
    cout << "1. Initialize/Reset file\n";
    cout << "2. Insert record\n";
    cout << "3. Delete record\n";
    cout << "4. Search/display record\n";
    cout << "5. Display all active records\n";
    cout << "6. Exit\n";
    int ch = readInt("Enter choice: ");
    if (ch == 1) initFileMenu();
    else if (ch == 2) insertRecord();
    else if (ch == 3) deleteRecord();
    else if (ch == 4) searchRecord();
    else if (ch == 5) displayAll();
    else if (ch == 6) break;
    else cout << "Invalid choice.\n";
  }
  cout << "\nComplexity: direct seek is O(1) for search/insert/delete (given valid ID range).\n";
  return 0;
}
