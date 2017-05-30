#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <queue>
#include <iostream>
#include <string>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
public:
	AwfulPlayer(string nm, const Game& g);
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
		bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);
private:
	Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
	: Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
	// Clustering ships is bad strategy
	for (int k = 0; k < game().nShips(); k++)
		if (!b.placeShip(Point(k, 0), k, HORIZONTAL))
			return false;
	return true;
}

Point AwfulPlayer::recommendAttack()
{
	if (m_lastCellAttacked.c > 0)
		m_lastCellAttacked.c--;
	else
	{
		m_lastCellAttacked.c = game().cols() - 1;
		if (m_lastCellAttacked.r > 0)
			m_lastCellAttacked.r--;
		else
			m_lastCellAttacked.r = game().rows() - 1;
	}
	return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
	bool /* shotHit */, bool /* shipDestroyed */,
	int /* shipId */)
{
	// AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
	// AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
	bool result(cin >> r >> c);
	if (!result)
		cin.clear();  // clear error state so can do more input operations
	cin.ignore(10000, '\n');
	return result;
}

class HumanPlayer : public Player
{
public:
	HumanPlayer(string nm, const Game& g);
	virtual bool isHuman() const;
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
		bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);
private:

};

HumanPlayer::HumanPlayer(string nm, const Game& g)
	:Player(nm, g)
{

}

bool HumanPlayer::isHuman() const
{
	return true;
}

bool HumanPlayer::placeShips(Board& b)
{
	//user input for r and c
	//call board.placeShip
	for (int i = 0; i < game().nShips(); i++) {
		int r, c = 0;
		Point tempPoint;
		char dir = ' ';
		cout << "Enter h or v for direction of " << game().shipName(i) << " <length " << game().shipLength(i) << ">:" << endl;
		cin >> dir;
		while (dir != 'h' && dir != 'v') {
			cout << "Direction must be h or v." << endl;
			cout << "Enter h or v for direction of " << game().shipName(i) << " <length " << game().shipLength(i) << ">:" << endl;
			cin >> dir;
		}

		cout << "Enter row and column of leftmost cell <e.g. 3 5>:" << endl;
		while (!getLineWithTwoIntegers(r, c)) {
			cout << "You must enter two integers." << endl;
			getLineWithTwoIntegers(r, c);
		}
		tempPoint.r = r;
		tempPoint.c = c;
		while (!game().isValid(tempPoint)) {
			cout << "The ship cannot be placed there." << endl;
			cout << "Enter row and column of leftmost cell <e.g. 3 5>:" << endl;
			if (!getLineWithTwoIntegers(r, c)) {
				cout << "You must enter two integers." << endl;
				cout << "Enter row and column of leftmost cell <e.g. 3 5>:" << endl;
			}
			getLineWithTwoIntegers(r, c);
		}

		tempPoint.r = r;
		tempPoint.c = c;
		if (dir == 'h') {
			if (!b.placeShip(tempPoint, i, HORIZONTAL))
				return false;
			b.placeShip(tempPoint, i, HORIZONTAL);
		}
		else if (dir == 'v') {
			if (!b.placeShip(tempPoint, i, VERTICAL))
				return false;
			b.placeShip(tempPoint, i, VERTICAL);
		}
		//display this player's board
		b.display(false);
	}

	return true;
}

Point HumanPlayer::recommendAttack()
{
	Point pointToReturn;
	int r, c;
	//user input for r and c
	cout << "Enter the row and column to attack <e.g. 3 5>:" << endl;
	getLineWithTwoIntegers(r, c);
	pointToReturn.r = r;
	pointToReturn.c = c;

	return pointToReturn;
}

//Human player does not use any algorithms to determine attack
void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool ShipDestroyed, int shipId)
{}

//Human player does not use any algorithms to determine attack
void HumanPlayer::recordAttackByOpponent(Point p)
{}




//*********************************************************************
//  MediocrePlayer
//*********************************************************************


class MediocrePlayer :public Player
{
public:
	MediocrePlayer(string nm, const Game& g);
	virtual bool placeShips(Board& b);
	bool checkPlacement(Board& b, int shipId);
	virtual void recordAttackByOpponent(Point p);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool ShipDestroyed, int shipId);
private:
	Point lastHit;
	bool lastValidShot;
	bool ShipDestroyed;
	int shipId;
	bool state;
};
// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

MediocrePlayer::MediocrePlayer(string nm, const Game& g)
	:Player(nm, g)
{
	state = false;
}

