#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
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
		cout << "Enter h or v for direction of " << game().shipName(i) << "<length " << game().shipLength(i) << ">:" << endl;
		cin >> dir;
		while (dir != 'h' && dir != 'v') {
			cout << "Direction must be h or v." << endl;
			cout << "Enter h or v for direction of " << game().shipName(i) << "<length " << game().shipLength(i) << ">:" << endl;
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

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class MediocrePlayer :public Player
{
public:
	MediocrePlayer(string nm, const Game& g);
	virtual bool MediocrePlayer::placeShips(Board& b);
	bool MediocrePlayer::checkPlacement(Board& b, int shipId);
	virtual void MediocrePlayer::recordAttackByOpponent(Point p);
	virtual Point MediocrePlayer::recommendAttack();
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

}

bool MediocrePlayer::placeShips(Board& b)
{
	b.block();

	checkPlacement(b, 0);
	if (!checkPlacement(b, 0))
		return false;
	b.unblock();
	return true;
}

bool MediocrePlayer::checkPlacement(Board& b, int shipId)
{
	Point tempPoint;
	for (int i = 0; i < game().rows(); i++) {
		for (int j = 0; j < game().cols(); j++) {
			tempPoint.r = i;
			tempPoint.c = j;
			if (b.placeShip(tempPoint, shipId, HORIZONTAL)) {
				checkPlacement(b, shipId + 1);
			}
			else {
				b.unplaceShip(tempPoint, shipId, HORIZONTAL);
				if (b.placeShip(tempPoint, shipId, VERTICAL)) {
					checkPlacement(b, shipId + 1);
				}
				else {
					//if can't do horizontal or vertical...unplace previous ships
					b.unplaceShip(tempPoint, shipId, VERTICAL);
					

				}
			}
		}
	}

	if (shipId == game().nShips() - 1)
		return true;
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
	//random point not chosen before
	if (!state) {
		tempR = randInt(game().rows() - 1);
		tempC = randInt(game().cols() - 1);
		tempPoint.r = tempR;
		tempPoint.c = tempC;
		return tempPoint;
	}
	if (state) {
		//Attack missed: stay in state 2
		if (!lastValidShot) {
			state = true;
		}
		//HIts a ship without destroying - stay in state 2
		if (lastValidShot && !ShipDestroyed) {
			state = true;
		}
		//ship destroyed - switch to state 1
		if (ShipDestroyed) {
			state = false;
		}

		//Set of valid positions no more than 4 steps away from each dir

		limitedSet.clear();
		Point tempPoint;

		//4 spots north
		for (int i = 4; i > lastHit.r; i--) {
			tempPoint.r = i;
			tempPoint.c = lastHit.c;
			limitedSet.push_back(tempPoint);
		}
		//4 spots south
		for (int i = lastHit.r + 1; i <= 4; i++) {
			tempPoint.r = i;
			tempPoint.c = lastHit.c;
			limitedSet.push_back(tempPoint);
		}
		//4 spots west
		for (int i = 4; i > lastHit.c; i--) {
			tempPoint.c = i;
			tempPoint.r = lastHit.r;
			limitedSet.push_back(tempPoint);
		}
		//4 spots east
		for (int i = lastHit.c; i <= 4; i++) {
			tempPoint.c = i;
			tempPoint.r = lastHit.r;
			limitedSet.push_back(tempPoint);
		}

		int randIndex = randInt(limitedSet.size() - 1);
		tempPoint.r = limitedSet[randIndex].r;
		tempPoint.c = limitedSet[randIndex].c;

		return tempPoint;
	}


}

//Called in game.play() every time after this player attacks
void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool ShipDestroyed, int shipId)
{
	lastValidShot = validShot;
	this->shipId = shipId;
	this->ShipDestroyed = ShipDestroyed;

	bool state = false;

	if (validShot && shotHit && !ShipDestroyed) {
		state = true;
		lastHit = p;
	}

}

//*********************************************************************
//  GoodPlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
typedef AwfulPlayer GoodPlayer;

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
