#include "stdafx.h"
#include "CppUnitTest.h"

#include <iostream>
#include <ctime>
#include <nnfw/ut/Random.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GameFrameworkTest
{
    TEST_CLASS(RandomTest)
    {
    public:
        TEST_METHOD(RandomTestBasic)
        {
            char buffer[256];

            fw::util::Random random(time(nullptr));
            for (int i = 0; i < 10; ++i)
            {
                float f = random.NextFloat();
                
                sprintf_s(buffer, "Float %f\n", f);
                Logger::WriteMessage(buffer);
            }
        }
    };
}
