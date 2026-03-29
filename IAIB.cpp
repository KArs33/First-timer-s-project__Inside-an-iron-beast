// Single-source minimal IAIB implementation
//Kevin Arsenault
//Big dumb project to test out a bunch of things with my minimal coding experience
//1/18/2025
//first game test . game
//None save those named Kevin R. Arsenault are allowed to claim ownership over this project. Any may repost this project elsewhere so long as proper citation of origin is given. Any may edit this program so long as the origin of the program remains cited.

#include "IAIB.h"
#include <algorithm>
#include <random>
#include <sstream>

using namespace std;

static string fixEscapedQuotes(const string &s) { return s; }

// safe stoi helper: returns defaultVal if conversion fails
static int safeStoi(const string &s, int defaultVal = -1) {
	try {
		// trim whitespace
		size_t i = 0; while (i < s.size() && isspace((unsigned char)s[i])) ++i;
		size_t j = s.size(); while (j > i && isspace((unsigned char)s[j-1])) --j;
		if (i >= j) return defaultVal;
		string sub = s.substr(i, j - i);
		// find first number in the string
		size_t p = 0;
		// allow optional +/-
		if (p < sub.size() && (sub[p] == '+' || sub[p] == '-')) p++;
		while (p < sub.size() && !isdigit((unsigned char)sub[p])) p++;
		if (p >= sub.size()) return defaultVal;
		// extract contiguous digits (and optional sign before)
		size_t start = (sub[0] == '+' || sub[0] == '-') ? 0 : p;
		size_t q = p;
		while (q < sub.size() && isdigit((unsigned char)sub[q])) q++;
		string num = sub.substr(start, q - start);
		return stoi(num);
	} catch (...) {
		return defaultVal;
	}
}

startGame::startGame(string foeFile, string locFile) {
	m_foeFile = foeFile;
	m_locFile = locFile;
	buildFoeList();
	buildMapLists(mapList1, mapList2, mapList3); // populates mapList0..4 via partitionLocsByZone
	if (!mapList1.empty() && !foeV1.empty()) fillInMap(mapList1, foeV1, 1);
	// runPrologue uses mapList0 (populated above) and mapList1 for the post-prologue placement
	runPrologue();
}

// Runs the starting prologue locations (zone-0 entries from loc.txt).
// buildMapLists must have already been called so mapList0 is populated.
void startGame::runPrologue(){
	if (mapList0.empty()) return;

	// Use at most the first 3 zone-0 locations as prologue stops
	int count = (int)mapList0.size() < 3 ? (int)mapList0.size() : 3;

	for (int i = 0; i < count; ++i) {
		loc &cur = *mapList0[i];
		if (cur.getOptions() <= 0) {
			cout << "[Prologue] Skipping '" << cur.getName() << "' (no options).\n";
			continue;
		}
		cout << "\n[Prologue " << (i + 1) << "]\n";
		cout << "Player stats — Bd:" << player.getStatBd() << " Ag:" << player.getStatAg()
		     << " In:" << player.getStatIn() << " Gu:" << player.getStatGu() << "\n";

		bool showLocationText = true;
		while (true) {
			int xpBefore = player.getXp();
			takeAction(cur, player, showLocationText);
			if (player.getXp() > xpBefore) {
				cout << "[Prologue] Passed '" << cur.getName() << "'.\n";
				break;
			}
			cout << "You failed the check. Try again.\n";
			showLocationText = false;
		}
	}

	// Place the player at the map cell holding the first zone-1 location (start of main game)
	if (!mapList1.empty()) {
		string firstName = mapList1[0]->getName();
		for (int x = 0; x < WidthMAPMAX; ++x) {
			for (int y = 0; y < HeightMapMax; ++y) {
				loc* lp = gameMap[x][y].getLocObj();
				if (lp && lp->getName() == firstName) {
					setCurLocation(x, y);
					cout << "Placed player at '" << firstName << "' (" << x << "," << y << ").\n";
					mainMenu();
					return;
				}
			}
		}
	}
	// Fallback: put the player at (0,0)
	cout<<"ERROR, runPrologue() error" <<endl;
	setCurLocation(0, 0); mainMenu();
}

