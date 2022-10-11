#include "Action.h"

Action::Action(CSimpleIniA& a_ini, const char* a_section, const char* a_label, const char* a_actionComment, bool a_doComment) :
	enabled(a_ini.GetBoolValue(a_section, "Enabled", true)),
	action(string::lexical_cast<Type>(a_ini.GetValue(a_section, "Default action", "0"))),  //kTake
	primaryActionLabel(a_ini.GetValue(a_section, "Alternate action label", a_label))
{
	a_ini.SetBoolValue(a_section, "Enabled", enabled, nullptr);
	a_ini.SetValue(a_section, "Default action", std::to_string(action).c_str(), a_actionComment);
	a_ini.SetValue(a_section, "Alternate action label", primaryActionLabel.c_str(), a_doComment ? ";Activate label when hovering over the item, ie. 'Equip' for armor/weapons" : nullptr);
}

bool Action::IsEnabled() const
{
	return enabled;
}

Action::Type Action::GetDefaultAction() const
{
	return action;
}

std::string Action::get_take_label(RE::TESObjectREFR* a_activator)
{
	constexpr auto get_take_or_steal = [](RE::TESObjectREFR* b_activator) {
		return b_activator->IsCrimeToActivate() ? RE::GameSettingCollection::GetSingleton()->GetSetting("sSteal")->GetString() :
                                                  RE::GameSettingCollection::GetSingleton()->GetSetting("sTake")->GetString();
	};

	if (const auto count = a_activator->extraList.GetCount(); count > 1) {
		return fmt::format("{}\n{} ({})", get_take_or_steal(a_activator), a_activator->GetDisplayFullName(), count);
	} else {
		return fmt::format("{}\n{}", get_take_or_steal(a_activator), a_activator->GetDisplayFullName());
	}
}

std::string Action::get_alt_action_label(RE::TESObjectREFR* a_activator, const std::string& a_label)
{
	if (a_activator->IsCrimeToActivate()) {
		if (const auto count = a_activator->extraList.GetCount(); count > 1) {
			return fmt::format("<font color='#FF0000'>{}</font>\n{} ({})", a_label, a_activator->GetDisplayFullName(), count);
		} else {
			return fmt::format("<font color='#FF0000'>{}</font>\n{}", a_label, a_activator->GetDisplayFullName());
		}
	} else {
		if (const auto count = a_activator->extraList.GetCount(); count > 1) {
			return fmt::format("{}\n{} ({})", a_label, a_activator->GetDisplayFullName(), count);
		} else {
			return fmt::format("{}\n{}", a_label, a_activator->GetDisplayFullName());
		}
	}
}

std::string Action::GetActionLabel(RE::TESObjectREFR* a_activator, RE::TESBoundObject*, bool a_keyPressed, bool) const
{
	if (GetDefaultAction() == kTake ? a_keyPressed : !a_keyPressed) {
		return get_alt_action_label(a_activator, primaryActionLabel);
	} else {
		return get_take_label(a_activator);
	}
}

AlchemyAction::AlchemyAction(CSimpleIniA& a_ini, const char* a_section, const char* a_label, const char* a_foodLabel, const char* a_poisonLabel, const char* a_actionComment) :
	Action(a_ini, a_section, a_label, a_actionComment, false),
	foodActionLabel(a_ini.GetValue(a_section, "Alternate action label (Food)", a_foodLabel)),
	poisonActionLabel(a_ini.GetValue(a_section, "Alternate action label (Poison)", a_poisonLabel))
{
	a_ini.SetValue(a_section, "Alternate action label (Food)", foodActionLabel.c_str(), ";Activate label for eating food");
	a_ini.SetValue(a_section, "Alternate action label (Poison)", poisonActionLabel.c_str(), ";Activate label for applying poisons");
}

std::string AlchemyAction::GetActionLabel(RE::TESObjectREFR* a_activator, RE::TESBoundObject* a_base, bool a_keyPressed, bool) const
{
	if (GetDefaultAction() == kTake ? a_keyPressed : !a_keyPressed) {
		return get_alt_action_label(a_activator, GetLabelForAlchType(a_base));
	} else {
		return get_take_label(a_activator);
	}
}

const std::string& AlchemyAction::GetLabelForAlchType(RE::TESBoundObject* a_base) const
{
	if (const auto alch = a_base->As<RE::AlchemyItem>(); alch) {
		if (alch->IsFood()) {
			if (const auto useSound = alch->data.consumptionSound; useSound && useSound->GetFormID() == 0x000B6435) {  //ITMPotionUse
				return primaryActionLabel;                                                                             //Drink
			}
			return foodActionLabel;  //Eat
		}
		if (alch->IsPoison()) {
			return poisonActionLabel;  //Apply
		}
	}

	return primaryActionLabel;
}

SecondaryAction::SecondaryAction(CSimpleIniA& a_ini, const char* a_section, const char* a_label, const char* a_secondarylabel, const char* a_actionComment, bool a_doComment) :
	Action(a_ini, a_section, a_label, a_actionComment, false),
	secondaryActionLabel(a_ini.GetValue(a_section, "Alternate secondary action label", a_secondarylabel))
{
	a_ini.SetValue(a_section, "Alternate secondary action label", secondaryActionLabel.c_str(), a_doComment ? ";Activate label for secondary action (eg. 'Equip and draw' weapons)" : nullptr);
}

std::string SecondaryAction::GetActionLabel(RE::TESObjectREFR* a_activator, RE::TESBoundObject*, bool a_keyPressed, bool a_keyHeld) const
{
	if (a_keyHeld) {
		return get_alt_action_label(a_activator, secondaryActionLabel);
	} else if (GetDefaultAction() == kTake ? a_keyPressed : !a_keyPressed) {
		return get_alt_action_label(a_activator, primaryActionLabel);
	}

	return get_take_label(a_activator);
}
