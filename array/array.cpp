// array.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "JArray.h"
#include "vector"
//CJArray 与 vector性能对比
int _tmain(int argc, _TCHAR* argv[])
{
	//count大小可以任意修改
	size_t count = 100000;
	DWORD tick;
	tick = ::GetTickCount();
	CJArray<int> arr;
	for (size_t i = 0; i < count; i++)
	{
		arr.Add(i);
		CJArray<int> arrayTemp = arr.Left(arr.GetSize() / 2);
	}
	printf("CJArray time:%d\n", ::GetTickCount() - tick);


	tick = ::GetTickCount();
	std::vector<int> vec;
	for (size_t i = 0; i < count; i++)
	{
		vec.push_back(i);
		std::vector<int> vecTemp;
		vecTemp.assign(vec.begin(), vec.begin() + vec.size()/2);
		
	}
	printf("vector time:%d\n", ::GetTickCount() - tick);
	::system("pause");
	return 0;
}

