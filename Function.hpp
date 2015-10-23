#ifndef __FUNCTION_HPP_
# define __FUNCTION_HPP_
# include <exception>
# include <functional>
# include <iostream>

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
	bool			_alternate = false;

public:
	explicit	Function() {}

	explicit 	Function(void (*func)()) : _func_ptr(func) {

	}

	virtual		~Function() {

	}

	inline void	setAlternate(bool value) {
		this->_alternate = value;
	}

	inline bool	getAlternate() const noexcept {
		return this->_alternate;
	}

	inline void	set(void (*func)()) {
		this->_func_ptr = func;
	}

	template <typename Ret, typename ... Args>
	Ret call(Args ... arguments) {
		Function<Ret, Args...>	*ptr = dynamic_cast<Function<Ret, Args...>*>(this);
		if (ptr != nullptr && !this->getAlternate())
			return reinterpret_cast<Ret (*)(Args...)>(this->_func_ptr)(arguments...);
		else if (ptr != nullptr && this->getAlternate())
			return reinterpret_cast<Ret (*)(Function<Ret, Args...>*, Args...)>(this->_func_ptr)(ptr, arguments...);
		throw InvalidCall();
	}
};

template <typename Ret>
class Function<Ret> : public Function<void>
{
public:
	explicit	Function() : Function<void>(nullptr) {}

	explicit 	Function(Ret (*func)()) : Function<void>(reinterpret_cast<void (*)()>(func)) {

	}

	virtual		~Function() {

	}

	inline void		set(Ret (*func)()) {
		Function<void>::set(reinterpret_cast<void (*)()>(func));
	}

	template <typename ... Args>
	inline Ret call(Args ... arguments) {
		return Function<void>::template call<Ret, Args...>(arguments...);
	}
};

template <typename Ret, typename ... Args>
class Function : public Function<Ret>
{
public:
	explicit Function() : Function<Ret>(nullptr) {}

	explicit Function(Ret (*func)(Args...)) : Function<Ret>(reinterpret_cast<Ret (*)()>(func)) {

	}

	explicit Function(Ret (*func)(Function<Ret, Args...>*, Args...)) : Function<Ret>(reinterpret_cast<Ret (*)()>(func)) {
		this->setAlternate(true);
	}

	virtual ~Function() {

	}

	inline void	set(Ret (*func)(Args...)) {
		Function<Ret>::set(reinterpret_cast<Ret (*)()>(func));
		this->setAlternate(false);
	}

	inline void	set(Ret (*func)(Function<Ret, Args...>*, Args...)) {
		Function<Ret>::set(reinterpret_cast<Ret (*)()>(func));
		this->setAlternate(true);
	}

	inline Ret	call(Args... arguments) {
		return Function<Ret>::call(arguments...);
	}
};

template <typename Object, typename Ret, typename ... Args>
class MemberFunction : public Function<Ret, Args...>
{
private:
	std::function<Ret(Args...)>									_function;

	static Ret	_call(Function<Ret, Args...>* self, Args ... arguments) {
		return dynamic_cast<MemberFunction<Object, Ret, Args...>*>(self)->call(arguments...);
	}

public:
	explicit	MemberFunction(Object * obj, Ret (Object::*func)(Args...)) : Function<Ret, Args...>()
	{
		this->_function = [obj, func](Args... arguments) {
			return reinterpret_cast<Ret (*)(Object*, Args...)>(func)(obj, arguments...);
		};
		Function<Ret, Args...>::set(&MemberFunction<Object, Ret, Args...>::_call);
	}

	virtual		~MemberFunction() {

	}

	inline void	set(Object * obj, Ret (Object::*func)(Args...)) {
		this->_function = [obj, func](Args... arguments) {
			return reinterpret_cast<Ret (*)(Object*, Args...)>(func)(obj, arguments...);
		};
	}

	inline Ret	call(Args... arguments) {
		return this->_function(arguments...);
	}
};

#endif