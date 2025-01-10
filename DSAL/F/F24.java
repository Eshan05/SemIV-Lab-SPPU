// 24. Company maintains employee information as employee ID, name, designation and salary. Allow user to add, delete information of employee. Display information of particular employee. If employee does not exist an appropriate message is displayed. If it is, then the system displays the employee details. Use index sequential file to maintain the data.

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.io.RandomAccessFile;

public class F24 {
	static final String DATA_FILE = "F24_Employees.txt";
	static final String INDEX_FILE = "F24_Index.txt";
	static final int MAXR = 300;

	static BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

	static class Employee {
		int id;
		String name;
		String designation;
		double salary;
	}

	static int readInt(String prompt) throws Exception {
		while (true) {
			System.out.print(prompt);
			String s = br.readLine();
			if (s == null) return 0;
			s = s.trim();
			try {
				return Integer.parseInt(s);
			} catch (Exception e) {
				System.out.println("Invalid number. Try again.");
			}
		}
	}

	static double readDouble(String prompt) throws Exception {
		while (true) {
			System.out.print(prompt);
			String s = br.readLine();
			if (s == null) return 0.0;
			s = s.trim();
			try {
				return Double.parseDouble(s);
			} catch (Exception e) {
				System.out.println("Invalid number. Try again.");
			}
		}
	}

	static String readLine(String prompt) throws Exception {
		System.out.print(prompt);
		String s = br.readLine();
		return (s == null) ? "" : s;
	}

	static String sanitizeField(String s) {
		if (s == null) return "";
		StringBuilder out = new StringBuilder();
		for (int i = 0; i < s.length(); i++) {
			char c = s.charAt(i);
			if (c == '|') c = '/';
			if (c == '\r' || c == '\n') c = ' ';
			out.append(c);
		}
		return out.toString();
	}

	static boolean parseEmployeeLine(String line, Employee e) {
		// id|name|designation|salary
		if (line == null) return false;
		int p1 = line.indexOf('|');
		if (p1 < 0) return false;
		int p2 = line.indexOf('|', p1 + 1);
		if (p2 < 0) return false;
		int p3 = line.indexOf('|', p2 + 1);
		if (p3 < 0) return false;

		String idStr = line.substring(0, p1).trim();
		String nameStr = line.substring(p1 + 1, p2);
		String desigStr = line.substring(p2 + 1, p3);
		String salStr = line.substring(p3 + 1).trim();
		try {
			e.id = Integer.parseInt(idStr);
			e.salary = Double.parseDouble(salStr);
		} catch (Exception ex) {
			return false;
		}
		e.name = nameStr;
		e.designation = desigStr;
		return true;
	}

	static String toEmployeeLine(Employee e) {
		return e.id + "|" + sanitizeField(e.name) + "|" + sanitizeField(e.designation) + "|" + e.salary;
	}

	static int loadEmployees(Employee[] arr) throws Exception {
		File f = new File(DATA_FILE);
		if (!f.exists()) return 0;
		BufferedReader r = new BufferedReader(new FileReader(f));
		String line;
		int n = 0;
		while ((line = r.readLine()) != null && n < MAXR) {
			Employee e = new Employee();
			if (!parseEmployeeLine(line, e)) continue;
			arr[n++] = e;
		}
		r.close();
		return n;
	}

	static void sortById(Employee[] arr, int n) {
		for (int i = 0; i < n - 1; i++) {
			for (int j = 0; j < n - i - 1; j++) {
				if (arr[j].id > arr[j + 1].id) {
					Employee t = arr[j];
					arr[j] = arr[j + 1];
					arr[j + 1] = t;
				}
			}
		}
	}

	static void rewriteDataAndIndex(Employee[] arr, int n) throws Exception {
		sortById(arr, n);

		RandomAccessFile data = new RandomAccessFile(DATA_FILE, "rw");
		data.setLength(0);
		BufferedWriter idx = new BufferedWriter(new FileWriter(INDEX_FILE, false));

		for (int i = 0; i < n; i++) {
			long offset = data.getFilePointer();
			String line = toEmployeeLine(arr[i]) + "\n";
			data.writeBytes(line);
			idx.write(arr[i].id + " " + offset);
			idx.newLine();
		}

		idx.close();
		data.close();
	}

	static int loadIndex(int[] ids, long[] offsets) throws Exception {
		File f = new File(INDEX_FILE);
		if (!f.exists()) return 0;
		BufferedReader r = new BufferedReader(new FileReader(f));
		String line;
		int n = 0;
		while ((line = r.readLine()) != null && n < MAXR) {
			line = line.trim();
			if (line.length() == 0) continue;
			int sp = line.indexOf(' ');
			if (sp < 0) continue;
			try {
				ids[n] = Integer.parseInt(line.substring(0, sp).trim());
				offsets[n] = Long.parseLong(line.substring(sp + 1).trim());
				n++;
			} catch (Exception e) {
				// ignore
			}
		}
		r.close();
		return n;
	}

	static int binarySearch(int[] ids, int n, int target, int[] comparisonsOut) {
		int lo = 0;
		int hi = n - 1;
		int comps = 0;
		while (lo <= hi) {
			int mid = lo + (hi - lo) / 2;
			comps++;
			if (ids[mid] == target) {
				comparisonsOut[0] = comps;
				return mid;
			}
			if (ids[mid] < target) lo = mid + 1;
			else hi = mid - 1;
		}
		comparisonsOut[0] = comps;
		return -1;
	}

