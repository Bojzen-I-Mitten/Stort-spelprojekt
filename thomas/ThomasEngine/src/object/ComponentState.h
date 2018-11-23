#pragma once

namespace ThomasEngine
{
	namespace Comp
	{
		public enum State
		{
			Uninitialized,
			Awake,			// First step of initiation called
			Enabled,		// Component is fully active
			Disabled,		// Enabled at some point but no longer active
			EndState		// Count
		};
		inline State& operator++ (State& d) {	// Prefix
			d = static_cast<State>((static_cast<int>(d) + 1) % EndState);
			return d;
		}
		inline State operator++ (State d, int) {	// postfix
			++d;
			return d;
		}
	}
}