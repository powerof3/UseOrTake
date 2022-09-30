#include "Events.h"
#include "Settings.h"

namespace Event
{
	Manager* Manager::GetSingleton()
	{
		static Manager singleton;
		return std::addressof(singleton);
	}

	void Manager::Register()
	{
		logger::info("{:*^30}", "EVENTS");

		if (const auto inputMgr = RE::BSInputDeviceManager::GetSingleton()) {
			inputMgr->AddEventSink(GetSingleton());

			logger::info("Registered for hotkey event");
		}
	}

	struct detail
	{
		static void UpdateCrosshairs(RE::PlayerCharacter* a_player)
		{
			using func_t = decltype(&UpdateCrosshairs);
			REL::Relocation<func_t> func{ RELOCATION_ID(39535, 40621) };
			return func(a_player);
		}
	};

	void Manager::UpdateCrosshairs()
	{
		if (const auto crossHairPickData = RE::CrosshairPickData::GetSingleton()) {
			const auto target = crossHairPickData->target.get();
			const auto base = target ? target->GetBaseObject() : nullptr;

			if (const auto action = base ? Settings::GetSingleton()->GetActionForType(base->GetFormType()) : nullptr; action && action->IsEnabled()) {
				detail::UpdateCrosshairs(RE::PlayerCharacter::GetSingleton());
			}
		}
	}

	EventResult Manager::ProcessEvent(RE::InputEvent* const* a_evn, RE::BSTEventSource<RE::InputEvent*>*)
	{
		using InputType = RE::INPUT_EVENT_TYPE;

		if (!a_evn) {
			return EventResult::kContinue;
		}

		auto player = RE::PlayerCharacter::GetSingleton();
		if (!player || !player->Is3DLoaded()) {
			return EventResult::kContinue;
		}

		const auto UI = RE::UI::GetSingleton();

		if (!UI || UI->IsMenuOpen(RE::Console::MENU_NAME)) {
			return EventResult::kContinue;
		}

		auto settings = Settings::GetSingleton();

		auto hotKey = settings->GetHotkey();
		auto keyHeldDur = settings->GetKeyHeldDuration();

		for (auto event = *a_evn; event; event = event->next) {
			if (const auto button = event->AsButtonEvent(); button) {
				if (const auto key = static_cast<Key>(button->GetIDCode()); key == hotKey) {
					if (settings->GetHotkeyPressed() != button->IsPressed()) {
						settings->SetHotkeyPressed(button->IsPressed());

						if (!settings->GetHotkeyPressed()) {
							settings->SetHotkeyHeld(false);
						}

						UpdateCrosshairs();
					}
					if (settings->GetHotkeyPressed() && !settings->GetHotkeyHeld() && button->HeldDuration() > keyHeldDur) {
						settings->SetHotkeyHeld(true);

						UpdateCrosshairs();
					}
				}
			}
		}

		return EventResult::kContinue;
	}
}
