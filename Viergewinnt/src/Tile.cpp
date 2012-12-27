#include "Tile.h"

#include <sstream>
#include "cinder/app/App.h"
#include "cinder/Vector.h"

using namespace ci;

const ci::Color Tile::m_emptyColor = ci::Color(0.1, 0.1, 0.1);
const ci::Color Tile::m_p0Color	  = ci::Color(0.7,0.7,0);
const ci::Color Tile::m_p1Color	  = ci::Color(0.7,0,0);
const ci::Color Tile::m_selectionColor = ci::Color(0.3, 0.3, 1);

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

bool Tile::belongsToPlayer(int n) const
{
	return (m_player == n);
}

void Tile::draw()
{
	
}

void Tile::drawAt( int x, int y, bool active )
{
	Vec2f where(x*TILE_WIDTH + OFFSET_LEFT, y*TILE_HEIGHT + OFFSET_TOP);
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

	if (active)
	{
		gl::color(m_selectionColor);
		gl::drawStrokedRect(rec);
	}
}

std::string Tile::toString() const
{
	std::ostringstream oss;
	oss << m_player;
	return oss.str();
}
