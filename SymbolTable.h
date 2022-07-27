#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class HashNode {
public:
    string id;
    long long key;
    int scp, parameters=-1;
    int type;               // 0 -> number, 1 -> string, -1 -> not yet
    int* args = NULL;        // store argument's type

    HashNode(long long key, string name, int sc, int para=-1) {
        this->id = name;
        this->key = key;
        this->scp = sc;
        this->parameters = para;
        this->type = -1;

        if (para != -1) {
            this->args = new int[para];
            for (int i=0; i<para; i++) {
                this->args[i] = -1;
            }
        }
    }
};

class HashMap {
protected:
    HashNode** arr;
    int capacity;
    int size, c;

public:
    HashMap() {}

    virtual ~HashMap() {
        for (int i=0; i < capacity; i++) {
            if (arr[i] == NULL) {}
            else {
                delete [] arr[i]->args;
                delete arr[i];
                arr[i] = NULL;
            }
        }
        delete [] arr;
        arr = NULL;
    }

    virtual int hashKey(long long key) { return key % capacity; }

    virtual int hashProbing(long long k, int i)=0;
    
    void insertNode(long long key, string value, int scp, int& slot, string cmd, int para=-1) {
        if (size == capacity) { throw(Overflow(cmd)); }
        HashNode* tmp = new HashNode(key, value, scp, para);

        int hashIndex = hashKey(key);
        int i = 1, counter = 0;

        while (arr[hashIndex] != NULL && arr[hashIndex]->key != key) {
            if (counter++ > capacity) {
                throw(Overflow(cmd));
            }
            slot++;
            hashIndex = hashProbing(key, i);
            i++;
            hashIndex %= capacity;
        }
        
        if (arr[hashIndex] == NULL) {
            size++;
        } else if (arr[hashIndex]->key == key) {
            throw(Redeclared(arr[hashIndex]->id));
        }
        arr[hashIndex] = tmp;
    }

    int get_pos(long long key) {
        int hashIndex = hashKey(key);
        int counter = 0, i = 1;
  
        while (arr[hashIndex] != NULL) { 
            if (counter++ > capacity) {
                return -1;
            }
  
            if (arr[hashIndex]->key == key)
                return hashIndex;
            hashIndex = hashProbing(key, i);
            i++;
            hashIndex %= capacity;
        }
        return -1;
    }

    virtual HashNode* get(long long key, int& slot) {
        int hashIndex = hashKey(key);
        int counter = 0, i = 1;
  
        while (arr[hashIndex] != NULL) { 
            if (counter++ > capacity) {
                return NULL;
            }
  
            if (arr[hashIndex]->key == key){
                return arr[hashIndex];
            }
            hashIndex = hashProbing(key, i);
            i++;
            slot++;
            hashIndex %= capacity;
        }
  
        return NULL;
    }

    void deleteScope(int scpe) {
        if (scpe < 0) return;
        for (int i=0; i < capacity; i++) {
            if (arr[i] == NULL) {}
            else {
                if (arr[i]->scp == scpe) {
                    delete arr[i];
                    arr[i] = NULL;
                }
            }
        }
    }

    void print() {
        int i=0;
        for (;i < capacity; i++) {
            if (arr[i] != NULL) {
                cout << i << " "
                << arr[i]->id << "//"
                << arr[i]->scp;
                break;
            }
        }
        if (i == capacity) return;
        for (i++;i < capacity; i++) {
            if (arr[i] != NULL) {
                cout << ";" << i << " "
                << arr[i]->id << "//"
                << arr[i]->scp;
            }
        }
        cout << '\n';
    }

};

class LinearHashing : public HashMap {
public:
    LinearHashing() {}

    LinearHashing(int cap, int cns) {
        capacity = cap;
        c = cns;
        size = 0;
        arr = new HashNode*[capacity];

        for (int i=0; i < capacity; i++) {
            arr[i] = NULL;
        }
    }

    int hashProbing(long long k, int i) {
        return (hashKey(k) + c*i) % capacity;
    }
};

class QuadraticHashing : public HashMap {
protected:
    int c1, c2;

public:
    QuadraticHashing() {}

    QuadraticHashing(int cap, int cns1, int cns2) {
        capacity = cap;
        c1 = cns1;
        c2 = cns2;
        size = 0;        
        arr = new HashNode*[capacity];

        for (int i=0; i < capacity; i++) {
            arr[i] = NULL;
        }
    }

    int hashProbing(long long k, int i) {
        return (hashKey(k) + c1*i + c2*i*i) % capacity;
    }
};

class DoubleHashing : public HashMap {
public:
    DoubleHashing() {}

    DoubleHashing(int cap, int cns) {
        capacity = cap;
        c = cns;
        size = 0;         
        arr = new HashNode*[capacity];

        for (int i=0; i < capacity; i++) {
            arr[i] = NULL;
        }        
    }

    int hashKey2(long long key) {
        return 1 + (key % (capacity - 2));
    }

    int hashProbing(long long k, int i) {
        return (hashKey(k) + c*i*hashKey2(k)) % capacity;
    }
};

class SymbolTable
{
protected:
    HashMap* hash_ptr = NULL;
    
public:
    SymbolTable() {}
    ~SymbolTable() {
        if (hash_ptr == NULL) {}
        else {
            delete hash_ptr;
            hash_ptr = NULL;
        }
    }
    void run(string filename);
    bool is_valid_argument(string str, string cmd, const int& scp, int& slot, HashNode* funct);

};
#endif