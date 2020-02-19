#pragma once
class ValveStatus {
public:
	enum Code {
		inactive = -1,
		sampled,
		free,
		operating
	} _code;

	ValveStatus(Code code)
		: _code(code) {
	}

	Code code() const {
		return _code;
	}

	operator int() const {
		return _code;
	}
};