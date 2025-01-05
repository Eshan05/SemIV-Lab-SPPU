class Record:
    def __init__(self, name="", number=0):
        self.name = name
        self.number = number

    def set_name(self, name):
        self.name = name

    def get_name(self):
        return self.name

    def set_number(self, number):
        self.number = number

    def get_number(self):
        return self.number

    def __str__(self):
        return f"Name: {self.name}, Number: {self.number}"

class HashTableLinearProbe:
    def __init__(self, size):
        self.size = size
        self.table = [None] * size
        self.comparisons = 0

    def hash_func(self, key):
        return key % self.size

    def insert(self, record):
        key = record.get_number()
        pos = self.hash_func(key)
        while self.table[pos] is not None:
            pos = (pos + 1) % self.size
        self.table[pos] = record

    def search(self, record):
        key = record.get_number()
        pos = self.hash_func(key)
        comparisons = 0
        start_pos = pos
        while self.table[pos] is not None:
            comparisons += 1
            if self.table[pos].get_name() == record.get_name() and self.table[pos].get_number() == record.get_number():
                print(f"Found at {pos}, comparisons: {comparisons}")
                self.comparisons += comparisons
                return pos
            pos = (pos + 1) % self.size
            if pos == start_pos:
                break
        print("Not found")
        return -1

    def display(self):
        print("\n--- Linear Probing ---")
        for i, r in enumerate(self.table):
            print(f"{i}: {r}")
        print(f"Total comparisons: {self.comparisons}")

class HashTableSeparateChaining:
    def __init__(self, size):
        self.size = size
        self.table = [[] for _ in range(size)]
        self.comparisons = 0

    def hash_func(self, key):
        return key % self.size

    def insert(self, record):
        key = record.get_number()
        index = self.hash_func(key)
        self.table[index].append(record)

    def search(self, record):
        key = record.get_number()
        index = self.hash_func(key)
        chain = self.table[index]
        comparisons = 0
        for r in chain:
            comparisons += 1
            if r.get_name() == record.get_name() and r.get_number() == record.get_number():
                print(f"Found in chain {index}, comparisons: {comparisons}")
                self.comparisons += comparisons
                return index
        print("Not found")
        return -1

    def display(self):
        print("\n--- Separate Chaining ---")
        for i, chain in enumerate(self.table):
            print(f"{i}: {chain}")
        print(f"Total comparisons: {self.comparisons}")

def input_record():
    name = input('Enter Name: ')
    number = int(input('Enter Number: '))
    return Record(name, number)

def run_hashing(hash_table_type):
    size = int(input('Enter size of contact book: '))
    if hash_table_type == "linear":
        ht = HashTableLinearProbe(size)
    elif hash_table_type == "chaining":
        ht = HashTableSeparateChaining(size)
    else:
        print("Invalid hash table type")
        return

    while True:
        print('\nPlease Enter Your Choice:')
        print('1. Add Contact')
        print('2. Search Contact')
        print('3. Display Contacts')
        print('4. Go Back')
        print('5. Exit')
        choice = int(input())
        if choice == 1:
            record = input_record()
            ht.insert(record)
        elif choice == 2:
            record = input_record()
            ht.search(record)
        elif choice == 3:
            ht.display()
        elif choice == 4:
            break
        elif choice == 5:
            exit()
        else:
            print('Invalid Choice, Please Try Again')

# Main
while True:
    ch = int(input('Please Enter Your Choice:\n1. Linear Probing\t2. Separate Chaining\n3. Exit '))
    if ch == 1:
        run_hashing("linear")
    elif ch == 2:
        run_hashing("chaining")
    elif ch == 3:
        exit()
    else:
        print('Invalid Choice, Please Try Again')