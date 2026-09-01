#include "Manager.h"

void Manager::LoadSettings()
{
	const auto store = REX::FIniSettingStore::GetSingleton();
	store->Init(path.data(), "");

	store->Load();
	store->Save();
}

void Manager::Register()
{
	REX::INFO("{:*^30}", "EVENTS");

	if (const auto inputMgr = RE::BSInputDeviceManager::GetSingleton()) {
		inputMgr->AddEventSink(GetSingleton());

		REX::INFO("Registered for hotkey event");
	}
}

Action* Manager::GetActionForType(RE::FormType a_type)
{
	switch (a_type) {
	case RE::FormType::Scroll:
		return &scrolls;
	case RE::FormType::Armor:
		return &armors;
	case RE::FormType::Ingredient:
		return &ingestibles;
	case RE::FormType::Weapon:
		return &weapons;
	case RE::FormType::AlchemyItem:
		return &alchemy;
	case RE::FormType::Light:
		return &torches;
	case RE::FormType::Ammo:
		return &ammo;
	default:
		return nullptr;
	}
}

void Manager::UpdateCrosshairs()
{
	if (const auto crossHairPickData = RE::CrosshairPickData::GetSingleton()) {
		const auto target = crossHairPickData->target.get();
		const auto base = target ? target->GetBaseObject() : nullptr;
		const auto action = base ? Manager::GetSingleton()->GetActionForType(base->GetFormType()) : nullptr;

		if (action && action->IsEnabled()) {
			RE::PlayerCharacter::GetSingleton()->UpdateCrosshairs();
		}
	}
}

RE::BSEventNotifyControl Manager::ProcessEvent(RE::InputEvent* const* a_evn, RE::BSTEventSource<RE::InputEvent*>*)
{
	using InputType = RE::INPUT_EVENT_TYPE;

	if (!a_evn) {
		return RE::BSEventNotifyControl::kContinue;
	}

	const auto player = RE::PlayerCharacter::GetSingleton();
	if (!player || !player->Is3DLoaded()) {
		return RE::BSEventNotifyControl::kContinue;
	}

	if (const auto UI = RE::UI::GetSingleton(); !UI || UI->IsMenuOpen(RE::Console::MENU_NAME) || UI->GameIsPaused()) {
		return RE::BSEventNotifyControl::kContinue;
	}

	for (auto event = *a_evn; event; event = event->next) {
		if (const auto buttonEvent = event->AsButtonEvent()) {
			const auto device = event->GetDevice();
			auto       key = buttonEvent->GetIDCode();

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

			if (key == GetHotkey() || (device == RE::INPUT_DEVICE::kGamepad && key == GetHotkeyGamePad())) {
				if (GetHotkeyPressed() != buttonEvent->IsPressed()) {
					SetHotkeyPressed(buttonEvent->IsPressed());

					if (!buttonEvent->IsPressed()) {
						SetHotkeyHeld(false);
					}

					UpdateCrosshairs();
				} else if (GetHotkeyPressed() && !GetHotkeyHeld() && buttonEvent->HeldDuration() > GetKeyHeldDuration()) {
					SetHotkeyHeld(true);

					UpdateCrosshairs();
				} else if (!buttonEvent->IsPressed()) {
					SetHotkeyPressed(false);
					SetHotkeyHeld(false);
				}
			}
		}
	}

	return RE::BSEventNotifyControl::kContinue;
}
