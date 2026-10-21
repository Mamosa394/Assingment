#ifndef HASHSETOPEN_H
#define HASHSETOPEN_H

#include <list>
#include <string>
#include <vector>
#include <QString>
#include "hashstrategy.h"

class HashSetOpen {
public:
    struct Entry {
        int key;
        std::string value;
        Entry(int k, const std::string& v) : key(k), value(v) {}
    };

    // cap = exact array size (M) chosen by the user
    explicit HashSetOpen(int cap);
    ~HashSetOpen();

    void setHashStrategy(const HashStrategy& s) { strategy = s; }
    HashStrategy getHashStrategy() const        { return strategy; }

    void insert(int key, const std::string& value);
    bool get(int key, std::string& outvalue) const;
    bool contains(int key) const;
    bool remove(int key);

    void clear();
    int  size() const;
    int  getCapacity() const;
    int  getCount() const { return count; }
    bool isEmpty() const;
    double loadFactor() const;

    QString display() const;
    QString displayAsTable() const;
    QString explainInsert(int key, const std::string& value) const;

    void rehashWithStrategy();
    void resize(int newCapacity);

private:
    std::list<Entry>* table;
    int capacity;
    int count;
    HashStrategy strategy;

    int bucketFor(int key) const { return strategy.indexFor(key, capacity); }
};

#endif