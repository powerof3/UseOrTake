#include "Manager.h"

void Manager::LoadSettings()
{
	const auto path = std::format("Data/SKSE/Plugins/{}.ini", Version::PROJECT);

	CSimpleIniA ini;
	ini.SetUnicode();

	ini.LoadFile(path.c_str());

	ini::get_value(ini, hotKey, "Settings", "Alternate action hotkey", ";Press hotkey + Activate key to perform alternative action. Default is Left Shift\n;Hold hotkey to perform secondary action, if available.\n\n;DXScanCodes : https://www.creationkit.com/index.php?title=Input_Script");
	ini::get_value(ini, keyHeldDuration, "Settings", "Hotkey hold duration", ";How long should the hotkey be held down (in seconds) before switching to secondary action.");

	armors = Action(ini, "Armors", "Equip", ";Default action upon activating\n;0 - Take | 1 - Equip.", true);

	weapons = SecondaryAction(ini, "Weapons", "Equip", "Equip and draw", ";0 - Take | 1 - Equip | 2 - Equip and Draw.", true);

	alchemy = AlchemyAction(ini, "Potions", "Drink", "Eat", "Apply", ";0 - Take | 1 - Drink potion/Eat food/Apply poison.");

	ingestibles = Action(ini, "Ingredients", "Eat", ";0 - Take | 1 - Eat.", false);

	scrolls = SecondaryAction(ini, "Scrolls", "Equip", "Read", ";0 - Take | 1 - Equip | 2 - Read (cast scrolls).", false);

	torches = Action(ini, "Torches", "Equip", ";0 - Take | 1 - Equip.", false);

	ammo = Action(ini, "Ammo", "Equip", ";0 - Take | 1 - Equip.", false);

	(void)ini.SaveFile(path.c_str());
}

void Manager::Register()
{
	logger::info("{:*^30}", "EVENTS");

	if (const auto inputMgr = RE::BSInputDeviceManager::GetSingleton()) {
		inputMgr->AddEventSink(GetSingleton());

		logger::info("Registered for hotkey event");
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

Key Manager::GetHotkey() const
{
	return hotKey;
}

bool Manager::GetHotkeyPressed() const
{
	return keyPressed;
}

void Manager::SetHotkeyPressed(bool a_pressed)
{
	keyPressed = a_pressed;
}

bool Manager::GetHotkeyHeld() const
{
	return keyHeld;
}

void Manager::SetHotkeyHeld(bool a_held)
{
	keyHeld = a_held;
}

float Manager::GetKeyHeldDuration() const
{
	return keyHeldDuration;
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

			if (key == GetHotkey()) {
				if (GetHotkeyPressed() != buttonEvent->IsPressed()) {
					SetHotkeyPressed(buttonEvent->IsPressed());

					if (!GetHotkeyPressed()) {
						SetHotkeyHeld(false);
					}

					UpdateCrosshairs();
				} else if (GetHotkeyPressed() && !GetHotkeyHeld() && buttonEvent->HeldDuration() > GetKeyHeldDuration()) {
					SetHotkeyHeld(true);

					UpdateCrosshairs();
				}
			}
		}
	}

	return RE::BSEventNotifyControl::kContinue;
}
