// 23. Department maintains a student information. The file contains roll number, name, division and address. Allow user to add, delete information of student. Display information of particular employee. If record of student does not exist an appropriate message is displayed. If it is, then the system displays the student details. Use sequential file to main the data.

#include <fstream>
#include <iostream>
#include <string>
using namespace std;

static const char *DATA_FILE = "F23_Students.txt";
static const char *TEMP_FILE = "F23_Students_tmp.txt";

struct Student {
	int roll;
	char division;
	string name;
	string address;
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

char readDivision(const string &prompt) {
	while (true) {
		cout << prompt;
		string s;
		cin >> s;
		if (s.size() >= 1) return s[0];
		cout << "Invalid division. Try again.\n";
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

bool parseStudentLine(const string &line, Student &st) {
	// Format: roll|division|name|address
	size_t p1 = line.find('|');
	if (p1 == string::npos) return false;
	size_t p2 = line.find('|', p1 + 1);
	if (p2 == string::npos) return false;
	size_t p3 = line.find('|', p2 + 1);
	if (p3 == string::npos) return false;

	string rollStr = line.substr(0, p1);
	string divStr = line.substr(p1 + 1, p2 - (p1 + 1));
	string nameStr = line.substr(p2 + 1, p3 - (p2 + 1));
	string addrStr = line.substr(p3 + 1);

	if (rollStr.size() == 0 || divStr.size() == 0) return false;

	int roll = 0;
	bool neg = false;
	for (size_t i = 0; i < rollStr.size(); ++i) {
		char c = rollStr[i];
		if (i == 0 && c == '-') {
			neg = true;
			continue;
		}
		if (c < '0' || c > '9') return false;
		roll = roll * 10 + (c - '0');
	}
	if (neg) roll = -roll;

	st.roll = roll;
	st.division = divStr[0];
	st.name = nameStr;
	st.address = addrStr;
	return true;
}

string toStudentLine(const Student &st) {
	return to_string(st.roll) + "|" + string(1, st.division) + "|" + sanitizeField(st.name) + "|" + sanitizeField(st.address);
}

bool fileExistsAndReadable(const char *path) {
	ifstream f(path);
	return (bool)f;
}

bool findStudentByRoll(int roll, Student &out) {
	ifstream in(DATA_FILE);
	if (!in) return false;
	string line;
	while (getline(in, line)) {
		Student s;
		if (!parseStudentLine(line, s)) continue;
		if (s.roll == roll) {
			out = s;
			return true;
		}
	}
	return false;
}

void addStudent() {
	Student st;
	cout << "\n--- Add Student ---\n";
	st.roll = readInt("Roll number: ");
	if (st.roll <= 0) {
		cout << "Roll number should be positive.\n";
		return;
	}
	Student existing;
	if (findStudentByRoll(st.roll, existing)) {
		cout << "Student with roll " << st.roll << " already exists.\n";
		return;
	}
	st.name = readLine("Name: ");
	st.division = readDivision("Division (single char): ");
	st.address = readLine("Address: ");

	ofstream out(DATA_FILE, ios::app);
	if (!out) {
		cout << "Unable to open data file for writing.\n";
		return;
	}
	out << toStudentLine(st) << "\n";
	cout << "Record added.\n";
}

void displayStudent(const Student &st) {
	cout << "\nRoll: " << st.roll << "\n";
	cout << "Name: " << st.name << "\n";
	cout << "Division: " << st.division << "\n";
	cout << "Address: " << st.address << "\n";
}

void searchStudent() {
	cout << "\n--- Search Student ---\n";
	int roll = readInt("Enter roll number: ");
	Student st;
	if (!findStudentByRoll(roll, st)) {
		cout << "Record not found.\n";
		return;
	}
	cout << "Record found.\n";
	displayStudent(st);
}

void displayAll() {
	cout << "\n--- All Students ---\n";
	ifstream in(DATA_FILE);
	if (!in) {
		cout << "No data file found / empty database.\n";
		return;
	}
	cout << "Roll\tDiv\tName\tAddress\n";
	string line;
	int shown = 0;
	while (getline(in, line)) {
		Student st;
		if (!parseStudentLine(line, st)) continue;
		cout << st.roll << "\t" << st.division << "\t" << st.name << "\t" << st.address << "\n";
		shown++;
	}
	if (shown == 0) cout << "(no records)\n";
}

void deleteStudent() {
	cout << "\n--- Delete Student ---\n";
	int roll = readInt("Enter roll number to delete: ");
	ifstream in(DATA_FILE);
	if (!in) {
		cout << "No data file found.\n";
		return;
	}
	ofstream temp(TEMP_FILE, ios::trunc);
	if (!temp) {
		cout << "Unable to open temp file.\n";
		return;
	}

	bool deleted = false;
	string line;
	while (getline(in, line)) {
		Student st;
		if (!parseStudentLine(line, st)) {
			// keep malformed lines as-is
			temp << line << "\n";
			continue;
		}
		if (st.roll == roll) {
			deleted = true;
			continue;
		}
		temp << toStudentLine(st) << "\n";
	}
	in.close();
	temp.close();

	if (!deleted) {
		cout << "Record not found. Nothing deleted.\n";
		return;
	}

	// Replace original file with temp
	ifstream checkTemp(TEMP_FILE);
	if (!checkTemp) {
		cout << "Temp file missing; cannot finalize delete.\n";
		return;
	}
	checkTemp.close();

	// Overwrite original file with temp contents (portable, no rename needed)
	ifstream tempIn(TEMP_FILE);
	ofstream out(DATA_FILE, ios::trunc);
	string l;
	while (getline(tempIn, l)) out << l << "\n";
	tempIn.close();
	out.close();

	cout << "Record deleted.\n";
}

int main() {
	while (true) {
		cout << "\n===== Group F23 (Sequential File - Student Info) =====\n";
		cout << "1. Add student\n";
		cout << "2. Delete student\n";
		cout << "3. Display student (by roll)\n";
		cout << "4. Display all\n";
		cout << "5. Exit\n";
		int ch = readInt("Enter choice: ");

		if (ch == 1) addStudent();
		else if (ch == 2) deleteStudent();
		else if (ch == 3) searchStudent();
		else if (ch == 4) displayAll();
		else if (ch == 5) break;
		else cout << "Invalid choice.\n";
	}
	return 0;
}
