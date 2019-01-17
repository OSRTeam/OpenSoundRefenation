//-----------------------------------------------------------------------------
// VST Module Architecture SDK
// Version 1.0    Date : 01/2004
//
// Project      : Common Base Classes
// Filename     : linkedlist.h
// Created by   : Matthias Juwan
// Description  : Double-linked List
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

#ifndef __linkedlist__
#define __linkedlist__

//------------------------------------------------------------------------
#define LIST_FOREACH(Type, obj, list) \
{for(Type* obj = (Type*)(list).getRoot (); obj; obj = (Type*)obj->getNext ()) {

#define LIST_FOREACH_REVERSE(Type, obj, list) \
{for(Type* obj = (Type*)(list).getLast (); obj; obj = (Type*)obj->getPrev ()) {

#define LIST_ENDFOR }}

//------------------------------------------------------------------------
//  CListElement class declaration
//------------------------------------------------------------------------
class CListElement
{
public:
//------------------------------------------------------------------------
	CListElement (): next (0), prev (0) {}
	virtual ~CListElement ()  {}

	CListElement* getNext () const { return next; }
	CListElement* getPrev () const { return prev; }
//------------------------------------------------------------------------
protected:
	friend class CLinkedList;

	CListElement* next;
	CListElement* prev;
};

//------------------------------------------------------------------------
//  CLinkedList class declaration
//------------------------------------------------------------------------
class CLinkedList
{
public:
//------------------------------------------------------------------------
	CLinkedList (): root (0), count (0) {}
	virtual ~CLinkedList () { removeAll (); }

	void append (CListElement* e);
	void prepend (CListElement* e);
	bool remove (CListElement* e);
	bool insertBefore (CListElement* before, CListElement* newElement);
	void removeAll ();

	long total () const { return count; }
	CListElement* at (long index) const;
	bool contains (CListElement* e) const;
	long indexOf (CListElement* e) const;

	CListElement* getRoot () const { return root; }
	CListElement* getLast () const;
//------------------------------------------------------------------------
protected:
	CListElement* root;
	long count;
};

#endif