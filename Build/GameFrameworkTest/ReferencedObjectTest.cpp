#include "stdafx.h"
#include "CppUnitTest.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>  

#include <nnfw/core/ReferencedObject.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GameFrameworkTest
{
    ///
    /// 参照オブジェクトのテスト
    ///
    class TestReferencedObject : public fw::sys::ReferencedObject
    {
    public:
        int DoSomething()
        {
            return 0;
        }
    };

    TEST_CLASS(ReferencedObjectTest)
    {
    public:
        TEST_METHOD(ReferencedObjectTestBasic)
        {
            // メモリリーク検出テスト
            // @see http://puarts.com/?pid=1156
            _CrtMemState mem_state_before, mem_state_after, mem_state_diff;
            _CrtMemCheckpoint(&mem_state_before);

            TestReferencedObject* obj = new TestReferencedObject();

            Assert::IsTrue(obj->GetRef() == 1);

            {
                TestReferencedObject obj2 = *obj;
                obj2.DoSomething();

                Assert::IsTrue(obj->GetRef() == 2);

                TestReferencedObject obj3 = *obj;
                obj3.DoSomething();

                Assert::IsTrue(obj->GetRef() == 3);
            }

            Assert::IsTrue(obj->GetRef() == 1);

            {
                TestReferencedObject obj3 = *obj;
                obj3.DoSomething();
            }

            if (obj->Release() == 0)
            {
                delete obj;
            }

            _CrtMemCheckpoint(&mem_state_after);
            if (_CrtMemDifference(&mem_state_diff, &mem_state_before, &mem_state_after))
            {
                _CrtMemDumpStatistics(&mem_state_diff);
                Assert::Fail(L"メモリリークが検出されました");
            }
        }
    };
}
