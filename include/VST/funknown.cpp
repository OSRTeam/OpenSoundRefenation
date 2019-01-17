//-----------------------------------------------------------------------------
// VST Module Architecture SDK
// Version 1.0    Date : 01/2004
//
// Category     : SDK Core Interfaces
// Filename     : funknown.cpp
// Created by   : Wolfgang Kundrus, Matthias Juwan
// Description  : Basic Interface
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

#ifndef __funknown__
#include "funknown.h"
#endif

#include <stdio.h>

#if WINDOWS
#include <objbase.h>
#endif

#if TARGET_API_MAC_CARBON
#include <CoreFoundation/CoreFoundation.h>
#endif

//------------------------------------------------------------------------
#if COM_COMPATIBLE
#if WINDOWS
#define GuidStruct GUID
#else
struct GuidStruct
{
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
};
#endif
#endif

static void toString (char* string, const char* data, int i1, int i2);
static void fromString (const char* string, char* data, int i1, int i2);

//------------------------------------------------------------------------
//  FUnknown static members
//------------------------------------------------------------------------

static long gNumObjects = 0;

//------------------------------------------------------------------------
void FUnknown::addObject ()
{
	gNumObjects++;
}

//------------------------------------------------------------------------
void FUnknown::releaseObject ()
{
	gNumObjects--;
}

//------------------------------------------------------------------------
long FUnknown::countObjects ()
{
	return gNumObjects;
}

//------------------------------------------------------------------------
//	FUID implementation
//------------------------------------------------------------------------

FUID::FUID ()
{
	memset (data, 0, 16);
}

//------------------------------------------------------------------------
FUID::FUID (const char* uid)
{
	if(uid)
		memcpy (data, uid, 16);
	else
		memset (data, 0, 16);
}

//------------------------------------------------------------------------
FUID::FUID (long l1, long l2, long l3, long l4)
{
#if COM_COMPATIBLE
	data [0]  = (char)((l1 & 0x000000FF)      );
	data [1]  = (char)((l1 & 0x0000FF00) >>  8);
	data [2]  = (char)((l1 & 0x00FF0000) >> 16);
	data [3]  = (char)((l1 & 0xFF000000) >> 24);
	data [4]  = (char)((l2 & 0x00FF0000) >> 16);
	data [5]  = (char)((l2 & 0xFF000000) >> 24);
	data [6]  = (char)((l2 & 0x000000FF)      );
	data [7]  = (char)((l2 & 0x0000FF00) >>  8);
	data [8]  = (char)((l3 & 0xFF000000) >> 24);
	data [9]  = (char)((l3 & 0x00FF0000) >> 16);
	data [10] = (char)((l3 & 0x0000FF00) >>  8);
	data [11] = (char)((l3 & 0x000000FF)      );
	data [12] = (char)((l4 & 0xFF000000) >> 24);
	data [13] = (char)((l4 & 0x00FF0000) >> 16);
	data [14] = (char)((l4 & 0x0000FF00) >>  8);
	data [15] = (char)((l4 & 0x000000FF)      );
#else
	data [0]  = (char)((l1 & 0xFF000000) >> 24);
	data [1]  = (char)((l1 & 0x00FF0000) >> 16);
	data [2]  = (char)((l1 & 0x0000FF00) >>  8);
	data [3]  = (char)((l1 & 0x000000FF)      );
	data [4]  = (char)((l2 & 0xFF000000) >> 24);
	data [5]  = (char)((l2 & 0x00FF0000) >> 16);
	data [6]  = (char)((l2 & 0x0000FF00) >>  8);
	data [7]  = (char)((l2 & 0x000000FF)      );
	data [8]  = (char)((l3 & 0xFF000000) >> 24);
	data [9]  = (char)((l3 & 0x00FF0000) >> 16);
	data [10] = (char)((l3 & 0x0000FF00) >>  8);
	data [11] = (char)((l3 & 0x000000FF)      );
	data [12] = (char)((l4 & 0xFF000000) >> 24);
	data [13] = (char)((l4 & 0x00FF0000) >> 16);
	data [14] = (char)((l4 & 0x0000FF00) >>  8);
	data [15] = (char)((l4 & 0x000000FF)      );
#endif
}

//------------------------------------------------------------------------
FUID::FUID (const FUID& f)
{
	memcpy (data, f.data, 16);
}

//------------------------------------------------------------------------
FUID::~FUID ()
{}

