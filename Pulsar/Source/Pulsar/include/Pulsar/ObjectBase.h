#pragma once

#include <CoreLib/List.h>
#include <cstdint>
#include <string>
#include <memory>
#include <cassert>
#include <CoreLib/CommonException.h>
#include <CoreLib/Events.hpp>
#include <CoreLib/Guid.h>
#include <Pulsar/EngineMath.h>
#include <unordered_map>
#include <unordered_set>
#include <CoreLib/IndexString.h>
#include <CoreLib/sser.hpp>
#include "Assembly.h"

#define WITH_EDITOR

#define DECL_PTR(Class) \
CORELIB_DECL_SHORTSPTR(Class); \
using Class##_ref = ::pulsar::ObjectPtr<Class>;

#define THIS_REF ObjectPtr<ThisClass>(GetObjectHandle())

#define assert_err(expr, msg)
#define assert_warn(expr, msg)

namespace pulsar
{
    class EngineException : public ExceptionBase
    {
    public:
        virtual const char* name() const override { return "EngineException"; }
        using ExceptionBase::ExceptionBase;
    };

    using ObjectHandle = guid_t;

    template<typename K, typename V>
    using hash_map = std::unordered_map<K, V>;

    template<typename T>
    using hash_set = std::unordered_set<T>;

    using ObjectFlags = uint64_t;
    enum ObjectFlags_ : uint64_t
    {
        OF_NoFlag           = 0,
        OF_Persistent       = 1 << 0,
        OF_Instantiable     = 1 << 1,
        OF_Instance         = 1 << 2,
        OF_NoPack           = 1 << 3,
        OF_LifecycleManaged = 1 << 4,
    };


    class ObjectBase : public Object
    {
        friend class RuntimeObjectWrapper;
        CORELIB_DEF_TYPE(AssemblyObject_pulsar, pulsar::ObjectBase, Object);
    public:
        ObjectBase();
        ~ObjectBase() noexcept override;
    public:
        [[always_inline]] ObjectHandle GetObjectHandle() const noexcept { return this->m_objectHandle; }
    public:
        void Construct(ObjectHandle handle = {});
        virtual void PostEditChange(FieldInfo* info);
    public:
        index_string GetIndexName() const noexcept { return m_name; }
        void         SetIndexName(index_string name) noexcept;
        string       GetName() const noexcept { return m_name.to_string(); }
        void         SetName(string_view name) noexcept;
        ObjectFlags  GetObjectFlags() const noexcept { return m_flags; }
        bool         HasObjectFlags(ObjectFlags flags) const noexcept { return m_flags & flags;}
        void         SetObjectFlags(uint16_t flags) noexcept { m_flags = flags; }
        bool         IsPersistentObject() const noexcept { return m_flags & OF_Persistent; }


        constexpr static int DependMsg_OnDestroy = 1;
        constexpr static int DependMsg_OnAvailable = 2;
        constexpr static int DependMsg_OnUnavailable = 3;

        virtual void OnDependencyMessage(ObjectHandle inDependency, int msg);

        void AddOutDependency(ObjectHandle obj);
        bool HasOutDependency(ObjectHandle obj) const;
        void RemoveOutDependency(ObjectHandle obj);
    protected:
        void SendMsgToOutDependency(int msg) const;
        virtual void OnConstruct();
        virtual void OnDestroy();
    private:
        void Destroy();
        // base class 24
        index_string m_name;         // 8
        ObjectHandle m_objectHandle; // 16
    protected:
        ObjectFlags  m_flags{};      // 8
    public:
        std::unique_ptr<array_list<ObjectHandle>> m_outDependency; // 8
    public:
    };

    inline constexpr int kSizeObjectBase = sizeof(ObjectBase);

    struct ManagedPointer
    {
        friend class RuntimeObjectWrapper;
    private:
        ObjectBase* Pointer{};
        int Counter{};
    public:
        int RefCount() const noexcept { return Counter; }
        void Incref() { ++Counter; }
        int Decref() { --Counter; return Counter; }
        [[always_inline]] ObjectBase* Get() const
        {
            return Pointer;
        }
        void SetEmptyPtr()
        {
            Pointer = nullptr;
        }
    };

