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

	void Manager::UpdateCrosshairs()
	{
		if (const auto crossHairPickData = RE::CrosshairPickData::GetSingleton()) {
			const auto target = crossHairPickData->target.get();
			const auto base = target ? target->GetBaseObject() : nullptr;
			const auto action = base ? Settings::GetSingleton()->GetActionForType(base->GetFormType()) : nullptr;

			if (action && action->IsEnabled()) {
				RE::PlayerCharacter::GetSingleton()->UpdateCrosshairs();
			}
		}
	}

	EventResult Manager::ProcessEvent(RE::InputEvent* const* a_evn, RE::BSTEventSource<RE::InputEvent*>*)
	{
		using InputType = RE::INPUT_EVENT_TYPE;

		if (!a_evn) {
			return EventResult::kContinue;
		}

		const auto player = RE::PlayerCharacter::GetSingleton();
		if (!player || !player->Is3DLoaded()) {
			return EventResult::kContinue;
		}

		const auto UI = RE::UI::GetSingleton();

		if (!UI || UI->IsMenuOpen(RE::Console::MENU_NAME) || UI->GameIsPaused()) {
			return EventResult::kContinue;
		}

		const auto settings = Settings::GetSingleton();

		const auto hotKey = settings->GetHotkey();
		const auto keyHeldDur = settings->GetKeyHeldDuration();

		for (auto event = *a_evn; event; event = event->next) {
			if (const auto button = event->AsButtonEvent(); button) {
                const auto device = event->GetDevice();

			    auto key = button->GetIDCode();

				switch (device) {
				case RE::INPUT_DEVICE::kMouse:
					key += SKSE::InputMap::kMacro_MouseButtonOffset;
					break;
				case RE::INPUT_DEVICE::kGamepad:
					key = SKSE::InputMap::GamepadMaskToKeycode(key);
					break;
				default:
					break;
				}

			    if (key == hotKey) {
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
