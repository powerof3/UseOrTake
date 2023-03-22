#include "Settings.h"

Settings::Settings() :
	keyPressed(false),
	keyHeld(false)
{
	const auto path = fmt::format("Data/SKSE/Plugins/{}.ini", Version::PROJECT);

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

Action* Settings::GetActionForType(RE::FormType a_type)
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

Key Settings::GetHotkey() const
{
	return hotKey;
}

bool Settings::GetHotkeyPressed() const
{
	return keyPressed;
}

void Settings::SetHotkeyPressed(bool a_pressed)
{
	keyPressed = a_pressed;
}

bool Settings::GetHotkeyHeld() const
{
	return keyHeld;
}

void Settings::SetHotkeyHeld(bool a_held)
{
	keyHeld = a_held;
}

float Settings::GetKeyHeldDuration() const
{
	return keyHeldDuration;
}
