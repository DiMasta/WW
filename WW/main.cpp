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
const int MINIMAX_DEPTH = 2;

const int SCORE_WEIGHT = 800;
const int LEVELS_WEIGHT = 200;

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

enum UnitIds {
	UI_INVALID = -1,
	UI_MY_UNIT = 0,
	UI_ENEMY_UNIT = 1,
};

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
	void copy(Grid* grid);
	void build(Coords coords);
	int getSurroundingLevels(Coords coords) const;

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
		Coords newPosition;
		newPosition.setXCoord(coords.getXCoord() + DIR_X[dirIdx]);
		newPosition.setYCoord(coords.getYCoord() + DIR_Y[dirIdx]);

		if (validPosition(newPosition)) {
			surroudingLevels += getCell(newPosition) - LEVEL_0;
		}
	}
	return 0;
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
	void makeTurn();
	void init(int minimaxActionsCount);
	void addMinimaxAction(MinimaxAction newAction);
	void copy(Unit* unit);
	void move(Coords coords);
	void incrementScore();

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
	if (minimaxActions) {
		delete[] minimaxActions;
		minimaxActions = NULL;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Unit::makeTurn() {
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

	void simulate(int unitIdx, MinimaxAction action);
	int evaluate() const;
	void init(int gridSize);
	void setMiniMaxUnitTurnActions();
	
	void copy(const State& state);
	void setUnitPosition(int unitIdx, Coords position);
	void setUnitPosetion(int unitIdx, Posetion posetion);
	bool isTerminal() const;

	void debug() const;

private:
	Grid* grid;
	Unit* units[GAME_UNITS_COUNT];
};

//*************************************************************************************************************
//*************************************************************************************************************

State::State() :
	grid()
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
		grid->setCell(action.getCoords(), grid->getCell(action.getCoords()) + 1);
	}

	setMiniMaxUnitTurnActions();
}

//*************************************************************************************************************
//*************************************************************************************************************

