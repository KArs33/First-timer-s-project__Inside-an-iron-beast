
#include "map.h"
#include <iostream>

MAP::MAP() : blank(true), mapFoe(false), foeID(0), location(false), isCurLoc(false), thisLoc(nullptr) {}

void MAP::setFoe() { mapFoe = !mapFoe; }
void MAP::setFoeID(int num){ foeID = num; }
int MAP::getFoeID(){ return foeID; }
void MAP::setLoc() { location = !location; }
void MAP::setBlank() { blank = !blank; }
bool MAP::getFoe() { return mapFoe; }
bool MAP::getLoc() { return location; }
bool MAP::getBlank() { return blank; }

bool MAP::getIsCurLoc(){ return isCurLoc; }
void MAP::setIsCurLoc(){ isCurLoc = !isCurLoc; }
bool MAP::checkCurLocFoe(){ if (isCurLoc && mapFoe) { if (thisFoeObj.getAlive()) return true; } return false; }

loc* MAP::getLocObj(){ return thisLoc; }
