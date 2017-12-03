#include <memory>
#include <chrono>

#include "Stopwatch.h"

namespace Framework {
namespace System {
namespace Utility {

Stopwatch::Stopwatch()
    : mStart()
{

}

void Stopwatch::Start()
{
    mStart = std::chrono::system_clock::now();
}

__int64 Stopwatch::GetElapsed()
{
    auto end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - mStart).count();
}

} // namespace Utility
} // namespace System
} // namespace Framework