    class RuntimeObjectWrapper final
    {
    public:
        static ObjectBase* GetObject(const ObjectHandle& id) noexcept;
        static SPtr<ObjectBase> GetSharedObject(const ObjectHandle& id) noexcept;
        static SPtr<ManagedPointer> GetPointer(const ObjectHandle& id) noexcept;
        static SPtr<ManagedPointer> AddWaitPointer(const ObjectHandle& id);
        static bool IsValid(const ObjectHandle& id) noexcept;
        static void NewInstance(SPtr<ObjectBase>&& managedObj, const ObjectHandle& handle) noexcept;
        static bool DestroyObject(const ObjectHandle& id, bool isForce = false) noexcept;
        static void Terminate();

        static void ForEachObject(const std::function<void(ObjectHandle, ObjectBase*)>& func);

        //<id, type, is_create>
        static Action<ObjectHandle, Type*, bool> ObjectHook;
        static Action<ObjectBase*> OnPostEditChanged;

        static void AddDependList(ObjectHandle src, ObjectHandle dest);
        static void NotifyDependObjects(ObjectHandle dest, int id);
    };



    struct ObjectPtrBase
    {
        ObjectHandle Handle;
        SPtr<ManagedPointer> Pointer;

        ObjectPtrBase() = default;
        ObjectPtrBase(const ObjectPtrBase&) = default;
        ObjectPtrBase(ObjectPtrBase&&) = default;
        ObjectPtrBase(const ObjectHandle& handle) : Handle(handle)
        {
            if (!handle.is_empty())
            {
                Pointer = RuntimeObjectWrapper::GetPointer(handle);
                if (Pointer == nullptr)
                {
                    Pointer = RuntimeObjectWrapper::AddWaitPointer(handle);
                }
            }
        }
        ObjectPtrBase& operator=(const ObjectPtrBase&) = default;
        ObjectPtrBase& operator=(ObjectPtrBase&&) = default;

        [[always_inline]] const ObjectHandle& GetHandle() const
        {
            return Handle;
        }
        [[always_inline]] ObjectHandle& GetHandle()
        {
            return Handle;
        }
        [[always_inline]] ObjectBase* GetObjectPointer() const
        {
            if (Pointer)
            {
                return Pointer->Get();
            }
            return nullptr;
        }
    };

    class BoxingObjectPtrBase : public BoxingObject, public IStringify
    {
        CORELIB_DEF_TYPE(AssemblyObject_pulsar, pulsar::BoxingObjectPtrBase, BoxingObject);
        CORELIB_IMPL_INTERFACES(IStringify)
    public:
        using unboxing_type = ObjectPtrBase;
        ObjectPtrBase get_unboxing_value() { return ptr; }
        BoxingObjectPtrBase() : CORELIB_INIT_INTERFACE(IStringify) {}
        explicit BoxingObjectPtrBase(const ObjectPtrBase& invalue) : ptr(invalue),
            CORELIB_INIT_INTERFACE(IStringify) {}

        void IStringify_Parse(const string& value) override
        {
            ptr = ObjectHandle::parse(value);
        }

        string IStringify_Stringify() override
        {
            return ptr.GetHandle().to_string();
        }

        [[always_inline]] ObjectHandle GetHandle() const noexcept
        {
            return ptr.Handle;
        }
        [[always_inline]] void SetHandle(const ObjectHandle& handle) noexcept
        {
            ptr.Handle = handle;
        }

        ObjectPtrBase ptr;
    };
}
CORELIB_DECL_BOXING(pulsar::ObjectPtrBase, pulsar::BoxingObjectPtrBase);

namespace pulsar
{

    template<typename T>
    concept baseof_objectbase = std::is_base_of<ObjectBase, T>::value;


    inline bool IsValid(const ObjectPtrBase& object) noexcept
    {
        return RuntimeObjectWrapper::IsValid(object.GetHandle());
    }


    template<typename T>
    struct ObjectPtr : public ObjectPtrBase
    {
        using base = ObjectPtrBase;
        using element_type = T;

        using base::base;
        ObjectPtr(T* ptr) noexcept : base(ptr ? ptr->GetObjectHandle() : ObjectHandle{})
        {
        }

        ObjectPtr(const ObjectPtrBase& ptr)
        {
            Handle = ptr.Handle;
            Pointer = ptr.Pointer;
        }

        ObjectPtr(const SPtr<T>& ptr) : ObjectPtr(ptr.get())
        {
        }

