//-----------------------------------------------------------------------------
// VST Module Architecture SDK
// Version 1.0    Date : 01/2004
//
// Project      : Common Base Classes
// Filename     : pluginfactory.cpp
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
#include "pluginfactory.h"
#endif

CPluginFactory* gPluginFactory = 0;

//------------------------------------------------------------------------
//  CPluginFactory implementation
//------------------------------------------------------------------------

CPluginFactory::CPluginFactory (PFactoryInfo& info)
: classCount (0)
{
	FUNKNOWN_CTOR

	memset (classes, 0, sizeof(PClassInfoEx) * kMaxClasses);
	factoryInfo = info;
}

//------------------------------------------------------------------------
CPluginFactory::~CPluginFactory ()
{
	if(gPluginFactory == this)
		gPluginFactory = 0;

	FUNKNOWN_DTOR
}

//------------------------------------------------------------------------
IMPLEMENT_FUNKNOWN_METHODS (CPluginFactory, IPluginFactory, IPluginFactory::iid)

//------------------------------------------------------------------------
bool CPluginFactory::registerClass (PClassInfo* info,
									FUnknown* (*createFunc)(void*),
									void* context)
{
	if(!info || !createFunc || classCount + 1 >= kMaxClasses)
		return false;

	classes[classCount].base = *info;
	classes[classCount].createFunc = createFunc;
	classes[classCount].context = context;
	classCount++;
	return true;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginFactory::getFactoryInfo (PFactoryInfo* info)
{
	if(info)
		memcpy (info, &factoryInfo, sizeof(PFactoryInfo));

	return kResultOk;
}

//------------------------------------------------------------------------
long PLUGIN_API CPluginFactory::countClasses ()
{
	return classCount;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginFactory::getClassInfo (long index, PClassInfo* info)
{
	if(info && (index >= 0 && index < classCount))
	{
		memcpy (info, &classes[index], sizeof(PClassInfo));
		return kResultOk;
	}

	return kInvalidArgument;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginFactory::createInstance (const char* cid, const char* iid, void** obj)
{
	for(long i = 0; i < classCount; i++)
		if(memcmp (classes[i].base.cid, cid, 16) == 0)
		{
			FUnknown* instance = classes[i].createFunc (classes[i].context);
			if(instance)
			{
				if(instance->queryInterface (iid, obj) == kResultOk)
				{
					instance->release ();
					return kResultOk;
				}
				else
					instance->release ();
			}
			break;
		}

	*obj = 0;
	return kNoInterface;
}