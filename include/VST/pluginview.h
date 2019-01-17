//-----------------------------------------------------------------------------
// VST Module Architecture SDK
// Version 1.0    Date : 01/2004
//
// Project      : Common Base Classes
// Filename     : pluginview.h
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
#define __pluginview__

#ifndef __iplugui__
#include "gui/iplugui.h"
#endif

//------------------------------------------------------------------------
//  CPluginView : plugin view base class
//------------------------------------------------------------------------

class CPluginView: public IPlugView
{
public:
//------------------------------------------------------------------------
	CPluginView (ViewRect* rect);
	virtual ~CPluginView ();

	const ViewRect& getRect () const	{ return rect; }
	void setRect (const ViewRect& r)	{ rect = r; }

	bool isAttached () const			{ return systemWindow != 0; }

//------------------------------------------------------------------------
	// Interface methods:
	DECLARE_FUNKNOWN_METHODS

	tresult PLUGIN_API attached (void* parent);
	tresult PLUGIN_API removed ();

	tresult PLUGIN_API idle () { return kResultFalse; }
	tresult PLUGIN_API onWheel (float distance) { return kResultFalse; }
	tresult PLUGIN_API onKey (char asciiKey, long keyMsg, long modifiers) { return kResultFalse; }
	tresult PLUGIN_API onSize (ViewRect* newSize);
//------------------------------------------------------------------------
protected:
	ViewRect rect;
	void* systemWindow;
};

#endif