void startGame::buildMapLists(vector<loc*>& m1, vector<loc*>& m2, vector<loc*>& m3) {
	ifstream in(m_locFile.empty() ? "loc.txt" : m_locFile);
	if (!in.is_open()) return;
	string line; int counter = 0;
	vector<loc*> all;
	while (getline(in, line)) {
		counter++;
		vector<string> parts; istringstream ss(line); string token;
		while (getline(ss, token, '|')) parts.push_back(token);

		// loc.txt column order (tab-separated in source, stored as | here):
		// 0:Name | 1:Desc | 2:options | 3:ZONE
		// 4:OP1  | 5:OP1STAT | 6:OP1NUM | 7:OP1FAIL | 8:OP1PASS
		// 9:OP2  | 10:OP2STAT | 11:OP2NUM | 12:OP2FAIL | 13:OP2PASS
		// 14:OP3 | 15:OP3STAT | 16:OP3NUM | 17:OP3FAIL | 18:OP3PASS
		// 19:pop1 | 20:fop1 | 21:pop2 | 22:fop2 | 23:pop3 | 24:fop3
		if (parts.size() < 4) continue;
		string name    =                              parts[0];
		string desc    =                              parts[1];
		int    options = safeStoi(                   parts[2], 0);
		int    zone    = safeStoi(                   parts[3], -1);
		string o1      = parts.size() > 4  ?          parts[4]       : string();
		string opS1    = parts.size() > 5  ?          parts[5]       : string();
		int    opN1    = parts.size() > 6  ? safeStoi(parts[6],  0)  : 0;
		string op1f    = parts.size() > 7  ?          parts[7]       : string();
		string op1p    = parts.size() > 8  ?          parts[8]       : string();
		string o2      = parts.size() > 9  ?          parts[9]       : string();
		string opS2    = parts.size() > 10 ?          parts[10]      : string();
		int    opN2    = parts.size() > 11 ? safeStoi(parts[11], 0)  : 0;
		string op2f    = parts.size() > 12 ?          parts[12]      : string();
		string op2p    = parts.size() > 13 ?          parts[13]      : string();
		string o3      = parts.size() > 14 ?          parts[14]      : string();
		string opS3    = parts.size() > 15 ?          parts[15]      : string();
		int    opN3    = parts.size() > 16 ? safeStoi(parts[16], 0)  : 0;
		string op3f    = parts.size() > 17 ?          parts[17]      : string();
		string op3p    = parts.size() > 18 ?          parts[18]      : string();
		string pop1    = parts.size() > 19 ?          parts[19]      : string();
		string fop1    = parts.size() > 20 ?          parts[20]      : string();
		string pop2    = parts.size() > 21 ?          parts[21]      : string();
		string fop2    = parts.size() > 22 ?          parts[22]      : string();
		string pop3    = parts.size() > 23 ?          parts[23]      : string();
		string fop3    = parts.size() > 24 ?          parts[24]      : string();

		loc* l = new loc(name, desc, options, zone,
		                 o1, opS1, opN1, op1f, op1p,
		                 o2, opS2, opN2, op2f, op2p,
		                 o3, opS3, opN3, op3f, op3p,
		                 pop1, fop1, pop2, fop2, pop3, fop3, counter);
		all.push_back(l);
	}
	partitionLocsByZone(all);
}

void startGame::partitionLocsByZone(const std::vector<loc*> &allLocs){
	mapList0.clear(); mapList1.clear(); mapList2.clear(); mapList3.clear(); mapList4.clear();
	for(auto l : allLocs){
		if(!l) continue;
		int z = l->getZone();
		switch(z){
			case 0: mapList0.push_back(l); break;
			case 1: mapList1.push_back(l); break;
			case 2: mapList2.push_back(l); break;
			case 3: mapList3.push_back(l); break;
			case 4: mapList4.push_back(l); break;
			default: delete l; break; // unknown zone, free it
		}
	}
	// shuffle each zone list
	std::random_device rd; std::mt19937 g(rd());
	shuffle(mapList0.begin(), mapList0.end(), g);
	shuffle(mapList1.begin(), mapList1.end(), g);
	shuffle(mapList2.begin(), mapList2.end(), g);
	shuffle(mapList3.begin(), mapList3.end(), g);
	shuffle(mapList4.begin(), mapList4.end(), g);
}

