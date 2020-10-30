#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <chrono>

// Remove duplicate elements in an unsorted vector.
template <typename FwdIterator>
FwdIterator removeDuplicates(FwdIterator first, FwdIterator last)
{
	auto newLast = first;

	for (auto current = first; current != last; ++current)
		if (std::find(first, newLast, *current) == newLast)
		{
			if (newLast != current)
				*newLast = *current;
			++newLast;
		}

	return newLast;
}

struct timer
{
	timer() { start = std::chrono::high_resolution_clock::now(); }

	~timer()
	{
		std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
		std::cout << "Elapsed time: " << duration.count() * 1000.0f << "ms.\n";
	}

	std::chrono::time_point<std::chrono::steady_clock> start, end;
};

#endif