//------------------------------------------------------------------------
bool FUID::generate ()
{
#if WINDOWS
	GUID guid;
	HRESULT hr = CoCreateGuid (&guid);
	switch (hr)
	{
	case RPC_S_OK:
		memcpy (data, (char*)&guid, 16);
		return true;

	case RPC_S_UUID_LOCAL_ONLY:
	default:
		return false;
	}

#elif TARGET_API_MAC_CARBON
	CFUUIDRef uuid = CFUUIDCreate (kCFAllocatorDefault);
	if (uuid)
	{
		CFUUIDBytes bytes = CFUUIDGetUUIDBytes (uuid);
		memcpy (data, (char*)&bytes, 16);
		CFRelease (uuid);
		return true;
	}
	return false;

#else
	// implement me!
	return false;
#endif
}

//------------------------------------------------------------------------
bool FUID::isValid ()
{
	char nulluid[16];
	memset (nulluid, 0, 16);

	return memcmp (data, nulluid, 16) != 0;
}

//------------------------------------------------------------------------
FUID& FUID::operator = (const FUID& f)
{
	memcpy (data, f.data, 16);
	return *this;
}

//------------------------------------------------------------------------
FUID& FUID::operator = (const char* uid)
{
	memcpy (data, uid, 16);
	return *this;
}

//------------------------------------------------------------------------
long FUID::getLong1 ()
{
#if COM_COMPATIBLE
	return (data[0] << 0)|(data[1] << 8)|(data[2] << 16)|(data[3] << 24);
#else
	return (data[0] << 24)|(data[1] << 16)|(data[2] << 8)|(data[3] << 0);
#endif
}

//------------------------------------------------------------------------
long FUID::getLong2 ()
{
#if COM_COMPATIBLE
	return (data[4] << 16)|(data[5] << 24)|(data[6] << 0)|(data[7] << 8);
#else
	return (data[4] << 24)|(data[5] << 16)|(data[6] << 8)|(data[7] << 0);
#endif
}

//------------------------------------------------------------------------
long FUID::getLong3 ()
{
	return (data[8] << 24)|(data[9] << 16)|(data[10] << 8)|(data[11] << 0);
}

//------------------------------------------------------------------------
long FUID::getLong4 ()
{
	return (data[12] << 24)|(data[13] << 16)|(data[14] << 8)|(data[15] << 0);
}

//------------------------------------------------------------------------
void FUID::toString (char* string) const
{
	if(!string)
		return;

	#if COM_COMPATIBLE
	GuidStruct* g = (GuidStruct*)data;

	char s[17];
	::toString (s, data, 8, 16);

	sprintf (string, "%08X%04X%04X%s", g->Data1, g->Data2, g->Data3, s);
	#else
	::toString (string, data, 0, 16);
	#endif
}

//------------------------------------------------------------------------
bool FUID::fromRegistryString (char* string)
{
	if(!string || !*string)
		return false;
	if(strlen (string) != 38)
		return false;

	// e.g. {c200e360-38c5-11ce-ae62-08002b2b79ef}

	#if COM_COMPATIBLE
	GuidStruct g;
	char s[10];

	strncpy (s, string + 1, 8);
	s[8] = 0;
	sscanf (s, "%x", &g.Data1);
	strncpy (s, string + 10, 4);
	s[4] = 0;
	sscanf (s, "%x", &g.Data2);
	strncpy (s, string + 15, 4);
	s[4] = 0;
	sscanf (s, "%x", &g.Data3);
	memcpy (data, &g, 8);

	::fromString (string + 20, data, 8, 10);
	::fromString (string + 25, data, 10, 16);
	#else
	::fromString (string + 1, data, 0, 4);
	::fromString (string + 10, data, 4, 6);
	::fromString (string + 15, data, 6, 8);
	::fromString (string + 20, data, 8, 10);
	::fromString (string + 25, data, 10, 16);
	#endif

	return true;
}

//------------------------------------------------------------------------
void FUID::toRegistryString (char* string) const
{
	// e.g. {c200e360-38c5-11ce-ae62-08002b2b79ef}

	#if COM_COMPATIBLE
	GuidStruct* g = (GuidStruct*)data;

	char s1[5];
	::toString (s1, data, 8, 10);

	char s2[13];
	::toString (s2, data, 10, 16);

	sprintf (string, "{%08X-%04X-%04X-%s-%s}", g->Data1, g->Data2, g->Data3, s1, s2);
	#else
	char s1[9];
	::toString (s1, data, 0, 4);
	char s2[5];
	::toString (s2, data, 4, 6);
	char s3[5];
	::toString (s3, data, 6, 8);
	char s4[5];
	::toString (s4, data, 8, 10);
	char s5[13];
	::toString (s5, data, 10, 16);

	sprintf (string, "{%s-%s-%s-%s-%s}", s1, s2, s3, s4, s5);
	#endif
}