        template<typename U> requires std::is_base_of_v<T, U>
        ObjectPtr(const ObjectPtr<U>& derived) noexcept
        {
            Handle = derived.Handle;
            Pointer = derived.Pointer;
        }

        ObjectPtr() = default;

        [[always_inline]] SPtr<T> GetShared() const noexcept
        {
            return sptr_cast<T>(RuntimeObjectWrapper::GetSharedObject(Handle));
        }
        [[always_inline]] T* GetPtr() const noexcept
        {
            return static_cast<T*>(GetObjectPointer());
        }
        T* operator->() const noexcept(false)
        {
            auto ptr = GetPtr();
            if(ptr == nullptr)
            {
                throw NullPointerException();
            }
            return ptr;
        }
        [[always_inline]] bool operator==(const ObjectPtrBase& r) const noexcept { return Handle == r.Handle; }
        [[always_inline]] bool operator==(std::nullptr_t) const noexcept { return !IsValid(); }
        template<typename U>
        bool operator==(const ObjectPtr<U>& r) const noexcept { return Handle == r.Handle; }

        [[always_inline]] bool IsValid() const noexcept
        {
            return RuntimeObjectWrapper::IsValid(Handle);
        }
        [[always_inline]] operator bool() const noexcept
        {
            return IsValid();
        }

        void Reset() noexcept
        {
            Handle = {};
            Pointer = nullptr;
        }
    };

    template<typename T, typename U>
    ObjectPtr<T> ref_cast(ObjectPtr<U> o)
    {
        if (!o) return {};
        return ptr_cast<T>(o.GetPtr());
    }


    class RCPtrBase
    {
    public:
        ObjectHandle Handle;
        SPtr<ManagedPointer> Pointer;
    private:
        [[always_inline]] void Incref()
        {
            Pointer->Incref();
        }
        [[always_inline]] void Decref()
        {
            if (Pointer->Decref() == 0)
            {
                RuntimeObjectWrapper::DestroyObject(Handle);
            }
        }
    public:
        [[always_inline]] ObjectHandle& GetHandle() noexcept
        {
            return Handle;
        }
        [[always_inline]] const ObjectHandle& GetHandle() const noexcept
        {
            return Handle;
        }
        [[always_inline]] ObjectBase* GetPointer() const noexcept
        {
            if (Pointer)
            {
                return Pointer->Get();
            }
            return nullptr;
        }
        RCPtrBase(const ObjectHandle& handle)
        {
            if (!handle.is_empty())
            {
                Handle = handle;
                Pointer = RuntimeObjectWrapper::GetPointer(handle);
                if (!Pointer)
                {
                    Pointer = RuntimeObjectWrapper::AddWaitPointer(handle);
                }
                Pointer->Incref();
            }
        }
        RCPtrBase() : Handle({}) {}
        RCPtrBase(const ObjectBase* ptr) : RCPtrBase(ptr ? ptr->GetObjectHandle() : ObjectHandle{})
        {
        }
        RCPtrBase(const RCPtrBase& ptr) noexcept : Handle(ptr.Handle), Pointer(ptr.Pointer)
        {
            if (Pointer) Pointer->Incref();
        }
        RCPtrBase(RCPtrBase&& ptr) noexcept : Handle(ptr.Handle), Pointer(std::move(ptr.Pointer))
        {
            ptr.Pointer = nullptr;
        }
        RCPtrBase& operator=(const RCPtrBase& ptr) noexcept
        {
            if (Pointer) Decref();
            Handle = ptr.Handle;
            Pointer = ptr.Pointer;
            if (Pointer) Incref();
            return *this;
        }
        RCPtrBase& operator=(RCPtrBase&& ptr) noexcept
        {
            if (Pointer) Decref();
            Handle = ptr.Handle;
            Pointer = std::move(ptr.Pointer);
            ptr.Pointer = nullptr;
            if (Pointer) Incref();
            return *this;
        }
        ~RCPtrBase() noexcept
        {
            if (Pointer)
            {
                Decref();
            }
        }

        bool operator==(const RCPtrBase& ptr) const noexcept
        {
            return Handle == ptr.Handle;
        }

        void Reset()
        {
            Handle = {};
            Pointer.reset();
        }
    };

    template <typename T>
    struct RCPtr : public RCPtrBase
    {
        using base = RCPtrBase;

        using base::base;

