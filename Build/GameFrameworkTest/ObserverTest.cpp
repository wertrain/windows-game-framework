#include "stdafx.h"
#include "CppUnitTest.h"

#include <nnfw/Includes.h>
#include <nnfw/fnd/GenericPool.h>
#include <nnfw/sys/Observer.h>
#include <nnfw/sys/Subject.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GameFrameworkTest
{
    ///
    /// �I�u�U�[�o�N���X�̃e�X�g
    ///
    TEST_CLASS(ObserverTest)
    {
    private:
        // ���̃C�x���gID
        enum EventId : uint32_t
        {
            DUMMY_EVENT = 0,
            SOME_WONDERFUL_EVENT,
            SOME_SPECIAL_EVENT
        };

        // �T�u�W�F�N�g�̎���
        // �f���炵���C�x���g���N�������ƒʒm����
        class SomeSubject : public NS_FW_SYS::Subject
        {
        public:
            uint32_t GetEventId() { return EventId::SOME_WONDERFUL_EVENT; }

            void CallNotification()
            {
                Notify(GetEventId());
            }
        };
        
        // �T�u�W�F�N�g�̎���
        // ���ʂȃC�x���g���N�������ƒʒm����
        class OtherSubject : public NS_FW_SYS::Subject
        {
        public:
            uint32_t GetEventId() { return EventId::SOME_SPECIAL_EVENT; }

            void CallNotification()
            {
                Notify(GetEventId());
            }
        };

        // �K���ȃI�u�U�[�o
        class PiyoObserver : public NS_FW_SYS::Observer
        {
        public:
            PiyoObserver()
                : mEventId(DUMMY_EVENT)
                , mCalledNotify(false)
            {

            }
            ~PiyoObserver() {}

            void OnNotify(const uint32_t eventId)
            {
                Assert::AreNotSame<uint32_t>(eventId, 0);
                mCalledNotify = true;
            }

            void SetEventId(const uint32_t id)
            {
                mEventId = id;
                mCalledNotify = false;
            }

            bool IsCalledNotification()
            {
                return mCalledNotify;
            }

        private:
            uint32_t mEventId;
            bool mCalledNotify;
        };

    public:
        TEST_METHOD(ObserverTestBasic)
        {
            // �����̃T�u�W�F�N�g�ɓ���̃I�u�U�[�o��o�^����
            SomeSubject some;
            OtherSubject other;
            PiyoObserver observer;

            some.AddObserver(&observer);
            other.AddObserver(&observer);

            // �ʒm�e�X�g
            observer.SetEventId(some.GetEventId());
            some.CallNotification();
            Assert::IsTrue(observer.IsCalledNotification());

            observer.SetEventId(other.GetEventId());
            other.CallNotification();
            Assert::IsTrue(observer.IsCalledNotification());

            some.RemoveObserver(&observer);
            other.RemoveObserver(&observer);

            // �ʒm����Ȃ��e�X�g
            observer.SetEventId(EventId::DUMMY_EVENT);
            some.CallNotification();
            Assert::IsFalse(observer.IsCalledNotification());

            observer.SetEventId(EventId::DUMMY_EVENT);
            other.CallNotification();
            Assert::IsFalse(observer.IsCalledNotification());
        }
    };
}
