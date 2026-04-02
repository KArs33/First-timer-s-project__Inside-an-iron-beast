#ifndef _LOC_H_
#define _LOC_H_

#include <string>
using namespace std;
//as with the other files, setters getters and constructors are stored in .h not .cpp

//MAJOR TODO: once the loc.txt file is done, record what location IDs exist where I will need custom script. 
//The custom scripts need to be stored here (or loc.cpp)

struct loc{
public:
	loc() {};
	loc(string nm, string des, int numop, int z, string o1,string opS1, int opN1, string op1f, string op1p, 
		string o2, string opS2, int opN2, string op2f, string op2p, string o3, string opS3, 
		int opN3, string op3f, string op3p, string pop_1, string fop_1, string pop_2, string fop_2, string pop_3, string fop_3, int loc_ID)
	{
		name = nm; description = des, options = numop; zone = z;
		op1 = o1; op1Stat = opS1; op1StatNum = opN1; op1fail = op1f; op1pass = op1p;
		op2 = o2; op2Stat = opS2; op2StatNum = opN2; op2fail = op2f; op2pass = op2p;
		op3 = o3; op3Stat = opS3; op3StatNum = opN3; op3fail = op3f; op3pass = op3p;
        pop1 = pop_1,  fop1=fop_1,  pop2=pop_2,  fop2=fop_2,  pop3=pop_3,  fop3=fop_3; locID=loc_ID;
	}

	bool getExplored() { return explored; };
	string getName() { return name; };
	string getDescription() { return description; };
	int getOptions() { return options; };
	string getOp1Stat() { return op1Stat; };
	int getOp1StatNum  (){ return op1StatNum; };
	string getOp2Stat  (){ return op2Stat; };
	int getOp2StatNum  (){ return op2StatNum; };
	string getOp3Stat() { return op3Stat; };
	int getOp3StatNum() { return op3StatNum; };
	string getOp1fail() { return op1fail; };
	string getOp1pass() { return op1pass; };
	string getOp2fail() { return op2fail; };
	string getOp2pass() { return op2pass; };
	string getOp3fail() { return op3fail; };
	string getOp3pass() { return op3pass; };
	void setDescription(string newstring){ description = newstring; };
	void setOption(int num){ options = num; };
	void setExplored() { explored = true; };
	int getZone() { return zone; };
	void setZone(int newZone) { zone = newZone; };
	void setOp1(string x) { op1 = x; };
	string getOp1() { return op1; };
	void setOp2(string x) { op2 = x; };
	string getOp2() { return op2; };
	void setOp3(string x) { op3 = x; };
	string getOp3() { return op3; };
	bool operator==( loc rhs){
		if(rhs.getName()!="The path continues")return false; else return true;
	}
private:
	bool explored = 0; //0=unexplored, 1=explored
	string name ="The path continues"; //name of place
	string description ="You search for new and interesting places to go."; //what is the place like
	int options = 1; //How many choices you may make while in the location. Min = 0, Max =3
	string op1Stat = "Gu"; //stat tied to first option
	int op1StatNum=0 ; //amount of stat needed
	string op2Stat = "";
	int op2StatNum =0;
	string op3Stat = "";
	int op3StatNum =0;
	string op1fail = ""; //what happens when you pass
	string op1pass = ""; //what happens when you fail
	string op2fail = ""; 
	string op2pass = "";
	string op3fail = "";
	string op3pass = "";
	string op1="I keep looking"; 
	string op2, op3; //description of the action to take
	string pop1, fop1,  pop2,  fop2,  pop3,  fop3 = "";
	int zone =-1; // 0=is the prologue, Then there are 3 main zones. Outside, Shanty/Upper, Middle, Core. 
				//4 is for the boss fight. -1 is used for a default loc, and I dont know if I will implement it in any way
	int locID=0;
};


#endif