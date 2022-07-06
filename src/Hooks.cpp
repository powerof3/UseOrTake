#include "Hooks.h"
#include "Settings.h"

namespace Hooks
{
	struct GetActivateText
	{
		static bool thunk(RE::TESBoundObject* a_this, RE::TESObjectREFR* a_activator, RE::BSString& a_dst)
		{
			const auto settings = Settings::GetSingleton();
			const auto action = settings->GetActionForType(a_this->GetFormType());

			a_dst = action->GetActionLabel(a_activator, a_this, settings->GetHotkeyPressed(), settings->GetHotkeyHeld());

			return true;
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static inline constexpr std::size_t size = 0x4C;
	};

	struct Activate
	{
		static bool thunk(RE::TESBoundObject* a_this,
			RE::TESObjectREFR* a_targetRef,
			RE::TESObjectREFR* a_activatorRef,
			std::uint8_t a_arg3,
			RE::TESBoundObject*,
			std::int32_t a_targetCount)
		{
			constexpr auto do_secondary_action = [](RE::Actor* a_actor, RE::TESBoundObject* a_base) {
				switch (a_base->GetFormType()) {
				case RE::FormType::Scroll:
					{
						if (const auto scroll = a_base->As<RE::ScrollItem>(); scroll) {
							a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->CastSpellImmediate(scroll, false, a_actor, 1.0f, false, 0.0f, nullptr);
							a_actor->RemoveItem(scroll, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
						}
					}
					break;
				case RE::FormType::Weapon:
					{
						RE::ActorEquipManager::GetSingleton()->EquipObject(a_actor, a_base);
						a_actor->DrawWeaponMagicHands(true);
					}
					break;
				default:
					break;
				}
			};

			if (a_targetRef && a_activatorRef) {
				if (const auto actor = a_activatorRef->As<RE::Actor>(); actor) {
					const auto settings = Settings::GetSingleton();

					if (const auto action = settings->GetActionForType(a_this->GetFormType()); action) {
						bool isHotKeyPressed = settings->GetHotkeyPressed();
						bool isHotKeyHeld = settings->GetHotkeyHeld();

						actor->PickUpObject(a_targetRef, a_targetCount, a_arg3, true);

						switch (action->GetDefaultAction()) {
						case Action::kTake:
							{
								if (isHotKeyHeld && a_this->Is(RE::FormType::Scroll, RE::FormType::Weapon)) {
									do_secondary_action(actor, a_this);
								} else if (isHotKeyPressed) {
									RE::ActorEquipManager::GetSingleton()->EquipObject(actor, a_this);
								}
							}
							break;
						case Action::kPrimaryAction:
							{
								if (isHotKeyHeld && a_this->Is(RE::FormType::Scroll, RE::FormType::Weapon)) {
									do_secondary_action(actor, a_this);
								} else if (!isHotKeyPressed) {
									RE::ActorEquipManager::GetSingleton()->EquipObject(actor, a_this);
								}
							}
							break;
						case Action::kSecondaryAction:
							{
								if (isHotKeyHeld) {
									RE::ActorEquipManager::GetSingleton()->EquipObject(actor, a_this);
								} else if (!isHotKeyPressed) {
									do_secondary_action(actor, a_this);
								}
							}
							break;
						}
					}
				}
			}
			return true;
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static inline constexpr std::size_t size = 0x37;
	};

	void Install()
	{
		const auto settings = Settings::GetSingleton();

		if (const auto armorAction = settings->GetActionForType(RE::FormType::Armor); armorAction && armorAction->IsEnabled()) {
			stl::write_vfunc<RE::TESObjectARMO, Activate>();
			stl::write_vfunc<RE::TESObjectARMO, GetActivateText>();
		}

		if (const auto weaponAction = settings->GetActionForType(RE::FormType::Weapon); weaponAction && weaponAction->IsEnabled()) {
			stl::write_vfunc<RE::TESObjectWEAP, Activate>();
			stl::write_vfunc<RE::TESObjectWEAP, GetActivateText>();
		}

		if (const auto ingredientAction = settings->GetActionForType(RE::FormType::Ingredient); ingredientAction && ingredientAction->IsEnabled()) {
			stl::write_vfunc<RE::IngredientItem, Activate>();
			stl::write_vfunc<RE::IngredientItem, GetActivateText>();
		}

		if (const auto alchemyAction = settings->GetActionForType(RE::FormType::AlchemyItem); alchemyAction && alchemyAction->IsEnabled()) {
			stl::write_vfunc<RE::AlchemyItem, Activate>();
			stl::write_vfunc<RE::AlchemyItem, GetActivateText>();
		}

		if (const auto scrollAction = settings->GetActionForType(RE::FormType::Scroll); scrollAction && scrollAction->IsEnabled()) {
			stl::write_vfunc<RE::ScrollItem, Activate>();
			stl::write_vfunc<RE::ScrollItem, GetActivateText>();
		}
	}
}
