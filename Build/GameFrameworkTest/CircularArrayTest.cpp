#include "stdafx.h"
#include "CppUnitTest.h"

#include <nnfw/Includes.h>
#include <nnfw/ut/CircularArray.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GameFrameworkTest
{
    TEST_CLASS(CircularArrayTest)
    {
    public:
        
        TEST_METHOD(CircularArrayEnqueue)
        {
            NS_FW_UTIL::CircularArray<int, 10> circularArray;

            circularArray.Clear(100);

            for (int i = 0; i < 9; ++i)
            {
                Assert::IsTrue(circularArray.Enqueue(i));
            }

            for (int i = 0; i < 9; ++i)
            {
                Assert::AreEqual(i, circularArray.Dequeue());
            }

            //fw::ut::CircularArray<>
            //std::string name = "Bill";
            //MyClass mc(name);
            //Assert::AreEqual(name, mc.GetName());
        }

    };
}