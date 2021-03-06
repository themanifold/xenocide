//#pragma once
#ifndef POSITION_H
#define POSITION_H

struct CPosition {
	int x;
	int y;
	CPosition()
	{
	};
	CPosition(const int &a_x, const int &a_y)
		: x(a_x), y(a_y)
	{
	};


	bool operator<(const CPosition &r) const
	{
		if (x<r.x)
			return true;
		else if (x==r.x && y<r.y)
			return true;
		return false;
	}

	bool operator== ( const CPosition &r) const
	{ 
		return (x==r.x && y == r.y); 
	};
	bool operator!= ( const CPosition &r) const
	{ 
		return !operator==(r); 
	};

	const CPosition & operator= ( const CPosition &r) 
	{ 
		x=r.x;
		y=r.y;
		return *this;
	};
};


#endif

