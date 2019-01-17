//-----------------------------------------------------------------------------
// VST Module Architecture SDK
// Version 1.0    Date : 01/2004
//
// Project      : Common Base Classes
// Filename     : plugguieditor.h
// Created by   : Matthias Juwan, Arne Scheffler
// Description  : Glue to VSTGUI Toolkit
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

#ifndef __plugguieditor__
#define __plugguieditor__

#ifndef __pluginview__
#include "pluginview.h"
#endif

//-----------------------------------------------------------------------------
// SDK Interface Headers
#ifndef __iguicontroller__
#include "host/iguicontroller.h"
#endif
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Include VSTGUI
#ifndef __vstgui__
#include "vstgui.h"
#endif
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  PluginGUIEditor class declaration
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class PluginGUIEditor: public CPluginView
{
public:
//-----------------------------------------------------------------------------
	PluginGUIEditor (ViewRect* rect, FUnknown* context, void* plugin = 0);
	virtual ~PluginGUIEditor ();

//-----------------------------------------------------------------------------
	// get the plugin attached to this editor
	virtual void* getPlugin ()	{ return plugin; }

	// get the CFrame object
	#if USE_NAMESPACE
	VSTGUI::CFrame* getFrame () { return frame; }
	#else
	CFrame* getFrame () { return frame; }
	#endif

//-----------------------------------------------------------------------------
	// CPluginView methods:
//-----------------------------------------------------------------------------
	
	tresult PLUGIN_API attached (void* parent);
	tresult PLUGIN_API removed ();
	tresult PLUGIN_API onSize (ViewRect* newSize);
	tresult PLUGIN_API idle ();
	tresult PLUGIN_API onWheel (float distance);
	
//-----------------------------------------------------------------------------
	// VSTGUI methods:
//-----------------------------------------------------------------------------
	
	virtual void update ()		{}
	virtual void postUpdate ()	{ updateFlag = 1; }

	void wait (unsigned long ms);	// wait (in ms)
	unsigned long getTicks ();		// get the current time (in ms)
	virtual void doIdleStuff ();	// feedback to appli.

	// set/get the knob mode
	virtual long setKnobMode (int val);
	static  long getKnobMode () { return knobMode; }
	enum { kCircularMode, kRelativCircularMode, kLinearMode };
//-----------------------------------------------------------------------------
protected:
	long updateFlag;

	#if USE_NAMESPACE
	VSTGUI::CFrame* frame;
	#else
	CFrame* frame;
	#endif

	void* plugin;

private:
	unsigned long lLastTicks;
	bool inIdleStuff;
	IGuiController* hostGuiController;
	static long knobMode;
};

#endif
