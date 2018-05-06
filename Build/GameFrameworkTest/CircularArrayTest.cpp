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
            const int N = 10;
            NS_FW_UTIL::CircularArray<int, N> circularArray;

            circularArray.Fill(-1);

            Assert::IsTrue(circularArray.IsEmpty());
            for (int i = 0; i <  N; ++i)
            {
                Assert::IsTrue(circularArray.Enqueue(i));
            }
            Assert::IsFalse(circularArray.IsEmpty());
            Assert::IsFalse(circularArray.Enqueue(11));

            for (int i = 0; i < N; ++i)
            {
                int value = 0;
                circularArray.Dequeue(value);
                Assert::AreEqual(i, value);
            }
            int value = 0;
            Assert::IsFalse(circularArray.Dequeue(value));
            Assert::IsTrue(circularArray.IsEmpty());
        }
    };
}
