#pragma once

namespace INI
{
	struct SettingsToUpdate
	{
		std::string_view section;
		std::string_view oldKey;
		std::string_view newKey;
	};

	inline std::vector<SettingsToUpdate>& GetSettingsToUpdate()
	{
		static std::vector<SettingsToUpdate> settings;
		return settings;
	}

	template <class T>
	class Setting : public REX::TIniSetting<T>
	{
	public:
		Setting(std::string_view a_section, std::string_view a_oldKey, std::string_view a_newKey, T a_default) :
			REX::TIniSetting<T>(a_section, a_newKey, a_default)
		{
			GetSettingsToUpdate().emplace_back(a_section, a_oldKey, a_newKey);
		}
	};
}
