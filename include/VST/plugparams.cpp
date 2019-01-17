//-----------------------------------------------------------------------------
// VST Module Architecture SDK
// Version 1.0    Date : 01/2004
//
// Project      : Common Base Classes
// Filename     : plugparams.cpp
// Created by   : Matthias Juwan
// Description  : Parameter Implementation
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

#ifndef __plugparams__
#include "plugparams.h"
#endif

//-----------------------------------------------------------------------------
#if WINDOWS
#include <windows.h>
#undef min
#undef max
#endif

#if MAC
#include <Carbon/Carbon.h>
#endif
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  CDependency
//-----------------------------------------------------------------------------
class CDependency: public CListElement
{
public:
//------------------------------------------------------------------------
	CDependency (IPlugController* c): controller (c) {}
	~CDependency () { if(controller) controller->release (); }
	IPlugController* controller;
//------------------------------------------------------------------------
};

//-----------------------------------------------------------------------------
// CParamString
//-----------------------------------------------------------------------------
class CParamString: public CListElement
{
public:
//------------------------------------------------------------------------
	CParamString (const char* text = 0)	{ set (text); }
	void set (const char* _text)		{ strncpy (text, _text, 128); text[127] = 0; }
	const char* get ()					{ return text; }
//------------------------------------------------------------------------
protected:
	char text[128];
};

