#include "Manager.h"

#include <SimpleIni.h>
#undef ERROR

void Manager::LoadSettings()
{
	UpdateINISettings();
	
	const auto store = REX::FIniSettingStore::GetSingleton();
	store->Init(path.data(), "");

	store->Load();
	store->Save();
}

void Manager::UpdateINISettings()
{
	CSimpleIniA ini;
	ini.SetUnicode();

	if (ini.LoadFile(path.data()) < SI_OK) {
		return;
	}

	if (ini.GetValue("Settings", "iAlternateActionKey")) {
		REX::INFO("No settings to migrate...");
		return; 
	}

	for (auto& [section, oldKey, newKey] : INI::GetSettingsToUpdate()) {
		CSimpleIniA::TNamesDepend values;
		if (ini.GetAllValues(section.data(), oldKey.data(), values) && !values.empty()) {
			const auto& entry = values.front();
			ini.SetValue(section.data(), newKey.data(), entry.pItem, entry.pComment);
			ini.Delete(section.data(), oldKey.data(), true);
			REX::INFO("Migrated [{}] {} -> {}", section, oldKey, newKey);
		}
	}

	(void)ini.SaveFile(path.data());
}

void Manager::Register()
{
	REX::INFO("{:*^30}", "EVENTS");

	if (const auto inputMgr = RE::BSInputDeviceManager::GetSingleton()) {
		inputMgr->AddEventSink(GetSingleton());

		REX::INFO("Registered for hotkey event");
	}

	if (auto scriptMgr = RE::ScriptEventSourceHolder::GetSingleton()) {
		scriptMgr->AddEventSink<RE::TESLoadGameEvent>(GetSingleton());
		REX::INFO("Registered for load game event");
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
	if (!a_evn) {
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

RE::BSEventNotifyControl Manager::ProcessEvent(const RE::TESLoadGameEvent* a_evn, RE::BSTEventSource<RE::TESLoadGameEvent>*)
{
	if (!a_evn) {
		return RE::BSEventNotifyControl::kContinue;
	}
	
	SetHotkeyPressed(false);
	SetHotkeyHeld(false);

	return RE::BSEventNotifyControl::kContinue;
}