void startGame::buildFoeList(){
	ifstream in(m_foeFile.empty() ? "foe.txt" : m_foeFile);
	if (!in.is_open()) return;
	string line;
	while (getline(in, line)) {
		vector<string> parts; istringstream ss(line); string tok;
		while (getline(ss, tok, '|')) parts.push_back(tok);

		// foe.txt column order:
		// 0:NAME | 1:ZONE | 2:DESC | 3:POW | 4:shield | 5:uniqueAI | 6:reward | 7:pursuit | 8:rewardCode | 9:defeatMsg
		if (parts.size() < 2) continue;
		string name    =                              parts[0];
		int    zone    = parts.size() > 1 ? safeStoi(parts[1], -1)  : -1;
		string desc    = parts.size() > 2 ?           parts[2]       : string();
		int    power   = parts.size() > 3 ? safeStoi(parts[3], 0)   : 0;
		int    shield  = parts.size() > 4 ? safeStoi(parts[4], 0)   : 0;
		bool   uAI     = parts.size() > 5 && parts[5] != "NULL" && !parts[5].empty();
		bool   reward  = parts.size() > 6 && parts[6] != "NULL" && !parts[6].empty();
		int    pursuit = parts.size() > 7 ? safeStoi(parts[7], 0)   : 0;
		// parts[8] is the reward code — stored but not yet consumed by this class
		string defeat  = parts.size() > 9 ?           parts[9]       : string();

		Foe f(name, power, desc, zone, shield, uAI, reward, pursuit, defeat);
		if      (zone == 1) foeV1.push_back(f);
		else if (zone == 2) foeV2.push_back(f);
		else if (zone == 3) foeV3.push_back(f);
		else if (zone == 4) foeV4.push_back(f);
	}
}

void startGame::fillInMap(vector<loc*> locVec, vector<Foe> foeVec, int curMapZone){
	// Determine region bounds based on curMapZone:
	// zones 1..4 map to quadrants; zone 0 maps to the center area.
	int xStart = 0, xEnd = WidthMAPMAX - 1, yStart = 0, yEnd = HeightMapMax - 1;
	int halfW = WidthMAPMAX / 2;
	int halfH = HeightMapMax / 2;
	if (curMapZone == 1) { // top-left
		xStart = 0; xEnd = max(0, halfW - 1);
		yStart = 0; yEnd = max(0, halfH - 1);
	} else if (curMapZone == 2) { // top-right
		xStart = halfW; xEnd = WidthMAPMAX - 1;
		yStart = 0; yEnd = max(0, halfH - 1);
	} else if (curMapZone == 3) { // bottom-left
		xStart = 0; xEnd = max(0, halfW - 1);
		yStart = halfH; yEnd = HeightMapMax - 1;
	} else if (curMapZone == 4) { // bottom-right
		xStart = halfW; xEnd = WidthMAPMAX - 1;
		yStart = halfH; yEnd = HeightMapMax - 1;
	} else { // zone 0 -> center region (try a small box around the center)
		int cx = WidthMAPMAX / 2;
		int cy = HeightMapMax / 2;
		xStart = max(0, cx - 1); xEnd = min(WidthMAPMAX - 1, cx + 1);
		yStart = max(0, cy - 1); yEnd = min(HeightMapMax - 1, cy + 1);
	}

	// collect playable parity cells within the region
	vector<pair<int,int>> cells;
	for (int i = xStart; i <= xEnd; ++i) {
		for (int j = yStart; j <= yEnd; ++j) {
			if ((i + j) % 2 == 0) cells.emplace_back(i, j);
			else gameMap[i][j].setBlank();
		}
	}
	// sort cells by distance to region center (center-first)
	double cx = (xStart + xEnd) / 2.0;
	double cy = (yStart + yEnd) / 2.0;
	sort(cells.begin(), cells.end(), [&](const pair<int,int> &a, const pair<int,int> &b){
		double da = (a.first - cx)*(a.first - cx) + (a.second - cy)*(a.second - cy);
		double db = (b.first - cx)*(b.first - cx) + (b.second - cy)*(b.second - cy);
		return da < db;
	});

	int idx = 0;
	for (auto &p : cells) {
		int i = p.first, j = p.second;
		if (idx < (int)locVec.size() && idx < (int)foeVec.size()) {
			gameMap[i][j] = MAP(foeVec[idx], idx, locVec[idx]);
		} else {
			gameMap[i][j].setBlank();
		}
		++idx;
	}

	// Any remaining map cells outside the chosen region should be blanked out
	for (int i = 0; i < WidthMAPMAX; ++i) {
		for (int j = 0; j < HeightMapMax; ++j) {
			if (i >= xStart && i <= xEnd && j >= yStart && j <= yEnd) continue;
			// keep the original parity behaviour
			if ((i + j) % 2 != 0) { gameMap[i][j].setBlank(); continue; }
			gameMap[i][j].setBlank();
		}
	}
}

