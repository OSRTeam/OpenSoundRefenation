/*********************************************************
* Copyright (C) VERTVER & FROWRIK, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR UI
**********************************************************
* DragAndDrop.cpp
* Drag & Drop Windows implementation
*********************************************************/
#include "stdafx.h"
#include "DragAndDrop.h"

ULONG
STDMETHODCALLTYPE
DropTarget::AddRef()
{
	return InterlockedIncrement(&Counter);
}

ULONG
STDMETHODCALLTYPE
DropTarget::Release()
{
	return InterlockedDecrement(&Counter);
}

HRESULT
STDMETHODCALLTYPE
DropTarget::QueryInterface(
	REFIID riid,
	void** ppvObject
)
{
	if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IDropTarget))
	{
		AddRef();
		*ppvObject = this;
		return S_OK;
	}

	*ppvObject = nullptr;
	return E_NOINTERFACE;
}

HRESULT
STDMETHODCALLTYPE
DropTarget::DragEnter(
	IDataObject *pDataObj,
	DWORD grfKeyState,
	POINTL pt,
	DWORD *pdwEffect
)
{
	if (!pHelper)
	{
		if (FAILED(CoCreateInstance(CLSID_DragDropHelper, nullptr, CLSCTX_INPROC_SERVER, IID_IDropTargetHelper, reinterpret_cast<VOID**>(&pHelper))))
		{
			THROW2(L"Can't init Drag and Drop helper.");
		}
	}
	
	pHelper->DragEnter(Window, pDataObj, reinterpret_cast<POINT*>(&pt), *pdwEffect);

	pObject = pDataObj;
	pObject->AddRef();

	*pdwEffect = DROPEFFECT_NONE;

	return S_OK;
}

HRESULT
STDMETHODCALLTYPE
DropTarget::DragLeave()
{
	if (!pHelper)
	{
		if (FAILED(CoCreateInstance(CLSID_DragDropHelper, nullptr, CLSCTX_INPROC_SERVER, IID_IDropTargetHelper, reinterpret_cast<VOID**>(&pHelper))))
		{
			THROW2(L"Can't init Drag and Drop helper.");
		}
	}

	pHelper->DragLeave();

	_RELEASE(pObject);

	return S_OK;
}

HRESULT
STDMETHODCALLTYPE
DropTarget::DragOver(
	DWORD grfKeyState, 
	POINTL pt,
	DWORD *pdwEffect
)
{
	if (!pHelper)
	{
		if (FAILED(CoCreateInstance(CLSID_DragDropHelper, nullptr, CLSCTX_INPROC_SERVER, IID_IDropTargetHelper, reinterpret_cast<VOID**>(&pHelper))))
		{
			THROW2(L"Can't init Drag and Drop helper.");
		}
	}

	pHelper->DragOver(reinterpret_cast<POINT*>(&pt), *pdwEffect);

	*pdwEffect = DROPEFFECT_COPY;

	return S_OK;
}

HRESULT
STDMETHODCALLTYPE
DropTarget::Drop(
	IDataObject *pDataObj,
	DWORD grfKeyState,
	POINTL pt, 
	DWORD *pdwEffect
)
{
	if (!pHelper)
	{
		if (FAILED(CoCreateInstance(CLSID_DragDropHelper, nullptr, CLSCTX_INPROC_SERVER, IID_IDropTargetHelper, reinterpret_cast<VOID**>(&pHelper))))
		{
			THROW2(L"Can't init Drag and Drop helper.");
		}
	}

	if (pHelper && pObject)
	{
		pHelper->Drop(pObject, reinterpret_cast<POINT*>(&pt), *pdwEffect);
	}

	{
		STGMEDIUM Medium = { 0 };
		FORMATETC Format = { 0 };
		Format.ptd = nullptr;
		Format.dwAspect = DVASPECT_CONTENT;
		Format.lindex = -1;
		Format.tymed = TYMED_HGLOBAL;
		Format.cfFormat = CF_HDROP;

		if (SUCCEEDED(pObject->GetData(&Format, &Medium)))
		{
			HDROP hDrop = static_cast<HDROP>(Medium.hGlobal);

			DWORD dwFileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
			WSTRING_PATH szFilePaths = { NULL };

			if (dwFileCount < 2)
			{
				for (DWORD i = 0; i < dwFileCount; i++) //-V1008
				{
					DWORD dwPathSize = DragQueryFileW(hDrop, i, nullptr, 0);

					if (dwPathSize > 0 && dwPathSize < MAX_PATH)
					{
						if (DragQueryFileW(hDrop, i, szFilePaths, dwPathSize + 1) > 0)
						{
							WIN32_FIND_DATAW findData = { NULL };

							HANDLE hFind = FindFirstFileW(szFilePaths, &findData);

							// open file here
							OutMixer->LoadSample(szFilePaths);

							FindClose(hFind);
						}
					}
				}
			}
		}
	}

	_RELEASE(pObject);

	return S_OK;
}
