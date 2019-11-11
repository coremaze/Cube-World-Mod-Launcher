#ifndef GENERICMOD_H
#define GENERICMOD_H

class GenericMod {
public:
	virtual void Initialize() {}
	virtual int OnChat(void* msg) { return 0; }
	virtual int OnCheckInventoryFull(void* player, void* item) { return 0; }
	virtual int OnP2PRequest(uint64_t steamID) { return 0; }
	virtual void OnGameTick(void* game) {}
	virtual float ModifyPlayerDamage(void* player, float damage) { return 100.0f; }
};

#endif // GENERICMOD_H