void startGame::mapPrint(){
	for (int y = 0; y < HeightMapMax; ++y) {
		for (int x = 0; x < WidthMAPMAX; ++x) {
			if ((x + y) % 2 == 0) {
				if (gameMap[x][y].getIsCurLoc()) cout << "P"; // player's position
				else cout << (gameMap[x][y].getBlank() ? "." : "#");
			}
			else cout << ' ';
			cout << ' ';
		}
		cout << '\n';
	}
}

void startGame::mainMenu(){
	cout << "Welcome to the main menu. Enter commands: m=map, i=info, o=outside, t=test loc, q=quit" << endl;
	string cmd;
	while (true) {
		cout << "main> ";
		if (!std::getline(cin, cmd)) break;
		// trim leading whitespace
		size_t p = 0; while (p < cmd.size() && isspace((unsigned char)cmd[p])) ++p;
		cmd = cmd.substr(p);
		if (cmd.empty()) continue;
		if (cmd == "m") { mapPrint(); }
		else if (cmd == "i") { player.printPlayerInfo(); }
		else if (cmd == "o") { outSide(); }
		else if (cmd == "t") {
			if (!mapList1.empty()) takeAction(*mapList1[0], player);
			else cout << "No locations loaded for testing." << endl;
		}
		else if (cmd == "q") { cout << "Exiting main menu." << endl; break; }
		else if (cmd == "xp") { 
			cout << "XP: " << player.getXp() << endl; 
			if(player.getXp()>=5){
				string answer;
				while(answer !="y" && answer !="n" && answer !="Y" && answer !="N"){
					cout <<"Would you like to level up? Y/N? ";
					getline(cin, answer);
				}
				if(answer=="y") player.levelUp();
			}
		}
		else if (cmd == "n" || cmd == "s" || cmd == "e" || cmd == "w" || cmd == "nw" || cmd == "ne" || cmd == "sw" || cmd == "se"){
			if(!movePlayer(cmd)) cout << "You cannot move that way." << endl;
		}
		else { cout << "Unknown command. Valid: m i o t xp q n s e w nw ne sw se" << endl; }
	}
}

void startGame::setCurLocation(int x, int y){
	for (int i = 0; i < WidthMAPMAX; ++i)
		for (int j = 0; j < HeightMapMax; ++j)
			gameMap[i][j].setIsCurLoc(false);
	gameMap[x][y].setIsCurLoc(true);
}

bool startGame::findCurLocation(int &outX, int &outY){
	for(int x=0;x<WidthMAPMAX;++x){
		for(int y=0;y<HeightMapMax;++y){
			if(gameMap[x][y].getIsCurLoc()){ outX=x; outY=y; return true; }
		}
	}
	return false;
}

bool startGame::movePlayer(const string &dir){
	int x,y; if(!findCurLocation(x,y)) return false;
	int nx=x, ny=y;
	if(dir=="n") ny -= 1;
	else if(dir=="s") ny += 1;
	else if(dir=="e") nx += 1;
	else if(dir=="w") nx -= 1;
	else if(dir=="nw"){ nx-=1; ny-=1; }
	else if(dir=="ne"){ nx+=1; ny-=1; }
	else if(dir=="sw"){ nx-=1; ny+=1; }
	else if(dir=="se"){ nx+=1; ny+=1; }
	if(nx<0||ny<0||nx>=WidthMAPMAX||ny>=HeightMapMax) return false;
	if(gameMap[nx][ny].getBlank()) return false;
	// move marker
	setCurLocation(nx, ny);
	cout << "You move "<<dir<<" to ("<<nx<<","<<ny<<")"<<endl;
	// if location exists, run its action
	if(gameMap[nx][ny].getLoc()){
		loc* cellLocPtr = gameMap[nx][ny].getLocObj();
		if (cellLocPtr) takeAction(*cellLocPtr, player);
	}
	return true;
}

