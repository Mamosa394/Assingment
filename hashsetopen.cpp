#include "hashsetopen.h"
#include <climits>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <algorithm>

HashSetOpen::HashSetOpen(int cap) : table(nullptr), capacity(cap), count(0) {
    if (cap <= 0) throw std::invalid_argument("Table size must be positive");
    table = new std::list<Entry>[capacity];
}

HashSetOpen::~HashSetOpen() { delete[] table; }

void HashSetOpen::insert(int key, const std::string& value) {
    // Chaining can hold more than capacity, but we auto-grow if it gets overly full (alpha > 1.0)
    if ((count + 1.0) / capacity > 1.0) {
        resize(capacity * 2);
    }

    int index = bucketFor(key);
    for (auto& entry : table[index]) {
        if (entry.key == key) { entry.value = value; return; }
    }
    table[index].emplace_back(key, value);
    count++;
}

bool HashSetOpen::get(int key, std::string& outvalue) const {
    int index = bucketFor(key);
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
    int index = bucketFor(key);
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

void HashSetOpen::rehashWithStrategy() {
    std::vector<Entry> snapshot;
    snapshot.reserve(count);
    for (int i = 0; i < capacity; i++)
        for (const auto& e : table[i]) snapshot.push_back(e);

    for (int i = 0; i < capacity; i++) table[i].clear();
    count = 0;

    for (const auto& e : snapshot) insert(e.key, e.value);
}

void HashSetOpen::resize(int newCapacity) {
    if (newCapacity <= 0) return;

    std::vector<Entry> snapshot;
    snapshot.reserve(count);
    for (int i = 0; i < capacity; i++)
        for (const auto& e : table[i]) snapshot.push_back(e);

    delete[] table;
    capacity = newCapacity;
    table = new std::list<Entry>[capacity];
    count = 0;

    for (const auto& e : snapshot) insert(e.key, e.value);
}

QString HashSetOpen::display() const {
    std::ostringstream os;
    os << "Open Hashing (Separate Chaining)\n";
    os << "Capacity: " << capacity << "   Entries: " << count << "\n\n";

    os << std::left << std::setw(8) << "Bucket" << std::setw(10) << "Key" << "Chain\n";
    os << std::string(50, '-') << "\n";

    for (int i = 0; i < capacity; i++) {
        if (table[i].empty()) {
            os << std::left << std::setw(8) << i << "(empty)\n";
        } else {
            bool firstRow = true;
            for (const auto& entry : table[i]) {
                if (firstRow) {
                    os << std::left << std::setw(8) << i << std::setw(10) << entry.key;
                    firstRow = false;
                } else {
                    os << std::left << std::setw(8) << "" << std::setw(10) << entry.key;
                }
                os << "-> ";
                for (const auto& e : table[i]) os << "[" << e.key << ":" << e.value << "] ";
                os << "-> null\n";
            }
        }
    }
    return QString::fromStdString(os.str());
}

QString HashSetOpen::displayAsTable() const {
    QString html;
    html += "<table cellspacing='0' cellpadding='6' border='1' "
            "style='border-collapse:collapse; font-family:Consolas,monospace; "
            "font-size:13px; width:100%;'>";

    html += "<tr style='background:#1a1a1a; color:#ffffff;'>"
            "<th>Bucket</th><th>Status</th><th>Key (ID)</th>"
            "<th>Value (Payload)</th><th>Hash Index</th><th>Chain</th>"
            "</tr>";

    for (int i = 0; i < capacity; i++) {
        const std::list<Entry>& bucket = table[i];
        if (bucket.empty()) {
            html += QString("<tr style='background:#fafafa;'>"
                            "<td align='center'>%1</td><td align='center'>empty</td>"
                            "<td align='center'>-</td><td>-</td>"
                            "<td align='center'>-</td><td align='center'>-</td></tr>").arg(i);
            continue;
        }

        int chainLen = static_cast<int>(bucket.size());
        bool collision = chainLen > 1;
        QString bg = collision ? "#e3f2fd" : "#e8f5e9";

        QString chainStr;
        bool first = true;
        for (const auto& e : bucket) {
            if (!first) chainStr += " -> ";
            chainStr += QString("[%1:%2]").arg(e.key).arg(QString::fromStdString(e.value));
            first = false;
        }
        chainStr += " -> null";

        bool firstRow = true;
        for (const auto& e : bucket) {
            int home = bucketFor(e.key);
            QString bucketCell = firstRow ? QString::number(i) : QString("");
            QString hashCell = QString("%1 (h=%2)").arg(home).arg(e.key);
            QString statusCell = firstRow ? (collision ? "collision" : "occupied") : "chain";

            html += QString("<tr style='background:%1;'>"
                            "<td align='center'>%2</td><td align='center'>%3</td>"
                            "<td align='center'>%4</td><td>%5</td>"
                            "<td align='center'>%6</td><td>%7</td></tr>")
                        .arg(bg, bucketCell, statusCell)
                        .arg(e.key).arg(QString::fromStdString(e.value))
                        .arg(hashCell, chainStr);
            firstRow = false;
        }
    }
    html += "</table>";
    return html;
}

QString HashSetOpen::explainInsert(int key, const std::string& value) const {
    QString s;
    int bucket = bucketFor(key);
    int chainLen = static_cast<int>(table[bucket].size());

    s += QString("HASH:      h(%1) = %2  (capacity %3)\n")
             .arg(key).arg(bucket).arg(capacity);

    if (chainLen == 1) {
        s += QString("BUCKET:    bucket %1 was empty -> inserted directly (NO collision)\n").arg(bucket);
    } else {
        int idx = 1;
        for (const auto& e : table[bucket]) {
            if (e.key == key) {
                s += QString("BUCKET:    bucket %1 already held %2 entries -> appended at position %3\n")
                .arg(bucket).arg(chainLen).arg(idx);
                break;
            }
            ++idx;
        }
        s += QString("COLLISION: %1 keys share bucket %2 -> SEPARATE CHAINING used\n")
                 .arg(chainLen).arg(bucket);
    }
    s += QString("VALUE:     \"%1\" stored with key %2\n").arg(QString::fromStdString(value)).arg(key);
    return s;
}