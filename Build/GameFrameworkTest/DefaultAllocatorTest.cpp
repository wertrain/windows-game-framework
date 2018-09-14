#include "stdafx.h"
#include "CppUnitTest.h"

#include <nnfw/Includes.h>
#include <nnfw/fnd/DefaultAllocator.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GameFrameworkTest
{
    ///
    /// アロケータのテスト
    ///
    TEST_CLASS(DefaultAllocatorTest)
    {
    public:
        TEST_METHOD(DefaultAllocatorTestBasic)
        {
            {
                NS_FW_FND::DefaultAllocator defaultAllocator;

                Assert::IsTrue(defaultAllocator.Initialize(0));
                void* memory = defaultAllocator.Alloc(64);
                Assert::IsNotNull(memory);
                void* newMemory = defaultAllocator.ReAlloc(memory, 1028);
                Assert::IsNotNull(newMemory);

                // 以下は常に成り立つ？
                Assert::IsTrue(memory == newMemory);
                defaultAllocator.Free(newMemory);

                defaultAllocator.Finalize();
            }


            NS_FW_FND::DefaultAllocatorManager manager;
            Assert::IsTrue(manager.Initialize());

            for (int i = 0; i < NS_FW_FND::DefaultAllocatorManager::AREA_NUM; ++i)
            {
                auto allocator = manager.GetAllocator(static_cast<NS_FW_FND::DefaultAllocatorManager::MemoryArea>(i));
                Assert::IsNotNull(allocator);
                void* memory = allocator->Alloc(64);
                Assert::IsNotNull(memory);

                // メモリ破壊チェック
                Assert::IsFalse(manager.CheckMemoryCorruption());
                // メモリ破壊
                static_cast<char*>(memory)[64] = 0x00;
                // メモリ破壊再チェック
                Assert::IsTrue(manager.CheckMemoryCorruption());
                // メモリ修復
                Assert::IsTrue(1 == manager.RestoreMemoryCorruption());

                allocator->Free(memory);
            }

            manager.Finalize();
        }
    };
}
