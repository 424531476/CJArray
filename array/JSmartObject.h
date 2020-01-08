#pragma once
#include <windows.h>

class  CJSmartObject
{
public:
	CJSmartObject();
	virtual ~CJSmartObject();
	
	static void* operator new(_In_ size_t size);
	static void operator delete(_In_ void* ptr);
	static void* operator new[](_In_ size_t size);
	static void operator delete[](_In_ void* ptr);
	virtual LPCTSTR GetName()const;
	friend class CJSmartBasePtr;
protected:
	void AddRef() throw();
	void Release() throw();
	virtual BOOL Equals(const CJSmartObject&object) const;
	BOOL IsShared()const throw();
private:
	long m_nRefs;
};


class CJSmartBasePtr{
public:
	BOOL Equals(const CJSmartObject*pObject)const;
protected:
	CJSmartBasePtr();
	void Attach(CJSmartObject* pObject);
	void Release();
	
	CJSmartObject* m_pObject;
};

template<class OBJ>
class CJSmartPtr:
	protected CJSmartBasePtr
{
public:
	CJSmartPtr(){}

	CJSmartPtr(OBJ* pObject)
	{
		(*this) = pObject;
	}

	CJSmartPtr(const CJSmartPtr<OBJ>& pObject)
	{
		(*this) = pObject.GetPtr();
	}

	~CJSmartPtr()
	{
		if (m_pObject){
			Release();
		}
	}

	operator const OBJ*()const
	{
		return (OBJ*)m_pObject;
	}

	OBJ*GetPtr()const
	{
		return (OBJ*)m_pObject;
	}

	OBJ* operator->() const
	{
		_ASSERTE(m_pObject != NULL);
		if (m_pObject == NULL){
			ThrowNullPointerException();
		}
		return (OBJ*)m_pObject;
	}

	CJSmartPtr&operator= (OBJ* pObject)
	{
		if (m_pObject){
			Release();
		}
		if (pObject){
			Attach(pObject);
		}
		return (*this);
	}

	CJSmartPtr&operator=(const CJSmartPtr<OBJ>& pObject)
	{
		return operator= (pObject.GetPtr());
	}

	BOOL operator == (const CJSmartPtr<OBJ>& pObject)
	{
		return Equals(pObject);
	}
	BOOL operator != (const CJSmartPtr<OBJ>& pObject)
	{
		return !Equals(pObject);
	}

	/*friend BOOL operator ==(const CJSmartPtr<OBJ>&sp, const OBJ* lp)
	{
		return sp.Equals(lp);
	}

	friend BOOL operator !=(const CJSmartPtr<OBJ>&sp, const  OBJ* lp)
	{
		return !sp.Equals(lp);
	}

	friend BOOL operator ==(const OBJ* lp, const CJSmartPtr<OBJ>&sp)
	{
		return sp.Equals(lp);
	}

	friend BOOL operator !=(const OBJ* lp, const CJSmartPtr<OBJ>&sp)
	{
		return !sp.Equals(lp);
	}*/

	BOOL IsNull()const
	{
		return m_pObject == NULL;
	}

	BOOL Equals(const CJSmartObject*pObject)const
	{
		return CJSmartBasePtr::Equals(pObject);
	}

	OBJ* CreateObject()
	{
		_ASSERTE(m_pObject == NULL);
		OBJ* pObject = new OBJ();
		Attach(pObject);
		return pObject;
	}

};

typedef CJSmartPtr<CJSmartObject> CJSmartObjectPtr;