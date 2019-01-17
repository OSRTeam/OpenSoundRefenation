# Open Sound Refenation
## Open-Source Windows DAW


## System requirements

### Minimal system: Windows 7 x64 with Platform Update
### Recommender system: Windows 10 x64 November Update


## Features

* Low percent load of CPU  '(< 10% on Intel Core i5-3470)'
* Support multiple media formats (Media Foundation, libsndfile decoder)
* Multiple outputs support (WASAPI, MME (in development), DirectSound, XAudio2 (in development))
* SSE3 vectorization by Intel intrinsics
* Fast UI drawing (D3D11 and ImGui)
* VST and VSTi (in development) plugins support
* [Discord sync](https://github.com/discordapp/discord-rpc)


## Building the project

Requires Visual Studio (2017 or later).

You can download the free [Visual Studio Community Edition](https://www.visualstudio.com/vs/community/)
to build, run or develop Open Sound Refenation.


## Additional information

Open Sound Refenation working only on AMD64 platforms. The x86-32 platforms are not supported.
ASIO Output now is not supported. 


## Installation builded versions

* Portable version: Copy all files from release archive to any folder you want.
* Full version: Open OSR installator, install application to any folder you want, set 'install Microsoft Visual C++ 2017' checkbar 
if you have not installed Microsoft Redistributable 2017.

## Thanks for

* [Discord Rich Presence](https://github.com/discordapp/discord-rpc)
* [CocoaDelay](https://github.com/tesselode/cocoa-delay)
* [Flutterbird](https://github.com/tesselode/flutterbird)
* [Airwindows](https://github.com/airwindows/airwindows)
* [RapidJSON](https://github.com/Tencent/rapidjson)
* [OBS-Studio](https://github.com/obsproject/obs-studio)
* [PortAudio](https://app.assembla.com/spaces/portaudio/git/source)
* [Libsndfile](https://github.com/erikd/libsndfile)
* [Libpng](https://github.com/glennrp/libpng)
* [Libsoxr](https://github.com/chirlu/soxr)
* [ImGui](https://github.com/ocornut/imgui)
* [Zlib](https://github.com/madler/zlib)
