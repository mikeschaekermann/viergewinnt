#include "Field.h"

#include "cinder/app/App.h"

Field::Field(void)
{

}


Field::~Field(void)
{
	init();
}

void Field::init()
{
	for(int x=0; x<FIELD_WIDTH; ++x)
	{
		for(int y=0; y<FIELD_HEIGHT; ++y)
		{
			m_tiles[x][y] = TilePtr(new Tile());
		}
	}
}

bool Field::isFull( int col ) const
{
	for(int y=0; y<FIELD_HEIGHT; ++y)
	{
		if(m_tiles[col][y]->isEmpty())
		{
			return false;
		}
	}
	return true;
}

bool Field::insert( int col, int player )
{
	
	for(int y=0; y<FIELD_HEIGHT; ++y)
	{
		if(m_tiles[col][y]->isEmpty())
		{
			m_tiles[col][y]->setPlayer(player);
			return true;
		}
	}
	return false;
	
}

void Field::draw()
{
	for(int y=FIELD_HEIGHT-1; y>=0; --y)
	{
		for(int x=0; x<FIELD_WIDTH; ++x)
		{
			m_tiles[x][y]->drawAt(x,y);
		}
	}
}

void Field::print() const
{
	for(int y=FIELD_HEIGHT-1; y>=0; --y)
	{
		for(int x=0; x<FIELD_WIDTH; ++x)
		{
			ci::app::console() << m_tiles[x][y]->toString() << "\t|";
		}
		ci::app::console() << std::endl;
	}
}
