#include "stdafx.h"
#include "CppUnitTest.h"

#include <nnfw/Includes.h>
#include <nnfw/ut/CircularArray.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GameFrameworkTest
{
    ///
    /// 循環配列のテスト
    ///
    TEST_CLASS(CircularArrayTest)
    {
    public:        
        TEST_METHOD(FixedCircularArrayTestBasic)
        {
            const size_t N = 512;
            NS_FW_UTIL::FixedCircularArray<int, N> circularArray;

            TestCircularArray<int>(&circularArray, N);
        }

        TEST_METHOD(CircularArrayTestBasic)
        {
            const size_t N = 512;
            NS_FW_UTIL::CircularArray<int> circularArray(N);

            TestCircularArray<int>(&circularArray, N);
        }

        template <typename T>
        void TestCircularArray(NS_FW_UTIL::CircularArrayInterface<T>* circularArray, const size_t N)
        {
            //circularArray.Fill(-1);

            Assert::IsTrue(circularArray->IsEmpty());
            for (int i = 0; i < N; ++i)
            {
                Assert::IsTrue(circularArray->Enqueue(i));
            }
            Assert::IsFalse(circularArray->IsEmpty());
            Assert::IsFalse(circularArray->Enqueue(static_cast<int>(N) + 1));

            for (int i = 0; i < N; ++i)
            {
                int value = (*circularArray)[i];
                Assert::AreEqual(i, value);
            }

            for (int i = 0; i < N; ++i)
            {
                int value = 0;
                circularArray->Dequeue(value);
                Assert::AreEqual(i, value);
            }
            int value = 0;
            Assert::IsFalse(circularArray->Dequeue(value));
            Assert::IsTrue(circularArray->IsEmpty());
        }
    };
}
