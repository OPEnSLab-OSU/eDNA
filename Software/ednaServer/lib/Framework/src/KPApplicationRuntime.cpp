#include <KPApplicationRuntime.hpp>
#include <KPController.hpp>
#include <KPSerialInput.hpp>

namespace Runtime {
	KPController * initialController;
	void setInitialAppController(KPController & controller) {
		initialController = &controller;
		initialController->setup();
	}

	void update() {
		initialController->update();
	}
};	// namespace Runtime