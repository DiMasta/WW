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

const int USE_HARDCODED_INPUT = 1;

const string INVALID_STR = "";
const string MOVE_BUILD_ACTION = "MOVE&BUILD";

const string N = "N";
const string NE = "NE";
const string E = "E";
const string SE = "SE";
const string S = "S";
const string SW = "SW";
const string W = "W";
const string NW = "NW";

const int DIRECTION_COUNT = 8;
const int INVALID_COORD = -1;
const int INVALID_INDEX = -1;
const int INVALID_NODE_DEPTH = -1;
const int GAME_UNITS_COUNT = 2;
const int PLAYER_UNITS_COUNT = 1;

const char INVALID_CELL = '-';
const char DOT = '.';
const char ENEMY = 'E';
const char ME = 'M';
const char LEVEL_0 = '0';
const char LEVEL_1 = '1';
const char LEVEL_2 = '2';
const char LEVEL_3 = '3';

enum MiniMaxActionType {
	MMAT_INVALID = -1,
	MMAT_MOVE,
	MMAT_BUILD,
};

enum Posetion {
	P_INAVALID_POSETION = -1,
	P_MINE,
	P_ENEMY,
};

enum Direction {
	D_INVALID_DIRECTION = -1,
	D_N = 0,
	D_NE,
	D_E,
	D_SE,
	D_S,
	D_SW,
	D_W,
	D_NW,
};

