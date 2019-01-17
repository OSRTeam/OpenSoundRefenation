/*********************************************************
* Copyright (C) VERTVER & FROWRIK, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR UI
**********************************************************
* DragAndDrop.h
* Drag & Drop Windows implementation
*********************************************************/
#pragma once
#include "stdafx.h"
#include "shobjidl_core.h"
#include "shlguid.h"

struct DropTarget : public IDropTarget
{
	DropTarget() : Counter(0), pHelper(nullptr), pObject(nullptr), Window(NULL), OutMixer(nullptr)
	{
		X = -1;
		Y = -1;
		DragAndDropRegionNumber = -1;
	}

	VOID AddMixer(OSR::Mixer* pMixer) { OutMixer = pMixer; };
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
	ULONG STDMETHODCALLTYPE AddRef() override;
	ULONG STDMETHODCALLTYPE Release() override;
	HRESULT STDMETHODCALLTYPE DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) override;
	HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) override;
	HRESULT STDMETHODCALLTYPE DragLeave() override;
	HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) override;

	LONG Counter;
	IDropTargetHelper* pHelper;
	IDataObject* pObject;
	HWND Window;
	OSR::Mixer* OutMixer;

	i32 X;
	i32 Y;
	i32 DragAndDropRegionNumber;
};
