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
#include <fstream>

using namespace std;

const int USE_HARDCODED_INPUT = 1;
const int PRINT_MINIMAX_TREE_TO_FILE = 0;
const int OUTPUT_FOR_DEBUG = 1;
const int MINIMAX_DEPTH = 4;
const int BREAK_TURN = 6;
const int USE_RAND_HEURISTIC = 0;

const int SCORE_WEIGHT = 800;
const int LEVELS_WEIGHT = 200;
const int UNIT_LEVEL_WEIGHT = 1000;
const int UNIT_ON_LEVEL_3_WEIGHT = 0;

const string INVALID_STR = "";
const string MOVE_BUILD_ACTION = "MOVE&BUILD";
const string PARENT_PATH = "P";
const string INVALID_OUTPUT = "NO_UNITS_TO_EXPAND";

const int DIRECTION_COUNT = 8;
const int INVALID_COORD = -1;
const int INVALID_INDEX = -1;
const int INVALID_NODE_DEPTH = -1;
const int GAME_UNITS_COUNT = 4;
const int PLAYER_UNITS_COUNT = 2;
const int BUILD_NODE_DEPTH = 2;
const int MOVE_NODE_DEPTH = 1;

const char PARENT_LABEL = 'P';
const char INVALID_CELL = '-';
const char DOT = '.';
const char ENEMY = 'E';
const char ME = 'M';
const char LEVEL_0 = '0';
const char LEVEL_1 = '1';
const char LEVEL_2 = '2';
const char LEVEL_3 = '3';

enum MaximizeMinimize {
	MM_MAXIMIZE = 0,
	MM_MINIMIZE
};

enum MinimaxActionType {
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

string DIR_STRS[DIRECTION_COUNT] = { "N", "NE", "E", "SE", "S", "SW", "W", "NW" };

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

	Coords& operator=(const Coords& other);
	bool operator==(const Coords& other);
	Coords operator+(const Coords& other);
	Coords& operator+=(const Coords& other);
	friend ostream& operator<<(ostream& os, const Coords& c);

	bool isValid() const;

	void debug() const;
private:
	int xCoord;
	int yCoord;
};

ostream& operator<<(ostream& os, const Coords& c) {
	os << "(" << c.xCoord << ", " << c.yCoord << ")";
	return os;
}

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

