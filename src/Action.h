class Action
{
public:
	enum Type : std::uint32_t
	{
		kTake,
		kPrimaryAction,
		kSecondaryAction
	};

	Action(std::string_view a_section, std::string a_label);
	virtual ~Action() = default;

	[[nodiscard]] bool IsEnabled() const { return enabled; }
	[[nodiscard]] Type GetDefaultAction() const { return static_cast<Type>(action.GetValue()); }

	[[nodiscard]] virtual std::string GetActionLabel(RE::TESObjectREFR* a_activator, RE::TESBoundObject* a_base, bool a_keyPressed, bool a_keyHeld) const;

protected:
	static std::string get_take_label(RE::TESObjectREFR* a_activator);
	static std::string get_alt_action_label(RE::TESObjectREFR* a_activator, std::string_view a_label);

	[[nodiscard]] const std::string& GetPrimaryLabel() const
	{
		return static_cast<const std::string&>(primaryActionLabel);
	}

	REX::TIniSetting<bool>          enabled;
	REX::TIniSetting<std::uint32_t> action;
	REX::TIniSetting<std::string>   primaryActionLabel;
};

class SecondaryAction final : public Action
{
public:
	SecondaryAction(std::string_view a_section, std::string a_label, std::string a_secondaryLabel);
	~SecondaryAction() override = default;

	[[nodiscard]] std::string GetActionLabel(RE::TESObjectREFR* a_activator, RE::TESBoundObject* a_base, bool a_keyPressed, bool a_keyHeld) const override;

private:
	REX::TIniSetting<std::string> secondaryActionLabel;
};

class AlchemyAction final : public Action
{
public:
	AlchemyAction(std::string_view a_section, std::string a_label, std::string a_foodLabel, std::string a_poisonLabel);
	~AlchemyAction() override = default;

	[[nodiscard]] std::string GetActionLabel(RE::TESObjectREFR* a_activator, RE::TESBoundObject* a_base, bool a_keyPressed, bool a_keyHeld) const override;

private:
	[[nodiscard]] const std::string& GetLabelForAlchType(RE::TESBoundObject* a_base) const;

	REX::TIniSetting<std::string> foodActionLabel;
	REX::TIniSetting<std::string> poisonActionLabel;
};