int State::evaluate() const {
	int score = 0;
	int surroundingLevels = 0;

	for (int unitIdx = 0; unitIdx < GAME_UNITS_COUNT; ++unitIdx) {
		Unit* unit = units[unitIdx];
		Posetion posetion = unit->getPosetion();

		int unitScore = unit->getScore();
		int unitSurroundingLevels = grid->getSurroundingLevels(unit->getPosition());

		if (P_MINE == posetion) {
			score += unitScore;
			surroundingLevels += unitSurroundingLevels;
		}
		else {
			score -= unitScore;
			surroundingLevels -= unitSurroundingLevels;
		}
	}

	int heuristic = (score * SCORE_WEIGHT) + (surroundingLevels * LEVELS_WEIGHT);

	return heuristic;
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
		Coords unitPosition = units[unitIdx]->getPosition();
		char cell = grid->getCell(unitPosition);

		for (int dirIdx = 0; dirIdx < DIRECTION_COUNT; ++dirIdx) {
			Coords newPosition;
			newPosition.setXCoord(unitPosition.getXCoord() + DIR_X[dirIdx]);
			newPosition.setYCoord(unitPosition.getYCoord() + DIR_Y[dirIdx]);

			if (grid->validPosition(newPosition)) {
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

bool State::isTerminal() const {
	return false;
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

	MinimaxActionType getNodeAction() const {
		return nodeAction;
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

	void setState(State* state) { this->state = state; }
	void setNodeDepth(int nodeDepth) { this->nodeDepth = nodeDepth; }
	void setNodeAction(MinimaxActionType nodeAction) { this->nodeAction = nodeAction; }
	void setParent(Node* parent) { this->parent = parent; }

	Node* createChild(
		int unitIdx,
		int actionIdx,
		Node* parent,
		int nodeDepth,
		MinimaxActionType typeForChildren
	);
	void addChild(Node* child);
	void copyState(const State& state);
	void init(int gridSize);

private:
	int nodeDepth;
	State* state;
	Node* parent;
	int childrenCount;
	Node** children;
	MinimaxActionType nodeAction;
};

//*************************************************************************************************************
//*************************************************************************************************************

Node::Node() :
	nodeDepth(INVALID_NODE_DEPTH),
	state(NULL),
	parent(NULL),
	childrenCount(0),
	children(NULL),
	nodeAction(MMAT_INVALID)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Node::~Node() {
	if (state) {
		delete state;
		state = NULL;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

Node* Node::createChild(
	int unitIdx,
	int actionIdx,
	Node* parent,
	int nodeDepth,
	MinimaxActionType typeForChildren
) {
	Node* child = new Node();
	child->init(state->getGrid()->getGridSize());
	child->copyState(*state);
	child->setNodeDepth(nodeDepth);
	child->setParent(parent);
	child->setNodeAction(typeForChildren);

	Unit* unit = child->state->getUnit(unitIdx);
	MinimaxAction action = unit->getMinimaxAction(actionIdx);

	child->getState()->simulate(unitIdx, action);

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

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

struct MinimaxResult {
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

	MinimaxActionType currentActionType(MinimaxActionType parentAction, MaximizeMinimize mm) const;
	void init(const State& state);
	void run();
	void deleteTree(Node* node);
	void clear();

	MinimaxResult maximize(Node* node, int unitIdx, int alpha, int beta);
	MinimaxResult minimize(Node* node, int unitIdx, int alpha, int beta);

private:
	Node* tree;
};

//*************************************************************************************************************
//*************************************************************************************************************

Minimax::Minimax() :
	tree(NULL)
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

	if (MMAT_BUILD == parentAction && MM_MAXIMIZE == mm) {
		action = MMAT_MOVE;
	}

	if (MMAT_MOVE == parentAction && MM_MINIMIZE == mm) {
		action = MMAT_MOVE;
	}

	if (MMAT_MOVE == parentAction && MM_MAXIMIZE == mm) {
		action = MMAT_BUILD;
	}

	if (MMAT_BUILD == parentAction && MM_MINIMIZE == mm) {
		action = MMAT_BUILD;
	}

	return action;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Minimax::init(const State& state) {
	tree = new Node();
	tree->init(state.getGrid()->getGridSize());
	tree->setNodeDepth(0);
	tree->setNodeAction(MMAT_BUILD);
	tree->copyState(state);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Minimax::run() {
	MinimaxResult res = maximize(tree, UI_MY_UNIT, INT_MIN, INT_MAX);
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

MinimaxResult Minimax::maximize(Node* node, int unitIdx, int alpha, int beta) {
	State* state = node->getState();
	Unit* unit = state->getUnit(unitIdx);

	if (MINIMAX_DEPTH == node->getNodeDepth() || state->isTerminal()) {
		int eval = state->evaluate();
		return MinimaxResult(node, eval);
	}

	MinimaxResult res = MinimaxResult(NULL, INT_MIN);

	const MinimaxActionType typeForChildren = currentActionType(node->getNodeAction(), MM_MAXIMIZE);
	int minimaxActionsCount = unit->getMinimaxActionsCout();

	for (int actionIdx = 0; actionIdx < minimaxActionsCount; ++actionIdx) {
		if (typeForChildren != unit->getMinimaxAction(actionIdx).getType()) {
			continue;
		}

		Node* child = node->createChild(unitIdx, actionIdx, node, node->getNodeDepth() + 1, typeForChildren);
		node->addChild(child);

		MinimaxResult minRes = minimize(child, UI_ENEMY_UNIT, alpha, beta);

		if (minRes.evaluationValue > res.evaluationValue) {
			res = minRes;
		}

		if (res.evaluationValue >= beta) {
			break;
		}

		if (res.evaluationValue > alpha) {
			alpha = res.evaluationValue;
		}
	}

	return res;
}

//*************************************************************************************************************
//*************************************************************************************************************

MinimaxResult Minimax::minimize(Node* node, int unitIdx, int alpha, int beta) {
	State* state = node->getState();
	Unit* unit = state->getUnit(unitIdx);

	if (MINIMAX_DEPTH == node->getNodeDepth() || state->isTerminal()) {
		int eval = state->evaluate();
		return MinimaxResult(node, eval);
	}

	MinimaxResult res = MinimaxResult(NULL, INT_MAX);

	const MinimaxActionType typeForChildren = currentActionType(node->getNodeAction(), MM_MINIMIZE);
	int minimaxActionsCount = unit->getMinimaxActionsCout();

	for (int actionIdx = 0; actionIdx < minimaxActionsCount; ++actionIdx) {
		if (typeForChildren != unit->getMinimaxAction(actionIdx).getType()) {
			continue;
		}

		Node* child = node->createChild(unitIdx, actionIdx, node, node->getNodeDepth(), typeForChildren);
		node->addChild(child);

		MinimaxResult maxRes = maximize(child, UI_MY_UNIT, alpha, beta);

		if (maxRes.evaluationValue < res.evaluationValue) {
			res = maxRes;
		}

		if (res.evaluationValue <= alpha) {
			break;
		}

		if (res.evaluationValue < beta) {
			beta = res.evaluationValue;
		}
	}

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
		getTurnInput();
		turnBegin();
		makeTurn();
		turnEnd();

		if (1 == turnsCount) {
			break;
		}
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

		turnState.setUnitPosition(unitIdx, Coords(unitX, unitY));
		turnState.setUnitPosetion(unitIdx, posetion);
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
