#include "stdafx.h"
#include "JSmartObject.h"
#include <WinBase.h>
#include <crtdbg.h>
#include <exception>





CJSmartObject::CJSmartObject()
{
	m_nRefs = 0;
}

CJSmartObject::~CJSmartObject()
{
}

void CJSmartObject::AddRef() throw()
{
	_ASSERTE(m_nRefs >= 0);
	InterlockedIncrement(&m_nRefs);
}

void CJSmartObject::Release() throw()
{
	_ASSERTE(m_nRefs != 0);
	if (InterlockedDecrement(&m_nRefs) <= 0){
		delete this;
	}
}

BOOL CJSmartObject::IsShared()const throw()
{
	return(m_nRefs > 1);
}

void* CJSmartObject::operator new(_In_ size_t size)
{
	void* p = malloc(size);
	if (p == NULL){
		throw std::bad_alloc();
	}
	return p;
}

void CJSmartObject::operator delete(_In_ void* ptr)
{
	free(ptr);
}

void* CJSmartObject::operator new[](_In_ size_t size)
{
	void* p = malloc(size);
	if (p == NULL){
		throw std::bad_alloc();
	}
	return p;
}

void CJSmartObject::operator delete[](_In_ void* ptr)
{
	free(ptr);
}

LPCTSTR CJSmartObject::GetName()const
{
	return _T("CJSmartObject");
}

BOOL CJSmartObject::Equals(const CJSmartObject&object)const
{
	return TRUE;
}


//CJSmartBasePtr
CJSmartBasePtr::CJSmartBasePtr() :
m_pObject(NULL){}

BOOL CJSmartBasePtr::Equals(const CJSmartObject*pObject)const{
	if (m_pObject == pObject){
		return TRUE;
	}
	if (m_pObject == NULL){
		return FALSE;
	}
	return m_pObject->Equals(*pObject);
}

void CJSmartBasePtr::Attach(CJSmartObject* pObject)
{
	_ASSERTE(pObject != NULL);
	_ASSERTE(m_pObject == NULL);
	if (pObject == NULL){
		throw std::exception("²ÎÊý´íÎó");
	}
	m_pObject = pObject;
	m_pObject->AddRef();
}

void CJSmartBasePtr::Release()
{
	_ASSERTE(m_pObject != NULL);
	m_pObject->Release();
	m_pObject = NULL;
}