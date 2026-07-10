#ifndef _PLAYER_H_
#define _PLAYER_H_
 
//TODO: I need to finish with loc encounters first. Then I take a log of every variable that needs to be tracked on the palyer
//sheet. Then I make a variable, a setter and getter for the variable.
#include <string>
#include "foe.h"
 
class Foe;//doing this jst to see if a foreward declaire fixes anything
struct Player {
public:
	bool getHasPet() { return hasPet; }; 
	bool getInspiredByMusic() { return inspired; }; bool getHasTradeGoods() { return hasTradeGoods; };
	int  getTradeGoods()            { return tradeGoods; }
	void setTradeGoods(int num)     { tradeGoods = num; hasTradeGoods = (num > 0); }
	int getStatBd() { return statBd; }; //body
	int getStatAg() {return statAg;}; //agility
	int getStatIn() { return statIn; }; //intelligence
	int getStatGu() { return statGu; }; //guile
	int getHp() {return hp;}
	int getMaxHp() {return maxHp;}
	int getArmor() {return armor;}
	int getShield() { return shield ; };
	int getInspired(){return inspired;}
	int getXp(){return xp;}
	void setXp(int num){xp=num;}
	void setInspired(int num){inspired=num;}
	void setHasPet() { hasPet = !hasPet; }; 
	void hasInspiredByMusic() { inspired = !inspired; }; 
	void setHasTradeGoods(){hasTradeGoods = !hasTradeGoods;};
	void setStatBd(int newBd) { statBd = newBd; };
	void setStatAg(int newAg) {  statAg=newAg; }; 
	void setStatIn(int newIn) {  statIn=newIn; };
	void setStatGu(int newGu) {  statGu=newGu; }; 
	void setHp(int newHP) { hp = newHP; }
	void setMaxHp(int newHP) { maxHp = newHP; } 
	void setArmor(int newA) { if(newA>armor)armor=newA; } //prevents better armor from being replaced by newer armor 
	void setShield(int newPower) { shield = newPower; };
	int getFood();
	void setFood(int num);
	bool levelUp();
	int getChoiceLevel();
	void printDefaultCombatInfo();
	// returns: 0 = unhandled, 1 = action processed (combat continues), 2 = fled successfully, 3 = foe killed
	int checkInputCombat(char input, Foe &thisFoe);
	int getHasBullet();
	int getHasEnergyPack();
	void setHasBullet(int num);
	void setHasEnergyPack(int num);
	bool getHasTool1();
	void setHasTool1();
	bool getHasTool2();
	void setHasTool2();
	int getFuel(){return fuel;};
	void setFuel(int has){fuel=has;};
	void setJavelin(int num); 
	int getJavelin();
	void setHasWeapon1(){hasWeapon1=true;};
	void setHasWeapon2(){hasWeapon2=true;};
	void setHasWeapon3(){hasWeapon3=true;};
	bool getHasWeapon1(){return hasWeapon1;};
	void setHasWeapon1(bool has){hasWeapon1=has;};
	bool getHasWeapon2(){return hasWeapon2;};
	void setHasWeapon2(bool has){hasWeapon2=has;};
	bool getHasWeapon3(){return hasWeapon3;};
	void setHasWeapon3(bool has){hasWeapon3=has;};
	void setHasAmulet(){hasAmulet=true;};
	bool getHasAmulet(){return hasAmulet;};
	void setHasCipher(){hasCipher =true;};
	bool getHasCipher(){return hasCipher;};
	void setHasGasMask(){hasGasMask = true;};
	bool getHasGasMask(){return hasGasMask;};
	void setHasForgeKey(){hasForgeKey = true;};
	bool getHasForgeKey(){return hasForgeKey;};
	void setHasVaultKey(){hasVaultKey = true;};
	bool getHasVaultKey(){return hasVaultKey;};
	void setHasPriestSecret(){hasPriestSecret = true;};
	bool getHasPriestSecret(){return hasPriestSecret;};
	void setHasShell(){hasShell = true;};
	bool getHasShell(){return hasShell;};
	void setHasRing(){hasRing = true;};
	bool getHasRing(){return hasRing;};
	bool getHasGrenade();
	void setHasGrenade(int has);
	int getOrb(){return hasOrb;};
	void setOrb(int has){hasOrb =has;};
	void setMeds(int num); 
	int getMeds();
	void setShieldRelic(int num); 
	int getShieldRelic();
	void printTradeInfo();
	void printPlayerInfo();
	void printItemList(); void askUseItem();//still need to work on
	bool useJavelin(Foe &foe);
	bool useGrenade(Foe &foe);
	bool useRifle(Foe &foe);
 
private:
	int statBd = 2; //body
	int statAg = 2; //agility
	int statIn = 2; //intelligence
	int statGu = 2; //guile
	int hp = 20;
	int maxHp = 30;
	int armor = 0;
	int shield = 0;
	int food = 0;
	int fuel = 0;
	bool hasPet = 0; bool hasTool1 = 0; int inspired = 0; bool hasTradeGoods = 0;
	int tradeGoods = 0; // how many trade-good units the player currently carries
	bool hasTool2 = 0;
	bool hasWeapon1=0; //rifle
	bool hasWeapon2 =0; //officers sword
	bool hasWeapon3=0; //
	int hasBullet=0; int hasEnergyPack=0;
	int shieldRelic=-1; //-1 signifies that it is not yet found, 0 is for when it has no charges left
	int javelin=0; int meds=0;
	int xp=0;
	int hasGernade=0;
	int hasOrb=0;
	bool hasShell = false;
	bool hasRing = false;
	bool hasPriestSecret = false;
	bool hasVaultKey = false;
	bool hasForgeKey=false;
	bool hasAmulet=false;
	bool hasCipher = false;
	bool hasGasMask =false;
	bool showOrb=false;
	bool showBullets = false;
	bool showEnergyPacks = false;
	bool showMedkits = false;
	bool showJavelins = false;
	bool showShieldRelic = false;
	bool showGrenades = false;
};
#endif