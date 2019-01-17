//-----------------------------------------------------------------------------
// VST Module Architecture SDK
// Version 1.0    Date : 01/2004
//
// Project      : Common Base Classes
// Filename     : plugxmlgui.cpp
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
#include "plugxmlgui.h"
#endif

//-----------------------------------------------------------------------------
#if WINDOWS
#include <windows.h>

extern HINSTANCE ghInst;

//-----------------------------------------------------------------------------
#elif MAC
#define __CF_USE_FRAMEWORK_INCLUDES__ 1
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

#include <sys/stat.h>
#include <sys/syslimits.h>
#include <stdio.h>
#include <stdarg.h>

extern CFBundleRef ghInst;
#endif

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool GetResource (void* loader, const CResourceID& resID, char* buffer, long* bufferSize)
{
	if(!loader)
		loader = (void*)ghInst;

	bool result = false;

#if WINDOWS
	if(resID.resId <= 0)
		return false;

	HRSRC res = FindResource ((HINSTANCE)loader, MAKEINTRESOURCE (resID.resId), RT_RCDATA);
	if(res)
	{
		DWORD resSize = SizeofResource ((HINSTANCE)loader, res);

		if(buffer)
		{
			HGLOBAL resData = LoadResource ((HINSTANCE)loader, res);
			void* ptr = LockResource (resData);
			if(ptr)
			{
				memcpy (buffer, ptr, resSize);
				result = true;
			}
		}
		else
			result = true;

		if(bufferSize)
			*bufferSize = resSize;
	}

#elif MAC
	if(!resID.resName)
		return false;

	CFStringRef resourceName = CFStringCreateWithCString (kCFAllocatorDefault, resID.resName, kCFStringEncodingMacRoman);
	if (resourceName)
	{
		static const CFStringRef resType[] = { CFSTR("xml"), CFSTR("jpg"), CFSTR("png"), CFSTR("bmp"), NULL };
		CFURLRef url = NULL;
		long i = 0;
		while (url == NULL)
		{
			url = CFBundleCopyResourceURL ((CFBundleRef)loader, resourceName, resType[i], NULL);
			if (resType[i++] == NULL)
				break;
		}
		if (url)
		{
			FSRef fsRef;
			if (CFURLGetFSRef ( url, &fsRef))
			{
				char path [PATH_MAX];
				if (FSRefMakePath (&fsRef, (unsigned char*)path, PATH_MAX) == noErr)
				{
					struct stat buf;
					if (stat (path, &buf) >= 0)
					{
						if (buffer)
						{
							FILE* file = fopen (path, "rb");
							if (file)
							{
								size_t bytes = fread (buffer, 1, buf.st_size, file);
								fclose (file);
								result = (bytes == buf.st_size);
							}
							else
								result = false;
						}
						else
							result = true;

						if (bufferSize)
							*bufferSize = buf.st_size;
					}
				}
			}
			CFRelease (url);
		}
		if (resourceName)
			CFRelease (resourceName);
	}
#endif

	return result;
}