//-----------------------------------------------------------------------------
// CParamEntry
//-----------------------------------------------------------------------------
class CParamEntry: public CListElement
{
public:
//-----------------------------------------------------------------------------
	CParamEntry (CParameter* p, const char* _name): param (p) { strcpy (name, _name); }
	~CParamEntry () { if(param) param->release (); }
	CParameter* param;
	char name[256];
//-----------------------------------------------------------------------------
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Parameter updates
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static CLinkedList theParameters;
static long useCounter = 0;
#define kUpdateInterval 20 // 20 ms

//-----------------------------------------------------------------------------
// WINDOWS
//-----------------------------------------------------------------------------
#if WINDOWS
static UINT_PTR timerID = 0;
void CALLBACK ParameterTimerProc (HWND, UINT, UINT_PTR, DWORD dwTime)
{
	CParameter::checkUpdates ();
}
#endif

//-----------------------------------------------------------------------------
// MAC
//-----------------------------------------------------------------------------
#if MAC
static EventLoopTimerRef eventTimer = 0;
pascal void ParameterTimerProc (EventLoopTimerRef inTimer, void *inUserData)
{
	CParameter::checkUpdates ();
}
#endif

//------------------------------------------------------------------------
void CParameter::beginUpdates ()
{
	if(++useCounter == 1)
	{
		#if WINDOWS
		timerID = SetTimer (0, 0, kUpdateInterval, ParameterTimerProc);

		#elif MAC
		InstallEventLoopTimer (GetMainEventLoop (),
								kUpdateInterval *  kEventDurationMillisecond,
								kUpdateInterval * kEventDurationMillisecond,
								ParameterTimerProc, NULL, &eventTimer);
		#endif
	}
}

//------------------------------------------------------------------------
void CParameter::endUpdates ()
{
	if(--useCounter == 0)
	{
		#if WINDOWS
		KillTimer (0, timerID);

		#elif MAC
		if(eventTimer)
		{
			RemoveEventLoopTimer (eventTimer);
			eventTimer = 0;
		}

		#endif
	}
}

//------------------------------------------------------------------------
void CParameter::checkUpdates ()
{
	LIST_FOREACH (CParameter, p, theParameters)
		if(p->mustUpdate ())
			p->updateDependents ();
	LIST_ENDFOR
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  CPlugParams
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CPlugParams::CPlugParams (IPlugController* controller)
: controller (controller)
{}

//-----------------------------------------------------------------------------
CParameter* CPlugParams::add (CParameter* p, long tag, const char* name)
{
	if(controller)
		p->connect (controller, tag);

	params.append (new CParamEntry (p, name));
	return p;
}

//-----------------------------------------------------------------------------
CParameter* CPlugParams::addParam (long tag, const char* name)
{
	return add (new CParameter, tag, name);
}

//-----------------------------------------------------------------------------
CIntParameter* CPlugParams::addIntParam (long tag, const char* name, long min, long max)
{
	return (CIntParameter*)add (new CIntParameter (min, max), tag, name);
}

//-----------------------------------------------------------------------------
CStringParameter* CPlugParams::addStringParam (long tag, const char* name)
{
	return (CStringParameter*)add (new CStringParameter, tag, name);
}

//-----------------------------------------------------------------------------
CStringListParameter* CPlugParams::addListParam (long tag, const char* name, const char** strings)
{
	return (CStringListParameter*)add (new CStringListParameter (strings), tag, name);
}

//-----------------------------------------------------------------------------
long CPlugParams::total () const
{
	return params.total ();
}

//-----------------------------------------------------------------------------
CParameter* CPlugParams::at (long index) const
{
	CParamEntry* pe = (CParamEntry*)params.at (index);
	return pe ? pe->param : 0;
}

//-----------------------------------------------------------------------------
bool CPlugParams::getName (long index, char str[128]) const
{
	CParamEntry* pe = (CParamEntry*)params.at (index);
	if(pe)
	{
		strncpy (str, pe->name, 128);
		str[127] = 0;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
CParameter* CPlugParams::getParameter (const char* name) const
{
	LIST_FOREACH (CParamEntry, pe, params)
		if(!strcmp (pe->name, name))
			return pe->param;
	LIST_ENDFOR
	return 0;
}

//-----------------------------------------------------------------------------
CParameter* CPlugParams::getParameter (long tag) const
{
	LIST_FOREACH (CParamEntry, pe, params)
		if(pe->param->getTag () == tag)
			return pe->param;
	LIST_ENDFOR
	return 0;
}

//-----------------------------------------------------------------------------
//  CParameter
//-----------------------------------------------------------------------------

CParameter::CParameter ()
: tag (-1),
  flags (0),
  target (0),
  dependents (0)
{
	FUNKNOWN_CTOR

	theParameters.append (this);
}

//-----------------------------------------------------------------------------
CParameter::~CParameter ()
{
	theParameters.remove (this);

	if(dependents)
		delete dependents;

	FUNKNOWN_DTOR
}

//-----------------------------------------------------------------------------
void CParameter::enableUpdates (bool state)
{
	if(state)
		flags &= ~kNoUpdate;
	else
		flags |= kNoUpdate;
}

//-----------------------------------------------------------------------------
void CParameter::initValue (long value)
{
	IPlugController* t = target;
	target = 0;
	setValue (value);
	target = t;
}

//-----------------------------------------------------------------------------
void CParameter::initString (const char* string)
{
	IPlugController* t = target;
	target = 0;
	fromString (string);
	target = t;
}

//-----------------------------------------------------------------------------
float CParameter::getNormalized ()
{
	float range = (float)(getMaxValue () - getMinValue ());
	if(range == 0.f)
		return 0.f;
	return (float)(getValue () - getMinValue ()) / range;
}

//-----------------------------------------------------------------------------
void CParameter::setNormalized (float value)
{
	setValue (getMinValue () + (long)((float)(getMaxValue () - getMinValue ()) * value));
}

//-----------------------------------------------------------------------------
IMPLEMENT_REFCOUNT (CParameter)

//-----------------------------------------------------------------------------
tresult PLUGIN_API CParameter::queryInterface (const char* iid, void** obj)
{
	QUERY_INTERFACE (iid, obj, ::FUnknown::iid, IParameter)
	QUERY_INTERFACE (iid, obj, ::IParameter::iid, IParameter)
	QUERY_INTERFACE (iid, obj, ::IDependencies::iid, IDependencies)
	QUERY_INTERFACE (iid, obj, ::IDependencies2::iid, IDependencies2)
	*obj = 0;
	return kNoInterface;
}

//-----------------------------------------------------------------------------
void PLUGIN_API CParameter::toString (char str[128])
{
	strcpy (str, flags & kOn ? "On" : "Off");
}

//-----------------------------------------------------------------------------
void PLUGIN_API CParameter::fromString (const char* str)
{
	setValue (!strcmp (str, "Off") ? 0 : 1);
}

//-----------------------------------------------------------------------------
long PLUGIN_API CParameter::getValue ()
{
	return (flags & kOn) != 0;
}

//-----------------------------------------------------------------------------
void PLUGIN_API CParameter::setValue (long value)
{
	if(value != getValue ())
	{
		if(value)
			flags |= kOn;
		else
			flags &= ~kOn;

		if(target && canUpdate ())
			target->parameterChanged (this, tag);

		deferUpdate ();
	}
}

//-----------------------------------------------------------------------------
long PLUGIN_API CParameter::getMinValue ()
{
	return 0;
}

//-----------------------------------------------------------------------------
long PLUGIN_API CParameter::getMaxValue ()
{
	return 1;
}

//-----------------------------------------------------------------------------
void PLUGIN_API CParameter::connect (IPlugController* _target, long _tag)
{
	target = _target;
	tag = _tag;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API CParameter::addDependent (FUnknown* unk)
{
	IPlugController* c = 0;
	if(unk) unk->queryInterface (IPlugController::iid, (void**)&c);
	if(!c)
		return kInvalidArgument;

	if(!dependents)
		dependents = new CLinkedList;

	dependents->append (new CDependency (c));
	return kResultOk;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API CParameter::removeDependent (FUnknown* unk)
{
	IPlugController* c = 0;
	if(unk) unk->queryInterface (IPlugController::iid, (void**)&c);
	if(!c)
		return kInvalidArgument;

	tresult result = kResultFalse;
	if(dependents)
		LIST_FOREACH (CDependency, d, *dependents)
			if(d->controller == c)
			{
				dependents->remove (d);
				result = kResultOk;
				break;
			}
		LIST_ENDFOR

	c->release ();
	return result;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API CParameter::setUpdateFlag (long mask, bool state)
{
	if(mask == IDependencies2::kDoUpdate)
		enableUpdates (state);
	return kResultOk;
}

//-----------------------------------------------------------------------------
void CParameter::updateDependents ()
{
	if(dependents)
		LIST_FOREACH (CDependency, d, *dependents)
			d->controller->parameterChanged (this, tag);
		LIST_ENDFOR

	flags &= ~kDirty;
}

//-----------------------------------------------------------------------------
//  CIntParameter
//-----------------------------------------------------------------------------

CIntParameter::CIntParameter (long min, long max)
: value (min),
  min (min),
  max (max)
{}

//-----------------------------------------------------------------------------
void PLUGIN_API CIntParameter::toString (char str[128])
{
	sprintf (str, "%d", value);
}

//-----------------------------------------------------------------------------
void PLUGIN_API CIntParameter::fromString (const char* str)
{
	long v;
	if(sscanf (str, "%d", &v) == 1)
		setValue (v);
}

//-----------------------------------------------------------------------------
void PLUGIN_API CIntParameter::setValue (long v)
{
	if(v < min)
		v = min;
	else if(v > max)
		v = max;

	if(v != value)
	{
		value = v;

		if(target && canUpdate ())
			target->parameterChanged (this, tag);

		deferUpdate ();
	}
}

//-----------------------------------------------------------------------------
//  CStringParameter
//-----------------------------------------------------------------------------

CStringParameter::CStringParameter ()
{
	*string = 0;
}

//-----------------------------------------------------------------------------
void PLUGIN_API CStringParameter::toString (char str[128])
{
	strcpy (str, string);
}

//-----------------------------------------------------------------------------
void PLUGIN_API CStringParameter::fromString (const char* str)
{
	if(str && strcmp (string, str))
	{
		strncpy (string, str, 128);
		string[127] = 0;

		if(target && canUpdate ())
			target->parameterChanged (this, tag);

		deferUpdate ();
	}
}

//-----------------------------------------------------------------------------
//  CStringListParameter
//-----------------------------------------------------------------------------

CStringListParameter::CStringListParameter (const char** _strings)
: value (-1)
{
	if(_strings)
		addStrings (_strings);
}

//-----------------------------------------------------------------------------
void CStringListParameter::addString (const char* string)
{
	strings.append (new CParamString (string));
	deferUpdate ();
}

//-----------------------------------------------------------------------------
void CStringListParameter::addStrings (const char** _strings)
{
	for(; _strings && *_strings; _strings++)
		strings.append (new CParamString (*_strings));
	deferUpdate ();
}

//-----------------------------------------------------------------------------
bool CStringListParameter::replaceAt (long index, const char* string)
{
	CParamString* pstr = (CParamString*)strings.at (index);
	if(pstr && string && strcmp (pstr->get (), string))
	{
		pstr->set (string);
		deferUpdate ();
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
bool CStringListParameter::removeAt (int index)
{
	CParamString* pstr = (CParamString*)strings.at (index);
	if(pstr)
	{
		strings.remove (pstr);
		delete pstr;

		if(value >= strings.total ())
			value = strings.total () - 1;

		deferUpdate ();
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
void CStringListParameter::removeAll ()
{
	strings.removeAll ();
	value = -1;
	deferUpdate ();
}

//-----------------------------------------------------------------------------
void PLUGIN_API CStringListParameter::toString (char str[128])
{
	CParamString* pstr = value >= 0 ? (CParamString*)strings.at (value) : 0;
	if(pstr)
		strcpy (str, pstr->get ());
}

//-----------------------------------------------------------------------------
void PLUGIN_API CStringListParameter::fromString (const char* str)
{
	if(!str || (str && !*str))
		return;

	long i = 0;
	LIST_FOREACH (CParamString, pstr, strings)
		if(!strcmp (pstr->get (), str))
		{
			setValue (i);
			break;
		}
		i++;
	LIST_ENDFOR
}

//-----------------------------------------------------------------------------
void PLUGIN_API CStringListParameter::setValue (long v)
{
	if(v < 0)
		v = 0;
	else if(v >= strings.total ())
		v = strings.total () - 1;

	if(v != value)
	{
		value = v;

		if(target && canUpdate ())
			target->parameterChanged (this, tag);

		deferUpdate ();
	}
}
