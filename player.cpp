#include "player.h"
#include "IAIB.h"
#include <iostream>
#include <cctype>
#include <string>
// Keep most methods inline in header; implement the remaining getters/accessors declared in the header
//int Player::getChoiceLevel(){ return 5; } //what was the point of this?
bool Player::levelUp(){
	if (xp >= 5) {
		xp -= 5;
		std::cout << "XP -5. You're new total is " << getXp() <<"." <<std::endl;
		cout << "What attribute would you like to raise?" <<endl<<
			"(enter b for Body, a for Agility, g for Guile, or i for In): ";
		char cmd='p'; string choice;
		while(cmd != 'b' && cmd != 'a' && cmd != 'g' && cmd != 'i' ){
			string command="";
			getline(cin, command); cmd= tolower(command[0]); 
		}
		switch (cmd){
			case 'b':
				setStatBd(statBd+1); choice = "Body";
				break;
			case 'a':
				setStatAg(statAg+1); choice = "Agility";			
				break;

			case 'g':
				setStatGu(statGu+1); choice = "Guile";
				break;

			case 'i':
				setStatIn(statIn+1); choice = "Inteligence";
				break;		
			default:
				cout << "Some error occured in Player::levelUp" <<endl;
				break;
			}
			//purge the cin of newline
			std::cin.ignore(10000, '\n');

		hp = hp+5; if(hp>maxHp) hp=maxHp;
		cout << "You gained a level! " << choice << "increased, HP restored." << endl;
		return true;
	}
	return false;
}

