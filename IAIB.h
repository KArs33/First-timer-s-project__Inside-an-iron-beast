#ifndef _IAIB_H_
#define _IAIB_H_


#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <ctime>
#include "loc.h"
#include "foe.h"
#include "player.h"
#include "map.h"
#include <utility>
using namespace std;



//constants
const int MINOPTION = 0;
const int MAXOPTION = 3;
const int BASEFOODCOST = 1;
const int exitXY=4;

class Foe; //forward declairing. Plz work...

class startGame {
public:
	startGame(string foeFile, string locFile);
	~startGame(){
		for(int i =0; i<mapList0.size();i++)delete mapList0[i];
		for(int i =0; i<mapList1.size();i++)delete mapList1[i];
		for(int i =0; i<mapList2.size();i++)delete mapList2[i];
		for(int i =0; i<mapList3.size();i++)delete mapList3[i];
		for(int i =0; i<mapList4.size();i++)delete mapList4[i];

		/*  I'm commenting these out because the compiler is saying I don't need to delete them. We'll see... 
		for(int i =0; i<foeV1.size();i++)delete foeV1[i];
		for(int i =0; i<foeV2.size();i++)delete foeV2[i];
		for(int i =0; i<foeV3.size();i++)delete foeV3[i]; */
	};
	
	static const int WidthMAPMAX = 3*2; static const int HeightMapMax=5; //used for the dimensions of the map AS VECTORS	, plz don't change
	//const int WNUMCELL = WidthMAPMAX+2; const int HNUMCELL=HeightMapMax; OUPS, IGNORE THIS
	int curMapZone =0; //this is incriminated each time we move to a new map zone
	vector<loc*> mapList0, mapList1, mapList2, mapList3, mapList4;
	vector<Foe> foeV1;	vector<Foe> foeV2;	vector<Foe> foeV3; vector<Foe> foeV4;
	MAP gameMap[WidthMAPMAX][HeightMapMax]; //creates the empty map
	Player player = Player(); /// this feels incorrect...
	//note for future expansion, rather than make an empty player, maybe run a script asking for chracter customization?
	//do I give the player a name?

//Iterates over the class gameMap member and fills in locations.
void fillInMap(vector<loc*> locVec, vector<Foe> foeVec, int curMapZone);

//outputs the class gameMap to the terminal
void mapPrint();

//reads the loc.txt, and builds out the loc vectors 
// pass-by-reference so the caller's vectors are populated
void buildMapLists(vector<loc*>& mapList1, vector<loc*>& mapList2, vector<loc*>& mapList3);

//as as buildMapLists, but for the foe.txt
void buildFoeList();

//NO LONGER NEEDED, CAN REMOVE;;;;;;;As as fillInMap, but for foe vectors
//void placeFoes(vector<foe> foeV3, vector<foe> foeV2, vector<foe> foeV1); //used to read the foe.txt file, organize the foes into 3 vectors, and insert them into the map

//this function gets called when the player must make a choice at a location
void takeAction(loc place, Player &you, bool showLocationText = true);

//outputs a number using makeRoll
static int makeRolls(string stat, int mod, Player you);
//outputs a random 1-6 number
int makeRoll();

//the merchant functions run when entering a merchant menu
void merchantMeat(Player &you);
void merchantZ1(Player &you);
void merchantZ2(Player &you);

//this should be in a while loop. Will run most of the game
void mainMenu(); 

// movement helpers
bool findCurLocation(int &outX, int &outY);
bool movePlayer(const string &dir);
// sets (x,y) as the one and only current location, clearing all others first
void setCurLocation(int x, int y);

// partition helper: separate a list of loc* into mapList0..mapList4 by zone
void partitionLocsByZone(const std::vector<loc*> &allLocs);

//this needs to be built later. It will run the zone 0 locations
void outSide();

//runs when entering the last floor of the game
void lastFloor(); //still need to make this. It replaces gameMap.
// run the starting prologue locations (first three in loc.txt)
void runPrologue();
//combat functions
bool uniqueSelector(Foe &Foe);
void abruptAssialant(Foe &foe);
void highBornAi(Foe &foe);
void wireTrap(Foe &foe);
	void doorTrap(Foe &foe);
void startCombat(Foe &foe);
void printTutorial();



vector<string> mainMenuVec={"n","s","w","e","nw","sw","ne","se","m","xp","i"};
private:
	string m_locFile, m_foeFile;
};







#endif
