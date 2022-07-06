#pragma once

#include "Action.h"

class Settings
{
public:
	[[nodiscard]] static Settings* GetSingleton()
	{
		static Settings singleton;
		return std::addressof(singleton);
	}

	[[nodiscard]] Action* GetActionForType(RE::FormType a_type);
	[[nodiscard]] Key GetHotkey() const;

	[[nodiscard]] bool GetHotkeyPressed() const;
	void SetHotkeyPressed(bool a_pressed);

	[[nodiscard]] bool GetHotkeyHeld() const;
	void SetHotkeyHeld(bool a_held);

	[[nodiscard]] float GetKeyHeldDuration() const;

private:
	Settings();
	Settings(const Settings&) = delete;
	Settings(Settings&&) = delete;

	~Settings() = default;

	Settings& operator=(const Settings&) = delete;
	Settings& operator=(Settings&&) = delete;

	Action armors;
	SecondaryAction weapons;
	AlchemyAction alchemy;
	Action ingestibles;
	SecondaryAction scrolls;

	Key hotKey;
	bool keyPressed;
	bool keyHeld;
	float keyHeldDuration;
};
