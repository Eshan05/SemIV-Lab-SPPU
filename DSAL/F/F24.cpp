// 24. Company maintains employee information as employee ID, name, designation and salary. Allow user to add, delete information of employee. Display information of particular employee. If employee does not exist an appropriate message is displayed. If it is, then the system displays the employee details. Use index sequential file to maintain the data.

#include <fstream>
#include <iostream>
#include <string>
using namespace std;

static const char *DATA_FILE = "F24_Employees.txt";
static const char *INDEX_FILE = "F24_Index.txt";

const int MAXR = 300;

struct Employee {
  int id;
  string name;
  string designation;
  double salary;
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

double readDouble(const string &prompt) {
  double x;
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

string sanitizeField(const string &s) {
  string out = s;
  for (size_t i = 0; i < out.size(); ++i) {
    if (out[i] == '|') out[i] = '/';
    if (out[i] == '\r' || out[i] == '\n') out[i] = ' ';
  }
  return out;
}

bool parseEmployeeLine(const string &line, Employee &e) {
  // id|name|designation|salary
  size_t p1 = line.find('|');
  if (p1 == string::npos) return false;
  size_t p2 = line.find('|', p1 + 1);
  if (p2 == string::npos) return false;
  size_t p3 = line.find('|', p2 + 1);
  if (p3 == string::npos) return false;

  string idStr = line.substr(0, p1);
  string nameStr = line.substr(p1 + 1, p2 - (p1 + 1));
  string desigStr = line.substr(p2 + 1, p3 - (p2 + 1));
  string salStr = line.substr(p3 + 1);

  // parse id
  int id = 0;
  bool neg = false;
  if (idStr.size() == 0) return false;
  for (size_t i = 0; i < idStr.size(); ++i) {
    char c = idStr[i];
    if (i == 0 && c == '-') {
      neg = true;
      continue;
    }
    if (c < '0' || c > '9') return false;
    id = id * 10 + (c - '0');
  }
  if (neg) id = -id;

  // parse salary (simple)
  double salary = 0;
  try {
    salary = stod(salStr);
  } catch (...) {
    return false;
  }

  e.id = id;
  e.name = nameStr;
  e.designation = desigStr;
  e.salary = salary;
  return true;
}

string toEmployeeLine(const Employee &e) {
  return to_string(e.id) + "|" + sanitizeField(e.name) + "|" + sanitizeField(e.designation) + "|" + to_string(e.salary);
}

int loadEmployees(Employee arr[]) {
  ifstream in(DATA_FILE, ios::binary);
  if (!in) return 0;
  int n = 0;
  string line;
  while (getline(in, line) && n < MAXR) {
    Employee e;
    if (!parseEmployeeLine(line, e)) continue;
    arr[n++] = e;
  }
  return n;
}

void sortById(Employee arr[], int n) {
  for (int i = 0; i < n - 1; ++i) {
    for (int j = 0; j < n - i - 1; ++j) {
      if (arr[j].id > arr[j + 1].id) {
        Employee t = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = t;
      }
    }
  }
}

void rewriteDataAndIndex(Employee arr[], int n) {
  sortById(arr, n);

  ofstream data(DATA_FILE, ios::trunc | ios::binary);
  ofstream idx(INDEX_FILE, ios::trunc);
  if (!data || !idx) {
    cout << "Unable to write data/index files.\n";
    return;
  }

  for (int i = 0; i < n; ++i) {
    long long offset = (long long)data.tellp();
    string line = toEmployeeLine(arr[i]);
    data << line << "\n";
    idx << arr[i].id << " " << offset << "\n";
  }
}

int loadIndex(int ids[], long long offsets[]) {
  ifstream in(INDEX_FILE);
  if (!in) return 0;
  int n = 0;
  while (n < MAXR && (in >> ids[n] >> offsets[n])) {
    n++;
  }
  return n;
}

int binarySearchId(const int ids[], int n, int target, int &comparisons) {
  comparisons = 0;
  int lo = 0, hi = n - 1;
  while (lo <= hi) {
    int mid = lo + (hi - lo) / 2;
    comparisons++;
    if (ids[mid] == target) return mid;
    if (ids[mid] < target) lo = mid + 1;
    else hi = mid - 1;
  }
  return -1;
}

void showEmployee(const Employee &e) {
  cout << "\nID: " << e.id << "\n";
  cout << "Name: " << e.name << "\n";
  cout << "Designation: " << e.designation << "\n";
  cout << "Salary: " << e.salary << "\n";
}

bool fetchEmployeeUsingIndex(int id, Employee &out, int &comparisons) {
  int ids[MAXR];
  long long offs[MAXR];
  int n = loadIndex(ids, offs);
  if (n == 0) {
    comparisons = 0;
    return false;
  }
  int pos = binarySearchId(ids, n, id, comparisons);
  if (pos < 0) return false;

  ifstream data(DATA_FILE, ios::binary);
  if (!data) return false;
  data.seekg(offs[pos], ios::beg);
  string line;
  getline(data, line);
  Employee e;
  if (!parseEmployeeLine(line, e)) return false;
  out = e;
  return true;
}

void ensureSortedAndIndexed() {
  Employee arr[MAXR];
  int n = loadEmployees(arr);
  if (n == 0) {
    // Create empty index too
    ofstream idx(INDEX_FILE, ios::trunc);
    return;
  }
  rewriteDataAndIndex(arr, n);
}

void addEmployee() {
  Employee arr[MAXR];
  int n = loadEmployees(arr);
  sortById(arr, n);

  cout << "\n--- Add Employee ---\n";
  Employee e;
  e.id = readInt("Employee ID: ");
  if (e.id <= 0) {
    cout << "ID should be positive.\n";
    return;
  }
  for (int i = 0; i < n; ++i) {
    if (arr[i].id == e.id) {
      cout << "Employee ID already exists.\n";
      return;
    }
  }
  if (n >= MAXR) {
    cout << "Maximum records reached.\n";
    return;
  }
  e.name = readLine("Name: ");
  e.designation = readLine("Designation: ");
  e.salary = readDouble("Salary: ");

  arr[n++] = e;
  rewriteDataAndIndex(arr, n);
  cout << "Record added and index updated.\n";
}

void deleteEmployee() {
  Employee arr[MAXR];
  int n = loadEmployees(arr);
  if (n == 0) {
    cout << "No records.\n";
    return;
  }
  cout << "\n--- Delete Employee ---\n";
  int id = readInt("Employee ID to delete: ");
  int pos = -1;
  for (int i = 0; i < n; ++i) {
    if (arr[i].id == id) {
      pos = i;
      break;
    }
  }
  if (pos < 0) {
    cout << "Employee not found.\n";
    return;
  }
  for (int i = pos; i < n - 1; ++i) arr[i] = arr[i + 1];
  n--;
  rewriteDataAndIndex(arr, n);
  cout << "Record deleted and index updated.\n";
}

void searchEmployee() {
  cout << "\n--- Search Employee (using index) ---\n";
  int id = readInt("Employee ID: ");
  Employee e;
  int comps = 0;
  if (!fetchEmployeeUsingIndex(id, e, comps)) {
    cout << "Employee not found.\n";
    cout << "Comparisons (binary search on index): " << comps << "\n";
    return;
  }
  cout << "Employee found.\n";
  showEmployee(e);
  cout << "Comparisons (binary search on index): " << comps << "\n";
}

void displayAll() {
  cout << "\n--- All Employees (Sequential Data File) ---\n";
  ifstream in(DATA_FILE, ios::binary);
  if (!in) {
    cout << "No data file found / empty database.\n";
    return;
  }
  cout << "ID\tName\tDesignation\tSalary\n";
  string line;
  int shown = 0;
  while (getline(in, line)) {
    Employee e;
    if (!parseEmployeeLine(line, e)) continue;
    cout << e.id << "\t" << e.name << "\t" << e.designation << "\t" << e.salary << "\n";
    shown++;
  }
  if (shown == 0) cout << "(no records)\n";
}

void displayIndex() {
  cout << "\n--- Index File (id -> byte offset) ---\n";
  ifstream in(INDEX_FILE);
  if (!in) {
    cout << "No index file found.\n";
    return;
  }
  cout << "ID\tOffset\n";
  int id;
  long long off;
  int shown = 0;
  while (in >> id >> off) {
    cout << id << "\t" << off << "\n";
    shown++;
  }
  if (shown == 0) cout << "(empty index)\n";
}

int main() {
  ensureSortedAndIndexed();

  while (true) {
    cout << "\n===== Group F24 (Index Sequential File - Employee Info) =====\n";
    cout << "1. Add employee\n";
    cout << "2. Delete employee\n";
    cout << "3. Search employee by ID\n";
    cout << "4. Display all employees\n";
    cout << "5. Display index file\n";
    cout << "6. Exit\n";
    int ch = readInt("Enter choice: ");

    if (ch == 1) addEmployee();
    else if (ch == 2) deleteEmployee();
    else if (ch == 3) searchEmployee();
    else if (ch == 4) displayAll();
    else if (ch == 5) displayIndex();
    else if (ch == 6) break;
    else cout << "Invalid choice.\n";
  }
  return 0;
}
