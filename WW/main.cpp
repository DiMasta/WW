#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <climits>
#include <ctime>

using namespace std;

const char* MOVE_BUILD_ACTION = "MOVE&BUILD";

const int INVALID_COORD = -1;
const int INVALID_INDEX = -1;

const char* INVALID_STR= "";

string N = "N";
string NE = "NE";
string E = "E";
string SE = "SE";
string S = "S";
string SW = "SW";
string W = "W";
string NW = "NW";

string getOppositeDir(string moveDir) {
	string oppositeMoveDir = "";
	if (N == moveDir) { oppositeMoveDir = S; }
	else if (NE == moveDir) { oppositeMoveDir = SW; }
	else if (E == moveDir) { oppositeMoveDir = W; }
	else if (SE == moveDir) { oppositeMoveDir = NW; }
	else if (S == moveDir) { oppositeMoveDir = N; }
	else if (SW == moveDir) { oppositeMoveDir = NE; }
	else if (W == moveDir) { oppositeMoveDir = E; }
	else if (NW == moveDir) { oppositeMoveDir = SE; }

	return oppositeMoveDir;
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Coords {
public:
	Coords();
	Coords(int xCoord, int yCoord);

	int getXCoord() const { return xCoord; }
	int getYCoord() const { return yCoord; }

	void debug() const;
private:
	int xCoord;
	int yCoord;
};

//*************************************************************************************************************
//*************************************************************************************************************

Coords::Coords() :
	xCoord(INVALID_COORD),
	yCoord(INVALID_COORD)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords::Coords(
	int xCoord,
	int yCoord
) :
	xCoord(xCoord),
	yCoord(yCoord)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

void Coords::debug() const {
	cerr << "Position: X=" << xCoord << ", Y=" << yCoord << endl;
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Action {
public:
	Action();

	void filldata(string type, int unitIndex, string moveDir, string buildDir);
	void perform() const;
	void perFormInverse() const;
	bool isValid() const;

	string getMoveDir() const {
		return moveDir;
	}

	void debug() const;

private:
	string type;
	int unitIndex;
	string moveDir;
	string buildDir;
};

//*************************************************************************************************************
//*************************************************************************************************************

Action::Action() :
	type(INVALID_STR),
	unitIndex(INVALID_INDEX),
	moveDir(INVALID_STR),
	buildDir(INVALID_STR)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

void Action::filldata(string type, int unitIndex, string moveDir, string buildDir) {
	this->type = type;
	this->unitIndex = unitIndex;
	this->moveDir = moveDir;
	this->buildDir = buildDir;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Action::perform() const {
	cout << type << ' ' << unitIndex << ' ' << moveDir << ' ' << buildDir << endl;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Action::perFormInverse() const {
	cout << type << ' ' << unitIndex << ' ' << moveDir << ' ' << buildDir << endl;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Action::isValid() const {
	return INVALID_INDEX != unitIndex;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Action::debug() const {
	cerr << "Type=" << type << " Move dir=" << moveDir << " Build dir=" << buildDir << endl;
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Unit {
public:
	Unit();

	Coords getPosition() const {
		return position;
	}

	int getLegalActionsCount() const {
		return legalActionsCount;
	}

	Action* getLegalActions() const {
		return legalActions;
	}

	void makeTurn();

	void setPosition(Coords position) { this->position = position; }
	void setLegalactionsCount(int legalActionsCount) { this->legalActionsCount = legalActionsCount; }
	void setLegalActions(Action* legalActions) { this->legalActions = legalActions; }

	void initLegalactions(int legalActionsCount);
	void fillActionData(int actionIdx, int unitIndex, string type, string moveDir, string buildDir);
	void performAction(int actionIdx) const;

	void debug() const;

private:
	Coords position;
	int legalActionsCount;
	Action* legalActions;

	string lastMoveDir;
};

//*************************************************************************************************************
//*************************************************************************************************************

Unit::Unit() :
	position(),
	legalActionsCount(0),
	legalActions(NULL),
	lastMoveDir(INVALID_STR)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::makeTurn() {
	if ("" != lastMoveDir) {
		string opposite = getOppositeDir(lastMoveDir);
		cout << MOVE_BUILD_ACTION << " 0 " << opposite << " " << lastMoveDir << endl;
		lastMoveDir = opposite;
	}
	else {
		string moveDir = legalActions[0].getMoveDir();
		string opposite = getOppositeDir(moveDir);
		cout << MOVE_BUILD_ACTION << " 0 " << moveDir << " " << opposite << endl;
		lastMoveDir = moveDir;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::initLegalactions(int legalActionsCount) {
	this->legalActionsCount = legalActionsCount;
	legalActions = new Action[legalActionsCount];
}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::fillActionData(int actionIdx, int unitIndex, string type, string moveDir, string buildDir) {
	legalActions[actionIdx].filldata(type, unitIndex, moveDir, buildDir);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::performAction(int actionIdx) const {
	legalActions[actionIdx].perform();
}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::debug() const {
	position.debug();
	for (int actionIdx = 0; actionIdx < legalActionsCount; ++actionIdx) {
		legalActions[actionIdx].debug();
	}
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Player {
public:
	Player();
	~Player();

	int getUnitsCount() const {
		return unitsCount;
	}

	void setUnitsCount(int unitsCount) { this->unitsCount = unitsCount; }

	void initUnits(int unitsCount);
	Unit* getUnit(int index) const;

	void debug() const;
private:
	int unitsCount;
	Unit* units;
};

//*************************************************************************************************************
//*************************************************************************************************************

Player::Player() :
	unitsCount(0),
	units(NULL)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Player::~Player() {
	if (units) {
		delete[] units;
		units = NULL;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Player::initUnits(int unitsCount) {
	this->unitsCount = unitsCount;
	units = new Unit[unitsCount];
}

//*************************************************************************************************************
//*************************************************************************************************************

Unit* Player::getUnit(int index) const {
	return (units + index);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Player::debug() const {
	for (int unitIdx = 0; unitIdx < unitsCount; ++unitIdx) {
		units[unitIdx].debug();
	}
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Me : public Player {
public:
private:
};

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Enemy : public Player {
public:
private:
};

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Game {
public:
	Game();
	~Game();

	void initGame();
	void gameLoop();
	void getGameInput();
	void getTurnInput();
	void turnBegin();
	void makeTurn();
	void turnEnd();
	void play();

	void debug() const;

private:
	int turnsCount;
	int size;
	int unitsPerPlayer;

	Me* me;
	Enemy* enemy;
};

//*************************************************************************************************************
//*************************************************************************************************************

Game::Game() :
	turnsCount(0),
	size(0),
	unitsPerPlayer(0)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Game::~Game() {
	if (me) {
		delete me;
		me = NULL;
	}

	if (enemy) {
		delete enemy;
		enemy = NULL;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::initGame() {
	me = new Me();
	enemy = new Enemy();
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::gameLoop() {
	while (true) {
		getTurnInput();
		turnBegin();
		makeTurn();
		turnEnd();
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::getGameInput() {
	cin >> size;
	cin >> unitsPerPlayer;

	me->initUnits(unitsPerPlayer);
	enemy->initUnits(unitsPerPlayer);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::getTurnInput() {
	for (int i = 0; i < size; i++) {
		string row;
		cin >> row;
	}

	for (int i = 0; i < unitsPerPlayer; i++) {
		int unitX, unitY;
		cin >> unitX >> unitY;
		me->getUnit(i)->setPosition(Coords(unitX, unitY));
	}

	for (int i = 0; i < unitsPerPlayer; i++) {
		int otherX, otherY;
		cin >> otherX >> otherY;
		enemy->getUnit(i)->setPosition(Coords(otherX, otherY));
	}

	int legalActions;
	cin >> legalActions;
	me->getUnit(0)->initLegalactions(legalActions);

	for (int i = 0; i < legalActions; i++) {
		string type, dir1, dir2;
		int index;
		cin >> type >> index >> dir1 >> dir2;
		//cerr << "Legal action: " << " Type:" << type << " Idx:" << index << " Dir1:" << dir1 << " Dir2:" << dir2 << endl;

		me->getUnit(index)->fillActionData(i, index, type.c_str(), dir1.c_str(), dir2.c_str());
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::turnBegin() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::makeTurn() {
	me->getUnit(0)->makeTurn();
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::turnEnd() {
	++turnsCount;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::play() {
	initGame();
	getGameInput();
	gameLoop();
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::debug() const {
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

#ifdef TESTS
#include "debug.h"
#endif // TESTS

int main(int argc, char** argv) {
#ifdef TESTS
	doctest::Context context;
	int res = context.run();
#else
	Game game;
	game.play();
#endif // TESTS

	return 0;
}

/*
mapIndex=0
seed=313480975
*/