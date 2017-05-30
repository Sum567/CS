#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <vector>

using namespace std;

class GameImpl
{
public:
	GameImpl(int nRows, int nCols);
	~GameImpl();
	int rows() const;
	int cols() const;
	bool isValid(Point p) const;
	Point randomPoint() const;
	bool addShip(int length, char symbol, string name);
	int nShips() const;
	int shipLength(int shipId) const;
	char shipSymbol(int shipId) const;
	string shipName(int shipId) const;
	Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
private:
	//Board* m_board;
	int m_rows;
	int m_cols;

	struct Ship {
		int shipLength;
		char symbol;
		string shipName;

		Ship(int shipLength, char symbol, string shipName) {
			this->shipLength = shipLength;
			this->symbol = symbol;
			this->shipName = shipName;
		}

		Ship(const Ship &rhs) {
			this->shipLength = rhs.shipLength;
			this->symbol = rhs.symbol;
			this->shipName = rhs.shipName;
		}
	};

	vector <Ship> ships;

};

void waitForEnter()
{
	cout << "Press enter to continue: ";
	cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols)
	:m_rows(nRows), m_cols(nCols)
{


}

GameImpl::~GameImpl()
{
	/*for (int i = 0; i < ships.size(); i++) {
		delete &ships[i];
	}*/
	ships.clear();
}

int GameImpl::rows() const
{
	return m_rows;  // This compiles but may not be correct
}

int GameImpl::cols() const
{
	return m_cols;  // This compiles but may not be correct
}

bool GameImpl::isValid(Point p) const
{
	return p.r >= 0 && p.r < rows() && p.c >= 0 && p.c < cols();
}

//For awful player attack
Point GameImpl::randomPoint() const
{
	return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
	if (length < 0)
		return false;

	for (int i = 0; i < nShips(); i++) {
		if (ships[i].symbol == symbol) {
			return false;
		}
	}

	if (symbol == 'X' || symbol == 'o' || symbol == '.')
		return false;

	Ship* tempShip = new Ship(length, symbol, name);

	ships.push_back(*tempShip);

	return true;

}

int GameImpl::nShips() const
{
	return ships.size();
}

int GameImpl::shipLength(int shipId) const
{
	if (shipId < 0 || shipId > ships.size())
		return false;

	return ships[shipId].shipLength;
}

char GameImpl::shipSymbol(int shipId) const
{
	if (shipId < 0 || shipId > ships.size())
		return false;
	return ships[shipId].symbol;
}

