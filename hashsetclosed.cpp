#include "hashsetclosed.h"

static bool isPrime(int n) {
    if (n < 2) return false;
    for (int i = 2; i <= n / i; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

static int nextPrime(int n) {
    if (n < 2) return 2;
    while (!isPrime(n)) {
        if (n == INT_MAX) throw std::overflow_error("Overflow");
        n++;
    }
    return n;
}

static int calculateHash(int key, int capacity) {
    long long k = key;
    long long index = k % capacity;
    if (index < 0) index += capacity;
    return static_cast<int>(index);
}

HashSetClosed::HashSetClosed(int cap) : table(nullptr), capacity(cap), count(0) {
    if (cap <= 0) throw std::invalid_argument("Capacity must be positive.");
    capacity = nextPrime(cap);
    table = new Entry[capacity];
}

HashSetClosed::~HashSetClosed() { delete[] table; }

void HashSetClosed::insertWithoutRehash(int key, const std::string& value) {
    int index = calculateHash(key, capacity);
    for (int i = 0; i < capacity; i++) {
        if (!table[index].occupied || table[index].tombstone) {
            table[index] = Entry(key, value);
            count++;
            return;
        }
        index = (index + 1) % capacity;
    }
    throw std::logic_error("No available slot.");
}

int HashSetClosed::findExistingKey(int key) const {
    int index = calculateHash(key, capacity);
    for (int i = 0; i < capacity; i++) {
        if (!table[index].occupied && !table[index].tombstone) return -1;
        if (table[index].occupied && !table[index].tombstone && table[index].key == key) return index;
        index = (index + 1) % capacity;
    }
    return -1;
}

void HashSetClosed::rehash() {
    if (capacity > INT_MAX / 2) throw std::overflow_error("Cannot grow further.");
    int newCapacity = nextPrime(capacity * 2);
    Entry* oldTable = table;
    int oldCapacity = capacity;

    table = new Entry[newCapacity];
    capacity = newCapacity;
    count = 0;

    for (int i = 0; i < oldCapacity; i++) {
        if (oldTable[i].occupied && !oldTable[i].tombstone) {
            insertWithoutRehash(oldTable[i].key, oldTable[i].value);
        }
    }
    delete[] oldTable;
    rehashMessages.append(QString("Rehashed to capacity: %1").arg(capacity));
}

void HashSetClosed::insert(int key, const std::string& value) {
    int existingIndex = findExistingKey(key);
    if (existingIndex != -1) { table[existingIndex].value = value; return; }

    if (static_cast<double>(count + 1) / capacity >= LOAD_FACTOR) rehash();
    insertWithoutRehash(key, value);
}

bool HashSetClosed::get(int key, std::string& outvalue) const {
    int index = calculateHash(key, capacity);
    for (int i = 0; i < capacity; i++) {
        if (!table[index].occupied && !table[index].tombstone) return false;
        if (table[index].occupied && !table[index].tombstone && table[index].key == key) {
            outvalue = table[index].value;
            return true;
        }
        index = (index + 1) % capacity;
    }
    return false;
}

bool HashSetClosed::contains(int key) const {
    std::string dummy;
    return get(key, dummy);
}

bool HashSetClosed::remove(int key) {
    int index = calculateHash(key, capacity);
    for (int i = 0; i < capacity; i++) {
        if (!table[index].occupied && !table[index].tombstone) return false;
        if (table[index].occupied && !table[index].tombstone && table[index].key == key) {
            table[index].tombstone = true;
            count--;
            return true;
        }
        index = (index + 1) % capacity;
    }
    return false;
}

QString HashSetClosed::display() const {
    QString out;
    out += QString("Capacity: %1\n").arg(capacity);
    for (int i = 0; i < capacity; i++) {
        out += QString("[%1] ").arg(i);
        if (!table[i].occupied) out += "empty";
        else if (table[i].tombstone) out += "<tombstone>";
        else out += QString("%1:%2").arg(table[i].key).arg(QString::fromStdString(table[i].value));
        out += "\n";
    }
    return out;
}