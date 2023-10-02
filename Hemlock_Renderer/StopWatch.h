#pragma once
#include <chrono>
#include <bitset>

class stopwatch_t
{
public:

	void stopwatch();

	template<typename T>
	T returncurrenttime();

	int returnseconds();
	int returnminutes();
	int returnhours();
	float returnMiliseconds();
	float returnMicroseconds();

	void resetstopwatch();

	template <typename T, typename Rep, typename Period>
	T duration_cast(const std::chrono::duration<Rep, Period>& duration)
	{
		return duration.count() * static_cast<T>(Period::num) / static_cast<T>(Period::den);
	}

private:

	std::chrono::steady_clock::time_point start;
	int mins;
	int hours;
	int seconds;
	float miliseconds;
	float microSeconds;
	std::bitset<2> clockchecker;

};
