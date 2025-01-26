#pragma once

#include "Action.h"

class Manager :
	public REX::Singleton<Manager>,
	public RE::BSTEventSink<RE::InputEvent*>
{
public:
	void        LoadSettings();
	static void Register();

	[[nodiscard]] Action* GetActionForType(RE::FormType a_type);
	[[nodiscard]] Key     GetHotkey() const;

	[[nodiscard]] bool GetHotkeyPressed() const;
	void               SetHotkeyPressed(bool a_pressed);

	[[nodiscard]] bool GetHotkeyHeld() const;
	void               SetHotkeyHeld(bool a_held);

	[[nodiscard]] float GetKeyHeldDuration() const;

private:
	static void UpdateCrosshairs();

	RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_evn, RE::BSTEventSource<RE::InputEvent*>*) override;

	// members
	Action          armors;
	SecondaryAction weapons;
	AlchemyAction   alchemy;
	Action          ingestibles;
	SecondaryAction scrolls;
	Action          torches;
	Action          ammo;

	Key   hotKey{ 42 };
	float keyHeldDuration{ 0.7f };

	std::atomic_bool keyPressed{ false };
	std::atomic_bool keyHeld{ false };
};
