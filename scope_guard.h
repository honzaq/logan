#pragma once

#include <functional>

class scope_guard {
public:
	template<class Callable>
	scope_guard(Callable && undo_func) : f(std::forward<Callable>(undo_func)) {}

	scope_guard(scope_guard && other) : f(std::move(other.f)) {
		other.f = nullptr;
	}

	~scope_guard() {
		if(f) f(); // must not throw
	}

	void dismiss() noexcept {
		f = nullptr;
	}

	scope_guard(const scope_guard&) = delete;
	void operator=(const scope_guard&) = delete;

private:
	std::function<void()> f;
};
