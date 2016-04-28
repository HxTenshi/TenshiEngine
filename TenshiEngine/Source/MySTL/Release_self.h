#pragma once

#include <type_traits>

template<typename T>
class has_Release
{
private:
	template<typename U>
	static auto check(U v) -> decltype(v.Release(), std::true_type());
	template<typename U>
	static auto check(U* v) -> decltype(v->Release(), std::true_type());
	static auto check(...) -> decltype(std::false_type());

public:
	typedef decltype(check(std::declval<T>())) type;
	typedef decltype(check(std::declval<T*>())) type_ptr;
	static bool const value = type::value || type_ptr::value;
};

template<class T>
class Release_self{
public:
	Release_self()
		: m_ptr(NULL)
	{
		static_assert(!std::is_pointer<T>::value, "Release_self : Pointer Not Support.");
		static_assert(has_Release<T>::value, "Release_self : Release No Member Function.");
	}
	~Release_self()
	{
		if (m_ptr){
			m_ptr->Release();
			m_ptr = NULL;
		}
	}

	T* Get() const{
		return m_ptr;
	}


	Release_self<T>& operator = (T* value){

		if (m_ptr){
			m_ptr->Release();
			m_ptr = NULL;
		}

		m_ptr = value;

		return *this;
	}


private:
	//ÉRÉsÅ[ã÷é~
	Release_self<T>(const Release_self<T>&) = delete;
	Release_self<T>& operator=(const Release_self<T>&) = delete;

	T* m_ptr;
};