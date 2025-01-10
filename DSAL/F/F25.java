// 25. Implementation of a direct access file -Insertion and deletion of a record from a direct access file

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.io.RandomAccessFile;

public class F25 {
	static final String DATA_FILE = "F25_Direct.dat";
	static final int MAX_SLOTS = 100;
	static final int NAME_LEN = 30;
	static final int RECORD_SIZE = 4 + 1 + NAME_LEN; // int + status + name bytes

	static BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

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

	static String readLine(String prompt) throws Exception {
		System.out.print(prompt);
		String s = br.readLine();
		return (s == null) ? "" : s;
	}

	static long offsetOfId(int id) {
		return (long) (id - 1) * (long) RECORD_SIZE;
	}

	static void writeEmptyRecord(RandomAccessFile raf) throws Exception {
		raf.writeInt(0);
		raf.writeByte(0);
		for (int i = 0; i < NAME_LEN; i++) raf.writeByte(0);
	}

	static void initFile() throws Exception {
		RandomAccessFile raf = new RandomAccessFile(DATA_FILE, "rw");
		raf.setLength(0);
		for (int i = 0; i < MAX_SLOTS; i++) writeEmptyRecord(raf);
		raf.close();
		System.out.println("Direct access file initialized.");
	}

	static boolean ensureFileReady() throws Exception {
		File f = new File(DATA_FILE);
		if (!f.exists()) {
			initFile();
			return true;
		}
		long expected = (long) MAX_SLOTS * (long) RECORD_SIZE;
		if (f.length() != expected) {
			System.out.println("File size mismatch. Re-initializing direct access file.");
			initFile();
			return true;
		}
		return true;
	}

	static void writeName(RandomAccessFile raf, String name) throws Exception {
		if (name == null) name = "";
		int n = Math.min(NAME_LEN, name.length());
		for (int i = 0; i < n; i++) raf.writeByte((byte) name.charAt(i));
		for (int i = n; i < NAME_LEN; i++) raf.writeByte(0);
	}

	static String readName(RandomAccessFile raf) throws Exception {
		byte[] buf = new byte[NAME_LEN];
		raf.readFully(buf);
		StringBuilder sb = new StringBuilder();
		for (int i = 0; i < buf.length; i++) {
			if (buf[i] == 0) break;
			sb.append((char) (buf[i] & 0xFF));
		}
		return sb.toString();
	}

	static void insertRecord() throws Exception {
		ensureFileReady();
		int id = readInt("Enter ID (1-100): ");
		if (id < 1 || id > MAX_SLOTS) {
			System.out.println("ID out of range.");
			return;
		}
		RandomAccessFile raf = new RandomAccessFile(DATA_FILE, "rw");
		raf.seek(offsetOfId(id));
		int existingId = raf.readInt();
		byte status = raf.readByte();
		String existingName = readName(raf);
		if (status == 1) {
			System.out.println("Slot already occupied: ID=" + existingId + ", Name=" + existingName);
			int ow = readInt("Overwrite? 1.Yes 2.No : ");
			if (ow != 1) {
				raf.close();
				return;
			}
		}
		String name = readLine("Enter name: ");
		raf.seek(offsetOfId(id));
		raf.writeInt(id);
		raf.writeByte(1);
		writeName(raf, name);
		raf.close();
		System.out.println("Record inserted at direct slot " + id + ".");
	}

	static void deleteRecord() throws Exception {
		ensureFileReady();
		int id = readInt("Enter ID to delete (1-100): ");
		if (id < 1 || id > MAX_SLOTS) {
			System.out.println("ID out of range.");
			return;
		}
		RandomAccessFile raf = new RandomAccessFile(DATA_FILE, "rw");
		raf.seek(offsetOfId(id));
		int existingId = raf.readInt();
		byte status = raf.readByte();
		String existingName = readName(raf);
		if (status != 1) {
			System.out.println("Record not found (empty/deleted slot).");
			raf.close();
			return;
		}
		System.out.println("Deleting: ID=" + existingId + ", Name=" + existingName);
		raf.seek(offsetOfId(id) + 4); // after int
		raf.writeByte(2);
		raf.close();
		System.out.println("Record deleted.");
	}

	static void searchRecord() throws Exception {
		ensureFileReady();
		int id = readInt("Enter ID to search (1-100): ");
		if (id < 1 || id > MAX_SLOTS) {
			System.out.println("ID out of range.");
			return;
		}
		RandomAccessFile raf = new RandomAccessFile(DATA_FILE, "r");
		raf.seek(offsetOfId(id));
		int storedId = raf.readInt();
		byte status = raf.readByte();
		String name = readName(raf);
		raf.close();
		if (status != 1) {
			System.out.println("Record not found.");
			return;
		}
		System.out.println("Record found (direct access O(1)).");
		System.out.println("ID: " + storedId);
		System.out.println("Name: " + name);
	}

	static void displayAll() throws Exception {
		ensureFileReady();
		RandomAccessFile raf = new RandomAccessFile(DATA_FILE, "r");
		System.out.println("\n--- Active Records ---");
		System.out.println("Slot\tID\tName");
		int shown = 0;
		for (int slot = 1; slot <= MAX_SLOTS; slot++) {
			raf.seek(offsetOfId(slot));
			int storedId = raf.readInt();
			byte status = raf.readByte();
			String name = readName(raf);
			if (status == 1) {
				System.out.println(slot + "\t" + storedId + "\t" + name);
				shown++;
			}
		}
		raf.close();
		if (shown == 0) System.out.println("(no active records)");
	}

	public static void main(String[] args) throws Exception {
		ensureFileReady();
		while (true) {
			System.out.println("\n===== Group F25 (Direct Access File) =====");
			System.out.println("1. Initialize/Reset file");
			System.out.println("2. Insert record");
			System.out.println("3. Delete record");
			System.out.println("4. Search/display record");
			System.out.println("5. Display all active records");
			System.out.println("6. Exit");
			int ch = readInt("Enter choice: ");
			if (ch == 1) initFile();
			else if (ch == 2) insertRecord();
			else if (ch == 3) deleteRecord();
			else if (ch == 4) searchRecord();
			else if (ch == 5) displayAll();
			else if (ch == 6) break;
			else System.out.println("Invalid choice.");
		}
		System.out.println("\nComplexity: direct seek is O(1) for search/insert/delete (given valid ID range).\n");
	}
}
