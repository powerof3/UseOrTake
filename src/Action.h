#pragma once

class Action
{
public:
	enum Type
	{
		kTake,
		kPrimaryAction,
		kSecondaryAction
	};

	Action() = default;
    virtual ~Action() = default;

	Action(CSimpleIniA& a_ini, const char* a_section, const char* a_label, const char* a_actionComment, bool a_doComment);

	[[nodiscard]] bool IsEnabled() const;
	
	[[nodiscard]] Type GetDefaultAction() const;

	[[nodiscard]] virtual std::string GetActionLabel(RE::TESObjectREFR* a_activator, RE::TESBoundObject* a_base, bool a_keyPressed, bool a_keyHeld) const;

protected:
    static std::string get_take_label(RE::TESObjectREFR* a_activator);

	static std::string get_alt_action_label(RE::TESObjectREFR* a_activator, const std::string& a_label);

    bool enabled;
	Type action;
	std::string primaryActionLabel;
};

class SecondaryAction final : public Action
{
public:
	SecondaryAction() = default;
	~SecondaryAction() override = default;

	SecondaryAction(CSimpleIniA& a_ini, const char* a_section, const char* a_label, const char* a_secondarylabel, const char* a_actionComment, bool a_doComment);

	[[nodiscard]] std::string GetActionLabel(RE::TESObjectREFR* a_activator, RE::TESBoundObject* a_base, bool a_keyPressed, bool a_keyHeld) const override;

private:
	std::string secondaryActionLabel;
};

class AlchemyAction final : public Action
{
public:
	enum AlchType
	{
		kPotion,
		kPoison,
		kFood
	};

    AlchemyAction() = default;
	~AlchemyAction() override = default;

	AlchemyAction(CSimpleIniA& a_ini, const char* a_section, const char* a_label, const char* a_foodLabel, const char* a_poisonLabel, const char* a_actionComment);

	[[nodiscard]] std::string GetActionLabel(RE::TESObjectREFR* a_activator, RE::TESBoundObject* a_base, bool a_keyPressed, bool a_keyHeld) const override;

private:
	[[nodiscard]] const std::string& GetLabelForAlchType(RE::TESBoundObject* a_base) const;

    std::string foodActionLabel;
    std::string poisonActionLabel;
};
