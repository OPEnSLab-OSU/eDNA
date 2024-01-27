#pragma once
#include <KPSubject.hpp>
#include <KPStateMachineObserver.hpp>
#include <unordered_map>

class KPState;
class KPStateMachine : public KPComponent, public KPSubject<KPStateMachineObserver> {
private:
	using Middleware = std::function<void(int)>;
	using StateName	 = const char *;
	std::unordered_map<StateName, KPState *> mapNameToState;
	std::unordered_map<StateName, Middleware> mapNameToMiddleware;
	KPState * currentState = nullptr;

public:
	using KPComponent::KPComponent;

	/**
	 * Register a state with a given unique name. "Optionally" accepts a
	 * middleware functor as the third parameter.
	 *
	 * The middleware should define how the state would transition to another
	 * state according a passed-in integer value.
	 *
	 * @tparam T Deduced subtype of the state
	 * @param state State instance
	 * @param name Name of the state being registered
	 * @param middleware Any callable with signature: void(int)
	 */
	template <typename T>
	void registerState(T && state, StateName name, Middleware middleware = nullptr) {
		if (mapNameToState.count(name)) {
			halt(TRACE, name, " is already used");
		}

		if (name == nullptr) {
			halt(TRACE, "State must have a name");
		}

		T * copy			 = new T{std::forward<T>(state)};
		copy->name			 = name;
		mapNameToState[name] = copy;
		if (middleware) {
			mapNameToMiddleware[name] = middleware;
		} else {
			mapNameToMiddleware[name] = [name](int code) {
				halt(TRACE, "Unhandled state transition: ", name);
			};
		}
	}

	/**
	 * Convenient method for registering a state with one transition path
	 *
	 * @tparam T Deduced subtype of the state
	 * @param state State instance
	 * @param name Name of the state being registered
	 * @param next Name of the next state to transition to
	 */
	template <typename T>
	void registerState(T && state, StateName name, StateName next) {
		registerState(std::forward<T>(state), name, [this, next](int code) { transitionTo(next); });
	}

	/**
	 * Get the State object with optional static casting to the given state
	 * subclasss. The function doesn't return if the name doesn't exist.
	 *
	 * @tparam KPState Type to cast to
	 * @param name Name of the state registered in registerState()
	 * @return T& Reference to state in storage
	 */
	template <typename T = KPState>
	T & getState(StateName name) {
		auto c = mapNameToState.find(name);
		if (c != mapNameToState.end()) {
			return *static_cast<T *>(c->second);
		} else {
			halt(TRACE, "Unregistered state name: ", name);
		}
	}

	/**
	 * Get the Current State object
	 *
	 * @return KPState* Ptr to the internal state object
	 */
	KPState * getCurrentState() const {
		return currentState;
	}

	/**
	 * This method is to be called in subclassese of KPState
	 *
	 * @param code exitcode StateController/StateMachine should decide how to
	 * deal with this code
	 */
	void next(int code = 0) const;

	/**
	 * Restart the state by passing the currentState name to transitionTo(name)
	 */
	void restart();

	/**
	 * Transition to the state registered using the given name
	 *
	 * @param name Name of a state that was registered by calling
	 * registerState()
	 */
	void transitionTo(StateName name);

protected:
	void setup() override;
	void update() override;
};