int DIR_X[DIRECTION_COUNT] = { 0,  1, 1, 1, 0, -1, -1, -1};
int DIR_Y[DIRECTION_COUNT] = {-1, -1, 0, 1, 1,  1,  0, -1};

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Coords {
public:
	Coords();
	Coords(int xCoord, int yCoord);

	int getXCoord() const {
		return xCoord;
	}

	int getYCoord() const {
		return yCoord;
	}

	void setXCoord(int xCoord) { this->xCoord = xCoord; }
	void setYCoord(int yCoord) { this->yCoord = yCoord; }

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

class Grid {
public:
	Grid();
	~Grid();

	int getGridSize() const {
		return gridSize;
	}

	char getCell(Coords psoiton) const;
	void setCell(Coords position, char c);
	void init(int gridSize);
	bool validPosition(Coords position) const;
	bool canMoveFromTo(Coords from, Coords to) const;
	bool canBuildFromTo(Coords from, Coords to) const;
	bool playableCell(Coords positon) const;

private:
	int gridSize;
	char** grid;
};

//*************************************************************************************************************
//*************************************************************************************************************

Grid::Grid() :
	gridSize(0),
	grid(NULL)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Grid::~Grid() {
	for (int rowIdx = 0; rowIdx < gridSize; ++rowIdx) {
		delete[] grid[rowIdx];
	}

	delete[] grid;
}

//*************************************************************************************************************
//*************************************************************************************************************

char Grid::getCell(Coords position) const {
	return grid[position.getXCoord()][position.getYCoord()];
}

//*************************************************************************************************************
//*************************************************************************************************************

void Grid::setCell(Coords position, char c) {
	grid[position.getXCoord()][position.getYCoord()] = c;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Grid::init(int gridSize) {
	this->gridSize = gridSize;

	grid = new char*[gridSize];
	for (int rowIdx = 0; rowIdx < gridSize; ++rowIdx) {
		grid[rowIdx] = new char[gridSize];
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Grid::validPosition(Coords position) const {
	bool valid = false;
	bool validX = position.getXCoord() >= 0 && position.getXCoord() < gridSize;
	bool validY = position.getYCoord() >= 0 && position.getYCoord() < gridSize;

	if (validX && validY) {
		valid = playableCell(position);
	}

	return valid;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Grid::canMoveFromTo(Coords from, Coords to) const {
	bool canMove = false;
	
	char cellFromLevel = getCell(from);
	char cellToLevel = getCell(to);

	return cellFromLevel + 1 >= cellToLevel;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Grid::canBuildFromTo(Coords from, Coords to) const {
	char cellToLevel = getCell(to);

	return cellToLevel < LEVEL_3;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Grid::playableCell(Coords position) const {
	char cellValue = getCell(position);

	return cellValue >= LEVEL_0 && cellValue <= LEVEL_3;
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

class MiniMaxAction {
public:
	MiniMaxAction();
	MiniMaxAction(MiniMaxActionType type, Coords coords);

	MiniMaxActionType getType() const {
		return type;
	}

	Coords getCoords() const {
		return coords;
	}

	void setType(MiniMaxActionType type) { this->type = type; }
	void setCoords(Coords coords) { this->coords = coords; }

	bool isValid() const;

private:
	MiniMaxActionType type;
	Coords coords;
};

//*************************************************************************************************************
//*************************************************************************************************************

MiniMaxAction::MiniMaxAction() :
	type(MMAT_INVALID),
	coords()
{
}

//*************************************************************************************************************
//*************************************************************************************************************

MiniMaxAction::MiniMaxAction(
	MiniMaxActionType type,
	Coords coords
) :
	type(type),
	coords(coords)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

bool MiniMaxAction::isValid() const {
	return MMAT_INVALID != type;
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

	Posetion getPosetion() const {
		return posetion;
	}

	void setPosition(Coords position) { this->position = position; }
	void setLegalactionsCount(int legalActionsCount) { this->legalActionsCount = legalActionsCount; }
	void setLegalActions(Action* legalActions) { this->legalActions = legalActions; }
	void setPosetion(Posetion posetion) { this->posetion = posetion; }

	void initLegalactions(int legalActionsCount);
	void fillActionData(int actionIdx, int unitIndex, string type, string moveDir, string buildDir);
	void performAction(int actionIdx) const;
	void move(Direction direction);
	Coords build(Direction direction);
	void makeTurn();
	void init();
	void addMiniMaxAction(MiniMaxAction newAction);

	void debug() const;

private:
	Coords position;
	int legalActionsCount;
	Action* legalActions;
	Posetion posetion;
	int minimaxActionsCount;
	MiniMaxAction* minimaxActions;
};

//*************************************************************************************************************
//*************************************************************************************************************

Unit::Unit() :
	position(),
	legalActionsCount(0),
	legalActions(NULL),
	posetion(P_INAVALID_POSETION),
	minimaxActionsCount(0),
	minimaxActions(NULL)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::makeTurn() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::init() {

}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::addMiniMaxAction(MiniMaxAction newAction) {
	++minimaxActionsCount;
	MiniMaxAction* temp = new MiniMaxAction[minimaxActionsCount];

	for (int actionIdx = 0; actionIdx < minimaxActionsCount - 1; ++actionIdx) {
		temp[actionIdx] = minimaxActions[actionIdx];
	}

	temp[minimaxActionsCount - 1] = newAction;
	
	delete[] minimaxActions;
	minimaxActions = temp;
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

void Unit::move(Direction direction) {
	position.setXCoord(position.getXCoord() + DIR_X[direction]);
	position.setYCoord(position.getYCoord() + DIR_Y[direction]);
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords Unit::build(Direction direction) {
	int xBuildCoord = position.getXCoord() + DIR_X[direction];
	int yBuildCoord = position.getYCoord() + DIR_Y[direction];

	return Coords(xBuildCoord, yBuildCoord);
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

class State {
public:
	State();
	~State();

	Grid* getGrid() const {
		return grid;
	}

	void simulate();
	int evaluate() const;
	void init(int gridSize);
	void setMiniMaxUnitTurnActions();
	Unit& getUnit(int unitIdx) { return units[unitIdx]; }

	void debug() const;
private:
	Grid* grid;
	Unit units[GAME_UNITS_COUNT];
};

//*************************************************************************************************************
//*************************************************************************************************************

State::State() :
	grid()
{
	for (int unitIdx = 0; unitIdx < GAME_UNITS_COUNT; ++unitIdx) {
		units[unitIdx].init();
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

State::~State() {
	if (grid) {
		delete grid;
		grid = NULL;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::simulate() {
}

//*************************************************************************************************************
//*************************************************************************************************************

int State::evaluate() const {
	return 0;
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::init(int gridSize) {
	grid = new Grid();
	grid->init(gridSize);
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::setMiniMaxUnitTurnActions() {
	for (int unitIdx = 0; unitIdx < GAME_UNITS_COUNT; ++unitIdx) {
		Coords unitPosition = units[unitIdx].getPosition();
		char cell = grid->getCell(unitPosition);

		for (int dirIdx = 0; dirIdx < DIRECTION_COUNT; ++dirIdx) {
			Coords newPosition;
			newPosition.setXCoord(unitPosition.getXCoord() + DIR_X[dirIdx]);
			newPosition.setYCoord(unitPosition.getYCoord() + DIR_Y[dirIdx]);

			if (grid->validPosition(newPosition)) {
				MiniMaxAction newAction;

				if (grid->canMoveFromTo(unitPosition, newPosition)) {
					newAction.setCoords(newPosition);
					newAction.setType(MMAT_MOVE);
				}

				if (grid->canBuildFromTo(unitPosition, newPosition)) {
					newAction.setCoords(newPosition);
					newAction.setType(MMAT_BUILD);
				}

				if (newAction.isValid()) {
					units[unitIdx].addMiniMaxAction(newAction);
				}
			}
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::debug() const {
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Node {
public:
	Node();

	void setState(const State& state) { this->state = state; }
	void setNodeDepth(int nodeDepth) { this->nodeDepth = nodeDepth; }

	Node* createChild();
	void addChild(Node* child);
	void deleteTree();

private:
	int nodeDepth;
	State state;
	Node* parent;
	Node* children;
};

//*************************************************************************************************************
//*************************************************************************************************************

Node::Node() :
	nodeDepth(INVALID_NODE_DEPTH),
	state(),
	parent(NULL),
	children(NULL)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Node* Node::createChild() {
	return nullptr;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Node::addChild(Node* child) {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Node::deleteTree() {
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class MiniMax {
public:
	MiniMax();
	~MiniMax();

	void init(const State& state);
	void run();
private:
	Node tree;
};

//*************************************************************************************************************
//*************************************************************************************************************

MiniMax::MiniMax() :
	tree()
{
}

//*************************************************************************************************************
//*************************************************************************************************************

MiniMax::~MiniMax() {
	tree.deleteTree();
}

//*************************************************************************************************************
//*************************************************************************************************************

void MiniMax::init(const State& state) {
	tree.setNodeDepth(0);
	tree.setState(state);
}

//*************************************************************************************************************
//*************************************************************************************************************

void MiniMax::run() {
}

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

	State turnState;
	MiniMax minimax;
};

//*************************************************************************************************************
//*************************************************************************************************************

Game::Game() :
	turnsCount(0),
	size(0),
	unitsPerPlayer(0),
	turnState(),
	minimax()
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Game::~Game() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::initGame() {
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
	if (USE_HARDCODED_INPUT) {
		size = 5;
		unitsPerPlayer = 1;
	}
	else {
		cin >> size;
		cin >> unitsPerPlayer;

		//cerr << "size=" << size << endl;
		//cerr << "unitsPerPlayer" << unitsPerPlayer << endl;
	}

	turnState.init(size);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::getTurnInput() {
	for (int rowIdx = 0; rowIdx < size; ++rowIdx) {
		for (int colIdx = 0; colIdx < size; ++colIdx) {
			char c;

			if (USE_HARDCODED_INPUT) {
				c = LEVEL_0;
			}
			else {
				cin >> c;
				cerr << c;
			}

			turnState.getGrid()->setCell(Coords(rowIdx, colIdx), c);
		}
		//cerr << endl;
	}

	for (int unitIdx = 0; unitIdx < GAME_UNITS_COUNT; ++unitIdx) {
		int unitX, unitY;

		if (USE_HARDCODED_INPUT) {
			if (0 == unitIdx) { unitX = 3; unitY = 0; }
			if (1 == unitIdx) { unitX = 0; unitY = 0; }
		}
		else {
			cin >> unitX >> unitY;
			cerr << unitX << ' ' << unitY << endl;
		}

		Posetion posetion = P_MINE;
		if (unitIdx >= PLAYER_UNITS_COUNT) {
			posetion = P_ENEMY;
		}

		turnState.getUnit(unitIdx).setPosition(Coords(unitX, unitY));
		turnState.getUnit(unitIdx).setPosetion(posetion);
	}

	if (!USE_HARDCODED_INPUT) {
		int legalActions;
		cin >> legalActions;
		//me->getUnit(0)->initLegalactions(legalActions);

		for (int i = 0; i < legalActions; i++) {
			string type, dir1, dir2;
			int index;
			cin >> type >> index >> dir1 >> dir2;
			//cerr << "Legal action: " << " Type:" << type << " Idx:" << index << " Dir1:" << dir1 << " Dir2:" << dir2 << endl;

			//me->getUnit(index)->fillActionData(i, index, type.c_str(), dir1.c_str(), dir2.c_str());
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::turnBegin() {
	turnState.setMiniMaxUnitTurnActions();
	minimax.init(turnState);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::makeTurn() {
	minimax.run();
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
