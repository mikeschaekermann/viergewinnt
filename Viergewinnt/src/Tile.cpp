#include "Tile.h"

#include <sstream>
#include "cinder/app/App.h"
#include "cinder/Vector.h"

using namespace ci;

const ci::Color Tile::m_emptyColor = ci::Color(1,1,1);
const ci::Color Tile::m_p0Color	  = ci::Color(1,1,0);
const ci::Color Tile::m_p1Color	  = ci::Color(1,0,0);

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

void Tile::drawAt( int x, int y )
{
	Vec2f where(x*TILE_WIDTH, y*TILE_HEIGHT);
	Rectf rec(where, where+Vec2f(TILE_WIDTH, TILE_HEIGHT));

	Color color;
	if(m_isEmpty){
		color=m_emptyColor;
	}
	else
	{
		if(m_player==0)
		{
			color=m_p0Color;
		}
		else if(m_player==1)
		{
			color=m_p1Color;
		}
	}
	gl::color(color);
	gl::drawSolidRect(rec);
}

std::string Tile::toString() const
{
	std::ostringstream oss;
	oss << m_player;
	return oss.str();
}
