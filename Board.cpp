#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>

using namespace std;

class BoardImpl
{
public:
	BoardImpl(const Game& g);
	~BoardImpl();
	void clear();
	void block();
	void unblock();
	bool placeShip(Point topOrLeft, int shipId, Direction dir);
	bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
	void display(bool shotsOnly) const;
	bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
	bool allShipsDestroyed() const;

private:
	// TODO:  Decide what private members you need.  Here's one that's likely
	//        to be useful:
	const Game& m_game;
	char**grid;
};

BoardImpl::BoardImpl(const Game& g)
	: m_game(g)
{
	grid = new char*[g.rows()];

	for (int i = 0; i < g.rows(); i++) {
		grid[i] = new char[g.cols()];
	}
	// This compiles, but may not be correct
}

BoardImpl::~BoardImpl()
{
	delete grid[m_game.rows()];
	for (int i = 0; i < m_game.rows(); i++) {
		delete grid[m_game.cols()];
	}
}

void BoardImpl::clear()
{
	for (int i = 0; i < m_game.rows(); i++) {
		for (int j = 0; j < m_game.cols(); j++) {
			grid[i][j] = '.';
		}
	}
	// This compiles, but may not be correct
}

void BoardImpl::block()
{
	// Block cells with 50% probability
	for (int r = 0; r < m_game.rows(); r++)
		for (int c = 0; c < m_game.cols(); c++)
			if (randInt(2) == 0)
			{
				grid[r][c] = '#';
			}
}

void BoardImpl::unblock()
{
	for (int r = 0; r < m_game.rows(); r++)
		for (int c = 0; c < m_game.cols(); c++)
		{
			grid[r][c] = '.';
		}
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
	bool allClear = true;

	if (shipId < 0 || shipId >= m_game.nShips())
		allClear = false;

	if (!m_game.isValid(topOrLeft))
		allClear = false;

	if (dir == HORIZONTAL) {
		if (m_game.cols() - topOrLeft.c < m_game.shipLength(shipId)) {
			allClear = false;
		}
		for (int j = topOrLeft.c; j < topOrLeft.c + m_game.shipLength(shipId); j++) {
			grid[topOrLeft.r][j] = m_game.shipSymbol(shipId);
		}
	}

	if (dir == VERTICAL) {
		if (m_game.rows() - topOrLeft.r < m_game.shipLength(shipId)) {
			allClear = false;
		}
		for (int i = topOrLeft.r; i < topOrLeft.r + m_game.shipLength(shipId); i++) {
			grid[i][topOrLeft.c] = m_game.shipSymbol(shipId);
		}
	}

	return allClear;
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
	bool allClear = true;

	if (shipId < 0 || shipId >= m_game.nShips())
		allClear = false;

	if (m_game.isValid(topOrLeft))
		allClear = false;

	if (dir == HORIZONTAL) {
		if (m_game.cols() - topOrLeft.c < m_game.shipLength(shipId))
			allClear = false;
		for (int j = topOrLeft.c; j < m_game.shipLength(shipId); j++) {
			grid[topOrLeft.r][j] = '.';
		}
	}

	if (dir == VERTICAL) {
		if (m_game.rows() - topOrLeft.r < m_game.shipLength(shipId))
			allClear = false;
		for (int i = topOrLeft.r; i < m_game.shipLength(shipId); i++) {
			grid[i][topOrLeft.c] = '.';
		}
	}

	return allClear;
}

//Missed shots recorded in player
void BoardImpl::display(bool shotsOnly) const
{
	cout << "  ";
	for (int i = 0; i < m_game.cols(); i++) {
		cout << i;
	}
	cout << endl;

	for (int i = 0; i < m_game.rows(); i++) {
		cout << i << " ";
		for (int j = 0; j < m_game.cols(); j++) {
			if (shotsOnly == false)
				cout << grid[i][j];

			else if (shotsOnly == true){
				//Ship is present, hide it!
				if (grid[i][j] != '.' && grid[i][j] != 'X' && grid[i][j] != 'o') {
					cout << '.';
				}
				else {
					cout << grid[i][j];
				}
			}
		}
		cout << endl;
	}
}

//
bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
	shipDestroyed = true;
	char shipSym;
	if (!m_game.isValid(p))
		return false;

	if (shipId < 0 || shipId >= m_game.nShips())
		return false;

	if (grid[p.r][p.c] == 'X' || grid[p.r][p.c] == 'o')
		return false;

	//If the position is not empty, a ship was hit
	if (grid[p.r][p.c] != '.') {
		shotHit = true;
		shipSym = grid[p.r][p.c];
		grid[p.r][p.c] = 'X';

		//Check if the ship still exists on the board
		for (int i = 0; i < m_game.rows(); i++) {
			for (int j = 0; j < m_game.cols(); j++) {
				if (grid[i][j] == shipSym) {
					shipDestroyed = false;
					break;
				}

			}
		}

		if (shipDestroyed) {
			//Assign shipId if ship was destroyed
			for (int i = 0; i < m_game.nShips(); i++) {
				if (m_game.shipSymbol(i) == shipSym)
					shipId = i;
			}
		}
	}
	else if (grid[p.r][p.c] == '.') {
		shipDestroyed = false;
		shotHit = false;
		grid[p.r][p.c] = 'o';
	}

	return true;
}

bool BoardImpl::allShipsDestroyed() const
{
	for (int i = 0; i < m_game.rows(); i++) {
		for (int j = 0; j < m_game.cols(); j++) {
			if (grid[i][j] != '.' && grid[i][j] != 'X' && grid[i][j] != 'o')
				return false;
		}
	}

	return true;
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
	m_impl = new BoardImpl(g);
}

Board::~Board()
{
	delete m_impl;
}

void Board::clear()
{
	m_impl->clear();
}

void Board::block()
{
	return m_impl->block();
}

void Board::unblock()
{
	return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
	return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
	return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
	m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
	return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
	return m_impl->allShipsDestroyed();
}
