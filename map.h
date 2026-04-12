#ifndef _MAP_H_
#define _MAP_H_

#include "foe.h"
#include "loc.h"

struct MAP {
public:
	MAP();
	MAP(Foe thisMapFoe, int foeNum, loc* mapLocation){
		// store foe by value and mark mapFoe if foe is non-default
		thisFoeObj = thisMapFoe;
		mapFoe = !(thisMapFoe == Foe());
		foeID = foeNum;
		if (mapLocation == nullptr) { blank = true; }
		else { thisLoc = mapLocation; blank = false; foeID = foeNum; }

		bool isExit = false;
	}

	void setIsExit();
	bool getIsExit();
	void setFoe();
	void setFoeID(int num);
	int getFoeID();
	void setLoc();
	void setBlank();
	bool getFoe();
	bool getLoc();
	bool getBlank();

	bool getIsCurLoc();
	void setIsCurLoc(bool val);
	bool checkCurLocFoe();

	// return stored location pointer (may be nullptr)
	loc* getLocObj();

private:
	bool blank = 1; //when 0, the location does not exist on the map
	bool mapFoe = 0; //when 0, there is no foe
	int foeID=0;
	Foe thisFoeObj; //store foe by value to avoid dangling pointers
	bool location = 0; //when 0, there is no interesting location
	bool isCurLoc =0;
	loc* thisLoc = nullptr;
	bool isExit;
};

#endif