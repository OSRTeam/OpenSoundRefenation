#include "stdafx.h"

//BOOL 
//CheckPluginList(
//	VSTHost* pHost,
//	LPCWSTR lpPluginFolder, 
//	VSTPLUGIN_INFO* pPluginInfo, 
//	size_t StructSizes
//)
//{
//	BOOL isFind = TRUE;
//	WIN32_FIND_DATAW findData = { 0 };
//	HANDLE hFind = FindFirstFileW(lpPluginFolder, &findData);
//
//	if (hFind)
//	{
//		while (isFind)
//		{
//			
//			//findData.cFileName
//			isFind = FindNextFileW(hFind, &findData);
//		}
//	}
//	else
//	{
//		return FALSE;
//	}
//
//	return TRUE;
//}