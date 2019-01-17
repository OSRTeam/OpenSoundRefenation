//-----------------------------------------------------------------------------
// VST Module Architecture SDK
// Version 1.0    Date : 01/2004
//
// Category     : SDK Core Interfaces
// Filename     : funknown.h
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
#define __funknown__

#include <string>

//------------------------------------------------------------------------
//  platform switches
//------------------------------------------------------------------------

#undef WINDOWS
#undef MAC
#undef MOTIF

#if defined (WIN32) || defined (__FLAT__)
	#define WINDOWS 1
	#define WIN32_LEAN_AND_MEAN 1
	#define COM_COMPATIBLE 1
#elif __UNIX__
	#define MOTIF 1
#else
	#define MAC 1
	#if __MACH__
		#define PTHREADS 1
	#else
		#define MPLIB 1
	#endif
#endif

//------------------------------------------------------------------------
#undef PLUGIN_API
#if COM_COMPATIBLE
	#define PLUGIN_API __stdcall
#else
	#define PLUGIN_API
#endif

//------------------------------------------------------------------------
#if WINDOWS
	typedef __int64 LARGE_INT;
#else
	typedef long long LARGE_INT;
#endif

//------------------------------------------------------------------------
//  Unique Identifier macros
//------------------------------------------------------------------------

#if COM_COMPATIBLE
#define INLINE_UID(l1, l2, l3, l4) \
{ \
	(char)((l1 & 0x000000FF)      ), (char)((l1 & 0x0000FF00) >>  8), \
	(char)((l1 & 0x00FF0000) >> 16), (char)((l1 & 0xFF000000) >> 24), \
	(char)((l2 & 0x00FF0000) >> 16), (char)((l2 & 0xFF000000) >> 24), \
	(char)((l2 & 0x000000FF)      ), (char)((l2 & 0x0000FF00) >>  8), \
	(char)((l3 & 0xFF000000) >> 24), (char)((l3 & 0x00FF0000) >> 16), \
	(char)((l3 & 0x0000FF00) >>  8), (char)((l3 & 0x000000FF)      ), \
	(char)((l4 & 0xFF000000) >> 24), (char)((l4 & 0x00FF0000) >> 16), \
	(char)((l4 & 0x0000FF00) >>  8), (char)((l4 & 0x000000FF)      )  \
}
#else
#define INLINE_UID(l1, l2, l3, l4) \
{ \
	(char)((l1 & 0xFF000000) >> 24), (char)((l1 & 0x00FF0000) >> 16), \
	(char)((l1 & 0x0000FF00) >>  8), (char)((l1 & 0x000000FF)      ), \
	(char)((l2 & 0xFF000000) >> 24), (char)((l2 & 0x00FF0000) >> 16), \
	(char)((l2 & 0x0000FF00) >>  8), (char)((l2 & 0x000000FF)      ), \
	(char)((l3 & 0xFF000000) >> 24), (char)((l3 & 0x00FF0000) >> 16), \
	(char)((l3 & 0x0000FF00) >>  8), (char)((l3 & 0x000000FF)      ), \
	(char)((l4 & 0xFF000000) >> 24), (char)((l4 & 0x00FF0000) >> 16), \
	(char)((l4 & 0x0000FF00) >>  8), (char)((l4 & 0x000000FF)      )  \
}
#endif

//------------------------------------------------------------------------
#define DECLARE_UID(name, l1, l2, l3, l4) \
	const char name[16] = INLINE_UID (l1, l2, l3, l4);

//------------------------------------------------------------------------
#define EXTERN_UID(name) \
	extern const char name[16];

//------------------------------------------------------------------------
#ifdef INIT_CLASS_IID
#define DECLARE_CLASS_IID(ClassName, l1, l2, l3, l4) const FUID ClassName::iid (l1, l2, l3, l4);
#else
#define DECLARE_CLASS_IID(ClassName, l1, l2, l3, l4)
#endif

#define DEFINE_CLASS_IID(ClassName, l1, l2, l3, l4) const FUID ClassName::iid (l1, l2, l3, l4);

//------------------------------------------------------------------------
//  FUnknown implementation macros
//------------------------------------------------------------------------

#define DECLARE_FUNKNOWN_METHODS                                             \
public:	                                                                     \
	virtual tresult PLUGIN_API queryInterface (const char* iid, void** obj); \
	virtual unsigned long PLUGIN_API addRef ();                              \
	virtual unsigned long PLUGIN_API release ();                             \
protected :                                                                  \
	unsigned long __funknownRefCount;                                        \
public:

//------------------------------------------------------------------------
#define IMPLEMENT_REFCOUNT(ClassName)          \
unsigned long PLUGIN_API ClassName::addRef ()  \
{                                              \
	return ++__funknownRefCount;               \
}                                              \
unsigned long PLUGIN_API ClassName::release () \
{                                              \
	if(--__funknownRefCount == 0)              \
	{                                          \
		delete this;                           \
		return 0;                              \
	}                                          \
	return __funknownRefCount;                 \
}

