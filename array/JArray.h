#pragma once
#include<crtdbg.h>
#include<stdlib.h>
#include<wchar.h>
#include <new.h>
#include "JSmartObject.h"


template<class TYPE>
inline void JieConstructElements(_Out_ TYPE* pElements, _In_ INT_PTR nCount)
{
	for (INT_PTR i = 0; i < nCount; i++)
	{
#if _MSC_VER > 1200
#pragma push_macro("new")
#undef new
		::new((void*)(pElements + i)) TYPE;
#pragma pop_macro("new")
#else
		::new((void*)(pElements + i)) TYPE;
#endif
	}
}

template<class TYPE>
inline void JieConstructElements(_Out_ TYPE*pElements, _In_ INT_PTR nCount, _In_ const TYPE&element)
{
	for (INT_PTR i = 0; i < nCount; i++)
	{
#if _MSC_VER > 1200
#pragma push_macro("new")
#undef new
		::new((void*)(pElements + i)) TYPE(element);
#pragma pop_macro("new")
#else
		::new((void*)(pElements + i)) TYPE(element);
#endif
	}
}

template<class TYPE>
inline void JieConstructElements(_Out_ TYPE*pElements, _In_ INT_PTR nCount, _In_ const TYPE*src)
{
	for (INT_PTR i = 0; i < nCount; i++)
	{
#if _MSC_VER > 1200
#pragma push_macro("new")
#undef new
		::new((void*)(pElements + i)) TYPE(src[i]);
#pragma pop_macro("new")
#else
		::new((void*)(pElements + i)) TYPE(src[i]);
#endif
	}
}

template<class TYPE>
inline void JieDestructElements(_Out_ TYPE* pElements, _In_ INT_PTR nCount)
{
	for (INT_PTR i = 0; i < nCount; i++)
	{
		(pElements + i)->~TYPE();
	}
}

template<class TYPE>
inline void JieCopyElements(_Out_ TYPE* pDest, _In_ const TYPE* pSrc, _In_ INT_PTR nCount)
{
	while (nCount--) {
		*pDest++ = *pSrc++;
	}
}

template<class TYPE>
inline void JieSetElements(_Out_ TYPE* pDest, _In_ const TYPE&src, _In_ INT_PTR nCount)
{
	while (nCount--) {
		*pDest++ = src;
	}
}

template<class TYPE>
inline BOOL JieEqualsElements(_In_ const TYPE* p1, _In_ const TYPE* p2, _In_ INT_PTR nCount)
{
	while (nCount--) {
		if (*p1 != *p2){
			return FALSE;
		}
		p1++;
		p2++;
	}
	return TRUE;
}

template<class TYPE>
inline INT_PTR Compare(_In_ const TYPE&type1, _In_ const TYPE&type2)
{
	_ASSERTE(FALSE);//不支持的
}

template<> inline INT_PTR Compare<LPCSTR>(_In_ const LPCSTR&lp1, _In_ const LPCSTR&lp2)
{
	size_t i = 0;
	while ((lp1[i] != '\0') && (lp2[i] != '\0'))
	{
		if (lp1[i] != lp2[i]){
			break;
		}
		i++;
	}
	return lp1[i] - lp2[i];
}

template<> inline INT_PTR Compare<LPCWSTR>(_In_ const LPCWSTR&lp1, _In_ const LPCWSTR&lp2)
{
	size_t i = 0;
	while ((lp1[i] != L'\0') && (lp2[i] != L'\0'))
	{
		if (lp1[i] != lp2[i]){
			break;
		}
		i++;
	}
	return lp1[i] - lp2[i];
}


