#pragma once

namespace ThomasEngine
{
	public ref class YieldInstruction
	{
	public:
		virtual property bool keepWaiting
		{
			bool get() { return true; }
		}
	};

	public ref class WaitForSeconds : public YieldInstruction
	{
	public:
		float timeLeft;

		WaitForSeconds(float seconds) { timeLeft = seconds; }
		property bool keepWaiting
		{
			bool get() override;
		}
	};

	public ref class WaitForSecondsRealtime : public YieldInstruction
	{
	public:
		float timeLeft;

		WaitForSecondsRealtime(float seconds) { timeLeft = seconds; }
		property bool keepWaiting
		{
			bool get() override;
		}
	};
}