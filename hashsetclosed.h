#ifndef HASHSETCLOSED_H
#define HASHSETCLOSED_H

#include <string>
#include <stdexcept>
#include <climits>
#include <QString>
#include <QStringList>

class HashSetClosed {
private:
    struct Entry {
        int key;
        std::string value;
        bool occupied;
        bool tombstone;
        Entry() : key(0), value(""), occupied(false), tombstone(false) {}
        Entry(int k, const std::string& v) : key(k), value(v), occupied(true), tombstone(false) {}
    };
    Entry* table;
    int capacity;
    int count;
    QStringList rehashMessages;
    static constexpr double LOAD_FACTOR = 0.5;

    void insertWithoutRehash(int key, const std::string& value);
    int findExistingKey(int key) const;
    void rehash();

public:
    explicit HashSetClosed(int cap);
    ~HashSetClosed();

    HashSetClosed(const HashSetClosed&) = delete;
    HashSetClosed& operator=(const HashSetClosed&) = delete;

    // ---- Core operations ----
    void insert(int key, const std::string& value);   // insert or update
    bool get(int key, std::string& outvalue) const;   // lookup value by key
    bool contains(int key) const;                     // key membership test
    bool remove(int key);                             // delete (tombstone)
    void clear();                                     // remove all entries

    // ---- Introspection ----
    int  getCapacity() const { return capacity; }
    int  getCount()    const { return count; }
    bool isEmpty()     const { return count == 0; }
    QStringList rehashLog() const { return rehashMessages; }

    // ---- Rendering ----
    QString display() const;   // plain-text (kept for compatibility)
    QString displayAsTable() const; // NEW: HTML table view
};

#endif