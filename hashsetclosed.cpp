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
    if (index < 0) index += capacity;   // handle negative keys
    return static_cast<int>(index);
}

HashSetClosed::HashSetClosed(int cap) : table(nullptr), capacity(cap), count(0) {
    if (cap <= 0) throw std::invalid_argument("Capacity must be positive.");
    capacity = nextPrime(cap);
    table = new Entry[capacity];
}

HashSetClosed::~HashSetClosed() { delete[] table; }

// -------------------------------------------------------------------
// LINEAR PROBING — the probe sequence is:
//     h(k), h(k)+1, h(k)+2, ... (mod capacity)
// Each step checks ONE slot. If it is free (or a tombstone we can
// reuse), we stop. If it is occupied by a different key, we take the
// NEXT slot. The loop runs at most `capacity` times, which is enough
// to visit every slot exactly once because we are stepping by 1 mod
// a prime capacity — the sequence covers the whole table.
// -------------------------------------------------------------------
void HashSetClosed::insertWithoutRehash(int key, const std::string& value) {
    int index = calculateHash(key, capacity);           // start slot = h(key)
    for (int i = 0; i < capacity; i++) {                // try at most `capacity` slots
        if (!table[index].occupied || table[index].tombstone) {
            table[index] = Entry(key, value);           // claim this slot
            count++;
            return;
        }
        index = (index + 1) % capacity;                 // <-- THE PROBE STEP
    }
    throw std::logic_error("No available slot.");
}

int HashSetClosed::findExistingKey(int key) const {
    int index = calculateHash(key, capacity);
    for (int i = 0; i < capacity; i++) {
        if (!table[index].occupied && !table[index].tombstone) return -1; // hit a truly empty slot -> key cannot be further on
        if (table[index].occupied && !table[index].tombstone && table[index].key == key) return index;
        index = (index + 1) % capacity;                 // <-- SAME probe step as insert
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
        index = (index + 1) % capacity;                 // <-- SAME probe step
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
            table[index].tombstone = true;              // logical delete; probing will skip it
            count--;
            return true;
        }
        index = (index + 1) % capacity;                 // <-- SAME probe step
    }
    return false;
}

// Clears every slot. We cannot just delete[] and reconstruct because
// capacity is still valid — reset the existing array in place.
void HashSetClosed::clear() {
    for (int i = 0; i < capacity; i++) {
        table[i] = Entry();
    }
    count = 0;
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

// -------------------------------------------------------------------
// HTML table view: each row shows slot index, hash(key), occupancy,
// key, value. The "Home" column is h(key) — the slot linear probing
// STARTED from. If Home != Index, that row is a collision that was
// resolved by probing forward.
// -------------------------------------------------------------------
QString HashSetClosed::displayAsTable() const {
    QString html;
    html += "<table cellspacing='0' cellpadding='6' border='1' "
            "style='border-collapse:collapse; font-family:Consolas,monospace; "
            "font-size:13px; width:100%;'>";

    html += "<tr style='background:#1a1a1a; color:#ffffff;'>"
            "<th>Slot #</th>"
            "<th>Status</th>"
            "<th>Key (ID)</th>"
            "<th>Value (Payload)</th>"
            "<th>Hash h(key)</th>"
            "<th>Probed?</th>"
            "</tr>";

    for (int i = 0; i < capacity; i++) {
        const Entry& e = table[i];

        QString status, bg, keyTxt, valTxt, hashTxt, probedTxt;
        if (!e.occupied) {
            status = "empty";
            bg = "#fafafa";
            keyTxt = valTxt = hashTxt = probedTxt = "—";
        } else if (e.tombstone) {
            status = "tombstone";
            bg = "#fff4e5";
            keyTxt = valTxt = hashTxt = probedTxt = "—";
        } else {
            status = "occupied";
            int home = calculateHash(e.key, capacity);
            bg = (home == i) ? "#e8f5e9" : "#e3f2fd";   // green = direct, blue = probed
            keyTxt  = QString::number(e.key);
            valTxt  = QString::fromStdString(e.value);
            hashTxt = QString::number(home);
            probedTxt = (home == i)
                            ? QString("no (direct)")
                            : QString("yes (+%1)").arg((i - home + capacity) % capacity);
        }

        html += QString("<tr style='background:%1;'>"
                        "<td align='center'>%2</td>"
                        "<td align='center'>%3</td>"
                        "<td align='center'>%4</td>"
                        "<td>%5</td>"
                        "<td align='center'>%6</td>"
                        "<td align='center'>%7</td>"
                        "</tr>")
                    .arg(bg).arg(i).arg(status)
                    .arg(keyTxt).arg(valTxt).arg(hashTxt).arg(probedTxt);
    }

    html += "</table>";
    return html;
}