bool MediocrePlayer::placeShips(Board& b)
{
	int count = 0;
	b.block();
	//b.display(false);
	while (!checkPlacement(b, 0)) {
		b.clear();
		b.block();
		//b.display(false);
		b.unblock();
		//b.display(false);
		count++;
		if (count >= 50)
			return false;
	}
	b.unblock();
	//b.display(false);
	return true;
}

bool MediocrePlayer::checkPlacement(Board& b, int shipId)
{
	if (shipId >= game().nShips())
		return true;

	Point tempPoint;
	for (int i = 0; i < game().rows(); i++) {
		for (int j = 0; j < game().cols(); j++) {
			tempPoint.r = i;
			tempPoint.c = j;
			if (b.placeShip(tempPoint, shipId, HORIZONTAL)) {
				if (!checkPlacement(b, shipId + 1)) {
					b.unplaceShip(tempPoint, shipId, HORIZONTAL);
					b.placeShip(tempPoint, shipId, VERTICAL);
				}
				else {
					return true;
				}
			}
			else if (b.placeShip(tempPoint, shipId, VERTICAL)) {
				if (!checkPlacement(b, shipId + 1)) {
					b.unplaceShip(tempPoint, shipId, VERTICAL);
					b.placeShip(tempPoint, shipId, HORIZONTAL);
				}
				else {
					return true;
				}
			}
		}
	}

	return false;
}

//Mediocre Player does not record attacks done to itself
void MediocrePlayer::recordAttackByOpponent(Point p) {};

//return random position or a set of limited random positions
//Game will decide if the position has been hit before or not
Point MediocrePlayer::recommendAttack()
{
	int tempR;
	int tempC;
	Point tempPoint;
	vector <Point> limitedSet;

	//In first state - return random point
	tempR = randInt(game().rows() - 1);
	tempC = randInt(game().cols() - 1);
	tempPoint.r = tempR;
	tempPoint.c = tempC;

	//In second state - check if should stay in state 2, then begin narrowed attack sequence
	if (state) {
		tempPoint = lastHit;
		//Attack missed: stay in state 2
		if (!lastValidShot) {
			state = true;
		}
		//Hits a ship without destroying - stay in state 2
		if (lastValidShot && !ShipDestroyed) {
			state = true;
		}
		//ship destroyed - switch to state 1 and reset vector of shot range
		if (ShipDestroyed) {
			limitedSet.clear();
			state = false;
			return tempPoint;
		}

		//Set of valid positions no more than 4 steps away from each dir

		limitedSet.clear();

		//4 spots north
		for (int i = 4 + lastHit.r; i > lastHit.r; i--) {
			tempPoint.r = i;
			tempPoint.c = lastHit.c;
			if (game().isValid(tempPoint))
				limitedSet.push_back(tempPoint);

		}
		//4 spots south
		for (int i = lastHit.r + 1; i <= 4 + lastHit.r; i++) {
			tempPoint.r = i;
			tempPoint.c = lastHit.c;
			if (game().isValid(tempPoint))
				limitedSet.push_back(tempPoint);
		}
		//4 spots west
		for (int i = 4 + lastHit.c; i > lastHit.c; i--) {
			tempPoint.c = i;
			tempPoint.r = lastHit.r;
			if (game().isValid(tempPoint))
				limitedSet.push_back(tempPoint);
		}
		//4 spots east
		for (int i = lastHit.c; i <= 4 + lastHit.c; i++) {
			tempPoint.c = i;
			tempPoint.r = lastHit.r;
			if (game().isValid(tempPoint))
				limitedSet.push_back(tempPoint);
		}

		int randIndex = randInt(limitedSet.size() - 1);
		tempPoint.r = limitedSet[randIndex].r;
		tempPoint.c = limitedSet[randIndex].c;
	}

	return tempPoint;


}

//Called in game.play() every time after this player attacks
void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool ShipDestroyed, int shipId)
{
	lastValidShot = validShot;
	this->shipId = shipId;
	this->ShipDestroyed = ShipDestroyed;

	if (validShot && shotHit && !ShipDestroyed && !state) {
		state = true;
		lastHit = p;
	}

}

//*********************************************************************
//  GoodPlayer
//*********************************************************************

class GoodPlayer :public Player
{
public:
	GoodPlayer(string nm, const Game& g);
	virtual bool placeShips(Board& b);
	bool checkPlacement(Board& b, int shipId);
	virtual void recordAttackByOpponent(Point p);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool ShipDestroyed, int shipId);
	int longestShipAlive();
private:
	Point lastHit;
	bool lastValidShot;
	bool ShipDestroyed;
	int shipId;
	bool state;
	int radius;
	queue <Point> limitedSet;
};

GoodPlayer::GoodPlayer(string nm, const Game& g)
	:Player(nm, g)
{
	state = false;
	radius = 1;
}