//------------------------------------------------------------------------
#define FUNKNOWN_CTOR	{ __funknownRefCount = 1; ::FUnknown::addObject (); }
#define FUNKNOWN_DTOR	{ ::FUnknown::releaseObject (); }

//------------------------------------------------------------------------
#define QUERY_INTERFACE(iid, obj, InterfaceIID, InterfaceName) \
if(memcmp (iid, InterfaceIID, 16) == 0)                        \
{                                                              \
	addRef ();                                                 \
	*obj = (InterfaceName*)this;                               \
	return kResultOk;                                          \
}

//------------------------------------------------------------------------
#define IMPLEMENT_QUERYINTERFACE(ClassName, InterfaceName, ClassIID)       \
tresult PLUGIN_API ClassName::queryInterface (const char* iid, void** obj) \
{                                                                          \
	QUERY_INTERFACE (iid, obj, ::FUnknown::iid, InterfaceName)             \
	QUERY_INTERFACE (iid, obj, ClassIID, InterfaceName)                    \
	*obj = 0;                                                              \
	return kNoInterface;                                                   \
}

//------------------------------------------------------------------------
#define IMPLEMENT_FUNKNOWN_METHODS(ClassName,InterfaceName,ClassIID) \
	IMPLEMENT_REFCOUNT (ClassName)                                   \
	IMPLEMENT_QUERYINTERFACE (ClassName, InterfaceName, ClassIID)

//------------------------------------------------------------------------
//  Result Codes
//------------------------------------------------------------------------

typedef long tresult;

//------------------------------------------------------------------------
#if COM_COMPATIBLE
#if WINDOWS
enum
{
	kNoInterface		= 0x80004002L,		// E_NOINTERFACE
	kResultOk			= 0x00000000L,		// S_OK
	kResultTrue			= kResultOk,
	kResultFalse		= 0x00000001L,		// S_FALSE
	kInvalidArgument	= 0x80070057L,		// E_INVALIDARG
	kNotImplemented		= 0x80004001L,		// E_NOTIMPL
	kInternalError		= 0x80004005L,		// E_FAIL
	kNotInitialized		= 0x8000FFFFL,		// E_UNEXPECTED
	kOutOfMemory		= 0x8007000EL		// E_OUTOFMEMORY
};
#else
enum
{
	kNoInterface		= 0x80000004L,		// E_NOINTERFACE
	kResultOk			= 0x00000000L,		// S_OK
	kResultTrue			= kResultOk,
	kResultFalse		= 0x00000001L,		// S_FALSE
	kInvalidArgument	= 0x80000003L,		// E_INVALIDARG
	kNotImplemented		= 0x80000001L,		// E_NOTIMPL
	kInternalError		= 0x80000008L,		// E_FAIL
	kNotInitialized		= 0x8000FFFFL,		// E_UNEXPECTED
	kOutOfMemory		= 0x80000002L		// E_OUTOFMEMORY
};
#endif
#else
enum
{
	kNoInterface = -1,
	kResultOk,
	kResultTrue = kResultOk,
	kResultFalse,
	kInvalidArgument,
	kNotImplemented,
	kInternalError,
	kNotInitialized,
	kOutOfMemory
};
#endif

//------------------------------------------------------------------------
//	FUID class declaration
//------------------------------------------------------------------------

class FUID
{
public:
//------------------------------------------------------------------------
	FUID ();
	FUID (const char*);
	FUID (long l1, long l2, long l3, long l4);
	FUID (const FUID&);
	virtual ~FUID ();

	bool generate ();
	bool isValid ();

	FUID& operator = (const FUID& f);
	FUID& operator = (const char* uid);

	bool operator == (const FUID& f) const   { return memcmp (data, f.data, 16) == 0; }
	bool operator == (const char* uid) const { return memcmp (data, uid, 16) == 0; }

	bool operator != (const FUID& f) const   { return memcmp (data, f.data, 16) != 0; }
	bool operator != (const char* uid) const { return memcmp (data, uid, 16) != 0; }

	operator const char* () const { return data; }

	long getLong1 ();
	long getLong2 ();
	long getLong3 ();
	long getLong4 ();

	void toString (char* string) const;
	bool fromString (const char* string);

	void toRegistryString (char* string) const;
	bool fromRegistryString (char* string);

	enum { kINLINE_UID, kDECLARE_UID, kFUID };
	void print (char* string = 0, long style = kINLINE_UID);
//------------------------------------------------------------------------
protected:
	char data[16];
};

//------------------------------------------------------------------------
//	FUnknown interface declaration
//------------------------------------------------------------------------

class FUnknown
{
public:
//------------------------------------------------------------------------
	virtual tresult PLUGIN_API queryInterface (const char* iid, void** obj) = 0;
	virtual unsigned long PLUGIN_API addRef () = 0;
	virtual unsigned long PLUGIN_API release () = 0;
//------------------------------------------------------------------------
	static const FUID iid;

//------------------------------------------------------------------------
	// global object counting:
	static void addObject ();
	static void releaseObject ();
	static long countObjects ();
//------------------------------------------------------------------------
};

DECLARE_CLASS_IID (FUnknown, 0x00000000, 0x00000000, 0xC0000000, 0x00000046)