//------------------------------------------------------------------------
bool FUID::fromString (const char* string)
{
	if(!string || !*string)
		return false;
	if(strlen (string) != 32)
		return false;

	#if COM_COMPATIBLE
	GuidStruct g;
	char s[33];

	strcpy (s, string);
	s[8] = 0;
	sscanf (s, "%x", &g.Data1);
	strcpy (s, string + 8);
	s[4] = 0;
	sscanf (s, "%x", &g.Data2);
	strcpy (s, string + 12);
	s[4] = 0;
	sscanf (s, "%x", &g.Data3);

	memcpy (data, &g, 8);
	::fromString (string + 16, data, 8, 16);
	#else
	::fromString (string, data, 0, 16);
	#endif

	return true;
}

//------------------------------------------------------------------------
static inline long makeLong (unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4)
{
	return (long(b1) << 24) | (long(b2) << 16) | (long(b3) << 8) | long(b4);
}

//------------------------------------------------------------------------
void FUID::print (char* string, long style)
{
	if (!string) // no string: debug output
	{
		char str [128];
		print (str, style);

		#if WINDOWS
		OutputDebugString (str);
		OutputDebugString ("\n");
		#endif
		return;
	}

#if COM_COMPATIBLE
	long l1 = makeLong (data[3],  data[2],  data [1],  data [0]);
	long l2 = makeLong (data[5],  data[4],  data [7],  data [6]);
	long l3 = makeLong (data[8],  data[9],  data [10], data [11]);
	long l4 = makeLong (data[12], data[13], data [14], data [15]);
#else
	long l1 = makeLong (data[0],  data[1],  data [2],  data [3]);
	long l2 = makeLong (data[4],  data[5],  data [6],  data [7]);
	long l3 = makeLong (data[8],  data[9],  data [10], data [11]);
	long l4 = makeLong (data[12], data[13], data [14], data [15]);
#endif

	switch (style)
	{
	case kINLINE_UID:
		sprintf (string, "INLINE_UID (0x%08X, 0x%08X, 0x%08X, 0x%08X)", l1, l2, l3, l4);
		break;

	case kDECLARE_UID:
		sprintf (string, "DECLARE_UID (0x%08X, 0x%08X, 0x%08X, 0x%08X)", l1, l2, l3, l4);
		break;

	case kFUID:
		sprintf (string, "FUID (0x%08X, 0x%08X, 0x%08X, 0x%08X)", l1, l2, l3, l4);
		break;

	default:
		sprintf (string, "DECLARE_CLASS_IID (Interface, 0x%08X, 0x%08X, 0x%08X, 0x%08X)", l1, l2, l3, l4);
		break;
	}
}

//------------------------------------------------------------------------
//  helpers
//------------------------------------------------------------------------

static void toString (char* string, const char* data, int i1, int i2)
{
	*string = 0;
	for(int i = i1; i < i2; i++)
	{
		char s[3];
		sprintf (s, "%02X", (unsigned char)data[i]);
		strcat (string, s);
	}
}

//------------------------------------------------------------------------
static void fromString (const char* string, char* data, int i1, int i2)
{
	for(int i = i1; i < i2; i++)
	{
		char s[3];
		s[0] = *string++;
		s[1] = *string++;
		s[2] = 0;

		int d = 0;
		sscanf (s, "%2x", &d);
		data[i] = (char)d;
	}
}

//------------------------------------------------------------------------
//	FVariant class
//------------------------------------------------------------------------

FVariant::FVariant (const FVariant& variant)
: type (variant.type)
{
	if ((type & kString) && variant.string)
	{
		string = new char[strlen (variant.string) + 1];
		strcpy ((char*)string, variant.string);
		type |= kOwner;
	}
	else if ((type & kObject) && variant.object)
	{
		object = variant.object;
		object->addRef ();
		type |= kOwner;
	}
	else
		intValue = variant.intValue; // copy memory
}

//------------------------------------------------------------------------
void FVariant::empty ()
{
	if (type & kOwner)
	{
		if ((type & kString) && string)
			delete [] (char *)string;

		else if ((type & kObject) && object)
			object->release ();
	}
	memset (this, 0, sizeof(FVariant));
}

//------------------------------------------------------------------------
FVariant& FVariant::operator= (const FVariant& variant)
{
	empty ();

	type = variant.type;

	if ((type & kString) && variant.string)
	{
		string = new char[strlen (variant.string) + 1];
		strcpy ((char*)string, variant.string);
		type |= kOwner;
	}
	else if ((type & kObject) && variant.object)
	{
		object = variant.object;
		object->addRef ();
		type |= kOwner;
	}
	else
		intValue = variant.intValue; // copy memory

	return *this;
}
