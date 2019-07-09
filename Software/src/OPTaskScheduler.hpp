#pragma once
#include "OPTask.hpp"
#include "OPComponent.hpp"

class OPTaskScheduler : public LinkedList<OPTask> {
public:
    OPTaskScheduler() : LinkedList() { }
    
    bool contains(const String & name) {
        auto current = head->next;
        while (current != tail) {
            OPTask & task = current->data;
            if (task.name == name) {
                return true;
            }
            current = current->next;
        }
        
        return false;
    }
    
    void update() {
        if (isEmpty()) {
            return;
        }
        
        auto current = head->next;
        while (current != tail) {
            OPTask & task = current->data;
            auto next = current->next;
            if (task.isEmpty() || task.repeat == 0) {
                remove(current);
            } else {
                task.update();
            }

            current = next;
        }
    }
};
