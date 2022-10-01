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

			if (const auto light = a_this->As<RE::TESObjectLIGH>(); light && !light->CanBeCarried()) {
				return false;
			}

			if (a_targetRef && a_activatorRef) {
				if (const auto actor = a_activatorRef->As<RE::Actor>(); actor) {
					const auto settings = Settings::GetSingleton();

					actor->PickUpObject(a_targetRef, a_targetCount, a_arg3, true);

					if (const auto action = settings->GetActionForType(a_this->GetFormType()); action) {
						switch (action->GetDefaultAction()) {
						case Action::kTake:
							{
								if (settings->GetHotkeyHeld() && a_this->Is(RE::FormType::Scroll, RE::FormType::Weapon)) {
									do_secondary_action(actor, a_this);
								} else if (settings->GetHotkeyPressed()) {
									RE::ActorEquipManager::GetSingleton()->EquipObject(actor, a_this);
								}
							}
							break;
						case Action::kPrimaryAction:
							{
								if (settings->GetHotkeyHeld() && a_this->Is(RE::FormType::Scroll, RE::FormType::Weapon)) {
									do_secondary_action(actor, a_this);
								} else if (!settings->GetHotkeyPressed()) {
									RE::ActorEquipManager::GetSingleton()->EquipObject(actor, a_this);
								}
							}
							break;
						case Action::kSecondaryAction:
							{
								if (settings->GetHotkeyHeld()) {
									RE::ActorEquipManager::GetSingleton()->EquipObject(actor, a_this);
								} else if (!settings->GetHotkeyPressed()) {
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
		logger::info("{:*^30}", "HOOKS");

	    const auto settings = Settings::GetSingleton();

	    if (const auto armorAction = settings->GetActionForType(RE::FormType::Armor); armorAction && armorAction->IsEnabled()) {
			stl::write_vfunc<RE::TESObjectARMO, Activate>();
			stl::write_vfunc<RE::TESObjectARMO, GetActivateText>();

			logger::info("Registered armor hook");
		}

		if (const auto weaponAction = settings->GetActionForType(RE::FormType::Weapon); weaponAction && weaponAction->IsEnabled()) {
			stl::write_vfunc<RE::TESObjectWEAP, Activate>();
			stl::write_vfunc<RE::TESObjectWEAP, GetActivateText>();

			logger::info("Registered weapon hook");
		}

		if (const auto ingredientAction = settings->GetActionForType(RE::FormType::Ingredient); ingredientAction && ingredientAction->IsEnabled()) {
			stl::write_vfunc<RE::IngredientItem, Activate>();
			stl::write_vfunc<RE::IngredientItem, GetActivateText>();

			logger::info("Registered ingredient hook");
		}

		if (const auto alchemyAction = settings->GetActionForType(RE::FormType::AlchemyItem); alchemyAction && alchemyAction->IsEnabled()) {
			stl::write_vfunc<RE::AlchemyItem, Activate>();
			stl::write_vfunc<RE::AlchemyItem, GetActivateText>();

			logger::info("Registered alchemy hook");
		}

		if (const auto scrollAction = settings->GetActionForType(RE::FormType::Scroll); scrollAction && scrollAction->IsEnabled()) {
			stl::write_vfunc<RE::ScrollItem, Activate>();
			stl::write_vfunc<RE::ScrollItem, GetActivateText>();

			logger::info("Registered scroll hook");
		}

		if (const auto torchAction = settings->GetActionForType(RE::FormType::Light); torchAction && torchAction->IsEnabled()) {
			stl::write_vfunc<RE::TESObjectLIGH, Activate>();
			stl::write_vfunc<RE::TESObjectLIGH, GetActivateText>();

			logger::info("Registered torch hook");
		}

		if (const auto ammoAction = settings->GetActionForType(RE::FormType::Ammo); ammoAction && ammoAction->IsEnabled()) {
			stl::write_vfunc<RE::TESAmmo, Activate>();
			stl::write_vfunc<RE::TESAmmo, GetActivateText>();

			logger::info("Registered ammo hook");
		}
	}
}
