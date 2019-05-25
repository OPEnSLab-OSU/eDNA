//
//  LinkedList.hpp
//  OPEnSamplerFramework
//
//  Created by Kawin on 2/10/19.
//  Copyright © 2019 Kawin. All rights reserved.
//

#pragma once

//--------------------------------------------------------
// Implementation of doubly linkedlist
//--------------------------------------------------------
template <typename T>
class LinkedList {
protected:
    struct Node {
        T data;
        Node * next = nullptr;
        Node * prev = nullptr;
        Node(T data) : data(data) {}
        Node() : data{} {}
    };
    
    Node * head = nullptr;
    Node * tail = nullptr;
    friend class OPSystem;

protected:
    void deInit() {
        auto current = head;
        while (current) {
            auto trash = current;
            current = current->next;
            delete trash;
        }

        size = 0;
        head = nullptr;
        tail = nullptr;
    }
    
    void append(Node * node) {
        tail->prev->next = node;
        node->prev = tail->prev;
        node->next = tail;
        tail->prev = node;
        size++;
    }

    Node * searchFromHead(int index) {
        if (invalidateIndex(index) == false) {
            return nullptr;
        }
        
		int i = 0;
        auto current = head->next;
        while(i != index) {
            current = current->next;
            i++;
        }
        return current;
    }
    
    Node * searchFromTail(int index) {
        if (invalidateIndex(index) == false) {
            return nullptr;
        }
        
		int i = size - 1;
        auto current = tail->prev;
        while (i != index) {
            current = current->prev;
            i--;
        }
        return current;
    }

    Node * search(int index) {
        return index < size / 2 ? searchFromHead(index) : searchFromTail(index);
    }
    
//------------------------------------------------------------------------------
//  Public Methods
//------------------------------------------------------------------------------
public:
    int size = 0;
    LinkedList<T>(){
        head = new Node();
        tail = new Node();
        head->next = tail;
        tail->prev = head;
    };
    
    // Copy-constructor override
    LinkedList<T>(const LinkedList<T> &obj) {
        head = new Node();
        tail = new Node();
        head->next = tail;
        tail->prev = head;
        auto source = obj.head->next;
        while (source != obj.tail) {
            auto node = new Node(*source);
            append(node);
            source = source->next;
        }
    }
    
    // Copy-assignment operator override
    LinkedList<T> & operator=(LinkedList<T> const & rhs) {
        deInit();
        head = rhs.head;
		tail = rhs.tail;
		size = rhs.size;
        return * this;
    }
    
    ~LinkedList<T>() {
        deInit();
    }

    bool isEmpty() {
        return size == 0;
    }
    
    void append(const T & data) {
        append(new Node(data));
    }
    
    void extend(LinkedList<T> const & other) {
        auto source = other.head->next;
        while (source != other.tail) {
            auto node = new Node(*source);
            node->next = nullptr;
            node->prev = nullptr;
            append(node);
            source = source->next;
        }
    }

    bool invalidateIndex(int index) {
        return index >= 0 && index <= size - 1;
    }
    
    T & get(int index) {
        
        return search(index)->data;
    }

    bool remove(int index) {
        if (invalidateIndex(index) == false) {
            return false;
        }
        
        auto target = index < size / 2 ? searchFromHead(index) : searchFromTail(index);
        remove(target);
        return true;
    }
    
    void remove(Node * target) {
        target->prev->next = target->next;
        target->next->prev = target->prev;
        size--;
        delete target;
    }
};
