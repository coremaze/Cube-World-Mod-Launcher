#ifndef GENERICMOD_H
#define GENERICMOD_H

class GenericMod {
public:
	enum Priority : unsigned long long int {
		VeryHighPriority = 0,
		HighPriority = 1,
		NormalPriority = 2,
		LowPriority = 3,
		VeryLowPriority = 4
	};

	virtual void Initialize() {}

	Priority OnChatPriority = NormalPriority;
	virtual int OnChat(void* msg) { return 0; }

	Priority OnCheckInventoryFullPriority = NormalPriority;
	virtual int OnCheckInventoryFull(void* player, void* item) { return 0; }

	Priority OnP2PRequestPriority = NormalPriority;
	virtual int OnP2PRequest(uint64_t steamID) { return 0; }

	Priority OnGameTickPriority = NormalPriority;
	virtual void OnGameTick(void* game) {}

	Priority OnZoneGeneratedPriority = NormalPriority;
	virtual void OnZoneGenerated(void* world, void* zone) {}

	Priority OnWindowProcPriority = NormalPriority;
	virtual int OnWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return 0; }

	Priority OnGetKeyboardStatePriority = NormalPriority;
	virtual void OnGetKeyboardState(BYTE* diKeys) {}

	Priority OnGetMouseStatePriority = NormalPriority;
	virtual void OnGetMouseState(void* diMouse) {}

	Priority OnPresentPriority = NormalPriority;
	virtual void OnPresent(void* SwapChain, UINT SyncInterval, UINT Flags) {}
};

#endif // GENERICMOD_H
