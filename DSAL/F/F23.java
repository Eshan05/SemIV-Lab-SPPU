// 23. Department maintains a student information. The file contains roll number, name, division and address. Allow user to add, delete information of student. Display information of particular employee. If record of student does not exist an appropriate message is displayed. If it is, then the system displays the student details. Use sequential file to main the data.

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.InputStreamReader;

public class F23 {
	static final String DATA_FILE = "F23_Students.txt";
	static final String TEMP_FILE = "F23_Students_tmp.txt";
	static BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

	static class Student {
		int roll;
		char division;
		String name;
		String address;
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

	static char readDivision(String prompt) throws Exception {
		while (true) {
			System.out.print(prompt);
			String s = br.readLine();
			if (s == null) return 'X';
			s = s.trim();
			if (s.length() >= 1) return s.charAt(0);
			System.out.println("Invalid division. Try again.");
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

	static boolean parseStudentLine(String line, Student st) {
		// roll|division|name|address
		if (line == null) return false;
		int p1 = line.indexOf('|');
		if (p1 < 0) return false;
		int p2 = line.indexOf('|', p1 + 1);
		if (p2 < 0) return false;
		int p3 = line.indexOf('|', p2 + 1);
		if (p3 < 0) return false;

		String rollStr = line.substring(0, p1).trim();
		String divStr = line.substring(p1 + 1, p2).trim();
		String nameStr = line.substring(p2 + 1, p3);
		String addrStr = line.substring(p3 + 1);
		if (rollStr.length() == 0 || divStr.length() == 0) return false;

		try {
			st.roll = Integer.parseInt(rollStr);
		} catch (Exception e) {
			return false;
		}
		st.division = divStr.charAt(0);
		st.name = nameStr;
		st.address = addrStr;
		return true;
	}

	static String toStudentLine(Student st) {
		return st.roll + "|" + st.division + "|" + sanitizeField(st.name) + "|" + sanitizeField(st.address);
	}

	static boolean findStudentByRoll(int roll, Student out) throws Exception {
		File f = new File(DATA_FILE);
		if (!f.exists()) return false;
		BufferedReader r = new BufferedReader(new FileReader(f));
		String line;
		while ((line = r.readLine()) != null) {
			Student st = new Student();
			if (!parseStudentLine(line, st)) continue;
			if (st.roll == roll) {
				out.roll = st.roll;
				out.division = st.division;
				out.name = st.name;
				out.address = st.address;
				r.close();
				return true;
			}
		}
		r.close();
		return false;
	}

	static void displayStudent(Student st) {
		System.out.println("\nRoll: " + st.roll);
		System.out.println("Name: " + st.name);
		System.out.println("Division: " + st.division);
		System.out.println("Address: " + st.address);
	}

	static void addStudent() throws Exception {
		System.out.println("\n--- Add Student ---");
		int roll = readInt("Roll number: ");
		if (roll <= 0) {
			System.out.println("Roll number should be positive.");
			return;
		}
		Student existing = new Student();
		if (findStudentByRoll(roll, existing)) {
			System.out.println("Student with roll " + roll + " already exists.");
			return;
		}
		String name = readLine("Name: ");
		char div = readDivision("Division (single char): ");
		String addr = readLine("Address: ");

		Student st = new Student();
		st.roll = roll;
		st.division = div;
		st.name = name;
		st.address = addr;

		BufferedWriter w = new BufferedWriter(new FileWriter(DATA_FILE, true));
		w.write(toStudentLine(st));
		w.newLine();
		w.close();
		System.out.println("Record added.");
	}

	static void searchStudent() throws Exception {
		System.out.println("\n--- Search Student ---");
		int roll = readInt("Enter roll number: ");
		Student st = new Student();
		if (!findStudentByRoll(roll, st)) {
			System.out.println("Record not found.");
			return;
		}
		System.out.println("Record found.");
		displayStudent(st);
	}

	static void displayAll() throws Exception {
		System.out.println("\n--- All Students ---");
		File f = new File(DATA_FILE);
		if (!f.exists()) {
			System.out.println("No data file found / empty database.");
			return;
		}
		BufferedReader r = new BufferedReader(new FileReader(f));
		System.out.println("Roll\tDiv\tName\tAddress");
		int shown = 0;
		String line;
		while ((line = r.readLine()) != null) {
			Student st = new Student();
			if (!parseStudentLine(line, st)) continue;
			System.out.println(st.roll + "\t" + st.division + "\t" + st.name + "\t" + st.address);
			shown++;
		}
		r.close();
		if (shown == 0) System.out.println("(no records)");
	}

	static void copyFile(String src, String dst) throws Exception {
		FileInputStream in = new FileInputStream(src);
		FileOutputStream out = new FileOutputStream(dst);
		byte[] buf = new byte[8192];
		int n;
		while ((n = in.read(buf)) > 0) out.write(buf, 0, n);
		in.close();
		out.close();
	}

	static void deleteStudent() throws Exception {
		System.out.println("\n--- Delete Student ---");
		int roll = readInt("Enter roll number to delete: ");
		File f = new File(DATA_FILE);
		if (!f.exists()) {
			System.out.println("No data file found.");
			return;
		}

		BufferedReader r = new BufferedReader(new FileReader(f));
		BufferedWriter w = new BufferedWriter(new FileWriter(TEMP_FILE, false));
		boolean deleted = false;
		String line;
		while ((line = r.readLine()) != null) {
			Student st = new Student();
			if (!parseStudentLine(line, st)) {
				w.write(line);
				w.newLine();
				continue;
			}
			if (st.roll == roll) {
				deleted = true;
				continue;
			}
			w.write(toStudentLine(st));
			w.newLine();
		}
		r.close();
		w.close();

		if (!deleted) {
			System.out.println("Record not found. Nothing deleted.");
			new File(TEMP_FILE).delete();
			return;
		}

		// Replace original with temp
		File original = new File(DATA_FILE);
		File temp = new File(TEMP_FILE);
		if (original.exists() && !original.delete()) {
			System.out.println("Warning: could not delete original file; attempting overwrite.");
			copyFile(TEMP_FILE, DATA_FILE);
			temp.delete();
			System.out.println("Record deleted.");
			return;
		}
		if (!temp.renameTo(original)) {
			// fallback
			copyFile(TEMP_FILE, DATA_FILE);
			temp.delete();
		}
		System.out.println("Record deleted.");
	}

	public static void main(String[] args) throws Exception {
		while (true) {
			System.out.println("\n===== Group F23 (Sequential File - Student Info) =====");
			System.out.println("1. Add student");
			System.out.println("2. Delete student");
			System.out.println("3. Display student (by roll)");
			System.out.println("4. Display all");
			System.out.println("5. Exit");
			int ch = readInt("Enter choice: ");

			if (ch == 1) addStudent();
			else if (ch == 2) deleteStudent();
			else if (ch == 3) searchStudent();
			else if (ch == 4) displayAll();
			else if (ch == 5) break;
			else System.out.println("Invalid choice.");
		}
	}
}