int Player::checkInputCombat(char input, Foe &thisFoe){
	// simple handler for combat input: attack (a), flee (f), use item (u)
	input = std::tolower(static_cast<unsigned char>(input));
	if (input == 'a') {
		int attack = startGame::makeRolls("Bd", (getHasWeapon2() ? 2:0), *this); // simple single-value attack
		bool killed = thisFoe.clash(*this, attack);
		if (killed) {
			xp += 3;
			//std::cout << "You defeated the foe and gained 3 XP." << std::endl;
			std::cout << thisFoe.getMsg() << std::endl;
			return 3; // foe killed
		}
		return 1; // action processed
	}
	if (input == 'j') {
    if (getJavelin() > 0) return useJavelin(thisFoe) ? 3 : 1;
  	  std::cout << "You have no javelins.\n"; return 1;
	}
	if (input == 'g') {
	    if (getHasGrenade()) return useGrenade(thisFoe) ? 3 : 1;
	    std::cout << "You have no grenades.\n"; return 1;
	}
	if (input == 'r') {
 	   if (getHasBullet() > 0 && getHasWeapon1()) return useRifle(thisFoe) ? 3 : 1;
 	   std::cout << "You can't fire the rifle.\n"; return 1;
	}
	if (input == 'f') {
		int fleeRoll = startGame::makeRolls("Ag", 0, *this);
		if (fleeRoll > thisFoe.getPersuit()) { std::cout << "You successfully flee." 
			<< std::endl; return 2; }
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
void Player::setHasBullet(int num){ hasBullet = num; showBullets = true; }
void Player::setHasEnergyPack(int num){ hasEnergyPack = num; showEnergyPacks = true; }
void Player::setHasTool1(){ hasTool1 = true; }
void Player::setHasTool2(){ /* placeholder: enable tool2 */ }
void Player::setJavelin(int num){ javelin = num; showJavelins = true; }
void Player::setHasGrenade(int has){ hasGernade = has; showGrenades = true; }
void Player::setMeds(int num){ meds = num; showMedkits = true; }
void Player::setShieldRelic(int num){ shieldRelic = num; showShieldRelic = true; }

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
	using std::cout;
	cout << "Trade goods: " << tradeGoods << "\n";
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
	bool showedAny = false;
	if (showBullets) {
		cout << " - Bullets: " << hasBullet << "\n";
		showedAny = true;
	}
	if (showEnergyPacks) {
		cout << " - Energy Packs: " << hasEnergyPack << "\n";
		showedAny = true;
	}
	if (showMedkits) {
		cout << " - Medkits: " << meds << "\n";
		showedAny = true;
	}
	if (showJavelins) {
		cout << " - Javelins: " << javelin << "\n";
		showedAny = true;
	}
	if (showShieldRelic) {
		cout << " - Shield relic charges: " << shieldRelic << "\n";
		showedAny = true;
	}
	if (showGrenades) {
		cout << " - Grenades: " << hasGernade << "\n";
		showedAny = true;
	}
	if (!showedAny) cout << " - None yet\n";
}

void Player::askUseItem(){
	using std::cout; using std::cin; using std::endl;
	auto readChar = [&]() -> char {
		string line;
		if (!std::getline(cin, line) || line.empty()) return 'E';
		return std::toupper((unsigned char)line[0]);
	};
	cout << "Item menu: (M)edkit, (E)xit\n";
	char input = readChar();
	while (input != 'E'){
		if (input == 'M'){
			if (getMeds() > 0){
				cout << "You use a medkit." << endl;
				setMeds(getMeds() - 1);
				setHp(getHp() + 10);
				if (getHp() > getMaxHp()) setHp(getMaxHp());
				cout << "HP is now " << getHp() << "." << endl;
				return;
			} else {
				cout << "No medkits available." << endl;
			}
		} else {
			cout << "Unknown item choice." << endl;
		}
		cout << "Item menu: (M)edkit, (E)xit\n";
		input = readChar();
	}
}

bool	Player::useJavelin(Foe &foe){
		//we are assuming a check was already run to see that the user has any javelins to use
		setJavelin(getJavelin()-1);

		cout<<"You make an opening, and launch a javelin at the foe."<<endl;
		int attack = startGame::makeRolls("Ag", 2, *this);

		int defense = foe.getPower() + foe.getArmor();
	if ( attack > defense) {
		if (foe.getShield() > 0) { foe.setShield(foe.getShield()-1); 
		std::cout << "Foe's shield absorbed the javelin hit.\n";}
		else{std::cout << "You strike the foe true and wound them." << std::endl;
		foe.setAlive(false); return true;}
	}
	else std::cout << "The foe dodges the attack the javelin." << std::endl;

	//the javelin has been used. Now the player makes a follow up attack
	cout<<"You press the advantage created by your javelin, the initiative is yours."<<endl;
	if(checkInputCombat('a', foe))return true;
	else return false;
	}
bool Player::useGrenade(Foe &foe){
			//we are assuming a check was already run to see that the user has any gernade to use
		setHasGrenade(getHasGrenade()-1);

		cout<<"You pull the pin and toss a grenade, while saying a silent pray..."<<endl;
		int attack = 20;
		int defense = foe.getPower() + foe.getArmor();
	if (foe.getShield() > 0) { foe.setShield(foe.getShield()-1); 
		std::cout << "Foe's shield absorbed the grenade hit.\n";}

	else{ if ( attack > defense) {
		{std::cout << "You strike the foe true and wound them." << std::endl;
		foe.setAlive(false); return true;}
	}
	std::cout << "The foe dodges behind cover and is safe from the blast." << std::endl;

	//the javelin has been used. Now the player makes a follow up attack
	cout<<"You press the advantage created by your attack, the initiative is yours."<<endl;
	if(checkInputCombat('a', foe))return true;
 		}return false;	
	}

bool Player::useRifle(Foe &foe){
	//we assume it has already been checked that the user has bullets
	setHasBullet(getHasBullet()-1);
	cout << "You charge the bolt of your rifle and fire away!";
	int attack = startGame::makeRolls("Ag", 2, *this);
	int defense = foe.getPower() + foe.getArmor();
	if ( attack > defense) {
		if (foe.getShield() > 0) { foe.setShield(foe.getShield()-1); 
		std::cout << "Foe's shield absorbed the bullet.\n"; return false;}
		else{std::cout << "You strike the foe true and wound them." << std::endl;
		foe.setAlive(false); return true;}
	}
	else std::cout << "The foe is only grazed by your shoot." << std::endl; return false;

}