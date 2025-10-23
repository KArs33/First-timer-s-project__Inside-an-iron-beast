#include "player.h"
#include <iostream>
#include <cctype>
// Keep most methods inline in header; implement the remaining getters/accessors declared in the header
int Player::getChoiceLevel(){ return 5; }
bool Player::levelUp(){
	// simple level up: every 5 xp you gain +1 to Bd and Ag and heal to new max HP
	if (xp >= 5) {
		xp -= 5;
		statBd += 1;
		statAg += 1;
		maxHp += 5;
		hp = maxHp;
		std::cout << "You gained a level! Bd and Ag increased, HP restored." << std::endl;
		return true;
	}
	return false;
}

int Player::checkInputCombat(char input, Foe &thisFoe){
	// simple handler for combat input: attack (a), flee (f), use item (u)
	input = std::tolower(static_cast<unsigned char>(input));
	if (input == 'a') {
		int attack = statBd; // simple single-value attack
		bool killed = thisFoe.clash(*this, attack);
		if (killed) {
			xp += 2;
			std::cout << "You defeated the foe and gained 2 XP." << std::endl;
			return 3; // foe killed
		}
		return 1; // action processed
	}
	if (input == 'f') {
		int fleeRoll = statAg;
		if (fleeRoll > 6) { std::cout << "You successfully flee." << std::endl; return 2; }
		std::cout << "You fail to flee." << std::endl; return 1;
	}
	if (input == 'u') {
		askUseItem();
		return 1;
	}
	return 0; // unhandled
}

// Getters that were declared but not implemented in the header
int Player::getFood(){ return food; }
int Player::getHasBullet(){ return hasBullet; }
int Player::getHasEnergyPack(){ return hasEnergyPack; }
int Player::getJavelin(){ return javelin; }
int Player::getMeds(){ return meds; }
int Player::getShieldRelic(){ return shieldRelic; }
bool Player::getHasTool1(){ return hasTool1; }
bool Player::getHasTool2(){ return hasTool2; }
bool Player::getHasGrenade(){ return (hasGernade != 0); }

// Setters now implemented
void Player::setFood(int num){ food = num; }
void Player::setHasBullet(int num){ hasBullet = num; }
void Player::setHasEnergyPack(int num){ hasEnergyPack = num; }
void Player::setHasTool1(){ hasTool1 = true; }
void Player::setHasTool2(){ /* placeholder: enable tool2 */ }
void Player::setJavelin(int num){ javelin = num; }
void Player::setHasGrenade(int has){ hasGernade = has; }
void Player::setMeds(int num){ meds = num; }
void Player::setShieldRelic(int num){ shieldRelic = num; }

// Player UI implementations
void Player::printDefaultCombatInfo(){
	using std::cout; using std::endl;
	string armorType = "None";
	if (armor == 1) armorType = "Light";
	else if (armor == 2) armorType = "Medium";
	else if (armor == 3) armorType = "Heavy";
	cout << "| Bd: " << statBd << " | Ag: " << statAg << " | In: " << statIn << " | Gu: " << statGu << " |" << endl;
	cout << "| HP: " << hp << "/" << maxHp << " | Armor: " << armorType << " | Shield: " << shield << " |" << endl;
}

void Player::printTradeInfo(){
	using std::cout; using std::endl;
	cout << "Trade goods: " << (hasTradeGoods ? 1 : 0) << "\n";
}

void Player::printPlayerInfo(){
	using std::cout; using std::endl;
	printDefaultCombatInfo();
	cout << "Food: " << food << ", XP: " << xp << "\n";
	printItemList();
}

void Player::printItemList(){
	using std::cout; using std::endl;
	cout << "Items:\n";
	cout << " - Bullets: " << hasBullet << "\n";
	cout << " - Energy Packs: " << hasEnergyPack << "\n";
	cout << " - Medkits: " << meds << "\n";
	cout << " - Javelins: " << javelin << "\n";
	cout << " - Shield relic charges: " << shieldRelic << "\n";
}

void Player::askUseItem(){
	using std::cout; using std::cin; using std::endl;
	cout << "Item menu: (M)edkit, (E)xit\n";
	char input; cin >> input; input = std::toupper(input);
	while (input != 'E'){
		if (input == 'M'){
			if (getMeds() > 0){
				cout << "You use a medkit." << endl;
				setMeds(getMeds() - 1);
				setHp(getHp() + 10);
				if (getHp() > getMaxHp()) setHp(getMaxHp());
				cout << "HP is now " << getHp() << "." << endl;
				return; // use consumes and exit
			} else {
				cout << "No medkits available." << endl;
			}
		} else {
			cout << "Unknown item choice." << endl;
		}
		cout << "Item menu: (M)edkit, (E)xit\n";
		cin >> input; input = std::toupper(input);
	}
}

