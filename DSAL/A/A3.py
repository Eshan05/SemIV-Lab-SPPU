# 3. For given set of elements create skip list. Find the element in the set that is closest to some given value. (Note: Decide the level of element in the list Randomly with some upper limit)

class Random:
    def __init__(self, seed=None):
        if seed is None:
            self.state = id(object()) % 2147483647
        else:
            self.state = seed
        if self.state == 0:
            self.state = 1

    def rand(self):
        a = 48271
        m = 2147483647
        self.state = (a * self.state) % m
        return self.state

    def random_float(self):
        return self.rand() / 2147483647.0

class Node:
    def __init__(self, key, level):
        self.key = key
        self.forward = [None] * (level + 1)

class SkipList:
    def __init__(self, max_level, p):
        self.max_level = max_level
        self.p = p
        self.header = Node(-1, max_level)
        self.level = 0
        self.random = Random()

    def random_level(self):
        level = 0
        while self.random.random_float() < self.p and level < self.max_level:
            level += 1
        return level

    def insert(self, key):
        update = [None] * (self.max_level + 1)
        current = self.header

        for i in range(self.level, -1, -1):
            while current.forward[i] is not None and current.forward[i].key < key:
                current = current.forward[i]
            update[i] = current

        current = current.forward[0]

        if current is None or current.key != key:
            r_level = self.random_level()

            if r_level > self.level:
                for i in range(self.level + 1, r_level + 1):
                    update[i] = self.header
                self.level = r_level

            new_node = Node(key, r_level)

            for i in range(r_level + 1):
                new_node.forward[i] = update[i].forward[i]
                update[i].forward[i] = new_node
            
            print("Inserted", key, "at level", r_level)
        else:
            print("Key", key, "already exists")

    def find_closest(self, key):
        current = self.header
        
        for i in range(self.level, -1, -1):
            while current.forward[i] is not None and current.forward[i].key < key:
                current = current.forward[i]
        
        candidate1 = current
        candidate2 = current.forward[0]

        dist1 = float('inf')
        dist2 = float('inf')

        if candidate1.key != -1:
            dist1 = abs(candidate1.key - key)
        
        if candidate2 is not None:
            dist2 = abs(candidate2.key - key)

        if dist1 == float('inf') and dist2 == float('inf'):
            return None
        
        if dist1 <= dist2:
            return candidate1.key
        else:
            return candidate2.key

    def display(self):
        print("\nSkip List Structure:")
        for i in range(self.level, -1, -1):
            head = self.header.forward[i]
            print("Level", i, ":", end=" ")
            while head is not None:
                print(head.key, end=" ")
                head = head.forward[i]
            print()

def main():
    max_lvl = int(input("Enter max level for skip list: "))
    probability = 0.5
    
    sl = SkipList(max_lvl, probability)

    while True:
        print("\n1. Insert Element")
        print("2. Find Closest Element")
        print("3. Display List")
        print("4. Exit")
        
        choice = input("Enter choice: ")

        if choice == '1':
            val = int(input("Enter integer element: "))
            sl.insert(val)

        elif choice == '2':
            val = int(input("Enter value to find closest: "))
            result = sl.find_closest(val)
            if result is not None:
                print("Closest element to", val, "is", result)
            else:
                print("List is empty")

        elif choice == '3':
            sl.display()

        elif choice == '4':
            break

        else:
            print("Invalid choice")

if __name__ == "__main__":
    main()