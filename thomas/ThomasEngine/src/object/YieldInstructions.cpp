#include "YieldInstructions.h"
#include "../Time.h"
namespace ThomasEngine
{
	bool WaitForSeconds::keepWaiting::get()
	{
		timeLeft -= Time::DeltaTime;
		return timeLeft > 0.0f;
	}

	bool WaitForSecondsRealtime::keepWaiting::get()
	{
		timeLeft -= Time::ActualDeltaTime;
		return timeLeft > 0.0f;
	}
}