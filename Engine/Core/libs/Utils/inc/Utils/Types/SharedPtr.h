//
// Created by Monika on 23.08.2022.
//

#ifndef SRENGINE_SHAREDPTR_H
#define SRENGINE_SHAREDPTR_H

#include <Utils/Common/StringFormat.h>
#include <Utils/Types/Function.h>

namespace SR_HTYPES_NS {
    struct SharedPtrDynamicData {
        uint32_t m_useCount {};
        bool m_valid {};
    };

    template<class T> class SR_DLL_EXPORT SharedPtr {
    public:
        SharedPtr() = default;
        SharedPtr(const T* constPtr); /** NOLINT */
        SharedPtr(SharedPtr const &ptr);
        SharedPtr(SharedPtr&& ptr) noexcept
            : m_ptr(std::exchange(ptr.m_ptr, { }))
            , m_data(std::exchange(ptr.m_data, { }))
        { }
        ~SharedPtr(); /// не должен быть виртуальным

    public:
        SR_NODISCARD SR_FORCE_INLINE operator bool() const noexcept { return m_data && m_data->m_valid; } /** NOLINT */
        SharedPtr<T>& operator=(const SharedPtr<T> &ptr);
        SharedPtr<T>& operator=(T *ptr);
        SharedPtr<T>& operator=(SharedPtr<T>&& ptr) noexcept {
            if (m_data) {
                SRAssert(m_data->m_useCount > 0);
                --m_data->m_useCount;
            }

            m_data = std::exchange(ptr.m_data, {});

            if (m_data) {
                ++m_data->m_useCount;
            }

            m_ptr = std::exchange(ptr.m_ptr, {});

            return *this;
        }
        SR_NODISCARD SR_FORCE_INLINE T *operator->() const noexcept { return m_ptr; }
        SR_NODISCARD SR_INLINE bool operator==(const SharedPtr<T>& right) const noexcept {
            return m_ptr == right.m_ptr;
        }
        SR_NODISCARD SR_INLINE bool operator!=(const SharedPtr<T>& right) const noexcept {
            return m_ptr != right.m_ptr;
        }
        template<typename U> SharedPtr<U> DynamicCast() const {
            if constexpr (std::is_same_v<T, void>) {
                return SharedPtr<U>();
            }
            return SharedPtr<U>(dynamic_cast<U*>(m_ptr));
        }

        template<typename U> U ReinterpretCast() {
            return reinterpret_cast<U>(m_ptr);
        }

        SR_NODISCARD SR_FORCE_INLINE T* Get() const noexcept { return m_ptr; }
        SR_NODISCARD SR_FORCE_INLINE SharedPtrDynamicData* GetPtrData() const noexcept { return m_data; }
        SR_NODISCARD SR_FORCE_INLINE void* GetRawPtr() const noexcept { return (void*)m_ptr; }
        SR_NODISCARD SharedPtr<T> GetThis() const {
            return *this;
        }
        SR_NODISCARD SR_FORCE_INLINE bool Valid() const noexcept { return m_data && m_data->m_valid; }

        bool AutoFree(const SR_HTYPES_NS::Function<void(T *ptr)> &freeFun);

        /// Оставляем методы для совместимости с SafePtr
        void Replace(const SharedPtr &ptr);
        SR_NODISCARD SR_FORCE_INLINE bool RecursiveLockIfValid() const noexcept;
        SR_NODISCARD SR_FORCE_INLINE bool TryRecursiveLockIfValid() const noexcept;
        SR_FORCE_INLINE void Unlock() const noexcept { /** nothing */  }

    private:
        bool FreeImpl(const SR_HTYPES_NS::Function<void(T *ptr)> &freeFun);

    private:
        SharedPtrDynamicData* m_data = nullptr;
        T* m_ptr = nullptr;

    };

    template<class T> SharedPtr<T>::SharedPtr(const T* constPtr) {
        T* ptr = const_cast<T*>(constPtr);
        bool needAlloc = true;

        if constexpr (IsDerivedFrom<SharedPtr, T>::value) {
            if (ptr && (m_data = ptr->GetPtrData())) {
                ++(m_data->m_useCount);
                needAlloc = false;
                m_ptr = ptr;
            }
        }

        if (needAlloc && ptr) {
            m_data = new SharedPtrDynamicData{
                1,     /// m_useCount
                (bool)(m_ptr = ptr), /// m_valid
            };
        }
    }

