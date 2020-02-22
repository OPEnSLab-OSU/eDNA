#pragma once
#include <KPFoundation.hpp>
#include <KPDataStoreInterface.hpp>
#include <KPArray.hpp>

#include <Application/Config.hpp>
#include <Valve/Valve.hpp>
#include <Valve/ValveStatus.hpp>

#include <Utilities/FileLoader.hpp>

#include <vector>

class ValveManager;
class ValveManagerEventListner {
public:
	virtual void valvesChanged(const ValveManager & vm) = 0;
};

//
// ─── SECTION VALVE MANAGER ──────────────────────────────────────────────────────
//
// NOTE: This object provide APIs for keeping track of the valve system
// ────────────────────────────────────────────────────────────────────────────────
class ValveManager {
private:
	std::vector<ValveManagerEventListner *> listeners;

public:
	constexpr static size_t MAX_VALVES = ProgramSettings::MAX_VALVES;
	KPArray<Valve, MAX_VALVES> valves;

	void init(Config & config) {
		valves.resize(config.valveUpperBound + 1);
		for (size_t i = 0; i < valves.size(); i++) {
			valves[i].id = i;
			valves[i].setStatus(ValveStatus::Code(config.valves[i]));
		}
	}

	void updateListeners() {
		for (auto listener : listeners) {
			listener->valvesChanged(*this);
		}
	}

	void addListener(ValveManagerEventListner * listener) {
		listeners.push_back(listener);
	}

	void setValveSampled(int id) {
		setValveStatus(id, ValveStatus::sampled);
	}

	void setValveOperating(int id) {
		setValveStatus(id, ValveStatus::operating);
	}

	void setValveStatus(int id, ValveStatus status) {
		valves[id].setStatus(status);
		updateListeners();
	}

	KPArray<int, MAX_VALVES> filter(std::function<bool(const Valve &)> filter_func) {
		KPArray<int, MAX_VALVES> ids;
		for (const Valve & v : valves) {
			if (filter_func(v)) {
				ids.append(v.id);
			}
		}

		return ids;
	}

	void loadValvesFromDirectory(const char * dir) {
		FileLoader loader;
		loader.createDirectoryIfNeeded(dir);

		unsigned long start = millis();
		for (size_t i = 0; i < valves.size(); i++) {
			KPStringBuilder<32> filename("valve-", i < 10 ? "0" : "", i, ".js");
			KPStringBuilder<64> filepath(dir, "/", filename);
			valves[i].load(filepath);
			valves[i].id = i;
		}

		println("ValveManager: finished loading in ", millis() - start, " ms");
		updateListeners();
	}

	void updateValves(const JsonArrayConst & task_array) {
		for (const JsonObjectConst & object : task_array) {
			int id = object[JsonKeys::VALVE_ID];
			if (valves[id].status) {
				valves[id].decodeJSON(object);
			} else {
				println("Valve is already sampled");
			}
		}

		updateListeners();
	}

	void saveValvesToDirectory(const char * dir) {
		FileLoader loader;
		loader.createDirectoryIfNeeded(dir);

		unsigned long start = millis();
		for (size_t i = 0; i < valves.size(); i++) {
			KPStringBuilder<32> filename("valve-", i < 10 ? "0" : "", i, ".js");
			KPStringBuilder<64> filepath(dir, "/", filename);
			valves[i].save(filepath);
		}

		println("ValveManager: finished saving in ", millis() - start, " ms");
		updateListeners();
	}
};