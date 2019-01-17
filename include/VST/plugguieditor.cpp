//-----------------------------------------------------------------------------
// VST Module Architecture SDK
// Version 1.0    Date : 01/2004
//
// Project      : Common Base Classes
// Filename     : plugguieditor.cpp
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
#include "plugguieditor.h"
#endif

#if WINDOWS
#include <windows.h>
#include <ole2.h>

#elif MAC
#include <unistd.h>
#endif

//-----------------------------------------------------------------------------
#define kIdleRate    100 // host idle rate in ms
#define kIdleRate2    50
#define kIdleRateMin   4 // minimum time between 2 idles in ms

//-----------------------------------------------------------------------------
//  PluginGUIEditor implementation
//-----------------------------------------------------------------------------

PluginGUIEditor::PluginGUIEditor (ViewRect* rect, FUnknown* context, void* plugin) 
: CPluginView (rect),
  updateFlag (0),
  frame (0),
  plugin (plugin),
  inIdleStuff (false),
  hostGuiController (0)
{
	if(context)
		context->queryInterface (IGuiController::iid, (void**)&hostGuiController);
	
	lLastTicks = getTicks ();

	#if WINDOWS
	OleInitialize (0);
	#endif
}

//-----------------------------------------------------------------------------
PluginGUIEditor::~PluginGUIEditor () 
{
	#if WINDOWS
	OleUninitialize ();
	#endif

	if(hostGuiController)
		hostGuiController->release ();
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PluginGUIEditor::attached (void* parent)
{
	// add this to update the value the first time
	postUpdate ();

	return CPluginView::attached (parent);
}

//------------------------------------------------------------------------
tresult PLUGIN_API PluginGUIEditor::removed ()
{
	if(frame)
		delete frame;
	frame = 0;

	return CPluginView::removed ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API PluginGUIEditor::onSize (ViewRect* r)
{
	if(frame)
		frame->setSize (r->right - r->left, r->bottom - r->top);
	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PluginGUIEditor::idle ()
{
	if (inIdleStuff)
		return kResultOk;

	if(updateFlag) { updateFlag = 0; update (); }
	if (frame)
		frame->idle ();
		
	return kResultOk;
}

//-----------------------------------------------------------------------------
long PluginGUIEditor::knobMode = PluginGUIEditor::kCircularMode;

//-----------------------------------------------------------------------------
long PluginGUIEditor::setKnobMode (int val) 
{
	PluginGUIEditor::knobMode = val;
	return 1;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PluginGUIEditor::onWheel (float distance)
{
	if (frame)
		return frame->onWheel (0, CPoint (), distance);
	
	return false;
}

//-----------------------------------------------------------------------------
void PluginGUIEditor::wait (unsigned long ms)
{
	#if MAC
	usleep (ms * 1000);
	
	#elif WINDOWS
	Sleep (ms);

	#endif
}

//-----------------------------------------------------------------------------
unsigned long PluginGUIEditor::getTicks ()
{
	#if MAC
	return (TickCount () * 1000) / 60;
	
	#elif WINDOWS
	return (unsigned long)GetTickCount ();
	
	#endif

	return 0;
}

//-----------------------------------------------------------------------------
void PluginGUIEditor::doIdleStuff ()
{
	unsigned long currentTicks = getTicks ();

	if (currentTicks < lLastTicks)
	{
		#if MAC
		RunCurrentEventLoop (kEventDurationMillisecond * kIdleRateMin);
		#else
		wait (kIdleRateMin);
		#endif
		currentTicks += kIdleRateMin;
		if (currentTicks < lLastTicks - kIdleRate2)
			return;
	}
	idle ();

	#if WINDOWS
	MSG windowsMessage;
	if (PeekMessage (&windowsMessage, NULL, WM_PAINT, WM_PAINT, PM_REMOVE))
		DispatchMessage (&windowsMessage);
	#endif

 	lLastTicks = currentTicks + kIdleRate;

	inIdleStuff = true;

	if(hostGuiController)
		hostGuiController->onIdle ();

	inIdleStuff = false;
}
