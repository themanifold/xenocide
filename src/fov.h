#ifndef FOV_H
#define FOV_H

class CMap;
struct CPosition;

class CMutualFOV {
private:
	CMap *m_map;
	inline void ConvertCornersToVisibleCells();
	inline bool AnyCellBlocks(const CPosition &start,const CPosition &end);
	inline void CheckWayToCorner(const CPosition &start,const CPosition &end,const int &distance);
	inline bool LineBlocksHorizontally(const int &dx,const int &dy,const CPosition &start,const CPosition &end,const int &cx,const int &cy);
	inline bool LineBlocksVertically(const int &dx,const int &dy,const CPosition &start,const CPosition &end,const int &cx,const int &cy);
	inline void PropagateShadow(const CPosition &start, const CPosition &diff, const int &dist);
	inline void CheckLOS(const CPosition &start, const CPosition &end, const int &distance);
	inline void CorrectNearest(const CPosition &start);
	inline bool CornerBlocks(const int &corner_x,const int &corner_y,const int &sgn_x,const int &sgn_y);

public:
	void Start(CMap* map, unsigned int x, unsigned int y, int maxRadius);
};

#endif