void startGame::takeAction(loc place, Player &you, bool showLocationText){
	if (showLocationText) {
		cout << "Location: " << place.getName() << "\n" << place.getDescription() << endl;
	}
	int options = place.getOptions();
	if (options <= 0) { cout << "Nothing to do here." << endl; return; }
	if (showLocationText) cout << "Options:" << endl;
	if (options >= 1) cout << "1) " << place.getOp1() << endl;
	if (options >= 2) cout << "2) " << place.getOp2() << endl;
	if (options >= 3) cout << "3) " << place.getOp3() << endl;
	cout << "Choose option (1-" << options << "): ";
	int choice = 0;
	// Read user choice with getline and parse to avoid leaving cin in fail state
	string choiceLine;
	while (true) {
		if(!std::getline(cin, choiceLine)){
			// EOF or input error: default to option 1
			choice = 1;
			break;
		}
		// trim leading whitespace
		size_t p = 0; while (p < choiceLine.size() && isspace((unsigned char)choiceLine[p])) ++p;
		if (p == choiceLine.size()) { cout << "Invalid, re-enter: "; continue; }
		try {
			choice = stoi(choiceLine.substr(p));
		} catch (...) {
			cout << "Invalid, re-enter: ";
			continue;
		}
		if (choice >= 1 && choice <= options) break;
		cout << "Invalid, re-enter: ";
	}
	string stat; int need = 0; string passMsg, failMsg;
	if (choice == 1) { stat = place.getOp1Stat(); need = place.getOp1StatNum(); passMsg = place.getOp1pass(); failMsg = place.getOp1fail(); }
	else if (choice == 2) { stat = place.getOp2Stat(); need = place.getOp2StatNum(); passMsg = place.getOp2pass(); failMsg = place.getOp2fail(); }
	else { stat = place.getOp3Stat(); need = place.getOp3StatNum(); passMsg = place.getOp3pass(); failMsg = place.getOp3fail(); }
	int xpBefore = you.getXp();
	int roll = makeRolls(stat, 0, you);
	cout << "[DEBUG] need="<<need<<" stat='"<<stat<<"' roll="<<roll<<" xpBefore="<<xpBefore<<"\n";
	cout << "You roll " << roll << " against needed " << need << " (" << stat << ")" << endl;
	if (need == 0 || roll >= need) {
		cout << "Success: " << passMsg << endl;
		you.setXp(you.getXp() + 1);
	} else {
		cout << "Failure: " << failMsg << endl;
		you.setHp(you.getHp() - 2);
	}
}

int startGame::makeRolls(string stat, int mod, Player you){
	int statNum = 0, roll = 0;
	if (stat == "Bd") statNum = you.getStatBd();
	else if (stat == "Ag") statNum = you.getStatAg();
	else if (stat == "In") statNum = you.getStatIn();
	else if (stat == "Gu") statNum = you.getStatGu();
	if (mod != 0) statNum += mod;
	for (int i = 0; i < statNum; ++i) roll += makeRoll();
	return roll;
}

int startGame::makeRoll(){ return (rand() % 6) + 1; }

void startGame::merchantMeat(Player &you) {
	cout << "[merchantMeat] You encounter a traveling butcher." << endl;
	you.printTradeInfo();
	// Minimal safe effect: if player has trade goods, convert to food/xp
	if (you.getHasTradeGoods()) {
		cout << "You trade goods for provisions and feel more experienced." << endl;
		you.setXp(you.getXp() + 2);
	} else {
		cout << "You don't have goods to trade." << endl;
	}
}
void startGame::merchantZ1(Player &you) {
	cout << "[merchantZ1] A local vendor offers trinkets." << endl;
	you.printTradeInfo();
	if (you.getHasTradeGoods()) { you.setXp(you.getXp() + 1); cout << "Small trade completed." << endl; }
}
void startGame::merchantZ2(Player &you) {
	cout << "[merchantZ2] A refined stall sells rare items." << endl;
	you.printTradeInfo();
	if (you.getHasTradeGoods()) { you.setXp(you.getXp() + 3); cout << "You managed a profitable trade." << endl; }
}

void startGame::outSide(){ if (!mapList0.empty()) takeAction(*mapList0[0], player); }
void startGame::lastFloor(){
	cout << "You reach the last floor. The city trembles as you step forward." << endl;
	// Simple success state for now
	cout << "End of demo: congratulations on reaching the last floor." << endl;
}