//------------------------------------------------------------------------
//  FReleaser
//------------------------------------------------------------------------

struct FReleaser
{
//------------------------------------------------------------------------
	FReleaser (FUnknown* u): u (u) {}
	~FReleaser () { if(u) u->release (); }

	FUnknown* u;
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
//	FUnknownPtr - smart pointer template class
//	- can be used as an I* pointer, refCounts the actual interface
//	- tries to acquire the interface when an FUnknown* is assigned
//	- example:
//			FUnknown* unknown = ...
//			FUnknownPtr<IPath> path (unknown);
//			if (path)
//				path->ascend ();
//------------------------------------------------------------------------
template <class I>
class FUnknownPtr
{
public:
//------------------------------------------------------------------------
	inline FUnknownPtr (I* iface);
	inline FUnknownPtr (FUnknown* unknown);
	inline FUnknownPtr (const FUnknownPtr&);
	inline FUnknownPtr ();
	inline ~FUnknownPtr ();

	inline I* operator=(I* iface);
	inline I* operator=(FUnknown* unknown);

	inline operator I* ()  const { return iface; }      // act as I*
	inline I* operator->() const { return iface; }      // act as I*
	inline I* getInterface () { return iface; }
//------------------------------------------------------------------------
protected:
	I* iface;
};

//------------------------------------------------------------------------
template <class I>
inline FUnknownPtr<I>::FUnknownPtr (FUnknown* unknown)
: iface (0)
{
	if (unknown && unknown->queryInterface (I::iid, (void**)&iface) != kResultOk)
		iface = 0;
}
//------------------------------------------------------------------------
template <class I>
inline FUnknownPtr<I>::FUnknownPtr (I* ptr)
: iface (ptr)
{
	if (iface)
		iface->addRef ();
}

//------------------------------------------------------------------------
template <class I>
inline FUnknownPtr<I>::FUnknownPtr (const FUnknownPtr& other)
: iface (other.iface)
{
	if (iface)
		iface->addRef ();
}

//------------------------------------------------------------------------
template <class I>
inline FUnknownPtr<I>::FUnknownPtr ()
: iface (0)
{}

//------------------------------------------------------------------------
template <class I>
inline FUnknownPtr<I>::~FUnknownPtr ()
{
	if (iface)
		iface->release ();
}

//------------------------------------------------------------------------
template <class I>
inline I* FUnknownPtr<I>::operator=(I* ptr)
{
	if (iface != ptr)
	{
		if (iface)
			iface->release ();
		iface = ptr;
		if (iface)
			iface->addRef ();
	}
	return iface;
}

//------------------------------------------------------------------------
template <class I>
inline I* FUnknownPtr<I>::operator=(FUnknown* unknown)
{
	if (iface)
		iface->release ();
	if (unknown && unknown->queryInterface (I::iid, (void**)&iface) != kResultOk)
		iface = 0;
	return iface;
}

//------------------------------------------------------------------------
//  FVariant struct declaration
//------------------------------------------------------------------------
struct FVariant
{
//------------------------------------------------------------------------
	enum
	{
		kEmpty   = 0,
		kInteger = 1<<0,
		kFloat   = 1<<1,
		kString  = 1<<2,
		kObject  = 1<<3,
		kOwner   = 1<<4
	};

//------------------------------------------------------------------------
	unsigned short type;
	union
	{
		LARGE_INT   intValue;
		double      floatValue;
		const char* string;
		FUnknown*   object;
	};

//------------------------------------------------------------------------
	// ctors
	FVariant () { memset (this, 0, sizeof(FVariant)); }
	FVariant (const FVariant& variant);

	FVariant (LARGE_INT v)     : type (kInteger), intValue (v)   {}
	FVariant (double v)        : type (kFloat),   floatValue (v) {}
	FVariant (const char* str) : type (kString),  string (str)   {}
	FVariant (FUnknown* obj)   : type (kObject),  object (obj)   {}
	~FVariant () { empty (); }

//------------------------------------------------------------------------
	FVariant& operator= (const FVariant& variant);

	inline void setInt   (LARGE_INT v)    { empty (); type = kInteger; intValue = v;   }
	inline void setFloat (double v)       { empty (); type = kFloat;   floatValue = v; }
	inline void setString (const char* v) { empty (); type = kString;  string = v;     }
	inline void setObject (FUnknown* obj) { empty (); type = kObject;  object = obj;   }

	inline LARGE_INT getInt ()		{ return (type & kInteger) ? intValue : 0; }
	inline double getFloat ()		{ return (type & kFloat) ? floatValue : 0.; }
	inline double getNumber ()		{ return (type & kInteger) ? (double)intValue : (type & kFloat) ? floatValue : 0.; }
	inline const char* getString ()	{ return (type & kString) ? string : 0; }
	inline FUnknown* getObject ()	{ return (type & kObject) ? object : 0; }

	inline unsigned short getType () const { return (type & 0x0F); }
	inline bool isEmpty () const           { return getType () == kEmpty; }
	inline bool isOwner () const           { return (type & kOwner) != 0; }

	void empty ();
//------------------------------------------------------------------------
};

#endif
