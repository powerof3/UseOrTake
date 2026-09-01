#pragma once

#include "Action.h"

class Manager :
	public REX::TSingleton<Manager>,
	public RE::BSTEventSink<RE::InputEvent*>
{
public:
	void        LoadSettings();
	static void Register();

	[[nodiscard]] Action* GetActionForType(RE::FormType a_type);

	[[nodiscard]] Key GetHotkey() const { return static_cast<Key>(hotKey.GetValue()); }
	[[nodiscard]] Key GetHotkeyGamePad() const { return static_cast<Key>(hotKeyGamePad.GetValue()); }

	[[nodiscard]] bool GetHotkeyPressed() const { return keyPressed; }
	void               SetHotkeyPressed(bool a_pressed) { keyPressed = a_pressed; }

	[[nodiscard]] bool GetHotkeyHeld() const { return keyHeld; }
	void               SetHotkeyHeld(bool a_held) { keyHeld = a_held; }

	[[nodiscard]] float GetKeyHeldDuration() const { return keyHeldDuration; }

private:
	static void UpdateCrosshairs();

	RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_evn, RE::BSTEventSource<RE::InputEvent*>*) override;

	// members
	static inline constexpr auto path = R"(Data\SKSE\Plugins\po3_UseOrTake.ini)"sv;

	Action          armors{ "Armors"sv, "Equip" };
	SecondaryAction weapons{ "Weapons"sv, "Equip", "Equip and Draw" };
	AlchemyAction   alchemy{ "Potions"sv, "Drink", "Eat", "Apply" };
	Action          ingestibles{ "Ingredients"sv, "Eat" };
	SecondaryAction scrolls{ "Scrolls"sv, "Equip", "Read" };
	Action          torches{ "Torches"sv, "Equip" };
	Action          ammo{ "Ammo"sv, "Equip" };

	REX::TIniSetting<std::uint32_t> hotKey{ "Settings", "Alternate action hotkey", 42 };
	REX::TIniSetting<std::uint32_t> hotKeyGamePad{ "Settings", "Alternate action hotkey (Gamepad)", 0 };
	REX::TIniSetting<float>         keyHeldDuration{ "Settings", "Hotkey hold duration", 0.7f };

	std::atomic_bool keyPressed{ false };
	std::atomic_bool keyHeld{ false };
};
