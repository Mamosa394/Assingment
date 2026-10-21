#include "hashsetclosed.h"
#include <vector>
#include <algorithm>

HashSetClosed::HashSetClosed(int cap) : table(nullptr), capacity(cap), count(0) {
    if (cap <= 0) throw std::invalid_argument("Table size must be positive.");
    table = new Entry[capacity];
}

HashSetClosed::~HashSetClosed() { delete[] table; }

void HashSetClosed::insertWithoutRehash(int key, const std::string& value) {
    int index = homeSlot(key);
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
    int index = homeSlot(key);
    for (int i = 0; i < capacity; i++) {
        if (!table[index].occupied && !table[index].tombstone) return -1;
        if (table[index].occupied && !table[index].tombstone && table[index].key == key) return index;
        index = (index + 1) % capacity;
    }
    return -1;
}

void HashSetClosed::rehashWithStrategy() {
    std::vector<Entry> snapshot;
    snapshot.reserve(count);
    for (int i = 0; i < capacity; i++)
        if (table[i].occupied && !table[i].tombstone) snapshot.push_back(table[i]);

    for (int i = 0; i < capacity; i++) table[i] = Entry();
    count = 0;

    for (const auto& e : snapshot) insert(e.key, e.value);
}

void HashSetClosed::resize(int newCapacity) {
    if (newCapacity <= 0) return;

    std::vector<Entry> snapshot;
    snapshot.reserve(count);
    for (int i = 0; i < capacity; i++)
        if (table[i].occupied && !table[i].tombstone) snapshot.push_back(table[i]);

    delete[] table;
    capacity = newCapacity;
    table = new Entry[capacity];
    count = 0;

    for (const auto& e : snapshot) insertWithoutRehash(e.key, e.value);
}

void HashSetClosed::insert(int key, const std::string& value) {
    int existingIndex = findExistingKey(key);
    if (existingIndex != -1) { table[existingIndex].value = value; return; }

    // If full, trigger resize (auto-grow by 2x)
    if (static_cast<double>(count + 1) / capacity >= LOAD_FACTOR) {
        rehashMessages.append(QString("Auto-rehash triggered at load factor 0.5"));
        resize(capacity * 2);
    }

    insertWithoutRehash(key, value);
}

bool HashSetClosed::get(int key, std::string& outvalue) const {
    int index = homeSlot(key);
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
    int index = homeSlot(key);
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

void HashSetClosed::clear() {
    for (int i = 0; i < capacity; i++) table[i] = Entry();
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

QString HashSetClosed::displayAsTable() const {
    QString html;
    html += "<table cellspacing='0' cellpadding='6' border='1' "
            "style='border-collapse:collapse; font-family:Consolas,monospace; "
            "font-size:13px; width:100%;'>";

    html += "<tr style='background:#1a1a1a; color:#ffffff;'>"
            "<th>Slot #</th><th>Status</th><th>Key (ID)</th>"
            "<th>Value (Payload)</th><th>Hash h(key)</th><th>Probed?</th>"
            "</tr>";

    for (int i = 0; i < capacity; i++) {
        const Entry& e = table[i];
        QString status, bg, keyTxt, valTxt, hashTxt, probedTxt;

        if (!e.occupied) {
            status = "empty"; bg = "#fafafa";
            keyTxt = valTxt = hashTxt = probedTxt = "-";
        } else if (e.tombstone) {
            status = "tombstone"; bg = "#fff4e5";
            keyTxt = valTxt = hashTxt = probedTxt = "-";
        } else {
            status = "occupied";
            int home = homeSlot(e.key);

            bg = (home == i) ? "#e8f5e9" : "#e3f2fd";
            keyTxt  = QString::number(e.key);
            valTxt  = QString::fromStdString(e.value);
            hashTxt = QString("%1").arg(home);
            probedTxt = (home == i) ? QString("no (direct)") : QString("yes (+%1)").arg((i - home + capacity) % capacity);
        }

        html += QString("<tr style='background:%1;'>"
                        "<td align='center'>%2</td><td align='center'>%3</td>"
                        "<td align='center'>%4</td><td>%5</td>"
                        "<td align='center'>%6</td><td align='center'>%7</td></tr>")
                    .arg(bg).arg(i).arg(status)
                    .arg(keyTxt).arg(valTxt).arg(hashTxt).arg(probedTxt);
    }
    html += "</table>";
    return html;
}

QString HashSetClosed::explainInsert(int key, const std::string& value) const {
    QString s;
    int home = homeSlot(key);

    int storedAt = -1;
    for (int i = 0; i < capacity; ++i) {
        int idx = (home + i) % capacity;
        if (table[idx].occupied && !table[idx].tombstone && table[idx].key == key) {
            storedAt = idx;
            break;
        }
    }

    s += QString("HASH:      h(%1) = %2 (capacity %3)\n").arg(key).arg(home).arg(capacity);

    if (storedAt == home) {
        s += QString("SLOT:      bucket %1 was empty -> stored directly (NO collision)\n").arg(home);
    } else if (storedAt != -1) {
        int steps = (storedAt - home + capacity) % capacity;
        s += QString("COLLISION: bucket %1 was occupied by a different key\n").arg(home);
        s += "PROBES:    ";
        for (int i = 0; i <= steps; ++i) {
            int idx = (home + i) % capacity;
            if (i > 0) s += " -> ";
            s += QString("%1").arg(idx);
        }
        s += QString("\nSLOT:      bucket %1 is the first free slot -> stored after %2 probes\n").arg(storedAt).arg(steps);
    } else {
        s += "SLOT:      (key not found after probing - unexpected)\n";
    }
    s += QString("VALUE:     \"%1\" stored with key %2\n").arg(QString::fromStdString(value)).arg(key);
    return s;
}

QString HashSetClosed::explainRemove(int key) const {
    QString s;
    int home = homeSlot(key);

    s += QString("HASH:      h(%1) = %2 (capacity %3)\n").arg(key).arg(home).arg(capacity);

    int idx = home;
    for (int i = 0; i < capacity; ++i) {
        if (!table[idx].occupied && !table[idx].tombstone) {
            s += QString("SLOT:      bucket %1 is EMPTY -> key %2 not present\n").arg(idx).arg(key);
            return s;
        }
        if (table[idx].occupied && !table[idx].tombstone && table[idx].key == key) {
            if (idx == home) {
                s += QString("SLOT:      bucket %1 held key %2 -> marked TOMBSTONE\n").arg(idx).arg(key);
            } else {
                int steps = (idx - home + capacity) % capacity;
                s += "PROBES:    ";
                for (int j = 0; j <= steps; ++j) {
                    if (j > 0) s += " -> ";
                    s += QString("%1").arg((home + j) % capacity);
                }
                s += QString("\nSLOT:      bucket %1 held key %2 -> marked TOMBSTONE\n").arg(idx).arg(key);
            }
            s += "REASON:    a tombstone keeps the probe chain intact - later lookups still pass through.\n";
            return s;
        }
        idx = (idx + 1) % capacity;
    }
    s += "SLOT:      (key not found after probing - unexpected)\n";
    return s;
}