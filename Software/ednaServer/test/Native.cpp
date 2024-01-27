#include <KPController.hpp>
#include <ArduinoJson.h>
#include <StateControllers/HyperFlushStateController.hpp>

class TestApplication : public KPController {
public:
    HyperFlushStateController hyperFlushController;

    void setup() override {
        char name[100]{0};
        StaticJsonDocument<1000> doc;
        doc["name"].set(name);
        addComponent(hyperFlushController);
    }
} app;

void setup() {
    app.setup();
}

void loop() {
    app.update();
}