#define CJArray_TRAIT(TYPE) \
	template<> inline void JieConstructElements<TYPE>(_Out_ TYPE*pElements, _In_ INT_PTR nCount)\
{\
	memset(pElements, 0, sizeof(TYPE)*nCount); \
}\
	template<> inline void JieConstructElements<TYPE>(_Out_ TYPE*pElements, _In_ INT_PTR nCount, _In_ const TYPE&element)\
{\
	int nSize = sizeof(TYPE); \
if (nSize == 1)\
{\
	memset(pElements, (int)element, nCount); \
}\
	else if (nSize == 2)\
{\
	wmemset((wchar_t *)pElements, (wchar_t)element, nCount); \
}\
	else\
{\
for (int i = 0; i < nCount; i++)\
{\
	pElements[i] = element; \
}\
}\
}\
	template<> inline void JieConstructElements<TYPE>(_Out_ TYPE*pElements, _In_ INT_PTR nCount, _In_ const TYPE*src)\
{\
	memcpy(pElements, src,sizeof(TYPE)*nCount); \
}\
	template<> inline void JieDestructElements<TYPE>(_Out_ TYPE*pElements, _In_ INT_PTR nCount){}\
	template<> inline void JieCopyElements<TYPE>(_Out_ TYPE* pDest, _In_ const TYPE* pSrc, _In_ INT_PTR nCount)\
{\
	memcpy(pDest, pSrc, sizeof(TYPE)*nCount); \
}\
	template<> inline BOOL JieEqualsElements<TYPE>(_In_ const TYPE* p1, _In_ const TYPE* p2, _In_ INT_PTR nCount)\
{\
	return memcmp(p1, p2, sizeof(TYPE)*nCount) == 0; \
}\
	template<> inline INT_PTR  Compare<TYPE>(_In_ const TYPE&type1, _In_ const TYPE&type2)\
{\
if (type1 < type2)return -1; \
if (type1 > type2)return 1; return 0; \
}

CJArray_TRAIT(char)
CJArray_TRAIT(wchar_t)
CJArray_TRAIT(BYTE)
CJArray_TRAIT(short)
#if _MSC_VER > 1200
CJArray_TRAIT(WORD)
#endif
CJArray_TRAIT(int)
CJArray_TRAIT(DWORD)
CJArray_TRAIT(LONGLONG)
CJArray_TRAIT(ULONGLONG)



template<class TYPE>
class CJArrayData :
	public CJSmartObject
{
public:
	CJArrayData(void)
	{
		nMaxSize = 0;
		pData = NULL;
		nSize = 0;
	}

	~CJArrayData(void)
	{
		if (pData != NULL)
		{
			JieDestructElements<TYPE>(pData, nSize);
			Realloc(0);
		}
	}

	void AllocBuffer(_In_ INT_PTR nLength)
	{
		if (nLength <= nMaxSize){
			return;
		}
		INT_PTR nNewLength = nMaxSize;
#ifdef _WIN64
		if (nNewLength >4LL * 1024LL * 1024LL * 1024LL){
			nNewLength += (100 * 1024 * 1024);
		}
#else
		if (nNewLength > 1024 * 1024 * 1024){
			nNewLength += 1024 * 1024;
		}
#endif	
		else{
			nNewLength = nNewLength + nNewLength / 2;
		}
		if (nNewLength < nLength){
			nNewLength = nLength;
		}
#ifndef _DEBUG
		nNewLength |= 0x10;
#endif
		Realloc(nNewLength);
	}

	BOOL IsShared()const throw()
	{
		return CJSmartObject::IsShared();
	}

	//数组的指针
	TYPE* pData;
	//已包含元素数量
	INT_PTR nSize;
private:
	//已分配最大的元素容量
	INT_PTR nMaxSize;

	void Realloc(_In_ INT_PTR nNewSize)
	{
		if (nNewSize == 0) {
			if (pData != NULL){
				free(pData);
				pData = NULL;
			}
			nMaxSize = nNewSize;
			return;
		}
		if (pData == NULL){
			pData = (TYPE*)::malloc(nNewSize*sizeof(TYPE));
			if (pData == NULL){
				throw std::bad_alloc();
			}
			nMaxSize = nNewSize;
			return;
		}
		pData = (TYPE*)::realloc(pData, nNewSize*sizeof(TYPE));
		if (pData == NULL){
			throw std::bad_alloc();
		}
		nMaxSize = nNewSize;
		return;
	}
};



