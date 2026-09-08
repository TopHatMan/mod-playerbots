/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_BOTTALK_H
#define PLAYERBOTS_BOTTALK_H

#include "Define.h"
#include <map>
#include <string>
#include <string_view>

class Player;
class PlayerbotAI;

// First-class bot speech. Other mods should call this instead of rolling
// their own say/yell/party stacks.
//
// Grouped-with-player bots speak in party/raid so the player hears them.
// Nearby world bots say/yell. Combat callouts go through Speak/Callout;
// ambient chatter is Tick().
namespace BotTalk
{
enum class Channel : uint8
{
    Say = 0,
    Yell,
    Party,
    Raid,
    Guild,
    World,
    Whisper
};

enum class Callout : uint8
{
    CriticalHealth = 0,
    LowHealth,
    LowMana,
    Taunt,
    Aoe,
    Loot,
    Pull,
    Incoming
};

void LoadConfig();
bool IsEnabled();

bool Speak(Player* bot, Channel channel, std::string_view text, Player* whisperTo = nullptr);
bool SpeakLine(Player* bot, Channel channel, std::string const& textKey,
               std::map<std::string, std::string> const& placeholders = {});
bool SayCallout(Player* bot, Callout callout);
bool Tick(Player* bot, PlayerbotAI* botAI);

// Functional party/raid callouts (soulstone, brez). Not gated by BotTalk.Enable
// so raid common sense still speaks while ambient chatter stays off for the LLM.
bool RaidCall(Player* bot, std::string_view text);

Channel PreferredGroupChannel(Player* bot);
bool ShouldUseParty(Player* bot);
} // namespace BotTalk

#endif
