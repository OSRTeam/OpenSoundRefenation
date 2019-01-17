//-----------------------------------------------------------------------------
// VST Module Architecture SDK
// Version 1.0    Date : 01/2004
//
// Category     : SDK Core Interfaces
// Filename     : ipluginbase.h
// Created by   : Matthias Juwan
// Description  : Basic Plug-In Interfaces
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

#ifndef __ipluginbase__
#define __ipluginbase__

#ifndef __funknown__
#include "funknown.h"
#endif

//------------------------------------------------------------------------
//  IPluginBase interface declaration
//------------------------------------------------------------------------
class IPluginBase: public FUnknown
{
public:
//------------------------------------------------------------------------
	virtual tresult PLUGIN_API initialize (FUnknown* context) = 0;
	virtual tresult PLUGIN_API terminate () = 0;
//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IPluginBase, 0x22888DDB, 0x156E45AE, 0x8358B348, 0x08190625)


//------------------------------------------------------------------------
//  PFactoryInfo : Basic Information about the class factory of the plugin
//------------------------------------------------------------------------
struct PFactoryInfo
{
//------------------------------------------------------------------------
	enum FactoryFlags
	{
		kNoFlags = 0,
		kClassesDiscardable = 1
	};

//------------------------------------------------------------------------
	char vendor[64];			// e.g. "Steinberg Media Technologies"
	char url[256];				// e.g. "http://www.steinberg.de"
	char email[128];			// e.g. "info@steinberg.de"
	long flags;					// (see above)
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
//  PClassInfo : Basic Information about a class provided by the plugin
//------------------------------------------------------------------------
struct PClassInfo
{
//------------------------------------------------------------------------
	enum ClassCardinality
	{
		kManyInstances = 0x7FFFFFFF
	};

	enum
	{
		kCategorySize = 32,
		kNameSize = 64
	};
//------------------------------------------------------------------------
	char cid[16];					// Class ID 16 Byte class GUID
	long cardinality;				// currently ignored, set to kManyInstances
	char category[kCategorySize];	// class category, host uses this to categorize interfaces
	char name[kNameSize];			// class name, visible to the user
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
//  IPluginFactory interface declaration
//------------------------------------------------------------------------
/*
	The host uses this interface to create instances of classes implemented
	in the plugin (that usually define the IPluginBase interface).
	An implementation is provided in public.sdk/source/common/pluginfactory.cpp
*/
//------------------------------------------------------------------------

class IPluginFactory : public FUnknown
{
public:
//------------------------------------------------------------------------
	virtual tresult PLUGIN_API getFactoryInfo (PFactoryInfo* info) = 0;

	virtual long    PLUGIN_API countClasses () = 0;
	virtual tresult PLUGIN_API getClassInfo (long index, PClassInfo* info) = 0;
	virtual tresult PLUGIN_API createInstance (const char* cid, const char* iid, void** obj) = 0;
//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IPluginFactory, 0x7A4D811C, 0x52114A1F, 0xAED9D2EE, 0x0B43BF9F)

//------------------------------------------------------------------------
//  Plug-In entrypoint
//------------------------------------------------------------------------

extern "C"
{
	IPluginFactory* PLUGIN_API GetPluginFactory ();
	typedef IPluginFactory* (PLUGIN_API *GetFactoryProc)();
}

#endif