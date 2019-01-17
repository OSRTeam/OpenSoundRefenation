#pragma once
#include "discord-presence/include/discord_register.h"
#include "discord-presence/include/discord_rpc.h"
#include <time.h>

class DiscordNetwork
{
public:
	STRING128 szName;
	STRING128 szString;

	enum class StatusNumber
	{
		Waiting,
		OpenedAudio,
		PlayingAudio,
		Processing
	};

	void Init()
	{
		memset(szName, 0, 128);
		memset(szString, 0, 128);
		DiscordEventHandlers DiscordHandler = { nullptr };
		Discord_Initialize("519951399023280149", &DiscordHandler, (int)true, nullptr);
		bInit = true;
	}

	void Destroy()
	{
		if (bInit)
		{
			Discord_ClearPresence();
			Discord_Shutdown();
			bInit = false;
		}
	}

	void SetStatus(StatusNumber Status)
	{
		if (!bInit) { return; }

		DiscordRichPresence PresenceInfo = { nullptr };
		StatusNumber CStatus = Status;

		PresenceInfo.startTimestamp = time(nullptr);
		PresenceInfo.largeImageText = "Digital Workstation";
		PresenceInfo.smallImageText = "Version: OSR 0.46a";

		bool bOnLevel = true;

#pragma execution_character_set("utf-8")
		switch (CStatus)
		{
		case StatusNumber::OpenedAudio: 
			PresenceInfo.smallImageKey = "open";
			_snprintf_s(szString, 128, "%s%s", u8"Opened Track: ", szName);
			PresenceInfo.details = szString;
			break;
		case StatusNumber::PlayingAudio: 
			PresenceInfo.smallImageKey = "play";
			_snprintf_s(szString, 128, "%s%s", u8"Playing Track: ", szName);
			PresenceInfo.details = szString;
			break;
		case StatusNumber::Processing: 
			PresenceInfo.smallImageKey = "prcs";
			_snprintf_s(szString, 128, "%s%s", u8"Processing Track: ", szName);
			PresenceInfo.details = szString;
			break;
		case StatusNumber::Waiting: 
			PresenceInfo.smallImageKey = "wait";
			PresenceInfo.details = u8"Waiting for audio";
			break;
		}	
		
		PresenceInfo.largeImageKey = "osrlogo";

		Discord_UpdatePresence(&PresenceInfo);
	}

	~DiscordNetwork()
	{
		Destroy();
	}

private:
	bool bInit;

};