	static boolean fetchEmployeeUsingIndex(int id, Employee out, int[] comparisonsOut) throws Exception {
		int[] ids = new int[MAXR];
		long[] offs = new long[MAXR];
		int n = loadIndex(ids, offs);
		if (n == 0) {
			comparisonsOut[0] = 0;
			return false;
		}
		int[] compsTmp = new int[1];
		int pos = binarySearch(ids, n, id, compsTmp);
		comparisonsOut[0] = compsTmp[0];
		if (pos < 0) return false;

		RandomAccessFile data = new RandomAccessFile(DATA_FILE, "r");
		data.seek(offs[pos]);
		String line = data.readLine();
		data.close();
		if (line == null) return false;
		Employee e = new Employee();
		if (!parseEmployeeLine(line, e)) return false;
		out.id = e.id;
		out.name = e.name;
		out.designation = e.designation;
		out.salary = e.salary;
		return true;
	}

	static void ensureSortedAndIndexed() throws Exception {
		Employee[] arr = new Employee[MAXR];
		int n = loadEmployees(arr);
		if (n == 0) {
			new BufferedWriter(new FileWriter(INDEX_FILE, false)).close();
			return;
		}
		rewriteDataAndIndex(arr, n);
	}

	static void showEmployee(Employee e) {
		System.out.println("\nID: " + e.id);
		System.out.println("Name: " + e.name);
		System.out.println("Designation: " + e.designation);
		System.out.println("Salary: " + e.salary);
	}

	static void addEmployee() throws Exception {
		Employee[] arr = new Employee[MAXR];
		int n = loadEmployees(arr);
		System.out.println("\n--- Add Employee ---");
		int id = readInt("Employee ID: ");
		if (id <= 0) {
			System.out.println("ID should be positive.");
			return;
		}
		for (int i = 0; i < n; i++) {
			if (arr[i].id == id) {
				System.out.println("Employee ID already exists.");
				return;
			}
		}
		if (n >= MAXR) {
			System.out.println("Maximum records reached.");
			return;
		}
		Employee e = new Employee();
		e.id = id;
		e.name = readLine("Name: ");
		e.designation = readLine("Designation: ");
		e.salary = readDouble("Salary: ");
		arr[n++] = e;
		rewriteDataAndIndex(arr, n);
		System.out.println("Record added and index updated.");
	}

	static void deleteEmployee() throws Exception {
		Employee[] arr = new Employee[MAXR];
		int n = loadEmployees(arr);
		if (n == 0) {
			System.out.println("No records.");
			return;
		}
		System.out.println("\n--- Delete Employee ---");
		int id = readInt("Employee ID to delete: ");
		int pos = -1;
		for (int i = 0; i < n; i++) {
			if (arr[i].id == id) {
				pos = i;
				break;
			}
		}
		if (pos < 0) {
			System.out.println("Employee not found.");
			return;
		}
		for (int i = pos; i < n - 1; i++) arr[i] = arr[i + 1];
		n--;
		rewriteDataAndIndex(arr, n);
		System.out.println("Record deleted and index updated.");
	}

	static void searchEmployee() throws Exception {
		System.out.println("\n--- Search Employee (using index) ---");
		int id = readInt("Employee ID: ");
		Employee e = new Employee();
		int[] comps = new int[1];
		if (!fetchEmployeeUsingIndex(id, e, comps)) {
			System.out.println("Employee not found.");
			System.out.println("Comparisons (binary search on index): " + comps[0]);
			return;
		}
		System.out.println("Employee found.");
		showEmployee(e);
		System.out.println("Comparisons (binary search on index): " + comps[0]);
	}

	static void displayAll() throws Exception {
		System.out.println("\n--- All Employees (Sequential Data File) ---");
		File f = new File(DATA_FILE);
		if (!f.exists()) {
			System.out.println("No data file found / empty database.");
			return;
		}
		BufferedReader r = new BufferedReader(new FileReader(f));
		System.out.println("ID\tName\tDesignation\tSalary");
		String line;
		int shown = 0;
		while ((line = r.readLine()) != null) {
			Employee e = new Employee();
			if (!parseEmployeeLine(line, e)) continue;
			System.out.println(e.id + "\t" + e.name + "\t" + e.designation + "\t" + e.salary);
			shown++;
		}
		r.close();
		if (shown == 0) System.out.println("(no records)");
	}

	static void displayIndex() throws Exception {
		System.out.println("\n--- Index File (id -> byte offset) ---");
		File f = new File(INDEX_FILE);
		if (!f.exists()) {
			System.out.println("No index file found.");
			return;
		}
		BufferedReader r = new BufferedReader(new FileReader(f));
		System.out.println("ID\tOffset");
		String line;
		int shown = 0;
		while ((line = r.readLine()) != null) {
			line = line.trim();
			if (line.length() == 0) continue;
			System.out.println(line.replace(' ', '\t'));
			shown++;
		}
		r.close();
		if (shown == 0) System.out.println("(empty index)");
	}

	public static void main(String[] args) throws Exception {
		ensureSortedAndIndexed();
		while (true) {
			System.out.println("\n===== Group F24 (Index Sequential File - Employee Info) =====");
			System.out.println("1. Add employee");
			System.out.println("2. Delete employee");
			System.out.println("3. Search employee by ID");
			System.out.println("4. Display all employees");
			System.out.println("5. Display index file");
			System.out.println("6. Exit");
			int ch = readInt("Enter choice: ");
			if (ch == 1) addEmployee();
			else if (ch == 2) deleteEmployee();
			else if (ch == 3) searchEmployee();
			else if (ch == 4) displayAll();
			else if (ch == 5) displayIndex();
			else if (ch == 6) break;
			else System.out.println("Invalid choice.");
		}
	}
}
