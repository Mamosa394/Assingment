#ifndef HASHSETCLOSED_H
#define HASHSETCLOSED_H

#include <string>
#include <stdexcept>
#include <climits>
#include <QString>
#include <QStringList>
#include "hashstrategy.h"

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

    HashStrategy strategy;
    int homeSlot(int key) const { return strategy.indexFor(key, capacity); }

    void insertWithoutRehash(int key, const std::string& value);
    int findExistingKey(int key) const;

public:
    // capis the exact array size chosen by the user
    explicit HashSetClosed(int cap);
    ~HashSetClosed();

    HashSetClosed(const HashSetClosed&) = delete;
    HashSetClosed& operator=(const HashSetClosed&) = delete;

    void setHashStrategy(const HashStrategy& s) { strategy = s; }
    HashStrategy getHashStrategy() const        { return strategy; }

    void rehashWithStrategy();
    void resize(int newCapacity);

    void insert(int key, const std::string& value);
    bool get(int key, std::string& outvalue) const;
    bool contains(int key) const;
    bool remove(int key);
    void clear();

    int  getCapacity() const { return capacity; }
    int  getCount()    const { return count; }
    bool isEmpty()     const { return count == 0; }
    QStringList rehashLog() const { return rehashMessages; }
    double loadFactor() const { return capacity > 0 ? static_cast<double>(count) / capacity : 0.0; }

    QString display() const;
    QString displayAsTable() const;
    QString explainInsert(int key, const std::string& value) const;
    QString explainRemove(int key) const;
};

#endif