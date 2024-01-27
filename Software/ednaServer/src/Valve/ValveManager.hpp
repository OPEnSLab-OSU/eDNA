#pragma once
#include <KPFoundation.hpp>
#include <KPSubject.hpp>
#include <Application/Config.hpp>
#include <Valve/Valve.hpp>
#include <Valve/ValveStatus.hpp>
#include <Valve/ValveObserver.hpp>
#include <Utilities/FileLoader.hpp>

#include <vector>

//
// ────────────────────────────────────────────────────────────────── I ──────────
//   :::::: V A L V E   M A N A G E R : :  :   :    :     :        :          :
//   ────────────────────────────────────────────────────────────────────────────
//

class ValveManager : public JsonEncodable, public KPSubject<ValveObserver> {
public:
    std::vector<Valve> valves;
    const char * valveFolder   = nullptr;
    size_t numberOfValvesInUse = 0;

    /** ────────────────────────────────────────────────────────────────────────────
     *  Initialize ValveManager with the config object. This method sets
     *  status for each valve according to config object.
     *
     *  @param config onfig object containing meta information about the system
     *  ──────────────────────────────────────────────────────────────────────────── */
    void init(Config & config) {
        valveFolder = config.valveFolder;
        valves.resize(config.numberOfValves);

        for (size_t i = 0; i < valves.size(); i++) {
            auto valveAvailability = config.valves[i];
            ValveStatus status     = ValveStatus::Code(valveAvailability ? valveAvailability : -1);
            valves[i].id           = i;
            valves[i].setStatus(status);

            if (status != ValveStatus::unavailable) {
                numberOfValvesInUse++;
            }

            println(status);
        }

        updateObservers(&ValveObserver::valveArrayDidUpdate, valves);
    }

    void setValveStatus(int id, ValveStatus status) {
        valves[id].setStatus(status);
        updateObservers(&ValveObserver::valveDidUpdate, valves[id]);
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  @brief Set the status of the valve to "free" if the valve is not yet sampled
     *
     *  @param id Id of the valve (usally the index number)
     *  ──────────────────────────────────────────────────────────────────────────── */
    void setValveFreeIfNotYetSampled(int id) {
        if (valves[id].status != ValveStatus::sampled) {
            setValveStatus(id, ValveStatus::free);
            updateObservers(&ValveObserver::valveDidUpdate, valves[id]);
        }
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  @brief Update valve objects from JSON array
     *
     *  @param task_array SON Array encoded by ArduinoJson
     *  ──────────────────────────────────────────────────────────────────────────── */
    void updateValves(const JsonArray & task_array) {
        for (const JsonObject & object : task_array) {
            int id = object[ValveKeys::ID];
            if (valves[id].status != ValveStatus::sampled) {
                valves[id].decodeJSON(object);
            } else {
                println("Valve is already sampled");
            }
        }

        updateObservers(&ValveObserver::valveArrayDidUpdate, valves);
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  @brief Reading and decode each JSON file in the given directory to
     *  corresponding valve object.
     *
     *  @param _dir Path to the valve folder (default=~/valves)
     *  ──────────────────────────────────────────────────────────────────────────── */
    void loadValvesFromDirectory(const char * _dir = nullptr) {
        const char * dir = _dir ? _dir : valveFolder;

        JsonFileLoader loader;
        loader.createDirectoryIfNeeded(dir);

        auto start = millis();
        for (size_t i = 0; i < valves.size(); i++) {
            if (valves[i].status != ValveStatus::unavailable) {
                KPStringBuilder<32> filename("valve-", i, ".js");
                KPStringBuilder<64> filepath(dir, "/", filename);
                loader.load(filepath, valves[i]);
            }
        }

        println(GREEN("Valve Manager"), " finished reading in ", millis() - start, " ms\n");
        updateObservers(&ValveObserver::valveArrayDidUpdate, valves);
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  @brief Encode and store each valve object to corresponding JSON file in the
     *  given directory
     *
     *  @param _dir Path to the valve folder (default=~/valves)
     *  ──────────────────────────────────────────────────────────────────────────── */
    void writeToDirectory(const char * _dir = nullptr) {
        const char * dir = _dir ? _dir : valveFolder;

        JsonFileLoader loader;
        loader.createDirectoryIfNeeded(dir);

        auto start = millis();
        for (size_t i = 0; i < valves.size(); i++) {
            if (valves[i].status != ValveStatus::unavailable) {
                KPStringBuilder<32> filename("valve-", i, ".js");
                KPStringBuilder<64> filepath(dir, "/", filename);
                loader.save(filepath, valves[i]);
            }
        }

        println("\033[1;32mValveManager\033[0m: finished writing in ", millis() - start, " ms");
        updateObservers(&ValveObserver::valveArrayDidUpdate, valves);
    }

#pragma region JSONENCODABLE
    static const char * encoderName() {
        return "ValveManager";
    }

    static constexpr size_t encodingSize() {
        return Valve::encodingSize() * ProgramSettings::MAX_VALVES;
    }

    bool encodeJSON(const JsonVariant & dest) const {
        for (decltype(auto) v : valves) {
            if (!v.encodeJSON(dest.createNestedObject())) {
                return false;
            }
        }

        return true;
    }
#pragma endregion
};