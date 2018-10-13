#include "stdafx.h"
#include "CppUnitTest.h"

#include <iostream>
#include <ctime>
#include <nnfw/ut/Random.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GameFrameworkTest
{
    ///
    /// ランダム関数のテスト
    ///
    TEST_CLASS(RandomTest)
    {
    public:
        TEST_METHOD(RandomTestBasic)
        {
            char buffer[256];

            fw::util::Random random((unsigned long)time(nullptr));
            for (int i = 0; i < 16; ++i)
            {
                float f = random.NextFloat();
                sprintf_s(buffer, "Float %f\n", f);
                Logger::WriteMessage(buffer);
            }
            for (int i = 0; i < 16; ++i)
            {
                uint32_t f = random.NextUInt();
                sprintf_s(buffer, "Uint %u\n", f);
                Logger::WriteMessage(buffer);
            }
            for (int i = 0; i < 16; ++i)
            {
                uint32_t f = random.GetRange(0, 10);
                sprintf_s(buffer, "Uint %u\n", f);
                Logger::WriteMessage(buffer);
            }
        }
    };
}
