#ifndef EVENT
#define EVENT

#include <vector>
#include <functional>

template <typename TFunc>
class Event;

template <class RetType, class... Args>
class Event<RetType(Args...)> final
{
	typedef typename std::function<RetType(Args ...)> Closure;
public:
	inline void operator+=(Closure c)
	{
		observers.push_back(c);
	}
	inline void operator()(Args... args)
	{
		for (auto closure : observers)
			closure(args...);
	}
private:
	std::vector<Closure> observers;
};

#endif //EVENT
