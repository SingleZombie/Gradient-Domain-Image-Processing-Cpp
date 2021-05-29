#include "QuadTree.h"

QuadTree::QuadTree(int _xl, int _xr, int _yl, int _yr)
	: xl(_xl), xr(_xr), yl(_yl), yr(_yr), length(_xr - _xl), depth(0),
	tl(nullptr), tr(nullptr), bl(nullptr), br(nullptr)
{

}

QuadTree::~QuadTree()
{
	if (tl)
	{
		delete tl, tr, bl, br;
	}
}

void QuadTree::createSon()
{
	int xm = (xl + xr) / 2;
	int ym = (yl + yr) / 2;
	tl = new QuadTree(xl, xm, yl, ym);
	tr = new QuadTree(xm, xr, yl, ym);
	bl = new QuadTree(xl, xm, ym, yr);
	br = new QuadTree(xm, xr, ym, yr);
}

pQuadTree QuadTree::find(int x, int y)
{
	if (x < xl || x >= xr || y < yl || y >= yr)
	{
		return nullptr;
	}
	if (isLeaf())
	{
		return this;
	}
	return findNextSon(x, y)->find(x, y);
}
pQuadTree QuadTree::findNextSon(int x, int y) const
{
	int xm = (xl + xr) / 2;
	int ym = (yl + yr) / 2;
	if (x < xm)
	{
		return y < ym ? tl : bl;
	}
	else
	{
		return y < ym ? tr : br;
	}
}