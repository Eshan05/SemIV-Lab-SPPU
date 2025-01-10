# 1. Consider telephone book database of N clients. Make use of a hash table implementation to quickly look up client‘s telephone number. Make use of two collision handling techniques and compare them using number of comparisons required to find a set of telephone numbers

# 1. Linear Probing
# 2. Separate Chaining

class LinearProbingHash:
    def __init__(self, size):
        self.size = size
        self.keys = [None] * size
        self.values = [None] * size

    def hash_function(self, key):
        h = 0
        for ch in key:
            h = h * 31 + ord(ch)
        if h < 0:
            h = -h
        return h % self.size

    def insert(self, key, value):
        index = self.hash_function(key)
        start = index
        collisions = 0

        while self.keys[index] is not None:
            if self.keys[index] == key:
                self.values[index] = value
                return True, collisions, "Updated existing key"
            index = (index + 1) % self.size
            collisions += 1
            if index == start:
                return False, collisions, "Hash Table Full"

        self.keys[index] = key
        self.values[index] = value
        return True, collisions, "Inserted"

    def search(self, key):
        index = self.hash_function(key)
        start = index
        comparisons = 0

        while self.keys[index] is not None:
            comparisons += 1
            if self.keys[index] == key:
                return self.values[index], comparisons
            index = (index + 1) % self.size
            if index == start:
                break

        return None, comparisons

    def display(self):
        print("\nLinear Probing Table:")
        for i in range(self.size):
            print(i, ":", end=" ")
            if self.keys[i] is not None:
                print(self.keys[i], "->", self.values[i])
            else:
                print("Empty")

class Node:
    def __init__(self, key, value):
        self.key = key
        self.value = value
        self.next = None

class ChainingHash:
    def __init__(self, size):
        self.size = size
        self.table = [None] * size

    def hash_function(self, key):
        h = 0
        for ch in key:
            h = h * 31 + ord(ch)
        if h < 0:
            h = -h
        return h % self.size

    def insert(self, key, value):
        index = self.hash_function(key)
        current = self.table[index]
        comparisons = 0

        while current is not None:
            comparisons += 1
            if current.key == key:
                current.value = value
                return True, comparisons, "Updated existing key"
            current = current.next

        new_node = Node(key, value)
        new_node.next = self.table[index]
        self.table[index] = new_node
        return True, comparisons, "Inserted"

    def search(self, key):
        index = self.hash_function(key)
        current = self.table[index]
        comparisons = 0

        while current is not None:
            comparisons += 1
            if current.key == key:
                return current.value, comparisons
            current = current.next

        return None, comparisons

    def display(self):
        print("\nSeparate Chaining Table:")
        for i in range(self.size):
            print(i, ":", end=" ")
            current = self.table[i]
            if current is None:
                print("Empty")
            else:
                while current is not None:
                    print(current.key, "->", current.value, end=" | ")
                    current = current.next
                print()

def main():
    size = int(input("Enter hash table size: "))
    linear = LinearProbingHash(size)
    chaining = ChainingHash(size)

    while True:
        print("\n1. Insert")
        print("2. Search")
        print("3. Display Tables")
        print("4. Exit")

        choice = input("Enter choice: ")

        if choice == '1':
            name = input("Enter client name: ")
            phone = input
            phone = input("Enter phone number: ")

            l_success, l_collisions, l_msg = linear.insert(name, phone)
            c_success, c_collisions, c_msg = chaining.insert(name, phone)

            print("\nLinear Probing:")
            print("Status:", l_msg)
            print("Collisions during insertion:", l_collisions)

            print("\nSeparate Chaining:")
            print("Status:", c_msg)
            print("Nodes traversed during insertion:", c_collisions)

        elif choice == '2':
            name = input("Enter client name to search: ")

            value1, comp1 = linear.search(name)
            value2, comp2 = chaining.search(name)

            print("\n--- Linear Probing ---")
            if value1 is not None:
              print("Found. Phone:", value1)
            else:
              print("Not Found")
              print("Comparisons:", comp1)

            print("\n--- Separate Chaining ---")
            if value2 is not None:
              print("Found. Phone:", value2)
            else:
              print("Not Found")
              print("Comparisons:", comp2)

        elif choice == '3':
            linear.display()
            chaining.display()

        elif choice == '4':
            print("Exiting.")
            break

        else:
            print("Invalid choice")

if __name__ == "__main__":
  main()