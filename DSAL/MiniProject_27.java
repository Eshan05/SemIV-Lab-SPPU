// 27. Design a mini project using JAVA which will use the different data structure with or without Java collection library and show the use of specific data structure on the efficiency (Performance) of the code.

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.InputStreamReader;

public class MiniProject_27 {
	static BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

	static int cacheCapacity = 3;
	static int[] trace = new int[0];
	static int traceLen = 0;

	static class IntList {
		int[] a;
		int n;

		IntList(int cap) {
			if (cap < 1) cap = 1;
			a = new int[cap];
			n = 0;
		}

		void add(int x) {
			if (n >= a.length) {
				int[] b = new int[a.length * 2];
				for (int i = 0; i < a.length; i++) b[i] = a[i];
				a = b;
			}
			a[n++] = x;
		}

		int[] toArray() {
			int[] out = new int[n];
			for (int i = 0; i < n; i++) out[i] = a[i];
			return out;
		}
	}

	static class Stats {
		long hits;
		long misses;
		long metric; // comparisons for naive, probes for fast
		long nanos;
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

	static String readLine(String prompt) throws Exception {
		System.out.print(prompt);
		String s = br.readLine();
		return (s == null) ? "" : s;
	}

	static int[] parseIntsFromString(String s) {
		IntList out = new IntList(16);
		if (s == null) return out.toArray();
		int i = 0;
		int len = s.length();
		while (i < len) {
			while (i < len && Character.isWhitespace(s.charAt(i))) i++;
			if (i >= len) break;
			int sign = 1;
			char c = s.charAt(i);
			if (c == '-') {
				sign = -1;
				i++;
			}
			boolean hasDigit = false;
			int val = 0;
			while (i < len) {
				char d = s.charAt(i);
				if (!Character.isDigit(d)) break;
				hasDigit = true;
				val = val * 10 + (d - '0');
				i++;
			}
			if (hasDigit) {
				out.add(val * sign);
			} else {
				while (i < len && !Character.isWhitespace(s.charAt(i))) i++;
			}
		}
		return out.toArray();
	}

	static void setCapacity() throws Exception {
		int cap = readInt("Enter cache capacity (>=0): ");
		if (cap < 0) cap = 0;
		cacheCapacity = cap;
		System.out.println("Cache capacity set to " + cacheCapacity);
	}

	static void inputTraceManual() throws Exception {
		System.out.println("\nEnter request trace as integers separated by spaces.");
		String line = readLine("Trace: ");
		int[] arr = parseIntsFromString(line);
		trace = arr;
		traceLen = arr.length;
		System.out.println("Loaded " + traceLen + " requests.");
	}

	static void loadTraceFromFile() throws Exception {
		String path = readLine("Enter file path: ").trim();
		if (path.length() == 0) {
			System.out.println("No path provided.");
			return;
		}
		File f = new File(path);
		if (!f.exists()) {
			System.out.println("File not found.");
			return;
		}
		BufferedReader r = new BufferedReader(new FileReader(f));
		IntList list = new IntList(128);
		String line;
		while ((line = r.readLine()) != null) {
			int[] arr = parseIntsFromString(line);
			for (int i = 0; i < arr.length; i++) list.add(arr[i]);
		}
		r.close();
		trace = list.toArray();
		traceLen = trace.length;
		System.out.println("Loaded " + traceLen + " requests from file.");
	}

	static void displayTrace() {
		System.out.println("\n--- Current Trace ---");
		if (traceLen == 0) {
			System.out.println("(empty)");
			return;
		}
		for (int i = 0; i < traceLen; i++) {
			System.out.print(trace[i] + " ");
			if ((i + 1) % 25 == 0) System.out.println();
		}
		System.out.println("\nTotal: " + traceLen);
	}

	static Stats simulateNaive(int[] t, int n, int cap) {
		Stats s = new Stats();
		long start = System.nanoTime();
		if (cap <= 0) {
			s.misses = n;
			s.nanos = System.nanoTime() - start;
			return s;
		}
		int[] cache = new int[cap];
		int size = 0;
		for (int i = 0; i < n; i++) {
			int key = t[i];
			int pos = -1;
			for (int j = 0; j < size; j++) {
				s.metric++; // comparisons
				if (cache[j] == key) {
					pos = j;
					break;
				}
			}
			if (pos >= 0) {
				s.hits++;
				int k = cache[pos];
				for (int j = pos; j > 0; j--) cache[j] = cache[j - 1];
				cache[0] = k;
			} else {
				s.misses++;
				if (size < cap) size++;
				for (int j = size - 1; j > 0; j--) cache[j] = cache[j - 1];
				cache[0] = key;
			}
		}
		s.nanos = System.nanoTime() - start;
		return s;
	}

	static class Node {
		int key;
		Node prev;
		Node next;
	}

	static class IntNodeMap {
		int[] keys;
		Node[] values;
		byte[] states; // 0 empty, 1 used, 2 deleted
		int size;
		int threshold;
		long probes;

		IntNodeMap(int expectedSize) {
			if (expectedSize < 4) expectedSize = 4;
			int tableSize = 1;
			while (tableSize < expectedSize * 2) tableSize <<= 1;
			keys = new int[tableSize];
			values = new Node[tableSize];
			states = new byte[tableSize];
			size = 0;
			threshold = (int) (tableSize * 7L / 10L);
			probes = 0;
		}

		int hash(int k) {
			int h = k ^ (k >>> 16);
			h *= 0x7feb352d;
			h ^= (h >>> 15);
			h *= 0x846ca68b;
			h ^= (h >>> 16);
			return h;
		}

