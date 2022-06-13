//
// Created by Monika on 18.03.2022.
//

#include <Types/Thread.h>
#include <Platform/Platform.h>
#include <Types/DataStorage.h>

namespace SR_HTYPES_NS {
    Thread::Thread(std::thread &&thread)
        : m_thread(std::exchange(thread, {}))
    {
        m_id = SR_UTILS_NS::GetThreadId(m_thread);
        m_context = new DataStorage();
    }

    void Thread::Sleep(uint64_t milliseconds) {
        Platform::Sleep(milliseconds);
    }

    void Thread::SetPriority(ThreadPriority priority) {
        Platform::SetThreadPriority(reinterpret_cast<void*>(m_thread.native_handle()), priority);
    }

    Thread::Ptr Thread::Factory::Create(std::thread thread) {
        SR_SCOPED_LOCK

        auto&& pThread = new Thread(std::move(thread));

        SR_LOG("Thread::Factory::Create() : create new \"" + ToString(pThread->m_id) + "\" thread...");

        m_threads.insert(std::make_pair(pThread->GetId(), pThread));

        return pThread;
    }

    Thread::Ptr Thread::Factory::Create(const std::function<void()> &fn) {
        return Create(std::thread(fn));
    }

    bool Thread::TryJoin() {
        if (Joinable()) {
            Join();
            return true;
        }

        return false;
    }

    Thread::Ptr Thread::Factory::GetMainThread() {
        return nullptr;
    }

    Thread::Ptr Thread::Factory::GetThisThread() {
        SR_SCOPED_LOCK

        if (auto&& pIt = m_threads.find(SR_UTILS_NS::GetThisThreadId()); pIt != m_threads.end()) {
            return pIt->second;
        }

        return nullptr;
    }

    void Thread::Factory::Remove(Thread* pThread) {
        SR_LOG("Thread::Free() : free \"" + ToString(pThread->GetId()) + "\" thread...");
        m_threads.erase(pThread->GetId());
    }

    Thread::ThreadId Thread::GetId() {
        return m_id;
    }

    void Thread::Free() {
        Factory::Instance().Remove(this);
        delete this;
    }

    Thread::~Thread() {
        SRAssert(!Joinable());
        if (m_context) {
            delete m_context;
            m_context = nullptr;
        }
    }

    uint32_t Thread::Factory::GetThreadsCount() {
        SR_SCOPED_LOCK

        return m_threads.size();
    }
}

