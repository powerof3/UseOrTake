#pragma once

namespace Event
{
	using EventResult = RE::BSEventNotifyControl;

	class Manager final : public RE::BSTEventSink<RE::InputEvent*>
	{
	public:
		static Manager* GetSingleton();

		static void Register();

	private:
		static void UpdateCrosshairs();

		EventResult ProcessEvent(RE::InputEvent* const* a_evn, RE::BSTEventSource<RE::InputEvent*>*) override;

		Manager() = default;
		Manager(const Manager&) = delete;
		Manager(Manager&&) = delete;

		~Manager() override = default;

		Manager& operator=(const Manager&) = delete;
		Manager& operator=(Manager&&) = delete;
	};
}
