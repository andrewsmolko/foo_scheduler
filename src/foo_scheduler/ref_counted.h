#pragma once

class IRefCounted
{
public:
	virtual void AddRef() = 0;
	virtual bool Release() = 0;

protected:
	~IRefCounted() {}
};

namespace Detail
{
	template<bool NeedsLocking>
	struct RefCountedBase
	{
		RefCountedBase() : m_count(0) {}
		boost::detail::atomic_count m_count;
	};

	template<>
	struct RefCountedBase<false>
	{
		RefCountedBase() : m_count(0) {}
		long m_count;
	};

} // namespace Detail

template<class TBase = IRefCounted, bool NeedsLocking = false>
class RefCountedImpl : public TBase, private Detail::RefCountedBase<NeedsLocking>
{
	typedef Detail::RefCountedBase<NeedsLocking> BaseType;

public: // IRefCounted
	virtual void AddRef()
	{
		++BaseType::m_count;
	}

	virtual bool Release()
	{
		_ASSERTE(BaseType::m_count > 0);

		bool objectReleased = --BaseType::m_count == 0;

		if (objectReleased)
			delete this;

		return objectReleased;
	}

protected:
	RefCountedImpl() {}
	virtual ~RefCountedImpl() {}

	RefCountedImpl(const RefCountedImpl&) : BaseType() {}
	RefCountedImpl & operator = (const RefCountedImpl&) { return *this; }
};

inline void intrusive_ptr_add_ref(IRefCounted* p)
{
	p->AddRef();
}

inline void intrusive_ptr_release(IRefCounted* p)
{
	p->Release();
}