bool startGame::uniqueSelector(Foe foe){
	if (!foe.getUniqueAI()) return false;
	string name = foe.getName();
	if (name.find("Highborn") != string::npos) { this->highBornAi(foe); return true; }
	if (name.find("wire") != string::npos || name.find("Wire") != string::npos) { this->wireTrap(foe); return true; }
	if (name.find("door") != string::npos || name.find("Door") != string::npos) { this->doorTrap(foe); return true; }
	if (name.find("Abrupt") != string::npos || name.find("Assailant") != string::npos) { this->abruptAssialant(foe); return true; }
	return false;
}

void startGame::abruptAssialant(Foe &foe){
	// Minimal scripted ambush: player may notice or take damage
	cout << "An abrupt assailant lunges from the shadows!" << endl;
	int notice = this->makeRolls("Gu", 0, this->player);
	if (notice > 6) {
		cout << "You react in time and avoid the worst of it." << endl;
	} else {
		cout << "You are struck and suffer wounds." << endl;
		this->player.setHp(this->player.getHp() - 6);
	}
	foe.setUniqueAI(false);
	if (foe.getAlive()) this->startCombat(foe);
}

void startGame::highBornAi(Foe &foe){
	cout << "The Highborn youth glares at you with strange confidence." << endl;
	int roll = this->makeRolls("Ag", 0, this->player);
	if (roll > 8) cout << "You dodge his initial strike." << endl;
	else { cout << "You are grazed by a searing attack." << endl; this->player.setHp(this->player.getHp() - 4); }
	foe.setUniqueAI(false);
	if (foe.getAlive()) this->startCombat(foe);
}

void startGame::wireTrap(Foe &foe){
	cout << "You spot something like a wire running along the path." << endl;
	int detect = this->makeRolls("In", 0, this->player);
	if (detect > 7) {
		cout << "You disarm the mechanism before it fires." << endl;
		foe.setAlive(false);
		return;
	}
	// Failed to disarm: take damage and neutralize foe (trap triggered)
	cout << "A hidden mechanism fires! You are hit by shrapnel." << endl;
	this->player.setHp(this->player.getHp() - 5);
	foe.setAlive(false);
}

void startGame::doorTrap(Foe &foe){
	cout << "You notice the door looks suspicious." << endl;
	int detect = this->makeRolls("Gu", 0, this->player);
	if (detect > 6) {
		cout << "You find and disable the cord-latched trap." << endl;
		foe.setAlive(false);
		return;
	}
	cout << "A blade swings down and injures you." << endl;
	this->player.setHp(this->player.getHp() - 5);
	foe.setAlive(false);
}

void startGame::startCombat(Foe &foe){
	cout << "Engaging in combat with: " << foe.getName() << endl;
	if (!foe.getAlive()) { cout << "But the foe is already down." << endl; return; }
	// Interactive turn-based loop: delegate action handling to Player::checkInputCombat
	while (this->player.getHp() > 0 && foe.getAlive()) {
		cout << "Your HP: " << this->player.getHp() << " | Foe: " << (foe.getAlive() ? "alive" : "dead") << "\n";
		cout << "Choose action: (a)ttack, (f)lee, (u)se item: ";
		string actionLine;
		char action = 'a';
		if (std::getline(cin, actionLine) && !actionLine.empty())
			action = tolower((unsigned char)actionLine[0]);
		int result = this->player.checkInputCombat(action, foe);
		if (result == 0) { cout << "Unknown action." << endl; continue; }
		if (result == 2) { cout << "You fled the combat." << endl; return; }
		if (result == 3) { cout << "Foe defeated!" << endl; this->player.setXp(this->player.getXp() + 5); break; }

		// foe's turn (if still alive)
		if (foe.getAlive()) {
			int fAttack = foe.getPower() + this->makeRoll();
			int damage = fAttack - this->player.getArmor();
			if (damage <= 0) damage = 1;
			cout << "Foe attacks for " << damage << " damage." << endl;
			this->player.setHp(this->player.getHp() - damage);
			if (this->player.getHp() <= 0) { cout << "You have been slain in combat." << endl; break; }
		}
	}
}

void printTutorial(){ cout << "Tutorial (minimal)\n"; }
