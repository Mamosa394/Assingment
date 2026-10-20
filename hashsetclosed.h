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

    void insert(int key, const std::string& value);
    bool get(int key, std::string& outvalue) const;
    bool contains(int key) const;
    bool remove(int key);
    int getCapacity() const { return capacity; }
    QStringList rehashLog() const { return rehashMessages; }
    QString display() const;
};

#endif