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
// Formatted plain-text display (kept for compatibility)
// ---------------------------------------------------------------------------
QString HashSetOpen::display() const {
    std::ostringstream os;

    os << "Open Hashing (Separate Chaining)\n";
    os << "Capacity: " << capacity
       << "   Entries: " << count
       << "   Load Factor: " << std::fixed << std::setprecision(2)
       << loadFactor() << "\n\n";

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
                    os << std::left
                       << std::setw(8)  << ""
                       << std::setw(10) << entry.key
                       << std::setw(22) << entry.value;
                }

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

// ---------------------------------------------------------------------------
// HTML table view — mirrors HashSetClosed::displayAsTable() visually.
//
// Columns:
//   Bucket       -> index in the hash table (0 .. capacity-1)
//   Status       -> empty / occupied / collision / chain
//   Key (ID)     -> unique integer used for hashing & lookup
//   Value        -> text payload stored alongside the key
//   Hash h(key)  -> the bucket this key hashes to
//   Chain        -> all (key:value) pairs that share this bucket
//
// Row colouring matches closed hashing:
//   grey  = empty bucket
//   green = single entry, no collision
//   blue  = bucket holds >1 entry (chain used to resolve collision)
// ---------------------------------------------------------------------------
QString HashSetOpen::displayAsTable() const {
    QString html;
    html += "<table cellspacing='0' cellpadding='6' border='1' "
            "style='border-collapse:collapse; font-family:Consolas,monospace; "
            "font-size:13px; width:100%;'>";

    html += "<tr style='background:#1a1a1a; color:#ffffff;'>"
            "<th>Bucket</th>"
            "<th>Status</th>"
            "<th>Key (ID)</th>"
            "<th>Value (Payload)</th>"
            "<th>Hash h(key)</th>"
            "<th>Chain (bucket contents)</th>"
            "</tr>";

    for (int i = 0; i < capacity; i++) {
        const std::list<Entry>& bucket = table[i];

        // ---- empty bucket ----
        if (bucket.empty()) {
            html += QString("<tr style='background:#fafafa;'>"
                            "<td align='center'>%1</td>"
                            "<td align='center'>empty</td>"
                            "<td align='center'>—</td>"
                            "<td>—</td>"
                            "<td align='center'>—</td>"
                            "<td align='center'>—</td>"
                            "</tr>")
                        .arg(i);
            continue;
        }

        // ---- occupied bucket ----
        int chainLen = static_cast<int>(bucket.size());
        bool collision = chainLen > 1;
        QString bg = collision ? "#e3f2fd"     // blue  = chain / collision
                               : "#e8f5e9";    // green = single entry

        // Pre-build the full chain string once
        QString chainStr;
        {
            bool first = true;
            for (const auto& e : bucket) {
                if (!first) chainStr += " → ";
                chainStr += QString("[%1:%2]")
                                .arg(e.key)
                                .arg(QString::fromStdString(e.value));
                first = false;
            }
            chainStr += " → null";
        }

        bool firstRow = true;
        for (const auto& e : bucket) {
            int home = calculateHash(e.key, capacity);

            QString bucketCell  = firstRow ? QString::number(i) : QString("");
            QString hashCell    = QString::number(home);
            QString statusCell  = firstRow
                                     ? (collision ? "collision"
                                                  : "occupied")
                                     : "chain";

            html += QString("<tr style='background:%1;'>"
                            "<td align='center'>%2</td>"
                            "<td align='center'>%3</td>"
                            "<td align='center'>%4</td>"
                            "<td>%5</td>"
                            "<td align='center'>%6</td>"
                            "<td>%7</td>"
                            "</tr>")
                        .arg(bg)
                        .arg(bucketCell)
                        .arg(statusCell)
                        .arg(e.key)
                        .arg(QString::fromStdString(e.value))
                        .arg(hashCell)
                        .arg(chainStr);

            firstRow = false;
        }
    }

    html += "</table>";
    return html;
}