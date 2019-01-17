//-----------------------------------------------------------------------------
// VST Module Architecture SDK
// Version 1.0    Date : 01/2004
//
// Project      : Common Base Classes
// Filename     : pluginview.cpp
// Created by   : Matthias Juwan
// Description  : Plug-In View Implementation
//
//-----------------------------------------------------------------------------
// LICENSE
// © 2004, Steinberg Media Technologies, All Rights Reserved
//-----------------------------------------------------------------------------
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of the Steinberg Media Technologies nor the names of its
//     contributors may be used to endorse or promote products derived from this
//     software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A  PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef __pluginview__
#include "pluginview.h"
#endif

//------------------------------------------------------------------------
//  CPluginView implementation
//------------------------------------------------------------------------

CPluginView::CPluginView (ViewRect* _rect)
: rect (0, 0, 0, 0),
  systemWindow (0)
{
	FUNKNOWN_CTOR
	if(_rect)
		rect = *_rect;
}

//------------------------------------------------------------------------
CPluginView::~CPluginView ()
{
	FUNKNOWN_DTOR
}

//------------------------------------------------------------------------
IMPLEMENT_REFCOUNT (CPluginView)

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginView::queryInterface (const char* iid, void** obj)
{
	QUERY_INTERFACE (iid, obj, ::FUnknown::iid, IPlugView)
	QUERY_INTERFACE (iid, obj, IPlugView::iid,  IPlugView)
	*obj = 0;
	return kNoInterface;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginView::attached (void* parent)
{
	systemWindow = parent;
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginView::removed ()
{
	systemWindow = 0;
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginView::onSize (ViewRect* newSize)
{
	if(newSize)
		rect = *newSize;
	return kResultTrue;
}
