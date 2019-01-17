//-----------------------------------------------------------------------------
// VST Module Architecture SDK
// Version 1.0    Date : 01/2004
//
// Project      : Common Base Classes
// Filename     : plugxmlgui.h
// Created by   : Matthias Juwan
// Description  : Plug-In XML GUI
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

#ifndef __plugxmlgui__
#define __plugxmlgui__

//-----------------------------------------------------------------------------
// SDK Interface Headers
#ifndef __funknown__
#include "base/funknown.h" // need this for platform macros
#endif
//-----------------------------------------------------------------------------

#include <stdio.h>

//------------------------------------------------------------------------
//  CResourceID : resource identifier
//
//	Windows : integer
//  MacOS X : const string pointer
//------------------------------------------------------------------------

struct CResourceID
{
//------------------------------------------------------------------------
	#if WINDOWS
	int resId;
	#elif MAC
	const char* resName;
	#endif

	CResourceID (long _resId = 0, const char* _resName = 0)
	{
		#if WINDOWS
		resId = _resId;
		#elif MAC
		resName = _resName;
		#endif
	}

	void set (const char* s)
	{
		#if WINDOWS
		sscanf (s, "%d", &resId);
		#elif MAC
		resName = s;
		#endif
	}
//------------------------------------------------------------------------
};

//-----------------------------------------------------------------------------
// helper function to load resources from plugin library
bool GetResource (void* loader, const CResourceID& resID, char* buffer, long* bufferSize);
//-----------------------------------------------------------------------------

#endif