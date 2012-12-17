#include "Tile.h"

#include <sstream>
#include "cinder/app/App.h"


Tile::Tile(void) : 
	m_isEmpty(true), m_player(-1)
{

}


Tile::~Tile(void)
{
}

bool Tile::isEmpty() const
{
	return m_isEmpty;
}

void Tile::setOccupied()
{
	m_isEmpty = false;
}

void Tile::setPlayer( int n )
{
	setOccupied();
	m_player = n;
}

void Tile::draw()
{
	
}

std::string Tile::toString() const
{
	std::ostringstream oss;
	oss << m_player;
	return oss.str();
}
