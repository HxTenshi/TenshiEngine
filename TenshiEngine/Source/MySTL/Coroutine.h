#pragma once

#include <list>
#include <functional>
#include <bitset>
#include <sstream>

#define YIELD_BREAK {return;}
#define YIELD_RETURN_NULL {*((int*)0) = 0;}


class Coroutine;

class CoroutineSystem{
public:
	CoroutineSystem();
	~CoroutineSystem();
	void StartCoroutine(const std::function<void(void)>& coroutine);
	void Tick();
	int ActiveCoroutineCount();

private:

	//ÉRÉsÅ[ã÷é~
	CoroutineSystem(const CoroutineSystem&) = delete;
	CoroutineSystem& operator=(const CoroutineSystem&) = delete;

	std::list<Coroutine*> mCoroutines;
};

