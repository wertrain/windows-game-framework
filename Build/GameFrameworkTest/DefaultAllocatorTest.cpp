#include "stdafx.h"
#include "CppUnitTest.h"

#include <nnfw/Includes.h>
#include <nnfw/fnd/DefaultAllocator.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GameFrameworkTest
{
    TEST_CLASS(DefaultAllocatorTest)
    {
    public:
        TEST_METHOD(DefaultAllocatorTestBasic)
        {
            NS_FW_FND::DefaultAllocator defaultAllocator;

            Assert::IsTrue(defaultAllocator.Initialize("DefaultAllocatorTestBasic"));
            void* memory = defaultAllocator.Alloc(64);
            Assert::IsNotNull(memory);
            void* newMemory = defaultAllocator.ReAlloc(memory, 1028);
            Assert::IsNotNull(newMemory);
            // à»â∫ÇÕèÌÇ…ê¨ÇËóßÇ¬ÅH
            Assert::IsTrue(memory == newMemory);
        }
    };
}
