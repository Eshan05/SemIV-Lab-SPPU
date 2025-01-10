# 2. Implement all the functions of a dictionary (ADT) using hashing and handle collisions using chaining with / without replacement. Data: Set of (key, value) pairs, Keys are mapped to values, Keys must be comparable, Keys must be unique. Standard Operations: Insert(key, value), Find(key), Delete(key)

# Dictionary ADT using Hashing
# Collision: Chaining
# Variations: With Replacement (Update), Without Replacement (Ignore Duplicate)

class Node:
    def __init__(self, key, value):
        self.key = key
        self.value = value
        self.next = None

class Dictionary:
    def __init__(self, size):
        self.size = size
        self.table = [None] * size

    def hash_function(self, key):
        h = 0
        for ch in str(key):
            h = h * 31 + ord(ch)
        if h < 0:
            h = -h
        return h % self.size

    def insert_with_replacement(self, key, value):
        index = self.hash_function(key)
        head = self.table[index]
        
        current = head
        while current is not None:
            if current.key == key:
                current.value = value
                return "Updated existing key"
            current = current.next
        
        new_node = Node(key, value)
        new_node.next = head
        self.table[index] = new_node
        return "Inserted new key"

    def insert_without_replacement(self, key, value):
        index = self.hash_function(key)
        head = self.table[index]
        
        current = head
        while current is not None:
            if current.key == key:
                return "Key already exists (No update)"
            current = current.next
        
        new_node = Node(key, value)
        new_node.next = head
        self.table[index] = new_node
        return "Inserted new key"

    def find(self, key):
        index = self.hash_function(key)
        current = self.table[index]
        comparisons = 0

        while current is not None:
            comparisons += 1
            if current.key == key:
                return current.value, comparisons
            current = current.next
        
        return None, comparisons

    def delete(self, key):
        index = self.hash_function(key)
        current = self.table[index]
        prev = None

        while current is not None:
            if current.key == key:
                if prev is None:
                    self.table[index] = current.next
                else:
                    prev.next = current.next
                return True
            prev = current
            current = current.next
        
        return False

    def display(self):
        print("\nDictionary State:")
        for i in range(self.size):
            print(i, ":", end=" ")
            current = self.table[i]
            if current is None:
                print("Empty")
            else:
                while current is not None:
                    print("[" + str(current.key) + ":" + str(current.value) + "]", end=" -> ")
                    current = current.next
                print("None")

def main():
    size = int(input("Enter hash table size: "))
    dictionary = Dictionary(size)

    while True:
        print("\n1. Insert (With Replacement)")
        print("2. Insert (Without Replacement)")
        print("3. Find")
        print("4. Delete")
        print("5. Display")
        print("6. Exit")
        
        choice = input("Enter choice: ")

        if choice == '1':
            key = input("Enter key: ")
            value = input("Enter value: ")
            msg = dictionary.insert_with_replacement(key, value)
            print(msg)

        elif choice == '2':
            key = input("Enter key: ")
            value = input("Enter value: ")
            msg = dictionary.insert_without_replacement(key, value)
            print(msg)

        elif choice == '3':
            key = input("Enter key to find: ")
            value, comps = dictionary.find(key)
            if value is not None:
                print("Found. Value:", value)
                print("Comparisons:", comps)
            else:
                print("Not Found")
                print("Comparisons:", comps)

        elif choice == '4':
            key = input("Enter key to delete: ")
            if dictionary.delete(key):
                print("Deleted successfully")
            else:
                print("Key not found")

        elif choice == '5':
            dictionary.display()

        elif choice == '6':
            break

        else:
            print("Invalid choice")

if __name__ == "__main__":
    main()