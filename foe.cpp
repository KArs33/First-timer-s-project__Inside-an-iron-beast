#include "foe.h"
#include "player.h"
#include <iostream>

// Implement only non-inline methods here
Foe::Foe() {
	name = "";
	power = 0;
	description = "";
	zone = 0;
	shield = 0;
	uniqueAI = false;
	reward = false;
	pursuit = 0;
	defeat = "";
	alive = false;
}

Foe::Foe(string foeName, int fowPower, string desc, int inZone, int foeShield, bool foeAI, 
	bool hasReward, int foePursuit, string defMsg)
{
	name = foeName;
	power = fowPower;
	description = desc;
	zone = inZone;
	shield = foeShield;
	uniqueAI = foeAI;
	reward = hasReward;
	pursuit = foePursuit;
	defeat = defMsg;
	alive = true;
}

void Foe::setArmor(int num){ armor = num; }
int Foe::getArmor(){ return armor; }

// More informative implementations
void Foe::printIntro(Foe guy){
	std::cout << "--- Foe Encounter ---\n";
	std::cout << "Name: " << guy.getName() << "\n";
	std::cout << "Desc: " << guy.getDescription() << "\n";
	std::cout << "Power: " << guy.getPower() << " | Shield: " << guy.getShield() << "\n";
}

void Foe::printStatus(Foe guy){
	std::cout << "Foe status - Alive: " << (guy.getAlive() ? "yes" : "no") << " | Armor: " << guy.getArmor() << "\n";
}

// clash: apply a single attack resolution where 'attack' is incoming player attack
bool Foe::clash(Player you, int attack){
	int defense = power + armor;
	if (you.getStatBd() + attack > defense) {
		if (shield > 0) { shield = shield - 1; std::cout << "Foe's shield absorbed the hit.\n"; return false; }
		std::cout << "You strike the foe true and wound them." << std::endl;
		alive = false; return true;
	}
	std::cout << "The foe weathers the attack." << std::endl;
	return false;
}
