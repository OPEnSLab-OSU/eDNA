#pragma once
#include "OPTask.hpp"

class OPTaskScheduler : public List<OPTask> {
public:    
    bool contains(const char * name) {
        auto current = head->next;
        while (current != tail) {
            OPTask & task = current->data;
            if (strcmp(task.name, name) == 0) {
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
