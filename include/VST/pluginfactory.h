//-----------------------------------------------------------------------------
// VST Module Architecture SDK
// Version 1.0    Date : 01/2004
//
// Project      : Common Base Classes
// Filename     : pluginfactory.h
// Created by   : Matthias Juwan
// Description  : Standard Plug-In Factory
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

#ifndef __pluginfactory__
#define __pluginfactory__

#ifndef __ipluginbase__
#include "ipluginbase.h"
#endif

//------------------------------------------------------------------------
//  PClassInfoEx : extended class info
//------------------------------------------------------------------------
struct PClassInfoEx
{
//------------------------------------------------------------------------
	PClassInfo base;
	FUnknown* (*createFunc)(void*);
	void* context;
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
//  CPluginFactory class declaration
//------------------------------------------------------------------------
class CPluginFactory: public IPluginFactory
{
public:
//------------------------------------------------------------------------
	CPluginFactory (PFactoryInfo& info);
	virtual ~CPluginFactory ();

	enum { kMaxClasses = 100 };

//------------------------------------------------------------------------
	bool registerClass (PClassInfo* info,
						FUnknown* (*createFunc)(void*),
						void* context = 0);

	DECLARE_FUNKNOWN_METHODS

//------------------------------------------------------------------------
	// Interface methods:
	tresult PLUGIN_API getFactoryInfo (PFactoryInfo* info);
	long PLUGIN_API countClasses ();
	tresult PLUGIN_API getClassInfo (long index, PClassInfo* info);
	tresult PLUGIN_API createInstance (const char* cid, const char* iid, void** obj);
//------------------------------------------------------------------------
protected:
	PFactoryInfo factoryInfo;
	PClassInfoEx classes[kMaxClasses];
	long classCount;
};

extern CPluginFactory* gPluginFactory;

#endif