#ifndef HASHSETOPEN_H
#define HASHSETOPEN_H

#include <list>
#include <string>
#include <vector>
#include <QString>

class HashSetOpen {
public:
    struct Entry {
        int key;
        std::string value;
        Entry(int k, const std::string& v) : key(k), value(v) {}
    };

    explicit HashSetOpen(int cap);
    ~HashSetOpen();

    // Core operations
    void insert(int key, const std::string& value);   // insert or update
    bool get(int key, std::string& outvalue) const;   // lookup value by key
    bool contains(int key) const;                     // key existence check
    bool remove(int key);                             // delete by key

    // Utility / introspection
    void clear();                                     // remove all entries
    int  size() const;                                // number of stored entries
    int  getCapacity() const;                         // number of buckets
    bool isEmpty() const;                             // size() == 0
    double loadFactor() const;                        // size / capacity

    std::vector<int> keys() const;                    // all keys
    std::vector<std::string> values() const;          // all values

    QString display() const;                          // formatted plain-text view
    QString displayAsTable() const;                   // NEW: HTML table view

private:
    std::list<Entry>* table;
    int capacity;
    int count;
};

#endif