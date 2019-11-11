#ifndef GENERICMOD_H
#define GENERICMOD_H

class GenericMod {
public:
	virtual void Initialize() {}
	virtual int OnChat(void* msg) { return 0; }
	virtual int OnCheckInventoryFull(void* player, void* item) { return 0; }
	virtual int OnP2PRequest(uint64_t steamID) { return 0; }
	virtual void OnGameTick(void* game) {}
	virtual float ModifyPlayerDamage(void* player, float damage) { return damage; }
	virtual float ModifyPlayerCrit(void* player, float crit) { return crit; }
	virtual float ModifyPlayerHealth(void* player, float health) { return health; }
	virtual float ModifyPlayerArmor(void* player, float armor) { return armor; }
	virtual float ModifyPlayerResistance(void* player, float resistance) { return resistance; }
	virtual float ModifyPlayerHaste(void* player, float haste) { return haste; }
	virtual float ModifyPlayerStaminaRegen(void* player, float staminaRegen) { return staminaRegen; }
};

#endif // GENERICMOD_H
