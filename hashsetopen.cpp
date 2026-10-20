#include "hashsetopen.h"
#include <climits>
#include <stdexcept>
#include <sstream>
#include <iomanip>

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

HashSetOpen::HashSetOpen(int cap) : table(nullptr), capacity(cap), count(0) {
    if (cap <= 0) throw std::invalid_argument("Capacity must be positive");
    capacity = nextPrime(cap);
    table = new std::list<Entry>[capacity];
}

HashSetOpen::~HashSetOpen() { delete[] table; }

void HashSetOpen::insert(int key, const std::string& value) {
    int index = calculateHash(key, capacity);
    for (auto& entry : table[index]) {
        if (entry.key == key) { entry.value = value; return; }  // update existing
    }
    table[index].emplace_back(key, value);
    count++;
}

bool HashSetOpen::get(int key, std::string& outvalue) const {
    int index = calculateHash(key, capacity);
    for (const auto& entry : table[index]) {
        if (entry.key == key) { outvalue = entry.value; return true; }
    }
    return false;
}

bool HashSetOpen::contains(int key) const {
    std::string dummy;
    return get(key, dummy);
}

bool HashSetOpen::remove(int key) {
    int index = calculateHash(key, capacity);
    auto& bucket = table[index];
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->key == key) { bucket.erase(it); count--; return true; }
    }
    return false;
}

void HashSetOpen::clear() {
    for (int i = 0; i < capacity; i++) table[i].clear();
    count = 0;
}

int HashSetOpen::size() const { return count; }
int HashSetOpen::getCapacity() const { return capacity; }
bool HashSetOpen::isEmpty() const { return count == 0; }

double HashSetOpen::loadFactor() const {
    return capacity > 0 ? static_cast<double>(count) / capacity : 0.0;
}

std::vector<int> HashSetOpen::keys() const {
    std::vector<int> out;
    out.reserve(count);
    for (int i = 0; i < capacity; i++)
        for (const auto& e : table[i]) out.push_back(e.key);
    return out;
}

std::vector<std::string> HashSetOpen::values() const {
    std::vector<std::string> out;
    out.reserve(count);
    for (int i = 0; i < capacity; i++)
        for (const auto& e : table[i]) out.push_back(e.value);
    return out;
}

// ---------------------------------------------------------------------------
// Formatted table display
//
// Column meanings:
//   Bucket   -> index in the hash table (0 .. capacity-1)
//   Key      -> the unique integer identifier used for hashing & lookup
//   Value    -> the string payload stored alongside the key
//   Chain    -> the full bucket contents (may hold >1 entry on collision)
// ---------------------------------------------------------------------------
QString HashSetOpen::display() const {
    std::ostringstream os;

    os << "Open Hashing (Separate Chaining)\n";
    os << "Capacity: " << capacity
       << "   Entries: " << count
       << "   Load Factor: " << std::fixed << std::setprecision(2)
       << loadFactor() << "\n\n";

    // Header
    os << std::left
       << std::setw(8)  << "Bucket"
       << std::setw(10) << "Key"
       << std::setw(22) << "Value"
       << "Chain (bucket contents)\n";
    os << std::string(70, '-') << "\n";

    for (int i = 0; i < capacity; i++) {
        if (table[i].empty()) {
            os << std::left
               << std::setw(8)  << i
               << std::setw(10) << "-"
               << std::setw(22) << "-"
               << "(empty)\n";
        } else {
            bool firstRow = true;
            for (const auto& entry : table[i]) {
                if (firstRow) {
                    os << std::left
                       << std::setw(8)  << i
                       << std::setw(10) << entry.key
                       << std::setw(22) << entry.value;
                    firstRow = false;
                } else {
                    // continuation of a collision chain in same bucket
                    os << std::left
                       << std::setw(8)  << ""
                       << std::setw(10) << entry.key
                       << std::setw(22) << entry.value;
                }

                // Show chain visualization
                os << "-> ";
                for (const auto& e : table[i])
                    os << "[" << e.key << ":" << e.value << "] ";
                os << "-> null\n";
            }
        }
    }

    os << std::string(70, '-') << "\n";
    os << "Legend:\n"
       << "  Bucket = hash index (key % capacity)\n"
       << "  Key    = unique integer identifier (used for hashing & lookup)\n"
       << "  Value  = string payload associated with the key\n"
       << "  Chain  = all entries sharing the same bucket (collisions)\n";

    return QString::fromStdString(os.str());
}