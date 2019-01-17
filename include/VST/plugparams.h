//-----------------------------------------------------------------------------
// VST Module Architecture SDK
// Version 1.0    Date : 01/2004
//
// Project      : Common Base Classes
// Filename     : plugparams.h
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
#define __plugparams__

//-----------------------------------------------------------------------------
// SDK Interface Headers
#ifndef __iplugparams__
#include "gui/iplugparams.h"
#endif
//-----------------------------------------------------------------------------

#ifndef __linkedlist__
#include "linkedlist.h"
#endif

//-----------------------------------------------------------------------------
// forward declaration of parameter classes:
class CParameter;
class CIntParameter;
class CStringParameter;
class CStringListParameter;
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  CPlugParams : parameter container
//-----------------------------------------------------------------------------
class CPlugParams
{
public:
//-----------------------------------------------------------------------------
	CPlugParams (IPlugController* controller = 0);
	virtual ~CPlugParams () {}
	void init (IPlugController* c) { controller = c; }

//-----------------------------------------------------------------------------
	CParameter* add (CParameter* p, long tag, const char* name);

	// add typed parameter:
	CParameter*				addParam		(long tag, const char* name);
	CIntParameter*			addIntParam		(long tag, const char* name, long min = 0, long max = 1);
	CStringParameter*		addStringParam	(long tag, const char* name);
	CStringListParameter*	addListParam	(long tag, const char* name, const char** strings = 0);

//-----------------------------------------------------------------------------
	// parameter access:
	long total () const;
	CParameter* at (long index) const;
	bool getName (long index, char str[128]) const;

	CParameter* getParameter (const char* name) const;
	CParameter* getParameter (long tag) const;
//-----------------------------------------------------------------------------
protected:
	IPlugController* controller;
	CLinkedList params;
};

//-----------------------------------------------------------------------------
//  CParameter : parameter base class (on/off)
//-----------------------------------------------------------------------------
class CParameter:	public IParameter,
					public IDependencies2,
					private CListElement
{
public:
//-----------------------------------------------------------------------------
	CParameter ();
	virtual ~CParameter ();

//-----------------------------------------------------------------------------
	long getTag () const { return tag; }

	void enableUpdates (bool state); // update dependent controllers?

	// set parameter value w/o target update
	void initValue (long value);
	void initString (const char* string);

	float getNormalized ();
	void setNormalized (float value);

//-----------------------------------------------------------------------------
	// Interface methods:
	DECLARE_FUNKNOWN_METHODS

	void PLUGIN_API toString (char str[128]);
	void PLUGIN_API fromString (const char* str);

	long PLUGIN_API getValue ();
	void PLUGIN_API setValue (long value);
	long PLUGIN_API getMinValue ();
	long PLUGIN_API getMaxValue ();

	void PLUGIN_API connect (IPlugController*, long tag);

	tresult PLUGIN_API addDependent (FUnknown*);
	tresult PLUGIN_API removeDependent (FUnknown*);
	tresult PLUGIN_API setUpdateFlag (long mask, bool state);

//-----------------------------------------------------------------------------
	static void beginUpdates ();
	static void checkUpdates ();
	static void endUpdates ();
//-----------------------------------------------------------------------------
protected:
	long tag;
	long flags;
	IPlugController* target;
	CLinkedList* dependents;

	enum { kOn = 1<<0, kDirty = 1<<1, kNoUpdate = 1<<2 };

	bool canUpdate ()	{ return (flags & kNoUpdate) == 0; }
	void deferUpdate ()	{ if(canUpdate ()) flags |= kDirty; }
	bool mustUpdate ()	{ return (flags & kDirty) != 0; }
	void updateDependents ();
};

//-----------------------------------------------------------------------------
//  CIntParameter : integer parameter class
//-----------------------------------------------------------------------------
class CIntParameter: public CParameter
{
public:
//-----------------------------------------------------------------------------
	CIntParameter (long min = 0, long max = 1);

//-----------------------------------------------------------------------------
	// CParameter overrides:
	void PLUGIN_API toString (char str[128]);
	void PLUGIN_API fromString (const char* str);
	void PLUGIN_API setValue (long value);

	long PLUGIN_API getValue ()		{ return value; }
	long PLUGIN_API getMinValue ()	{ return min; }
	long PLUGIN_API getMaxValue ()	{ return max; }
//-----------------------------------------------------------------------------
protected:
	long value;
	long min;
	long max;
};

//-----------------------------------------------------------------------------
//  CStringParameter : string parameter class
//-----------------------------------------------------------------------------
class CStringParameter: public CParameter
{
public:
//-----------------------------------------------------------------------------
	CStringParameter ();

//-----------------------------------------------------------------------------
	// CParameter overrides:
	void PLUGIN_API toString (char str[128]);
	void PLUGIN_API fromString (const char* str);
//-----------------------------------------------------------------------------
protected:
	char string[128];
};

//-----------------------------------------------------------------------------
//  CStringListParameter : string list parameter class
//-----------------------------------------------------------------------------
class CStringListParameter: public CParameter
{
public:
//-----------------------------------------------------------------------------
	CStringListParameter (const char** strings = 0);

//-----------------------------------------------------------------------------
	void addString (const char* string);
	void addStrings (const char** strings); // string array must be null-terminated!
	bool replaceAt (long index, const char* string);
	bool removeAt (int index);
	void removeAll ();

//-----------------------------------------------------------------------------
	// CParameter overrides:
	void PLUGIN_API toString (char str[128]);
	void PLUGIN_API fromString (const char* str);
	void PLUGIN_API setValue (long value);

	long PLUGIN_API getValue ()		{ return value; }
	long PLUGIN_API getMinValue ()	{ return 0; }
	long PLUGIN_API getMaxValue ()	{ return strings.total () - 1; }
//-----------------------------------------------------------------------------
protected:
	long value;
	CLinkedList strings;
};

#endif