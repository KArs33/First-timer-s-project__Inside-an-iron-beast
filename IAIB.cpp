// Single-source minimal IAIB implementation
//Kevin Arsenault
//Big dumb project to test out a bunch of things with my minimal coding experience
//1/18/2025
//first game test . game
//None save those named Kevin R. Arsenault are allowed to claim ownership over this project. Any may repost this project elsewhere so long as proper citation of origin is given. Any may edit this program so long as the origin of the program remains cited.

#include "IAIB.h"
#include "merchant.h"
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

	// ── Merchant stock ──────────────────────────────────────────────────────
	// MerchantItem(name, description, cost, stock, effectTag, effectAmt)
	// "effectTag" is read by the applyEffect block inside runShop().
	// To add a new item: add a MerchantItem here and a matching tag case there.

	stockMeat = {
		MerchantItem("Pottage (3 food)",
		             "Beef and onions sealed in a clay jar and cooked until compleatly soft. A layer of fat protects the food from spoiling. ",
		             1, 3, "food", 3),
	};

	stockZ1 = {
		MerchantItem("Light Armour",
		             "Padded leather lightly reinforced with chainmail.",
		             4, 1, "armor_l", 1),
		MerchantItem("Javelin (x3)",
		             "Two balanced throwing spears — useful for opening a gap.",
		             2, 3, "javelin", 3),
		MerchantItem("Medicine pouch",
		             "String and needle. Herbal pastes, and pure alchohol.",
		             2, 2, "meds", 1),
	};

	stockZ2 = {
		MerchantItem("Rifle Ammunition (x5)",
		             "A cartidge of 6.5x52 caliber ammo.",
		             2, 4, "bullet", 5),
		MerchantItem("Fragmentation Grenade",
		             "Supposedly sold to the merchant by a defector, a full brace of gernades can be found for sale",
		             3, 5, "grenade", 1),
		MerchantItem("Medium Armour",
		             "Stripped from a Vigilis sergeant. Heavy, yet reassuring.",
		             4, 1, "armor_m", 1),

		MerchantItem("Officer's Sword",
		             "This fourign style of sword was obviously loot from some far off land, used by an unfortunate officer during the battle here. May it serve you better than him.",
		             4, 1, "weapon2", 1),
	};

	// stockZ3 items are intentionally left empty until Zone-3 content is written.
	// Add MerchantItems here when ready.
	stockZ3 = {};

	// ── end merchant stock ──────────────────────────────────────────────────

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
	// Keep prologue locations in file order; shuffle the later zones.
	std::random_device rd; std::mt19937 g(rd());
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

	//clear the map
	for (int x =0; x< WidthMAPMAX; ++x){
		for(int y = 0; y <HeightMapMax; ++y){
			gameMap[x][y] = MAP(); //cell is now blank
		}
	}

	// top to bottom build of cells
	// row 0 is the starting row, row 1 is one row down, and so on
	vector<pair<int,int>> cells;
	for (int y =0; y <HeightMapMax; ++y){
		for(int x=0; x< WidthMAPMAX; ++x){
			cells.emplace_back(x,y);
		}
	}

	//place locations and foes into map cells
	int exitCol= WidthMAPMAX /2;
	int exitRow = HeightMapMax -1;
	int locIdx=0, foeIdx =0;
	for (auto &[x,y]: cells){
       if (x == exitCol && y == exitRow) continue; // reserve for exit
        if (locIdx < (int)locVec.size()) {
            Foe f = (foeIdx < (int)foeVec.size()) ? foeVec[foeIdx++] : Foe();
            gameMap[x][y] = MAP(f, foeIdx, locVec[locIdx++]);
        }		
	}

	//locates where the exit hex is, and activates it
	gameMap[exitCol][exitRow].setIsExit(true);

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
	while (player.getHp()>0) {
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
	//the player is now dead. 
	cout<<"The world continues to rotate. Only, now it does so without you"; return;
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

bool startGame::movePlayer(const string &dir) {
    int x, y;
    if (!findCurLocation(x, y)) return false;
    auto [nx, ny] = hexNeighbor(x, y, dir); 
    if (nx < 0 || ny < 0 || nx >= WidthMAPMAX || ny >= HeightMapMax) return false;
    if (gameMap[nx][ny].getBlank()) return false;

    setCurLocation(nx, ny);

    // Zone exit check
    if (gameMap[nx][ny].getIsExit()) {
        cout << "You find a passage deeper into the city...\n";
        advanceZone(); 
        return true;
    }

    if (gameMap[nx][ny].getLoc()) {
        loc* lp = gameMap[nx][ny].getLocObj();
        if (lp) takeAction(*lp, player);
    }
    return true;
}

void startGame::advanceZone(){
	//I need a way to detect if an incorrect curMapZone exists, or if the function is being called in bad places in my code
	curMapZone++;
	switch(curMapZone){
		case 2:
			fillInMap(mapList2, foeV2, 2);
			break;
		case 3:
			fillInMap(mapList3, foeV3, 3);
			break;
		case 4:
			fillInMap(mapList4, foeV4, 4);
			break;
		default:
			lastFloor();
			break;
	}
	setCurLocation(WidthMAPMAX/2,0);
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
	//cout << "[DEBUG] need="<<need<<" stat='"<<stat<<"' roll="<<roll<<" xpBefore="<<xpBefore<<"\n";
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

// ── runShop ───────────────────────────────────────────────────────────────
// Shared shopping loop used by all four merchants.
// Keeps looping (do-while) until the player enters 0 to leave.
// Merchants are never exhausted — the player can return any number of times
// and the remaining stock is remembered between visits.
void startGame::runShop(const string &merchantName, const string &greeting,
                        const string &farewell, vector<MerchantItem> &items,
                        Player &you)
{
	cout << "\n  [ " << merchantName << " ]\n";
	cout << "  " << greeting << "\n";

	string inputLine;
	int choice = -1;
	do {
		// ── print the shelf ────────────────────────────────────────────────
		cout << "\n  Trade goods on hand: " << you.getTradeGoods() << "\n";
		cout << "  ─────────────────────────────────────────\n";
		for (int i = 0; i < (int)items.size(); ++i) {
			string stockLabel = (items[i].stock > 0)
				? ("Stock: " + to_string(items[i].stock))
				: "SOLD OUT";
			cout << "  " << (i + 1) << ") " << items[i].name
			     << "  [Cost: " << items[i].cost << " | " << stockLabel << "]\n";
			cout << "     " << items[i].description << "\n";
		}
		cout << "  0) Leave\n";
		cout << "  > ";

		// ── read choice ────────────────────────────────────────────────────
		choice = -1;
		if (!getline(cin, inputLine)) break; // EOF / pipe closed
		try { choice = stoi(inputLine); } catch (...) { choice = -1; }

		if (choice == 0) break;

		if (choice < 1 || choice > (int)items.size()) {
			cout << "  That's not something I carry.\n";
			continue;
		}

		MerchantItem &item = items[choice - 1];

		if (item.stock <= 0) {
			cout << "  I'm all out of " << item.name << ".\n";
			continue;
		}
		if (you.getTradeGoods() < item.cost) {
			cout << "  You need " << item.cost << " trade goods for that — "
			     << "you only have " << you.getTradeGoods() << ".\n";
			continue;
		}

		// ── confirm ────────────────────────────────────────────────────────
		cout << "  " << item.name << " for " << item.cost
		     << " trade good(s). Deal? (y/n) > ";
		string confirm;
		if (!getline(cin, confirm)) break;
		if (confirm.empty() || tolower((unsigned char)confirm[0]) != 'y') {
			cout << "  Maybe next time.\n";
			continue;
		}

		// ── apply the purchase ─────────────────────────────────────────────
		you.setTradeGoods(you.getTradeGoods() - item.cost);
		item.stock--;

		// applyEffect: read the tag and call the right player setter
		const string &tag = item.effectTag;
		int amt = item.effectAmt;
		if      (tag == "food")    { you.setFood(you.getFood() + amt);
		                             cout << "  You pack away " << amt << " day(s) of food.\n"; }
		else if (tag == "meds")    { you.setMeds(you.getMeds() + amt);
		                             cout << "  You stow " << amt << " medkit(s).\n"; }
		else if (tag == "javelin") { you.setJavelin(you.getJavelin() + amt);
		                             cout << "  You strap " << amt << " javelin(s) to your back.\n"; }
		else if (tag == "bullet")  { you.setHasBullet(you.getHasBullet() + amt);
		                             cout << "  You pocket " << amt << " round(s).\n"; }
		else if (tag == "grenade") { you.setHasGrenade(you.getHasGrenade() + amt);
		                             cout << "  You add " << amt << " grenade(s) to your kit.\n"; }
		else if (tag == "energy")  { you.setHasEnergyPack(you.getHasEnergyPack() + amt);
		                             cout << "  You clip " << amt << " energy pack(s) to your belt.\n"; }
		else if (tag == "armor_l") { you.setArmor(1);
		                             cout << "  You pull on the light armour.\n"; }
		else if (tag == "armor_m") { you.setArmor(2);
		                             cout << "  The medium armour is heavy but reassuring.\n"; }
		else if (tag == "armor_h") { you.setArmor(3);
		                             cout << "  You buckle on the heavy armour.\n"; }
		else if (tag == "weapon1") { you.getHasWeapon1(true);
		                             cout << "  You shoulder the rifle.\n"; }
		else if (tag == "weapon2") { you.getHasWeapon2(true);
		                             cout << "  The officer's sword hangs at your hip.\n"; }
		else if (tag == "hp_max")  { you.setMaxHp(you.getMaxHp() + amt);
		                             you.setHp(you.getHp() + amt);
		                             cout << "  Maximum HP +" << amt << ".\n"; }
		else if (tag == "shield_rel") {
			int cur = you.getShieldRelic();
			you.setShieldRelic((cur < 0 ? 0 : cur) + amt);
			cout << "  Shield relic charges +" << amt << ".\n";
		}
		else { cout << "  (Nothing happens. The merchant shrugs.)\n"; }

		cout << "  Trade goods remaining: " << you.getTradeGoods() << "\n";

	} while (choice != 0);

	cout << "  " << farewell << "\n\n";
}

// ── Zone-1 · Food / Provisions merchant ─────────────────────────────────────
// A travelling butcher who followed the looters hoping to sell preserved meat
// and field medicine to anyone passing through.
void startGame::merchantMeat(Player &you) {
	runShop(
		"Arkell's Provisions",
		"Oi! Over here. You look like someone who hasn't eaten a hot meal in a week. "
		"Smart thing, stocking up while you can.",
		"Watch yourself in there. Come back if you run low.",
		stockMeat, you
	);
}

// ── Zone-1 · General salvager ────────────────────────────────────────────────
// A former city resident who stayed behind. He salvaged weapons and light gear
// from the outer sections and now trades them for travel goods.
void startGame::merchantZ1(Player &you) {
	runShop(
		"Salvager's Corner",
		"I know what you are. You're the same as me — came here looking for something "
		"worth having. Difference is I already found mine. Maybe we can help each other.",
		"Don't get killed before you can spend what you got from me.",
		stockZ1, you
	);
}

// ── Zone-2 · Specialist dealer ───────────────────────────────────────────────
// An independent arms dealer who has pushed deeper into the city than most.
// He carries rare ammunition, grenades, and armour stripped from inner defenders.
void startGame::merchantZ2(Player &you) {
	runShop(
		"The Quiet Stall",
		"Not many make it this far. You've earned the right to browse. "
		"If the price looks steep, consider what the alternative is.",
		"Spend those goods wisely. There aren't many places left to spend them.",
		stockZ2, you
	);
}

// ── Zone-3 · Core relic trader (STUB) ───────────────────────────────────────
// Placeholder until Zone-3 content and items are written.
// Add items to stockZ3 in the constructor and replace this greeting when ready.
void startGame::merchantZ3(Player &you) {
	runShop(
		"Vashti's Exchange",
		"... (The merchant eyes you silently. Their wares are not yet laid out.)",
		"Come back when there is more to offer.",
		stockZ3, you
	);
}

void startGame::outSide(){ if (!mapList0.empty()) takeAction(*mapList0[0], player); }
void startGame::lastFloor(){
	cout << "You reach the last floor. The city trembles as you step forward." << endl;
	// Simple success state for now
	cout << "End of demo: congratulations on reaching the last floor." << endl;
}

bool startGame::uniqueSelector(Foe &foe){
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
		cout << "Your HP: " << this->player.getHp() << " | Foe: " << (foe.getAlive() 
		? "alive" : "dead") << "\n";
		cout << "Choose action: (a)ttack, (f)lee, ";
		if(this->player.getJavelin()>0){cout<<"throw (j)avelin, "; }
		if(this->player.getHasGrenade()){cout<<"throw (g)renade, ";}
		if(this->player.getHasBullet()>0 && this->player.getHasWeapon1()){cout<<"fire (r)ifle, ";}
		
		cout << "(u)se item: ";
		string actionLine;
		char action = 'a';
		if (std::getline(cin, actionLine) && !actionLine.empty())
			action = tolower((unsigned char)actionLine[0]);
		int result = this->player.checkInputCombat(action, foe);
		if (result == 0) { cout << "Unknown action." << endl; continue; }
		if (result == 2) { cout << "You fled the combat." << endl; return; }
		if (result == 3) { cout << "Foe defeated!" << endl;  break; }

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
	//if you are here, player is dead

}

void printTutorial(){ cout << "Tutorial (minimal)\n"; }