    template<class T> SharedPtr<T>::SharedPtr(const SharedPtr &ptr) {
        m_ptr = ptr.m_ptr;
        if ((m_data = ptr.m_data)) {
            ++(m_data->m_useCount);
        }
    }

    template<class T> SharedPtr<T>::~SharedPtr() {
        if (m_data && m_data->m_useCount <= 1) {
            SR_SAFE_PTR_ASSERT(!m_data->m_valid, "Ptr was not freed!");
            delete m_data;
        }
        else if (m_data) {
            --(m_data->m_useCount);
        }
    }

    template<class T> SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr<T> &ptr) {
        if (m_data && m_data->m_useCount <= 1) {
            SR_SAFE_PTR_ASSERT(!m_data->m_valid, "Ptr was not freed!");
            delete m_data;
        }
        else if (m_data) {
            --(m_data->m_useCount);
        }

        m_ptr = ptr.m_ptr;

        if ((m_data = ptr.m_data)) {
            m_data->m_valid = bool(m_ptr);
            ++(m_data->m_useCount);
        }

        return *this;
    }

    template<class T> SharedPtr<T>& SharedPtr<T>::operator=(T *ptr) {
        if (m_ptr != ptr) {
            if (m_data && m_data->m_useCount <= 1) {
                SR_SAFE_PTR_ASSERT(!m_data->m_valid, "Ptr was not freed!");
                delete m_data;
            }
            else if (m_data) {
                --(m_data->m_useCount);
            }

            m_data = nullptr;

            bool needAlloc = true;

            if constexpr (IsDerivedFrom<SharedPtr, T>::value) {
                if (ptr && (m_data = ptr->GetPtrData())) {
                    ++(m_data->m_useCount);
                    needAlloc = false;
                    m_ptr = ptr;
                }
            }

            if (needAlloc && ptr) {
                m_data = new SharedPtrDynamicData{
                        1,     /// m_useCount
                        true, /// m_valid
                };
            }
        }

        m_ptr = ptr;

        if (m_data) {
            m_data->m_valid = bool(m_ptr);
        }

        return *this;
    }

    template<typename T> bool SharedPtr<T>::AutoFree(const SR_HTYPES_NS::Function<void(T *ptr)> &freeFun) {
        SharedPtr<T> ptrCopy = SharedPtr<T>(*this);
        /// после вызова FreeImpl this может потенциально инвалидироваться!

        return ptrCopy.Valid() ? ptrCopy.FreeImpl(freeFun) : false;
    }

    template<typename T> bool SharedPtr<T>::FreeImpl(const SR_HTYPES_NS::Function<void(T *ptr)> &freeFun) {
        if (m_data && m_data->m_valid) {
            freeFun(m_ptr);
            m_data->m_valid = false;
            m_ptr = nullptr;
            return true;
        }
        else {
            return false;
        }
    }

    template<class T> void SharedPtr<T>::Replace(const SharedPtr &ptr) {
        if (ptr.m_ptr == m_ptr && ptr.m_data == m_data) {
            return;
        }

        SharedPtr copy = *this;
        *this = ptr;
    }

    template<class T> bool SharedPtr<T>::RecursiveLockIfValid() const noexcept {
        return m_data && m_data->m_valid;
    }

    template<class T> bool SharedPtr<T>::TryRecursiveLockIfValid() const noexcept {
        return m_data && m_data->m_valid;
    }
}

namespace std {
    template<typename T> struct hash<SR_HTYPES_NS::SharedPtr<T>> {
        size_t operator()(SR_HTYPES_NS::SharedPtr<T> const& ptr) const {
            return std::hash<void*>()(ptr.GetRawPtr());
        }
    };

    template <typename T> struct less<SR_HTYPES_NS::SharedPtr<T>> {
        bool operator()(const SR_HTYPES_NS::SharedPtr<T> &lhs, const SR_HTYPES_NS::SharedPtr<T> &rhs) const {
            void* a = lhs.GetRawPtr();
            void* b = rhs.GetRawPtr();
            return a < b;
        }
    };
}

#endif //SRENGINE_SHAREDPTR_H
