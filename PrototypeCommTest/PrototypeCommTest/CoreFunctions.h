#ifndef COREFUNCTIONS_H_
#define COREFUNCTIONS_H_

#include <cstdint>
#include <mutex>
#include <vector>

typedef uint64_t millisecond;
typedef uint64_t microsecond;

millisecond millisecondsNow();
microsecond microsecondsNow();

void analyzeOutput(std::vector<microsecond> &vRunTimes);

extern std::mutex mtxPrint;

#endif