string GameImpl::shipName(int shipId) const
{
	return ships[shipId].shipName;
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
	b1.clear();
	b2.clear();
	if (p1->isHuman()) {
		cout << p1->name() << " the Human" << " must place " << nShips() << " ships." << endl;
		b1.display(false);
	}

	if (!p1->placeShips(b1))
		return nullptr;

	if (p2->isHuman()) {
		cout << p2->name() << " the Human" << " must place " << nShips() << " ships." << endl;
		b2.display(false);
	}
	if (!p2->placeShips(b2))
		return nullptr;

	while (!b1.allShipsDestroyed() && !b2.allShipsDestroyed()) {
		//initial display of opponent board
		if (p1->isHuman()) {
			b2.display(true);
		}
		else if (!p1->isHuman()) {
			cout << p1->name() << "'s turn. Board for " << p2->name() << ":" << endl;
			b2.display(false);
		}

		/*p1 attack*/
		bool p1_shotHit;
		bool p1_shipDestroyed;
		bool p1_shotValid;
		int p1_shipId = 0;
		Point p1_pointHit = p1->recommendAttack();
		b2.attack(p1_pointHit, p1_shotHit, p1_shipDestroyed, p1_shipId);
		if (isValid(p1_pointHit)) {
			p1_shotValid = true;
		}
		else {
			p1_shotValid = false;
		}
		p1->recordAttackResult(p1_pointHit, p1_shotValid, p1_shotHit, p1_shipDestroyed, p1_shipId);
		p2->recordAttackByOpponent(p1_pointHit);
		if (!p1_shotHit && !p1_shipDestroyed)
			cout << p1->name() << " attacked " << "<" << p1_pointHit.r << "," << p1_pointHit.c << ">" << " and missed, resulting in:" << endl;
		else if (p1_shotHit && !p1_shipDestroyed)
			cout << p1->name() << " attacked " << "<" << p1_pointHit.r << "," << p1_pointHit.c << ">" << " and hit something, resulting in:" << endl;
		else if (p1_shotHit && p1_shipDestroyed)
			cout << p1->name() << " attacked " << "<" << p1_pointHit.r << "," << p1_pointHit.c << ">" << " and destroyed the " << shipName(p1_shipId) << endl;
		//display after p1 attack
		if (p1->isHuman())
			b2.display(true);
		else if (!p1->isHuman())
			b2.display(false);

		if (shouldPause)
			waitForEnter();

		//initial display of opponent board
		if (p2->isHuman()) {
			cout << p2->name() << "'s turn. Board for " << p1->name() << ":" << endl;
			b1.display(true);
		}
		else if (!p2->isHuman()) {
			cout << p2->name() << "'s turn. Board for " << p1->name() << ":" << endl;
			b1.display(false);
		}

		
		/*p2 attack*/
		bool p2_shotHit;
		bool p2_shipDestroyed;
		bool p2_shotValid;
		int p2_shipId = 0;
		Point p2_pointHit = p2->recommendAttack();

		//Check if position has been fired at before
		while (!b1.attack(p2_pointHit, p2_shotHit, p2_shipDestroyed, p2_shipId) && p2->isHuman()) {
			p2_pointHit = p2->recommendAttack();
			b1.attack(p2_pointHit, p2_shotHit, p2_shipDestroyed, p2_shipId);
		}
			
		
		if (isValid(p2_pointHit)) {
			p2_shotValid = true;
		}
		else {
			p2_shotValid = false;
		}
		p2->recordAttackResult(p2_pointHit, p2_shotValid, p2_shotHit, p2_shipDestroyed, p2_shipId);
		p1->recordAttackByOpponent(p1_pointHit);
		if (p2_shotHit && !p2_shipDestroyed)
			cout << p2->name() << " attacked " << "<" << p2_pointHit.r << "," << p2_pointHit.c << ">" << " and hit something, resulting in:" << endl;
		else if (!p2_shotHit)
			cout << p2->name() << " attacked " << "<" << p2_pointHit.r << "," << p2_pointHit.c << ">" << " and missed, resulting in:" << endl;
		else if (p2_shotHit && p2_shipDestroyed)
			cout << p2->name() << " attacked " << "<" << p2_pointHit.r << "," << p2_pointHit.c << ">" << " and destroyed the " << shipName(p2_shipId) << endl;
		//display after p2 attack
		if (p2->isHuman())	
			b1.display(true);
		else if (!p2->isHuman())
			b1.display(false);

		if (shouldPause)
			waitForEnter();

	}


	//Returns winner player based on whose ships were destroyed
	if (b1.allShipsDestroyed())
		return p2;
	if (b2.allShipsDestroyed())
		return p1;
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
	if (nRows < 1 || nRows > MAXROWS)
	{
		cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
		exit(1);
	}
	if (nCols < 1 || nCols > MAXCOLS)
	{
		cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
		exit(1);
	}
	m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
	delete m_impl;
}

int Game::rows() const
{
	return m_impl->rows();
}

int Game::cols() const
{
	return m_impl->cols();
}

bool Game::isValid(Point p) const
{
	return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
	return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
	if (length < 1)
	{
		cout << "Bad ship length " << length << "; it must be >= 1" << endl;
		return false;
	}
	if (length > rows() && length > cols())
	{
		cout << "Bad ship length " << length << "; it won't fit on the board"
			<< endl;
		return false;
	}
	if (!isascii(symbol) || !isprint(symbol))
	{
		cout << "Unprintable character with decimal value " << symbol
			<< " must not be used as a ship symbol" << endl;
		return false;
	}
	if (symbol == 'X' || symbol == '.' || symbol == 'o')
	{
		cout << "Character " << symbol << " must not be used as a ship symbol"
			<< endl;
		return false;
	}
	int totalOfLengths = 0;
	for (int s = 0; s < nShips(); s++)
	{
		totalOfLengths += shipLength(s);
		if (shipSymbol(s) == symbol)
		{
			cout << "Ship symbol " << symbol
				<< " must not be used for more than one ship" << endl;
			return false;
		}
	}
	if (totalOfLengths + length > rows() * cols())
	{
		cout << "Board is too small to fit all ships" << endl;
		return false;
	}
	return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
	return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
	assert(shipId >= 0 && shipId < nShips());
	return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
	assert(shipId >= 0 && shipId < nShips());
	return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
	assert(shipId >= 0 && shipId < nShips());
	return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
	if (p1 == nullptr || p2 == nullptr || nShips() == 0)
		return nullptr;
	Board b1(*this);
	Board b2(*this);
	return m_impl->play(p1, p2, b1, b2, shouldPause);
}