template<class TYPE>
class CJArray :
	private CJSmartPtr<CJArrayData<TYPE> >
{
public:
	CJArray(void):
	m_pData(NULL),
	m_nSize(0){}

	CJArray(_In_ const TYPE& Element) :
		m_pData(NULL),
		m_nSize(0)
	{
		GetBuffer(1);
		JieConstructElements<TYPE>(m_pData, 1, Element);
		ReleaseBuffer(1);
	}

	CJArray(_In_ const CJArray<TYPE>&NewData) :
		m_pData(NULL),
		m_nSize(0)
	{
		const CJArrayData<TYPE>* pData = NewData.GetPtr();
		if (pData == NULL){
			return;
		}
		Attach(CloneData(pData));
		m_pData = NewData.m_pData;
		m_nSize = NewData.m_nSize;
	}

	CJArray(_In_ const CJArray<TYPE>&NewData, _In_ INT_PTR nFirst, _In_ INT_PTR nCount) :
		m_pData(NULL),
		m_nSize(0)
	{
		const CJArrayData<TYPE>* pData = NewData.GetPtr();
		if (pData == NULL){
			return;
		}
		_ASSERTE(pData->nSize >= nFirst + nCount);
		Attach(CloneData(pData));
		m_pData = NewData.m_pData + nFirst;
		m_nSize = nCount;
	}

	CJArray(_In_ const TYPE* ScrArr, _In_  INT_PTR nSize) :
		m_pData(NULL),
		m_nSize(0)
	{
		GetBuffer(nSize);
		JieConstructElements<TYPE>(m_pData, nSize, ScrArr);
		ReleaseBuffer(nSize);
	}

	~CJArray(void)
	{
		RemoveAll();
	}

	const TYPE& operator[](_In_ INT_PTR nIndex)const
	{
		return GetAt(nIndex);
	}

	TYPE& operator[](_In_ INT_PTR nIndex)
	{
		return SetAt(nIndex);
	}

	CJArray<TYPE>&operator << (_In_ const TYPE&newElement)
	{
		Add(newElement);
		return *this;
	}

	CJArray<TYPE> operator >> (_Out_ TYPE&newElement)const
	{
		INT_PTR nLastIndex= GetUpperBound();
		_ASSERTE(nLastIndex >= 0);
		newElement = m_pData[nLastIndex];
		return CJArray<TYPE>(*this, 0, nLastIndex);
	}

	operator const TYPE*() const
	{
		return m_pData;
	}

	CJArray<TYPE>& operator +=(_In_ const CJArray<TYPE>&NewData)
	{
		Append(NewData);
		return *this;
	}

	CJArray<TYPE>& operator =(_In_ const CJArray<TYPE>&NewData)
	{
		if (GetPtr()){
			RemoveAll();
		}
		const CJArrayData<TYPE>* pData = NewData.GetPtr();
		if (pData == NULL){
			return *this;
		}
		Attach(CloneData(pData));
		m_pData = NewData.m_pData;
		m_nSize = NewData.m_nSize;
		return *this;
	}

	BOOL operator ==(_In_ const CJArray<TYPE>&ary)const
	{
		INT_PTR nSize = ary.GetSize();
		if (m_nSize != nSize){
			return FALSE;
		}
		const TYPE* pData = ary.GetData();
		if (m_pData == pData){
			return TRUE;
		}
		return JieEqualsElements<TYPE>(m_pData,
			pData,
			nSize);
	}

	BOOL operator !=(_In_ const CJArray<TYPE>& Array)const
	{
		return !operator ==(Array);
	}
	//获取元素数
	INT_PTR GetSize() const
	{
		return m_nSize;
	}
	//设置元素数量
	void SetSize(_In_ INT_PTR nNewSize, _In_opt_ INT_PTR nBuffer = 0)
	{
		if (nNewSize < 0){
			nNewSize = 0;
		}
		if (nNewSize == 0 && nBuffer == 0){
			RemoveAll();
			return;
		}
		INT_PTR nOldSize = GetSize();
		if (nNewSize == nOldSize && nBuffer == 0)return;

		if (nNewSize < nOldSize&&GetPtr()->IsShared()){
			m_nSize = nNewSize;
			return;
		}

		GetBuffer(nNewSize + nBuffer);
		if (nNewSize > nOldSize)
		{
			JieConstructElements<TYPE>(m_pData + nOldSize,
				nNewSize - nOldSize);
		}
		else if (nNewSize < nOldSize)
		{
			JieDestructElements<TYPE>(m_pData + nNewSize,
				nOldSize - nNewSize);
		}
		ReleaseBuffer(nNewSize);
	}
	//取最大的有效索引值
	INT_PTR GetUpperBound() const
	{
		return GetSize() - 1;
	}
	//添加一个空元素，返回添加元素的索引
	INT_PTR Add()
	{
		INT_PTR nSize = GetSize();
		SetSize(nSize + 1);
		return nSize;
	}
	
	//在数组的末尾添加元素，返回添加元素的索引
	INT_PTR Add(_In_ const TYPE& newElement)
	{
		return Append(&newElement, 1);
	}

	void PushBack(_In_ const TYPE& newElement)
	{
		Append(&newElement, 1);
	}

	void PopBack()
	{
		SetSize(m_nSize - 1);
	}

	void PushFront(_In_ const TYPE& newElement)
	{
		InsertAt(0, &newElement,1);
	}

	void PopFront()
	{
		RemoveAt(0,1);
	}

	//从此数组移去所有元素元素访问
	void RemoveAll()
	{
		if (GetPtr()){
			Release();
			m_pData = NULL;
			m_nSize = 0;
		}
	}
	//取得元素
	const TYPE& GetAt(_In_ INT_PTR nIndex) const
	{
		_ASSERTE(nIndex >= 0);
		_ASSERTE(nIndex < m_nSize);
		if (nIndex < 0 || nIndex >= m_nSize){
			throw std::out_of_range("下标越界");
		}
		return m_pData[nIndex];
	}
	//设置元素
	void SetAt(_In_ INT_PTR nIndex, _In_  const TYPE&newElement)
	{
		SetAt(nIndex) = newElement;
	}

	TYPE& SetAt(_In_ INT_PTR nIndex)
	{
		_ASSERTE(nIndex >= 0);
		_ASSERTE(nIndex < m_nSize);
		if (nIndex < 0 || nIndex >= m_nSize){
			throw std::out_of_range("下标越界");
		}
		return GetBuffer()[nIndex];
	}
	//设置元素，如必要扩展
	void SetAtGrow(_In_ INT_PTR nIndex, _In_  const TYPE& newElement)
	{
		_ASSERTE(nIndex >= 0);
		if (nIndex < 0){
			throw std::out_of_range("下标越界");
		}
		if (nIndex >= GetSize()) {
			SetSize(nIndex + 1);
		}
		SetAt(nIndex) = newElement;
	}
	//取出数组中间
	CJArray Mid(_In_ const CJArray<TYPE>&begin, _In_  const CJArray<TYPE>&end,
		_In_opt_ INT_PTR nStart = 0) const
	{
		INT_PTR posL = Find(begin, nStart);
		if (posL == -1) {
			return CJArray<TYPE>();
		}
		posL += begin.GetSize();
		INT_PTR posR = Find(end, posL);
		if (posR == -1){
			return CJArray<TYPE>();
		}
		return Mid(posL, posR - posL);
	}
	//取出数组中间,从nFirst（从零开始的索引）指定的位置开始,提取一个长度为nCount个元素的数组 nCount为-1提取后面所有
	CJArray Mid(_In_ INT_PTR nFirst, _In_opt_ INT_PTR nCount = -1) const
	{
		if (nCount == 0){
			return CJArray<TYPE>();
		}
		_ASSERTE(nFirst >= 0);
		_ASSERTE(nFirst < m_nSize);
		if (nFirst < 0 || nFirst >= m_nSize){
			throw std::out_of_range("下标越界");
		}
		if (nCount < 0 || nFirst + nCount > m_nSize){
			nCount = m_nSize - nFirst;
		}
		return CJArray<TYPE>(*this, nFirst, nCount);
	}

	//取出数组左边
	CJArray Left(_In_ const CJArray<TYPE>&ary, _In_opt_ INT_PTR nStart = 0) const
	{
		return Left(Find(ary, nStart));
	}
	//从左边开始提前数组nCount个元素
	CJArray Left(_In_ INT_PTR nCount) const
	{
		if (nCount < 0) {
			return CJArray();
		}
		return Mid(0, nCount);
	}
	
	//取出数组右边
	CJArray Right(_In_ const CJArray<TYPE>&ary, _In_opt_ INT_PTR nStart = 0) const
	{
		INT_PTR pos = Find(ary, nStart);
		if (pos == -1) {
			return NULL;
		}
		pos += ary.GetSize();
		return Right(GetSize() - pos);
	}
	
	//从右边开始提前数组iCount个元素
	CJArray Right(_In_ INT_PTR nCount) const
	{
		if (nCount < 0) {
			return CJArray();
		}
		return Mid(GetSize() - nCount, nCount);
	}
	//数组替换 返回被替换的次数，如果这个数组没有改变则返回零。
	INT_PTR Replace(_In_ const CJArray<TYPE>& OldAry, _In_ const CJArray<TYPE>& NewAry)
	{
		INT_PTR cnt = Count(OldAry);
		const TYPE* pOldSubData = OldAry.GetData();
		INT_PTR nOldSubLen = OldAry.GetSize();
		const TYPE* pNewSubData = NewAry.GetData();
		INT_PTR nNewSubLen = NewAry.GetSize();
		INT_PTR nOldSize = GetSize();
		INT_PTR nNewSize = nOldSize + (nNewSubLen - nOldSubLen)*cnt;
		if (nOldSubLen <= 0){
			return 0;
		}
		if (nNewSize == 0){
			RemoveAll();
			return cnt;
		}
		TYPE*pData = GetBuffer(nNewSize);
		INT_PTR nStart = 0;
		for (INT_PTR i = 0; i < cnt; i++)
		{
			INT_PTR pos = Find(pData, nOldSize, pOldSubData, nOldSubLen, nStart);
			if (nNewSubLen == nOldSubLen){
				JieCopyElements<TYPE>(pData + pos, pNewSubData, nNewSubLen);
			}
			else{
				JieDestructElements<TYPE>(pData + pos, nOldSubLen);
				INT_PTR nBalance = nOldSize - pos - nOldSubLen;
				memmove_s(pData + pos + nNewSubLen, nBalance*sizeof(TYPE),
					pData + pos + nOldSubLen, nBalance*sizeof(TYPE));
				JieConstructElements<TYPE>(pData + pos, nNewSubLen, pNewSubData);
			}
			nOldSize += (nNewSubLen - nOldSubLen);
			nStart = pos + nNewSubLen;
		}
		_ASSERTE(nOldSize == nNewSize);
		ReleaseBuffer(nNewSize);
		return cnt;
	}
	//允许对数组中的元素访问。
	const TYPE* GetData() const
	{
		return m_pData;
	}

	TYPE* GetBuffer()
	{
		return GetBuffer(0);
	}
	
	//设置数据
	void SetData(_In_ const TYPE* pData, _In_ INT_PTR nSize)
	{
		if (nSize == 0){
			RemoveAll();
			return;
		}
		_ASSERTE(pData != NULL);
		SetSize(nSize);
		JieCopyElements<TYPE>(GetBuffer(), pData, nSize);
	}
	//是否相等
	BOOL equal(_In_ const CJArray<TYPE>& ary) const
	{
		return operator ==(ary);
	}
	//查找
	INT_PTR Find(_In_ const TYPE& Element, _In_opt_ INT_PTR nStart = 0) const
	{
		return Find(&Element, 1, nStart);
	}

	INT_PTR Find(_In_ const TYPE* lpSub, _In_ INT_PTR nSize, _In_opt_ INT_PTR nStart = 0) const
	{
		return Find(GetData(), GetSize(), lpSub, nSize, nStart);
	}

	INT_PTR Find(_In_ const CJArray<TYPE>& ary, _In_opt_ INT_PTR nStart = 0) const
	{
		return Find(ary.GetData(), ary.GetSize(), nStart);
	}

	static INT_PTR Find(_In_ const TYPE* pData, _In_ INT_PTR nSize,
		_In_ const TYPE* lpSub, _In_ INT_PTR nSubSize, _In_opt_ INT_PTR nStart = 0)
	{
		_ASSERTE(nSubSize > 0);
		_ASSERTE(lpSub != NULL);
		_ASSERTE(nStart >= 0);
		for (INT_PTR i = nStart; i <= nSize - nSubSize ; i++)
		{
			if (JieEqualsElements<TYPE>(pData + i, lpSub, nSubSize)){
				return i;
			}
		}
		return -1;
	}

	//附加
	INT_PTR Append(_In_ const TYPE& newElement)
	{
		return Append(&newElement,1);
	}

	INT_PTR Append(_In_ const TYPE* pSrc, INT_PTR nSize)
	{
		INT_PTR nOldSize = GetSize();
		INT_PTR nNewSize = nOldSize + nSize;
		TYPE*pData = GetBuffer(nNewSize);
		JieConstructElements<TYPE>(pData + nOldSize, nSize, pSrc);
		ReleaseBuffer(nNewSize);
		return nOldSize;
	}

	INT_PTR Append(_In_ const CJArray<TYPE>& Src)
	{
		INT_PTR nSize = Src.GetSize();
		if (nSize == 0){
			return GetSize();
		}
		if (GetSize() == 0){
			operator =(Src);
			return 0;
		}
		return Append(Src.GetData(), Src.GetSize());
	}

	CJArray<TYPE>&Copy(_In_ const CJArray<TYPE>&ary)
	{
		return operator =(ary);
	}

	void  InsertAt(_In_ INT_PTR nIndex, _In_ const TYPE* pNewData, _In_ INT_PTR nSize)
	{
		_ASSERTE(nSize >= 0);
		if (nSize == 0) {
			return;
		}
		_ASSERTE(nIndex >= 0);
		_ASSERTE(nIndex <= m_nSize);
		if (nIndex < 0 || nIndex > m_nSize){
			throw std::out_of_range("下标越界");
		}
		INT_PTR nOldSize = GetSize();
		INT_PTR nNewSize = nOldSize + nSize;
		CJArrayData<TYPE>*pAryData = GetPtr();
		if (nIndex == 0 && !pAryData->IsShared() && (m_pData - pAryData->pData) >= nSize){
			m_pData -= nSize;
			m_nSize += nSize;
			JieCopyElements<TYPE>(m_pData, pNewData, nSize);
			return;
		}
		TYPE*pData = GetBuffer(nNewSize);	
		if (nIndex != nOldSize) {
			memmove_s(pData + nIndex + nSize, (nOldSize - nIndex) * sizeof(TYPE),
				pData + nIndex, (nOldSize - nIndex) * sizeof(TYPE));
		}
		JieConstructElements<TYPE>(pData + nIndex, nSize, pNewData);
		ReleaseBuffer(nNewSize);
	}

	void InsertAt(_In_ INT_PTR nIndex, _In_ const TYPE& newElement, _In_opt_ INT_PTR nCount = 1)
	{
		_ASSERTE(nCount >= 0);
		if (nCount == 0) {
			return;
		}
		_ASSERTE(nIndex >= 0);
		_ASSERTE(nIndex <= m_nSize);
		if (nIndex < 0 || nIndex > m_nSize){
			throw std::out_of_range("下标越界");
		}
		INT_PTR nOldSize = GetSize();
		INT_PTR nNewSize = nOldSize + nCount;
		CJArrayData<TYPE>*pAryData = GetPtr();
		if (nIndex == 0 && !pAryData->IsShared() && (m_pData - pAryData->pData) >= nCount){
			m_pData -= nCount;
			m_nSize += nCount;
			JieSetElements<TYPE>(m_pData, newElement, nCount);
			return;
		}

		GetBuffer(nNewSize);
		if (nIndex != nOldSize) {
			memmove_s(m_pData + nIndex + nCount, (nOldSize - nIndex) * sizeof(TYPE),
				m_pData + nIndex, (nOldSize - nIndex) * sizeof(TYPE));
		}
		JieConstructElements<TYPE>(m_pData + nIndex, nCount, newElement);
		ReleaseBuffer(nNewSize);
	}

	void InsertAt(_In_ INT_PTR nIndex, _In_ const CJArray<TYPE>&Ary)
	{
		return InsertAt(nIndex, Ary.GetData(), Ary.GetSize());
	}

	//移除元素
	void RemoveAt(_In_ INT_PTR nIndex, _In_opt_ INT_PTR nCount = 1)
	{
		_ASSERTE(nCount >= 0);
		if (nCount<0){
			throw std::bad_alloc();
		}
		if (nCount == 0){
			return;
		}
		_ASSERTE(nIndex >= 0);
		_ASSERTE(nIndex < m_nSize);
		if (nIndex < 0 || nIndex >= m_nSize){
			throw std::out_of_range("下标越界");
		}
		INT_PTR nOldSize = GetSize();
		INT_PTR nNewSize = nOldSize - nCount;
		if (nIndex + nCount > nOldSize) {
			nCount = nOldSize - nIndex;
		}

		if (nIndex == 0){
			if (nCount == nOldSize){
				RemoveAll();
			}
			else{
				m_pData += nCount;
				m_nSize -= nCount;
			}
			return;
		}
		TYPE*pData = GetBuffer(nNewSize);
		JieDestructElements<TYPE>(pData + nIndex, nCount);
		INT_PTR nMoveCount = nOldSize - nIndex - nCount;
		if (nMoveCount) {
			memmove_s(pData + nIndex, nMoveCount * sizeof(TYPE),
				pData + nIndex + nCount, nMoveCount * sizeof(TYPE));
		}
		ReleaseBuffer(nNewSize);
	}

	void QuickSort(_In_opt_ BOOL bDescend = TRUE)
	{
		QuickSort(0, GetUpperBound(), Compare<TYPE>, bDescend);
	}

	void QuickSort(_In_ INT_PTR(__cdecl *compare)(const TYPE&, const TYPE&), _In_opt_ BOOL bDescend = TRUE)
	{
		QuickSort(0, GetUpperBound(), compare, bDescend);
	}

	void QuickSort(_In_ INT_PTR head, INT_PTR end, INT_PTR(__cdecl *compare)(const TYPE&, const TYPE&), _In_opt_ BOOL bDescend = TRUE)
	{
		if (head < 0 || head >= end || end >= GetSize() || compare == NULL) {
			return;
		}
		INT_PTR i = head, j = end;
		TYPE mid = GetAt((i + j) / 2);
		while (true)
		{
			INT_PTR com;
			com = compare(GetAt(i), mid);
			while ((bDescend && (com < 0)) || (bDescend == FALSE && (com > 0)))
			{
				i++;
				com = compare(GetAt(i), mid);
			}
			com = compare(GetAt(j), mid);
			while ((bDescend && (com > 0)) || (bDescend == FALSE && (com < 0)))
			{
				j--;
				com = compare(GetAt(j), mid);
			}
			if (i >= j)
				break;
			com = compare(GetAt(i), GetAt(j));
			if ((bDescend && (com > 0)) || ((bDescend == FALSE) && (com < 0)))  {
				TYPE temp = GetAt(i);
				SetAt(i, GetAt(j));
				SetAt(j, temp);
			}
			else if (com == 0)
				i++;
		}
		_ASSERTE(i == j);
		QuickSort(head, j - 1, compare, bDescend);
		QuickSort(j + 1, end, compare, bDescend);
	}

	void BubbleSort(_In_opt_ BOOL bDescend = TRUE)
	{
		BubbleSort(Compare<TYPE>, bDescend);
	}

	void BubbleSort(INT_PTR(__cdecl *compare)(const TYPE&, const TYPE&), _In_opt_ BOOL bDescend = TRUE)
	{
		INT_PTR nSize = GetSize();
		BOOL bEnd;
		for (int i = 0; i < nSize - 1; i++)
		{
			bEnd = TRUE;
			for (int j = 1; j < nSize - i; j++)
			{
				INT_PTR com = compare(GetAt(j - 1), GetAt(j));
				if ((bDescend&&com > 0) ||
					((bDescend == FALSE) && (com < 0)))
				{
					TYPE temp = GetAt(j);
					SetAt(j, GetAt(j - 1));
					SetAt(j - 1, temp);
					bEnd = FALSE;
				}
			}
			if (bEnd){
				return;
			}
		}
		return;
	}

	INT_PTR Count(_In_ const TYPE* lp, _In_ INT_PTR nSize) const
	{
		_ASSERTE(nSize > 0);
		_ASSERTE(lp != NULL);
		INT_PTR cnt = 0;
		INT_PTR nStart = 0;
		while (true)
		{
			nStart = Find(lp, nSize, nStart);
			if (nStart == -1){
				return cnt;
			}
			nStart += nSize;
			cnt++;
		}
	}

	INT_PTR Count(_In_ const CJArray<TYPE>&ary) const
	{
		return Count(ary.GetData(), ary.GetSize());
	}

	INT_PTR Count(_In_ const TYPE& Element) const
	{
		return Count(&Element, 1);
	}

	CJArray<CJArray<TYPE> > Split(_In_ const CJArray<TYPE>&ary) const
	{
		size_t cnt = Count(ary) + 1;
		CJArray<CJArray<TYPE> > res;
		res.SetSize(cnt);
		INT_PTR nIndexL = 0;
		INT_PTR nIndexR = 0;
		INT_PTR nSplitLen = ary.GetSize();
		for (size_t i = 0; i < cnt; i++)
		{
			nIndexR = Find(ary, nIndexL);
			CJArray<TYPE>&item = res.SetAt(i);
			if (nIndexR != -1)
			{
				item = Mid(nIndexL, nIndexR - nIndexL);
				nIndexL = nIndexR + nSplitLen;
			}
			else
			{
				_ASSERTE(cnt == i + 1);
				item = Mid(nIndexL, GetSize() - nIndexL);
				return res;
			}
		}
		_ASSERTE(FALSE);
		return res;
	}

	CJArray<CJArray<TYPE> > Split(_In_ const TYPE& Element) const
	{
		size_t cnt = Count(Element) + 1;
		CJArray<CJArray<TYPE> > res;
		res.SetSize(cnt);
		INT_PTR nIndexL = 0;
		INT_PTR nIndexR = 0;
		for (size_t i = 0; i < cnt; i++)
		{
			nIndexR = Find(Element, nIndexL);
			CJArray<TYPE>&item = res.SetAt(i);
			if (nIndexR != -1)
			{
				item = Mid(nIndexL, nIndexR - nIndexL);
				nIndexL = nIndexR + 1;
			}
			else
			{
				_ASSERTE(cnt == i + 1);
				item = Mid(nIndexL, GetSize() - nIndexL);
				return res;
			}
		}
		_ASSERTE(FALSE);
		return res;
	}

	void AllocBuffer(_In_ INT_PTR nLength)
	{
		GetBuffer(nLength);
	}
	//反向
	void Reverse()
	{
		BYTE temp[sizeof(TYPE)];
		TYPE* pData = GetBuffer();
		INT_PTR nSize = GetSize();
		for (INT_PTR i = 0; i < nSize - 1 - i; i++)
		{
			memcpy(temp, pData + i, sizeof(TYPE));
			memcpy(pData + i,
				pData + nSize - 1 - i, sizeof(TYPE));
			memcpy(pData + nSize - 1 - i,
				temp, sizeof(TYPE));
		}
	}
protected:
	TYPE* m_pData;
	INT_PTR m_nSize;
	CJArrayData<TYPE>* CloneData(const CJArrayData<TYPE>*pData)
	{
		return (CJArrayData<TYPE>*)pData ;
	}

	TYPE* GetBuffer(_In_ INT_PTR nLength)
	{
		CJArrayData<TYPE>*pData = GetPtr();
		if (pData == NULL)
			pData = CreateObject();
		if (!pData->IsShared()){
			INT_PTR nPos = m_pData - pData->pData;
			pData->AllocBuffer(nPos+nLength);
			m_pData = pData->pData + nPos;
			INT_PTR nRemove = pData->nSize - nPos - m_nSize;
			if (nRemove != 0){
				pData->nSize = nPos + m_nSize;
				JieDestructElements<TYPE>(pData->pData + pData->nSize, nRemove);
			}
			return m_pData;
		}
		CJArrayData<TYPE>*pDataNew = new CJArrayData<TYPE>();
		pDataNew->nSize = m_nSize;
		pDataNew->AllocBuffer(max(m_nSize, nLength));
		JieConstructElements<TYPE>(pDataNew->pData, m_nSize, m_pData);
		Release();
		Attach(pDataNew);
		m_pData = pDataNew->pData;
		return m_pData;
	}

	void ReleaseBuffer(_In_ INT_PTR nSize)
	{
		m_nSize = nSize;
		CJArrayData<TYPE>*pData = GetPtr();
		INT_PTR nPos = m_pData - pData->pData;
		pData->nSize = nPos + nSize;
	}
};

typedef CJArray<BYTE> CJByteArray;
typedef CJArray<DWORD> CJDWordArray;
typedef CJArray<int> CJIntArray;
typedef CJArray<LONGLONG> CJLong64Array;