Coords& Coords::operator=(const Coords& other) {
	if (this != &other) {
		xCoord = other.xCoord;
		yCoord = other.yCoord;
	}

	return *this;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Coords::operator==(const Coords& other) {
	return (xCoord == other.xCoord) && (yCoord == other.yCoord);
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords Coords::operator+(const Coords& other) {
	return Coords(xCoord + other.xCoord, yCoord + other.yCoord);
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords& Coords::operator+=(const Coords& other) {
	xCoord += other.xCoord;
	yCoord += other.yCoord;

	return *this;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Coords::isValid() const {
	return xCoord > INVALID_COORD && yCoord > INVALID_COORD;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Coords::debug() const {
	cerr << "Position: X=" << xCoord << ", Y=" << yCoord << endl;
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords DIRECTIONS[DIRECTION_COUNT] = {
	Coords( 0, -1), // N
	Coords( 1, -1), // NE
	Coords( 1,  0), // E
	Coords( 1,  1), // SE
	Coords( 0,  1), // S
	Coords(-1,  1), // SW
	Coords(-1,  0), // W
	Coords(-1, -1)  // NW
};

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
	void copy(Grid* grid);
	void build(Coords coords);
	int getSurroundingLevels(Coords coords) const;
	void debugPrint(ofstream& file) const;

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
	return grid[position.getYCoord()][position.getXCoord()];
}

//*************************************************************************************************************
//*************************************************************************************************************

void Grid::setCell(Coords position, char c) {
	grid[position.getYCoord()][position.getXCoord()] = c;
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

	return cellToLevel <= LEVEL_3;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Grid::playableCell(Coords position) const {
	char cellValue = getCell(position);

	return cellValue >= LEVEL_0 && cellValue <= LEVEL_3;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Grid::copy(Grid* grid) {
	for (int rowIdx = 0; rowIdx < gridSize; ++rowIdx) {
		for (int colIdx = 0; colIdx < gridSize; ++colIdx) {
			this->grid[rowIdx][colIdx] = grid->grid[rowIdx][colIdx];
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Grid::build(Coords coords) {
	setCell(coords, getCell(coords) + 1);
}

//*************************************************************************************************************
//*************************************************************************************************************

int Grid::getSurroundingLevels(Coords coords) const {
	int surroudingLevels = 0;

	for (int dirIdx = 0; dirIdx < DIRECTION_COUNT; ++dirIdx) {
		Coords newPosition = coords + DIRECTIONS[dirIdx];

		if (validPosition(newPosition)) {
			char cell = getCell(newPosition);
			if (cell > LEVEL_0 && cell <= LEVEL_3) {
				surroudingLevels += cell - LEVEL_0;
			}
		}
	}
	return surroudingLevels;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Grid::debugPrint(ofstream& file) const {
	for (int rowIdx = 0; rowIdx < gridSize; ++rowIdx) {
		for (int colIdx = 0; colIdx < gridSize; ++colIdx) {
			file << grid[rowIdx][colIdx];
		}
		file << "\\n";
	}
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

class MinimaxAction {
public:
	MinimaxAction();
	MinimaxAction(MinimaxActionType type, Coords coords);

	MinimaxActionType getType() const {
		return type;
	}

	Coords getCoords() const {
		return coords;
	}

	void setType(MinimaxActionType type) { this->type = type; }
	void setCoords(Coords coords) { this->coords = coords; }

	bool isValid() const;
	void debugPrint(ofstream& file) const;

private:
	MinimaxActionType type;
	Coords coords;
};

//*************************************************************************************************************
//*************************************************************************************************************

MinimaxAction::MinimaxAction() :
	type(MMAT_INVALID),
	coords()
{
}

//*************************************************************************************************************
//*************************************************************************************************************

MinimaxAction::MinimaxAction(
	MinimaxActionType type,
	Coords coords
) :
	type(type),
	coords(coords)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

bool MinimaxAction::isValid() const {
	return MMAT_INVALID != type;
}

//*************************************************************************************************************
//*************************************************************************************************************

void MinimaxAction::debugPrint(ofstream& file) const {
	if (MMAT_MOVE == type) {
		file << "MOVE: ";
	}
	else {
		file << "BUILD: ";
	}

	file << coords;
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Unit {
public:
	Unit();
	~Unit();

	Coords getPosition() const {
		return position;
	}

	Posetion getPosetion() const {
		return posetion;
	}

	int getMinimaxActionsCout() const {
		return minimaxActionsCount;
	}

	MinimaxAction getMinimaxAction(int actionIdx) const {
		return minimaxActions[actionIdx];
	}

	int getScore() const {
		return score;
	}

	void setPosition(Coords position) { this->position = position; }
	void setPosetion(Posetion posetion) { this->posetion = posetion; }

	void move(Direction direction);
	Coords build(Direction direction);
	void init(int minimaxActionsCount);
	void addMinimaxAction(MinimaxAction newAction);
	void copy(Unit* unit);
	void move(Coords coords);
	void incrementScore();
	void clearActions();

	void debug() const;

private:
	Coords position;
	Posetion posetion;
	int minimaxActionsCount;
	MinimaxAction* minimaxActions;
	int score;
};

//*************************************************************************************************************
//*************************************************************************************************************

Unit::Unit() :
	position(),
	posetion(P_INAVALID_POSETION),
	minimaxActionsCount(0),
	minimaxActions(NULL),
	score(0)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Unit::~Unit() {
	clearActions();
}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::init(int minimaxActionsCount) {
	this->minimaxActionsCount = minimaxActionsCount;
	minimaxActions = new MinimaxAction[minimaxActionsCount];
}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::addMinimaxAction(MinimaxAction newAction) {
	++minimaxActionsCount;
	MinimaxAction* temp = new MinimaxAction[minimaxActionsCount];

	for (int actionIdx = 0; actionIdx < minimaxActionsCount - 1; ++actionIdx) {
		temp[actionIdx] = minimaxActions[actionIdx];
	}

	temp[minimaxActionsCount - 1] = newAction;
	
	delete[] minimaxActions;
	minimaxActions = temp;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::copy(Unit* unit) {
	init(unit->minimaxActionsCount);

	for (int actionIdx = 0; actionIdx < minimaxActionsCount; ++actionIdx) {
		minimaxActions[actionIdx] = unit->minimaxActions[actionIdx];
	}

	this->position = unit->position;
	this->posetion = unit->posetion;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::move(Coords coords) {
	position = coords;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::incrementScore() {
	++score;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::clearActions() {
	minimaxActionsCount = 0;

	if (minimaxActions) {
		delete[] minimaxActions;
		minimaxActions = NULL;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::move(Direction direction) {
	position += DIRECTIONS[direction];
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords Unit::build(Direction direction) {
	Coords buildCoords = position + DIRECTIONS[direction];
	return buildCoords;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::debug() const {
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

	Unit* getUnit(int unitIdx) const {
		return units[unitIdx];
	}

	int getMyBestUnit() const {
		return myBestUnit;
	}

	int getEnemyBestUnit() const {
		return enemyBestUnit;
	}

	void setGrid(Grid* grid) { this->grid = grid; }
	void setMyBestUnit(int myBestUnit) { this->myBestUnit = myBestUnit; }
	void setEnemyBestUnit(int enemyBestUnit) { this->enemyBestUnit = enemyBestUnit; }

	void simulate(int unitIdx, MinimaxAction action);
	int evaluate() const;
	void init(int gridSize);
	void setMiniMaxUnitTurnActions();
	
	void copy(const State& state);
	void setUnitPosition(int unitIdx, Coords position);
	void setUnitPosetion(int unitIdx, Posetion posetion);
	bool isTerminal(int unitIdx) const;
	void updateScore();
	void clearUnitsActions();
	bool unitOnCell(Coords position) const;
	bool unitBlocked(int unitId) const;
	int chooseBestUnit(Posetion posetion) const;
	void chooseBestUnits();
	bool hasPlayableUnit() const;

	void debug() const;

private:
	Grid* grid;
	Unit* units[GAME_UNITS_COUNT];

	int myBestUnit;
	int enemyBestUnit;
};

//*************************************************************************************************************
//*************************************************************************************************************

State::State() :
	grid(),
	myBestUnit(INVALID_INDEX),
	enemyBestUnit(INVALID_INDEX)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

State::~State() {
	if (grid) {
		delete grid;
		grid = NULL;
	}

	for (int unitIdx = 0; unitIdx < GAME_UNITS_COUNT; ++unitIdx) {
		if (units[unitIdx]) {
			delete units[unitIdx];
			units[unitIdx] = NULL;
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::simulate(int unitIdx, MinimaxAction action) {
	if (MMAT_MOVE == action.getType()) {
		units[unitIdx]->move(action.getCoords());
		char cell = grid->getCell(action.getCoords());
		if (LEVEL_3 == cell) {
			units[unitIdx]->incrementScore();
		}
	}
	else if (MMAT_BUILD == action.getType()) {
		grid->build(action.getCoords());
	}

	setMiniMaxUnitTurnActions();
}

//*************************************************************************************************************
//*************************************************************************************************************

int State::evaluate() const {
	if (unitBlocked(myBestUnit)) {
		return INT_MIN + 1;
	}

	if (unitBlocked(enemyBestUnit)) {
		return INT_MAX - 1;
	}

	int score = 0;
	int surroundingLevels = 0;
	int unitLevel = 0;

	for (int unitIdx = 0; unitIdx < GAME_UNITS_COUNT; ++unitIdx) {
		Unit* unit = units[unitIdx];
		Coords position = unit->getPosition();
		Posetion posetion = unit->getPosetion();

		int unitScore = unit->getScore();
		int unitSurroundingLevels = grid->getSurroundingLevels(unit->getPosition());

		if (P_MINE == posetion) {
			unitLevel = grid->getCell(position) - LEVEL_0;
			score += unitScore;
			surroundingLevels += unitSurroundingLevels;
		}
		else {
			score -= unitScore;
			surroundingLevels -= unitSurroundingLevels;
		}
	}

	int heuristic =
		(score * SCORE_WEIGHT) +
		(surroundingLevels * LEVELS_WEIGHT) +
		(unitLevel * UNIT_LEVEL_WEIGHT);

	int res = heuristic;
	if (USE_RAND_HEURISTIC) {
		res = rand() % 100;
	}

	return res;
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::init(int gridSize) {
	grid = new Grid();
	grid->init(gridSize);

	for (int unitIdx = 0; unitIdx < GAME_UNITS_COUNT; ++unitIdx) {
		units[unitIdx] = new Unit();
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::setMiniMaxUnitTurnActions() {
	for (int unitIdx = 0; unitIdx < GAME_UNITS_COUNT; ++unitIdx) {
		if (myBestUnit != unitIdx && enemyBestUnit != unitIdx) {
			continue;
		}

		Coords unitPosition = units[unitIdx]->getPosition();
		char cell = grid->getCell(unitPosition);

		for (int dirIdx = 0; dirIdx < DIRECTION_COUNT; ++dirIdx) {
			Coords newPosition = unitPosition + DIRECTIONS[dirIdx];

			if (grid->validPosition(newPosition) && !unitOnCell(newPosition)) {
				MinimaxAction newAction;

				if (grid->canMoveFromTo(unitPosition, newPosition)) {
					newAction.setCoords(newPosition);
					newAction.setType(MMAT_MOVE);
					units[unitIdx]->addMinimaxAction(newAction);
				}

				if (grid->canBuildFromTo(unitPosition, newPosition)) {
					newAction.setCoords(newPosition);
					newAction.setType(MMAT_BUILD);
					units[unitIdx]->addMinimaxAction(newAction);
				}
			}
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::copy(const State& state) {
	grid->copy(state.getGrid());

	for (int unitIdx = 0; unitIdx < GAME_UNITS_COUNT; ++unitIdx) {
		units[unitIdx]->copy(state.getUnit(unitIdx));
	}

	myBestUnit = state.myBestUnit;
	enemyBestUnit = state.enemyBestUnit;
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::setUnitPosition(int unitIdx, Coords position) {
	units[unitIdx]->setPosition(position);
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::setUnitPosetion(int unitIdx, Posetion posetion) {
	units[unitIdx]->setPosetion(posetion);
}

//*************************************************************************************************************
//*************************************************************************************************************

bool State::isTerminal(int unitIdx) const {
	return unitBlocked(unitIdx);
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::updateScore() {
	for (int unitIdx = 0; unitIdx < GAME_UNITS_COUNT; ++unitIdx) {
		Unit* unit = units[unitIdx];
		Coords unitPosition = unit->getPosition();

		if (unitPosition.isValid()) {
			char cell = grid->getCell(unitPosition);

			if (LEVEL_3 == cell) {
				unit->incrementScore();
			}
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::clearUnitsActions() {
	for (int unitIdx = 0; unitIdx < GAME_UNITS_COUNT; ++unitIdx) {
		Unit* unit = units[unitIdx];
		unit->clearActions();
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

bool State::unitOnCell(Coords position) const {
	bool onCell = false;

	for (int unitIdx = 0; unitIdx < GAME_UNITS_COUNT; ++unitIdx) {
		if (units[unitIdx]->getPosition() == position) {
			onCell = true;
			break;
		}
	}

	return onCell;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool State::unitBlocked(int unitId) const {
	bool blocked = true;
	Coords unitCoords = units[unitId]->getPosition();
	char unitLevel = grid->getCell(unitCoords);

	for (int dirIdx = 0; dirIdx < DIRECTION_COUNT; ++dirIdx) {
		Coords newPosition = unitCoords + DIRECTIONS[dirIdx];

		if (grid->validPosition(newPosition)) {
			if (!unitOnCell(newPosition) && grid->canMoveFromTo(unitCoords, newPosition)) {
				blocked = false;
				break;
			}
		}
	}
	return blocked;
}

//*************************************************************************************************************
//*************************************************************************************************************

int State::chooseBestUnit(Posetion posetion) const {
	int bestUnitLevels = 0;
	int bestUnitIdx = INVALID_INDEX;

	for (int unitIdx = 0; unitIdx < GAME_UNITS_COUNT; ++unitIdx) {
		Unit* unit = units[unitIdx];

		if (!unit->getPosition().isValid() || unitBlocked(unitIdx)) {
			continue;
		}

		if (posetion == unit->getPosetion()) {
			int surroundingLevels = grid->getSurroundingLevels(unit->getPosition());
			int unitLevel = grid->getCell(unit->getPosition()) - LEVEL_0;

			int levels = surroundingLevels + unitLevel;
			if (levels >= bestUnitLevels) {
				bestUnitLevels = levels;
				bestUnitIdx = unitIdx;
			}
		}
	}

	return bestUnitIdx;
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::chooseBestUnits() {
	myBestUnit = chooseBestUnit(P_MINE);
	enemyBestUnit = chooseBestUnit(P_ENEMY);
}

//*************************************************************************************************************
//*************************************************************************************************************

bool State::hasPlayableUnit() const {
	return myBestUnit != INVALID_INDEX;
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
	~Node();

	int getNodeDepth() const {
		return nodeDepth;
	}

	State* getState() const {
		return state;
	}

	MinimaxAction getAction() const {
		return action;
	}

	Node* getParent() const {
		return parent;
	}

	int getChildrenCount() const {
		return childrenCount;
	}

	Node** getChildren() const {
		return children;
	}

	string getPath() const {
		return path;
	}

	Node* getChild(int childIdx) const {
		return children[childIdx];
	}

	int getEvaliValue() const {
		return evalValue;
	}

	void setState(State* state) { this->state = state; }
	void setNodeDepth(int nodeDepth) { this->nodeDepth = nodeDepth; }
	void setNodeAction(MinimaxAction action) { this->action = action; }
	void setParent(Node* parent) { this->parent = parent; }
	void setLabel(char label) { this->label = label; }
	void setPath(string path) { this->path = path; }
	void setEvalValue(int evalValue) { this->evalValue = evalValue; }

	Node* createChild(
		int unitIdx,
		int actionIdx,
		Node* parent,
		int nodeDepth
	);
	void addChild(Node* child);
	void copyState(const State& state);
	void init(int gridSize);
	void setTreePath();

private:
	int nodeDepth;
	State* state;
	Node* parent;
	int childrenCount;
	Node** children;
	MinimaxAction action;

	string path;
	char label;
	int evalValue;
};

//*************************************************************************************************************
//*************************************************************************************************************

Node::Node() :
	nodeDepth(INVALID_NODE_DEPTH),
	state(NULL),
	parent(NULL),
	childrenCount(0),
	children(NULL),
	action(),
	path(),
	label(PARENT_LABEL),
	evalValue(0)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Node::~Node() {
	//if (state && MMAT_BUILD == action.getType()) {
	// Do not delete the turn state
	if (PARENT_LABEL != label) {
		delete state;
		state = NULL;
	}
	//}

	path.clear();
}

//*************************************************************************************************************
//*************************************************************************************************************

Node* Node::createChild(
	int unitIdx,
	int actionIdx,
	Node* parent,
	int nodeDepth
) {
	Node* child = new Node();
	child->init(state->getGrid()->getGridSize());

	Unit* unit = parent->state->getUnit(unitIdx);
	MinimaxAction action = unit->getMinimaxAction(actionIdx);

	if (MMAT_BUILD == action.getType()) {
		child->getState()->getGrid()->copy(state->getGrid());
	}
	else if (MMAT_MOVE == action.getType()) {
		//child->getState()->setGrid(state->getGrid());
		child->getState()->getGrid()->copy(state->getGrid());
	}

	child->getState()->setMyBestUnit(state->getMyBestUnit());
	child->getState()->setEnemyBestUnit(state->getEnemyBestUnit());

	for (int i = 0; i < GAME_UNITS_COUNT; ++i) {
		child->getState()->getUnit(i)->setPosition(state->getUnit(i)->getPosition());
		child->getState()->getUnit(i)->setPosetion(state->getUnit(i)->getPosetion());
	}

	child->setNodeDepth(nodeDepth);
	child->setParent(parent);
	child->setNodeAction(action);

	child->getState()->simulate(unitIdx, action);

	char label = 'A' + actionIdx;
	child->setLabel(label);
	child->setTreePath();

	return child;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Node::addChild(Node* child) {
	++childrenCount;
	Node** temp = new Node*[childrenCount];

	for (int childIdx = 0; childIdx < childrenCount - 1; ++childIdx) {
		temp[childIdx] = children[childIdx];
	}

	temp[childrenCount - 1] = child;

	delete[] children;
	children = temp;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Node::copyState(const State& state) {
	this->state->copy(state);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Node::init(int gridSize) {
	state = new State();
	state->init(gridSize);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Node::setTreePath() {
	Node* n = NULL;
	Node* p = getParent();

	path.push_back(label);

	while (p) {
		n = p;
		path.push_back(n->label);
		p = n->getParent();
	}

	reverse(path.begin(), path.end());
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

struct MinimaxResult {
	MinimaxResult() :
		bestLeaveNode(NULL),
		evaluationValue(0)
	{}

	MinimaxResult(
		Node* bestLeaveNode,
		int evaluationValue
	) :
		bestLeaveNode(bestLeaveNode),
		evaluationValue(evaluationValue)
	{}

	Node* bestLeaveNode;
	int evaluationValue;
};

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Minimax {
public:
	Minimax();
	~Minimax();

	int getPlayUnitIdx() const {
		return playUnitIdx;
	}

	Coords getBuildCoords() const {
		return buildCoords;
	}

	Coords getMoveCoords() const {
		return moveCoords;
	}

	MinimaxActionType currentActionType(MinimaxActionType parentAction, MaximizeMinimize mm) const;
	int nextUnitToExpand(MinimaxActionType nodeAction, MaximizeMinimize mm) const;
	void init(const State& state);
	void run();
	void deleteTree(Node* node);
	void clear();
	void backtrack(Node* node);
	void printTreeToFile();
	void printChildren(Node* node, ofstream& file);

	MinimaxResult maximize(Node* node, int unitIdx, int alpha, int beta);
	MinimaxResult minimize(Node* node, int unitIdx, int alpha, int beta);

private:
	Node* tree;

	int playUnitIdx;
	Coords buildCoords;
	Coords moveCoords;
};

//*************************************************************************************************************
//*************************************************************************************************************

Minimax::Minimax() :
	tree(NULL),
	buildCoords(),
	moveCoords()
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Minimax::~Minimax() {
}

//*************************************************************************************************************
//*************************************************************************************************************

MinimaxActionType Minimax::currentActionType(MinimaxActionType parentAction, MaximizeMinimize mm) const {
	MinimaxActionType action = MMAT_INVALID;

	//if (MMAT_BUILD == parentAction && MM_MAXIMIZE == mm) {
	//	action = MMAT_MOVE;
	//}
	//
	//if (MMAT_MOVE == parentAction && MM_MINIMIZE == mm) {
	//	action = MMAT_MOVE;
	//}
	//
	//if (MMAT_MOVE == parentAction && MM_MAXIMIZE == mm) {
	//	action = MMAT_BUILD;
	//}
	//
	//if (MMAT_BUILD == parentAction && MM_MINIMIZE == mm) {
	//	action = MMAT_BUILD;
	//}

	if (MMAT_MOVE == parentAction) {
		action = MMAT_BUILD;
	}
	
	if (MMAT_BUILD == parentAction) {
		action = MMAT_MOVE;
	}

	return action;
}

//*************************************************************************************************************
//*************************************************************************************************************

int Minimax::nextUnitToExpand(MinimaxActionType nodeAction, MaximizeMinimize mm) const {
	int unitId = INVALID_INDEX;



	return unitId;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Minimax::init(const State& state) {
	tree = new Node();
	tree->init(state.getGrid()->getGridSize());
	tree->setNodeDepth(0);
	tree->setNodeAction(MinimaxAction(MMAT_BUILD, Coords()));
	tree->copyState(state);
	tree->setLabel(PARENT_LABEL);
	tree->setPath(PARENT_PATH);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Minimax::run() {
	MinimaxResult res = maximize(tree, tree->getState()->getMyBestUnit(), INT_MIN, INT_MAX);

	if (PRINT_MINIMAX_TREE_TO_FILE) {
		printTreeToFile();
	}

	if (res.bestLeaveNode) {
		backtrack(res.bestLeaveNode);
		playUnitIdx = res.bestLeaveNode->getState()->getMyBestUnit();
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Minimax::deleteTree(Node* node) {
	if (node) {
		int childrenCount = node->getChildrenCount();
		Node** children = node->getChildren();

		delete node;
		node = NULL;

		for (int childIdx = 0; childIdx < childrenCount; ++childIdx) {
			deleteTree(children[childIdx]);
		}

		delete[] children;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Minimax::clear() {
	deleteTree(tree);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Minimax::backtrack(Node* node) {
	if (BUILD_NODE_DEPTH == node->getNodeDepth()) {
		buildCoords = node->getAction().getCoords();
	}

	if (MOVE_NODE_DEPTH == node->getNodeDepth()) {
		moveCoords = node->getAction().getCoords();;
	}

	Node* parent = node->getParent();

	if (parent) {
		backtrack(parent);
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Minimax::printChildren(Node* node, ofstream& file) {
	string nodePath = node->getPath();

	file << nodePath << " [label=\"";
	node->getState()->getGrid()->debugPrint(file);
	file << nodePath << "\\n";
	file << node->getEvaliValue();
	file << "\"]\n";

	for (int childIdx = 0; childIdx < node->getChildrenCount(); ++childIdx) {
		Node* child = node->getChild(childIdx);
		string childPath = child->getPath();

		file << nodePath << "->" << childPath;
		file << " [label=\"";
		child->getAction().debugPrint(file);
		file << "\"]\n";

		printChildren(child, file);
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Minimax::printTreeToFile() {
	ofstream file;
	file.open("minimaxTree.gv");
	file << "digraph mytree{\n";

	printChildren(tree, file);

	file << "}";
	file.close();
}

//*************************************************************************************************************
//*************************************************************************************************************

MinimaxResult Minimax::maximize(Node* node, int unitIdx, int alpha, int beta) {
	State* state = node->getState();
	Unit* unit = state->getUnit(unitIdx);

	if (MINIMAX_DEPTH == node->getNodeDepth() || state->isTerminal(unitIdx)) {
		int eval = state->evaluate();
		node->setEvalValue(eval);
		return MinimaxResult(node, eval);
	}

	MinimaxResult res = MinimaxResult(NULL, INT_MIN);

	const MinimaxActionType typeForChildren = currentActionType(node->getAction().getType(), MM_MAXIMIZE);
	int minimaxActionsCount = unit->getMinimaxActionsCout();

	for (int actionIdx = 0; actionIdx < minimaxActionsCount; ++actionIdx) {
		if (typeForChildren != unit->getMinimaxAction(actionIdx).getType()) {
			continue;
		}

		Node* child = node->createChild(unitIdx, actionIdx, node, node->getNodeDepth() + 1);
		node->addChild(child);

		int bestEnemyUnit = state->getEnemyBestUnit();

		MinimaxResult minMaxRes;
		if (INVALID_INDEX == bestEnemyUnit || (MMAT_BUILD == node->getAction().getType() && unitIdx < 2)) {
			minMaxRes = maximize(child, state->getMyBestUnit(), alpha, beta);
		}
		else {
			minMaxRes = minimize(child, bestEnemyUnit, alpha, beta);
		}

		if (minMaxRes.evaluationValue > res.evaluationValue) {
			res = minMaxRes;
		}

		if (res.evaluationValue >= beta) {
			break;
		}

		if (res.evaluationValue > alpha) {
			alpha = res.evaluationValue;
		}
	}

	node->setEvalValue(res.evaluationValue);
	return res;
}

//*************************************************************************************************************
//*************************************************************************************************************

MinimaxResult Minimax::minimize(Node* node, int unitIdx, int alpha, int beta) {
	State* state = node->getState();
	Unit* unit = state->getUnit(unitIdx);

	if (MINIMAX_DEPTH == node->getNodeDepth() || state->isTerminal(unitIdx)) {
		int eval = state->evaluate();
		node->setEvalValue(eval);
		return MinimaxResult(node, eval);
	}

	MinimaxResult res = MinimaxResult(NULL, INT_MAX);

	const MinimaxActionType typeForChildren = currentActionType(node->getAction().getType(), MM_MINIMIZE);
	int minimaxActionsCount = unit->getMinimaxActionsCout();

	for (int actionIdx = 0; actionIdx < minimaxActionsCount; ++actionIdx) {
		if (typeForChildren != unit->getMinimaxAction(actionIdx).getType()) {
			continue;
		}

		Node* child = node->createChild(unitIdx, actionIdx, node, node->getNodeDepth() + 1);
		node->addChild(child);

		MinimaxResult minMaxRes;
		if (MMAT_BUILD == node->getAction().getType() && unitIdx > 1) {
			minMaxRes = minimize(child, state->getEnemyBestUnit(), alpha, beta);
		}
		else {
			minMaxRes = maximize(child, state->getMyBestUnit(), alpha, beta);
		}

		if (minMaxRes.evaluationValue < res.evaluationValue) {
			res = minMaxRes;
		}

		if (res.evaluationValue <= alpha) {
			break;
		}

		if (res.evaluationValue < beta) {
			beta = res.evaluationValue;
		}
	}

	node->setEvalValue(res.evaluationValue);
	return res;
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

	string coordsToDirection(Coords from, Coords to) const;
	void outPutMinimaxRes() const;

	void debug() const;

private:
	int turnsCount;
	int size;
	int unitsPerPlayer;

	State turnState;
	Minimax minimax;
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
		clock_t begin = clock();

		getTurnInput();
		turnBegin();
		makeTurn();
		turnEnd();

		if (USE_HARDCODED_INPUT) {
			clock_t end = clock();
			double elapsedMilliSecs = double(end - begin);

			cout << endl;
			cout << "Turn " << turnsCount << " milliseconds: " << elapsedMilliSecs << endl;
			cout << endl;

			if (BREAK_TURN == turnsCount) {
				break;
			}
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::getGameInput() {
	if (USE_HARDCODED_INPUT) {
		size = 5;
		unitsPerPlayer = 2;
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

				if (0 == turnsCount) {

				}

				if (1 == turnsCount) {
					if (0 == rowIdx && 2 == colIdx) { c = '1'; };
					if (2 == rowIdx && 2 == colIdx) { c = '1'; };
				}

				if (2 == turnsCount) {
					if (0 == rowIdx && 2 == colIdx) { c = '1'; };
					if (2 == rowIdx && 1 == colIdx) { c = '1'; };
					if (2 == rowIdx && 2 == colIdx) { c = '2'; };
				}

				if (3 == turnsCount) {
					if (0 == rowIdx && 2 == colIdx) { c = '2'; };
					if (2 == rowIdx && 1 == colIdx) { c = '1'; };
					if (2 == rowIdx && 2 == colIdx) { c = '2'; };
					if (3 == rowIdx && 0 == colIdx) { c = '1'; };
				}

				if (4 == turnsCount) {
					if (0 == rowIdx && 2 == colIdx) { c = '2'; };
					if (1 == rowIdx && 2 == colIdx) { c = '1'; };
					if (2 == rowIdx && 1 == colIdx) { c = '1'; };
					if (2 == rowIdx && 2 == colIdx) { c = '2'; };
					if (3 == rowIdx && 0 == colIdx) { c = '1'; };
					if (3 == rowIdx && 2 == colIdx) { c = '1'; };
				}

				if (5 == turnsCount) {
					if (0 == rowIdx && 2 == colIdx) { c = '3'; };
					if (1 == rowIdx && 2 == colIdx) { c = '1'; };
					if (2 == rowIdx && 1 == colIdx) { c = '1'; };
					if (2 == rowIdx && 2 == colIdx) { c = '2'; };
					if (3 == rowIdx && 0 == colIdx) { c = '1'; };
					if (3 == rowIdx && 2 == colIdx) { c = '1'; };
					if (4 == rowIdx && 4 == colIdx) { c = '1'; };
				}
			}
			else {
				cin >> c;

				if (OUTPUT_FOR_DEBUG) {
					if (LEVEL_0 != c) {
						cerr << "if (" << rowIdx << " == rowIdx && " << colIdx << " == colIdx) { c =\'" << c << "\'; };" << endl;
					}
				}
			}

			if (LEVEL_3 < c) {
				c = DOT;
			}

			turnState.getGrid()->setCell(Coords(colIdx, rowIdx), c);
		}
	}

	for (int unitIdx = 0; unitIdx < GAME_UNITS_COUNT; ++unitIdx) {
		int unitX, unitY;

		if (USE_HARDCODED_INPUT) {
			if (0 == turnsCount) {
				if (0 == unitIdx) { unitX = 1; unitY = 1; }
				if (1 == unitIdx) { unitX = 4; unitY = 1; }
				if (2 == unitIdx) { unitX = -1; unitY = -1; }
				if (3 == unitIdx) { unitX = 4; unitY = 2; }
			}

			if (1 == turnsCount) {
				if (0 == unitIdx) { unitX = 1; unitY = 1; }
				if (1 == unitIdx) { unitX = 3; unitY = 1; }
				if (2 == unitIdx) { unitX = -1; unitY = -1; }
				if (3 == unitIdx) { unitX = 4; unitY = 2; }
			}

			if (2 == turnsCount) {
				if (0 == unitIdx) { unitX = 1; unitY = 1; }
				if (1 == unitIdx) { unitX = 3; unitY = 1; }
				if (2 == unitIdx) { unitX = -1; unitY = -1; }
				if (3 == unitIdx) { unitX = 4; unitY = 2; }
			}

			if (3 == turnsCount) {
				if (0 == unitIdx) { unitX = 2; unitY = 1; }
				if (1 == unitIdx) { unitX = 3; unitY = 1; }
				if (2 == unitIdx) { unitX = 1; unitY = 2; }
				if (3 == unitIdx) { unitX = 4; unitY = 2; }
			}

			if (4 == turnsCount) {
				if (0 == unitIdx) { unitX = 1; unitY = 1; }
				if (1 == unitIdx) { unitX = 3; unitY = 1; }
				if (2 == unitIdx) { unitX = -1; unitY = -1; }
				if (3 == unitIdx) { unitX = 4; unitY = 2; }
			}

			if (5 == turnsCount) {
				if (0 == unitIdx) { unitX = 2; unitY = 1; }
				if (1 == unitIdx) { unitX = 3; unitY = 1; }
				if (2 == unitIdx) { unitX = -1; unitY = -1; }
				if (3 == unitIdx) { unitX = -1; unitY = -1; }
			}
		}
		else {
			cin >> unitX >> unitY;

			if (OUTPUT_FOR_DEBUG) {
				cerr << "if (" << unitIdx << " == unitIdx) { unitX = " << unitX << "; unitY = " << unitY << "; }" << endl;
			}
		}

		Posetion posetion = P_MINE;
		if (unitIdx >= PLAYER_UNITS_COUNT) {
			posetion = P_ENEMY;
		}

		turnState.setUnitPosition(unitIdx, Coords(unitX, unitY));
		turnState.setUnitPosetion(unitIdx, posetion);
	}

	turnState.updateScore();

	if (!USE_HARDCODED_INPUT) {
		int legalActions;
		cin >> legalActions;

		for (int i = 0; i < legalActions; i++) {
			string type, dir1, dir2;
			int index;
			cin >> type >> index >> dir1 >> dir2;
			//cerr << "Legal action: " << " Type:" << type << " Idx:" << index << " Dir1:" << dir1 << " Dir2:" << dir2 << endl;
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::turnBegin() {
	turnState.chooseBestUnits();
	turnState.setMiniMaxUnitTurnActions();
	minimax.init(turnState);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::makeTurn() {
	if (turnState.hasPlayableUnit()) {
		minimax.run();
		outPutMinimaxRes();
	}
	else {
		cout << INVALID_OUTPUT << endl;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::turnEnd() {
	++turnsCount;
	turnState.clearUnitsActions();
	minimax.clear();
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

string Game::coordsToDirection(Coords from, Coords to) const {
	string res = "";

	if (from.isValid() && to.isValid()) {
		int dirIdx = 0;
		for (; dirIdx < DIRECTION_COUNT; ++dirIdx) {
			Coords newCoords = from + DIRECTIONS[dirIdx];

			if (newCoords == to) {
				break;
			}
		}

		res = DIR_STRS[dirIdx];
	}

	return res;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::outPutMinimaxRes() const {
	const Coords myUnitPosition = turnState.getUnit(minimax.getPlayUnitIdx())->getPosition();

	const string moveDirection = coordsToDirection(myUnitPosition, minimax.getMoveCoords());
	const string buildDirection = coordsToDirection(minimax.getMoveCoords(), minimax.getBuildCoords());

	cout << MOVE_BUILD_ACTION << ' ' << minimax.getPlayUnitIdx() << ' ' << moveDirection << ' ' << buildDirection << endl;
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
seed=932265085
*/
