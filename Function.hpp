#ifndef __FUNCTION_HPP_
# define __FUNCTION_HPP_
# include <exception>

struct InvalidCall : public std::exception
{
	explicit 	InvalidCall() noexcept = default;
	virtual		~InvalidCall() noexcept = default;

	virtual const char *what() const noexcept {
		return "Invalid call to dynamic function";
	}
};

template <typename Ret, typename ... Args>
class Function;

template <>
class Function<void>
{
protected:
	void			(*_func_ptr)();

public:
	explicit 	Function(void (*func)()) : _func_ptr(func) {

	}

	virtual		~Function() {

	}

	template <typename Ret, typename ... Args>
	Ret call(Args ... arguments) {
		Function<Ret, Args...>	*ptr = dynamic_cast<Function<Ret, Args...>*>(this);
		if (ptr != nullptr)
			return ptr->call(arguments...);
		throw InvalidCall();
	}
};

template <typename Ret>
class Function<Ret> : public Function<void>
{
public:
	explicit 	Function(Ret (*func)()) : Function<void>(reinterpret_cast<void (*)()>(func)) {

	}

	virtual		~Function() {

	}

	template <typename ... Args>
	inline Ret call(Args ... arguments) {
		return reinterpret_cast<Ret (*)(Args...)>(Function<void>::_func_ptr)(arguments...);
	}
};

template <typename Ret, typename ... Args>
class Function : public Function<Ret>
{
public:
	explicit Function(Ret (*func)(Args...)) : Function<Ret>(reinterpret_cast<Ret (*)()>(func)) {

	}

	virtual ~Function() {

	}

	inline Ret	call(Args... arguments) {
		return Function<Ret>::call(arguments...);
	}
};

template <typename Object, typename Ret, typename ... Args>
class MemberFunction : public Function<Ret, Args...>
{
private:
	Object		*_object;

public:
	explicit	MemberFunction(Object * obj, Ret (Object::*func)(Args...)) : Function<Ret, Args...>(reinterpret_cast<Ret (*)(Args...)>(func)),
		_object(obj)
	{}

	virtual		~MemberFunction() {

	}

	inline Ret	call(Args... arguments) {
		return Function<Ret, Args...>::call(this->_object, arguments...);
	}
};

#endif