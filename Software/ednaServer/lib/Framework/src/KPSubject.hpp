
// References:
// https://embeddedartistry.com/blog/2017/02/01/improving-your-callback-game/
// https://en.cppreference.com/w/cpp/language/parameter_pack
#pragma once
#include <KPFoundation.hpp>
#include <functional>
#include <unordered_map>

template <typename T>
class KPSubject {
protected:
	using ObserverType = T;
	std::unordered_map<long, ObserverType *> observers;

public:
	int addObserver(ObserverType & o) {
		return addObserver(&o);
	}

	int addObserver(ObserverType * o) {
		int token = rand();
		while (!observers.insert({token, o}).second) {
			token = rand();
		}

		return token;
	}

	void removeObserver(long token) {
		observers.erase(token);
	}

	template <typename F, typename... Types>
	void updateObservers(F method, Types &&... args) {
		for (auto & k : observers) {
			println("Updating ", k.second->ObserverName());
			(k.second->*method)(std::forward<Types>(args)...);
		}
	}
};