        T* operator->() const
        {
            auto ptr = GetPtr();
            if (!ptr)
            {
                throw NullPointerException();
            }
            return ptr;
        }

        RCPtr(const RCPtrBase& ptr) : base(ptr) {}
        RCPtr(const SPtr<T>& t) : base(t.get()) {}

        [[always_inline]] T* GetPtr() const noexcept
        {
            return static_cast<T*>(GetPointer());
        }

        [[always_inline]] operator bool() const noexcept
        {
            if (Pointer == nullptr)
            {
                return false;
            }
            return Pointer->Get() != nullptr;
        }
    };

    template<typename T, typename U>
    RCPtr<T> cref_cast(RCPtr<U> o)
    {
        if (!o) return {};
        return ptr_cast<T>(o.GetPtr());
    }

    class BoxingRCPtrBase : public BoxingObject, public IStringify
    {
        CORELIB_DEF_TYPE(AssemblyObject_pulsar, pulsar::BoxingRCPtrBase, BoxingObject);
        CORELIB_IMPL_INTERFACES(IStringify)
    public:
        using unboxing_type = RCPtrBase;
        RCPtrBase get_unboxing_value() { return ptr; }
        BoxingRCPtrBase() : CORELIB_INIT_INTERFACE(IStringify), ptr({}) {}
        explicit BoxingRCPtrBase(const RCPtrBase& invalue) :
            CORELIB_INIT_INTERFACE(IStringify), ptr(invalue) {}

        void IStringify_Parse(const string& value) override
        {
            ptr = ObjectHandle::parse(value);
        }

        string IStringify_Stringify() override
        {
            return ptr.GetHandle().to_string();
        }

        [[always_inline]] ObjectHandle GetHandle() const noexcept
        {
            return ptr.GetHandle();
        }
        [[always_inline]] void SetHandle(const ObjectHandle& handle) noexcept
        {
            ptr.Handle = handle;
        }

        RCPtrBase ptr;
    };

    inline void DestroyObject(const ObjectPtrBase& object, bool isForce = false) noexcept
    {
        RuntimeObjectWrapper::DestroyObject(object.GetHandle(), isForce);
    }
    inline void DestroyObject(const RCPtrBase& object, bool isForce = false) noexcept
    {
        RuntimeObjectWrapper::DestroyObject(object.GetHandle(), isForce);
    }

    std::iostream& ReadWriteStream(std::iostream& stream, bool isWrite, ObjectPtrBase& obj);
    std::iostream& ReadWriteStream(std::iostream& stream, bool isWrite, RCPtrBase& obj);

}
CORELIB_DECL_BOXING(pulsar::RCPtrBase, pulsar::BoxingRCPtrBase);

namespace jxcorlib
{
    template<typename T>
    struct type_redirect<pulsar::ObjectPtr<T>>
    {
        using type = pulsar::ObjectPtrBase;
    };

    template<typename T>
    struct type_wrapper<pulsar::ObjectPtr<T>>
    {
        using type = T;
    };

    template<typename T>
    struct type_redirect<pulsar::RCPtr<T>>
    {
        using type = pulsar::RCPtrBase;
    };

    template<typename T>
    struct type_wrapper<pulsar::RCPtr<T>>
    {
        using type = T;
    };;
}

namespace pulsar
{
    DECL_PTR(ObjectBase);
}
namespace std
{

    template<>
    struct hash<pulsar::ObjectPtrBase>
    {
        size_t operator()(const pulsar::ObjectPtrBase& ptr) const noexcept
        {
            return std::hash<pulsar::ObjectHandle>()(ptr.GetHandle());
        }
    };

    template<typename T>
    struct hash<pulsar::ObjectPtr<T>>
    {
        size_t operator()(const pulsar::ObjectPtr<T>& ptr) const noexcept
        {
            return std::hash<pulsar::ObjectHandle>()(ptr.GetHandle());
        }
    };

    template<>
    struct hash<pulsar::RCPtrBase>
    {
        size_t operator()(const pulsar::RCPtrBase& ptr) const noexcept
        {
            return std::hash<pulsar::ObjectHandle>()(ptr.GetHandle());
        }
    };

    template<typename T>
    struct hash<pulsar::RCPtr<T>>
    {
        size_t operator()(const pulsar::RCPtr<T>& ptr) const noexcept
        {
            return std::hash<pulsar::ObjectHandle>()(ptr.GetHandle());
        }
    };
}