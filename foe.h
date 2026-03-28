#ifndef _FOE_H_
#define _FOE_H_

#include <string>
using namespace std;
class Player;
//due to making a lot of the setters and getters first, before splitting the .h and .cpp files they are
//already here. I will retain the .h and .cpp files as I add new functions and such
class Foe{
public:
	Foe();
	Foe(string foeName, int fowPower, string desc, int inZone, int foeShield, bool foeAI, 
    bool hasReward, int foePursuit, string defMsg);

	//These are all setters and getters
	string getName() { return name; } 
	string getMsg() {return defeat;}
	int getPersuit() {return pursuit;}
	void setPersuit(int newPer){pursuit=newPer;}
	bool getReward() {return reward;}
	void setReward(bool has){reward=has;}
	int getPower() { return power; } 
	bool getAlive() { return alive; } 
	string getDescription() { return description; } 
	void setName(string newName) { name = newName; } 
	void setDescription(string newDescript) { description = newDescript; } 
	void setPower(int newPower) { power = newPower; } 
	void setAlive(bool isAlive) { alive = isAlive; } 
	int getZone() { return zone; } 
	void setZone(int newZone) { zone = newZone; } 
	void setShield(int num){shield=num;}
	int getShield(){return shield;}
	bool getUniqueAI(){return uniqueAI;}
	void setDefeat(string newMsg){defeat=newMsg;}
	void setUniqueAI(bool newAI){uniqueAI=newAI;}
	bool operator==( Foe &rhs){ //this is used for checking if a foe is 'real' or not
		if(rhs.getName()=="")return false;
		else return true;
	}
	void printIntro(Foe guy);
	void printStatus(Foe guy);
	bool clash(Player you, int attack);
	void setArmor(int num); 
	int getArmor();

	bool operator == (Foe rhs){
		if (rhs.getName()==this->getName())return true;
		else return false;
	}

private:
	string defeat="";
	int armor=0; //in the future, roll this out into the foe.txt file and give it a getter and setter
	bool uniqueAI = false; //tells the function there will be unique interactions
	int power = 0;
	bool reward=false;//tells the controller that there is some sort of reward when defeated
	int pursuit=0;
	string name = "";
	string description = "";
	int shield = 0;
	bool alive = 1; //1 is alive, 0 is dead
	int zone = 0; // 0=does not exist, Then there are 4 main zones. Outside, Shanty/Upper, Middle, Core. 5 is for the boss fight.

	
};


#endif