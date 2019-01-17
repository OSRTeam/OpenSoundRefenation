//-----------------------------------------------------------------------------
// VST Module Architecture SDK
// Version 1.0    Date : 01/2004
//
// Project      : Common Base Classes
// Filename     : linkedlist.cpp
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
#include "linkedlist.h"
#endif

//------------------------------------------------------------------------
//  CLinkedList implementation
//------------------------------------------------------------------------

void CLinkedList::append (CListElement* e)
{
	if(!root)
		root = e;
	else
	{
		CListElement* last = getLast ();
		last->next = e;
		e->prev = last;
	}

	count++;
}

//------------------------------------------------------------------------
void CLinkedList::prepend (CListElement* e)
{
	if(!root)
		root = e;
	else
	{
		e->next = root;
		root->prev = e;
		root = e;
	}

	count++;
}

//------------------------------------------------------------------------
CListElement* CLinkedList::getLast () const
{
	CListElement* e = 0;
	if(root)
		for(e = root; e->next; e = e->next);
	return e;
}

//------------------------------------------------------------------------
void CLinkedList::removeAll ()
{
	CListElement* e = root;
	while(e)
	{
		CListElement* next = e->next;
		delete e;
		e = next;
	}

	root = 0;
	count = 0;
}

//------------------------------------------------------------------------
bool CLinkedList::remove (CListElement* toRemove)
{
	for(CListElement* e = root; e; e = e->next)
		if(e == toRemove)
		{
			if(e->prev)
				e->prev->next = e->next;
			if(e->next)
				e->next->prev = e->prev;

			if(e == root)
				root = e->next;

			count--;
			return true;
		}

	return false;
}

//------------------------------------------------------------------------
CListElement* CLinkedList::at (long index) const
{
	long i = 0;
	for(CListElement* e = root; e; e = e->next, i++)
		if(i == index)
			return e;

	return 0;
}

//------------------------------------------------------------------------
bool CLinkedList::insertBefore (CListElement* before, CListElement* newElement)
{
	if(!before || !newElement || !contains (before))
		return false;

	CListElement* prev = before->prev;

	before->prev = newElement;
	newElement->next = before;
	newElement->prev = prev;

	if(prev)
		prev->next = newElement;

	if(before == root)
		root = newElement;

	count++;
	return true;
}

//------------------------------------------------------------------------
bool CLinkedList::contains (CListElement* which) const
{
	return indexOf (which) != -1;
}

//------------------------------------------------------------------------
long CLinkedList::indexOf (CListElement* which) const
{
	long i = 0;
	for(CListElement* e = root; e; e = e->next, i++)
		if(e == which)
			return i;
	return -1;
}