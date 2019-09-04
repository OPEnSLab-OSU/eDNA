#pragma once
#include "KPAction.hpp"

class KPActionScheduler : public List<KPSequentialAction> {
protected:
	using List::removeAll;
public:    
    bool contains(const char * name) {
        auto current = head->next;
        while (current != tail) {
            KPSequentialAction & sa = current->data;
            if (strcmp(sa.name, name) == 0) {
                return true;
            }
            current = current->next;
        }
        
        return false;
    }

	bool removeAction(const char * name) {
		auto current = head->next;
        while (current != tail) {
            KPSequentialAction & sa = current->data;
            if (strcmp(sa.name, name) == 0) {
                remove(current);
				Serial.print("Removed action: ");
				Serial.println(name);
				return true;
            }
			
            current = current->next;
        }
        
        return false;
	}

	void removeAllActions() {
		removeAll();
	}
    
    void update() {
        if (isEmpty()) {
            return;
        }
        
        auto current = head->next;
        while (current != tail) {
            KPSequentialAction & sa = current->data;
            auto next = current->next;
            if (sa.isEmpty() || sa.repeat == 0) {
                remove(current);
            } else {
                sa.update();
            }

            current = next;
        }
    }
};
