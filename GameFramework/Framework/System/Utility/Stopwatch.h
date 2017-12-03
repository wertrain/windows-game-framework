#ifndef STOPWATCH_H_
#define STOPWATCH_H_

namespace Framework {
namespace System {
namespace Utility {

/// <summary>
/// ���Ԍv���p���[�e�B���e�B
/// </summary>
class Stopwatch
{
public:
    Stopwatch();

    void Start();
    __int64 GetElapsed();

private:
    std::chrono::system_clock::time_point mStart;
};

} // namespace Utility
} // namespace System
} // namespace Framework

#endif // STOPWATCH_H_