		void rehash(int newSize) {
			int[] oldKeys = keys;
			Node[] oldVals = values;
			byte[] oldStates = states;

			keys = new int[newSize];
			values = new Node[newSize];
			states = new byte[newSize];
			size = 0;
			threshold = (int) (newSize * 7L / 10L);

			for (int i = 0; i < oldKeys.length; i++) {
				if (oldStates[i] == 1) {
					put(oldKeys[i], oldVals[i]);
				}
			}
		}

		Node get(int key) {
			int mask = keys.length - 1;
			int idx = hash(key) & mask;
			while (true) {
				probes++;
				byte st = states[idx];
				if (st == 0) return null;
				if (st == 1 && keys[idx] == key) return values[idx];
				idx = (idx + 1) & mask;
			}
		}

		void put(int key, Node val) {
			if (size + 1 > threshold) rehash(keys.length * 2);
			int mask = keys.length - 1;
			int idx = hash(key) & mask;
			int firstDel = -1;
			while (true) {
				probes++;
				byte st = states[idx];
				if (st == 0) {
					int ins = (firstDel >= 0) ? firstDel : idx;
					states[ins] = 1;
					keys[ins] = key;
					values[ins] = val;
					size++;
					return;
				}
				if (st == 1 && keys[idx] == key) {
					values[idx] = val;
					return;
				}
				if (st == 2 && firstDel < 0) firstDel = idx;
				idx = (idx + 1) & mask;
			}
		}

		Node remove(int key) {
			int mask = keys.length - 1;
			int idx = hash(key) & mask;
			while (true) {
				probes++;
				byte st = states[idx];
				if (st == 0) return null;
				if (st == 1 && keys[idx] == key) {
					Node v = values[idx];
					states[idx] = 2;
					values[idx] = null;
					size--;
					return v;
				}
				idx = (idx + 1) & mask;
			}
		}
	}

	static class LRUFast {
		int capacity;
		int size;
		Node head;
		Node tail;
		IntNodeMap map;

		LRUFast(int cap) {
			capacity = cap;
			size = 0;
			head = new Node();
			tail = new Node();
			head.next = tail;
			tail.prev = head;
			map = new IntNodeMap(cap * 2 + 8);
		}

		void removeNode(Node n) {
			n.prev.next = n.next;
			n.next.prev = n.prev;
			n.prev = null;
			n.next = null;
		}

		void addToFront(Node n) {
			n.prev = head;
			n.next = head.next;
			head.next.prev = n;
			head.next = n;
		}

		void moveToFront(Node n) {
			removeNode(n);
			addToFront(n);
		}

		Node removeTail() {
			Node last = tail.prev;
			if (last == head) return null;
			removeNode(last);
			return last;
		}

		boolean access(int key) {
			Node n = map.get(key);
			if (n != null) {
				moveToFront(n);
				return true;
			}
			Node nn = new Node();
			nn.key = key;
			addToFront(nn);
			map.put(key, nn);
			size++;
			if (size > capacity) {
				Node ev = removeTail();
				if (ev != null) {
					map.remove(ev.key);
					size--;
				}
			}
			return false;
		}
	}

	static Stats simulateFast(int[] t, int n, int cap) {
		Stats s = new Stats();
		long start = System.nanoTime();
		if (cap <= 0) {
			s.misses = n;
			s.nanos = System.nanoTime() - start;
			return s;
		}
		LRUFast cache = new LRUFast(cap);
		for (int i = 0; i < n; i++) {
			if (cache.access(t[i])) s.hits++;
			else s.misses++;
		}
		s.metric = cache.map.probes;
		s.nanos = System.nanoTime() - start;
		return s;
	}

	static void runSimulation() {
		if (traceLen == 0) {
			System.out.println("Trace is empty. Enter/load a trace first.");
			return;
		}
		System.out.println("\n=== LRU Cache Simulation ===");
		System.out.println("Trace length: " + traceLen);
		System.out.println("Cache capacity: " + cacheCapacity);

		Stats naive = simulateNaive(trace, traceLen, cacheCapacity);
		Stats fast = simulateFast(trace, traceLen, cacheCapacity);

		System.out.println("\nNaive LRU (array + linear scan)");
		printStats(naive, "Comparisons");
		System.out.println("\nFast LRU (hash table + doubly linked list)");
		printStats(fast, "Hash probes");
		System.out.println("\nNote: timings vary per machine; comparisons/probes show the real structural difference.");
	}

	static void printStats(Stats s, String metricName) {
		long total = s.hits + s.misses;
		double hitRate = (total == 0) ? 0.0 : (100.0 * s.hits / (double) total);
		System.out.println("Hits: " + s.hits + ", Misses: " + s.misses + ", Hit%: " + String.format("%.2f", hitRate));
		System.out.println(metricName + ": " + s.metric);
		System.out.println("Time (ns): " + s.nanos);
	}

	public static void main(String[] args) throws Exception {
		while (true) {
			System.out.println("\n===== MiniProject 27: Cache Simulator (LRU) =====");
			System.out.println("1. Set cache capacity");
			System.out.println("2. Enter request trace (one line)");
			System.out.println("3. Load request trace from file");
			System.out.println("4. Display current trace");
			System.out.println("5. Run simulation (naive vs fast)");
			System.out.println("6. Exit");
			int ch = readInt("Enter choice: ");
			if (ch == 1) setCapacity();
			else if (ch == 2) inputTraceManual();
			else if (ch == 3) loadTraceFromFile();
			else if (ch == 4) displayTrace();
			else if (ch == 5) runSimulation();
			else if (ch == 6) break;
			else System.out.println("Invalid choice.");
		}
	}
}