bool GoodPlayer::placeShips(Board& b)
{
	int count = 0;
	b.block();

	while (!checkPlacement(b, 0)) {
		b.clear();
		b.block();
		b.unblock();
		count++;
		if (count >= 50)
			return false;
	}
	b.unblock();
	return true;
}

bool GoodPlayer::checkPlacement(Board& b, int shipId)
{
	if (shipId >= game().nShips())
		return true;

	Point tempPoint;
	for (int i = 0; i < game().rows(); i++) {
		for (int j = 0; j < game().cols(); j++) {
			tempPoint.r = i;
			tempPoint.c = j;
			if (b.placeShip(tempPoint, shipId, HORIZONTAL)) {
				if (!checkPlacement(b, shipId + 1)) {
					b.unplaceShip(tempPoint, shipId, HORIZONTAL);
					b.placeShip(tempPoint, shipId, VERTICAL);
				}
				else {
					return true;
				}
			}
			else if (b.placeShip(tempPoint, shipId, VERTICAL)) {
				if (!checkPlacement(b, shipId + 1)) {
					b.unplaceShip(tempPoint, shipId, VERTICAL);
					b.placeShip(tempPoint, shipId, HORIZONTAL);
				}
				else {
					return true;
				}
			}
		}
	}

	return false;
}

//Mediocre Player does not record attacks done to itself
void GoodPlayer::recordAttackByOpponent(Point p) {};

Point GoodPlayer::recommendAttack()
{
	int tempR;
	int tempC;
	Point tempPoint;
	//First state -- random point without borders
	if (!state) {
		//Reset radius and points to hit in second state
		radius = 0;
		for (int i = 0; i < limitedSet.size(); i++) {
			limitedSet.pop();
		}
		tempR = randInt(game().rows() - 2);
		tempC = randInt(game().cols() - 2);
		tempPoint.r = tempR;
		tempPoint.c = tempC;
		return tempPoint;
	}
	//Second state -- activated when ship is hit, takes points in a incrementally higher radius around hit point
	if (state) {
		Point pointToHit = limitedSet.front();
		limitedSet.pop();
		return pointToHit;
		if (radius <= game().shipLength(longestShipAlive()))
			radius++;
	}

}

int GoodPlayer::longestShipAlive()
{
	int length = 0;

	for (int i = 0; i < game().nShips(); i++) {
		if (game().shipLength(i) > length)
			length = game().shipLength(i);
	}

	return length;
}

//Called in game.play() every time after this player attacks
void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool ShipDestroyed, int shipId)
{
	lastValidShot = validShot;
	this->shipId = shipId;
	this->ShipDestroyed = ShipDestroyed;

	state = false;

	if (validShot && shotHit && !ShipDestroyed) {
		//Second state of selecting positions at specific radius activated
		state = true;
		lastHit = p;
		int tempR;
		int tempC;
		Point tempPoint;
		//Clears previous radius set
		for (int i = 0; i < limitedSet.size(); i++) {
			limitedSet.pop();
		}

		//Top Coord
		tempR = lastHit.r - radius;
		tempC = lastHit.c;
		tempPoint.r = tempR;
		tempPoint.c = tempC;
		if (game().isValid(tempPoint))
			limitedSet.push(tempPoint);

		//Left Coord
		tempR = lastHit.r;
		tempC = lastHit.c - radius;
		tempPoint.r = tempR;
		tempPoint.c = tempC;
		if (game().isValid(tempPoint))
			limitedSet.push(tempPoint);

		//Right Coord
		tempR = lastHit.r;
		tempC = lastHit.c + radius;
		tempPoint.r = tempR;
		tempPoint.c = tempC;
		if (game().isValid(tempPoint))
			limitedSet.push(tempPoint);

		//Bottom Coord
		tempR = lastHit.r + 1;
		tempC = lastHit.c;
		tempPoint.r = tempR;
		tempPoint.c = tempC;
		if (game().isValid(tempPoint))
			limitedSet.push(tempPoint);

	}
	else if (validShot && !shotHit && !ShipDestroyed) {
		state = true;
	}

}

//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
	static string types[] = {
		"human", "awful", "mediocre", "good"
	};

	int pos;
	for (pos = 0; pos != sizeof(types) / sizeof(types[0]) &&
		type != types[pos]; pos++)
		;
	switch (pos)
	{
	case 0:  return new HumanPlayer(nm, g);
	case 1:  return new AwfulPlayer(nm, g);
	case 2:  return new MediocrePlayer(nm, g);
	case 3:  return new GoodPlayer(nm, g);
	default: return nullptr;
	}
}
