#ifndef QUAD_TREE_H
#define QUAD_TREE_H

#include <algorithm>

class QuadTree;

using pQuadTree = QuadTree*;

class QuadTree
{
public:
	int xl, xr, yl, yr;
	int length, depth;
	pQuadTree tl, tr, bl, br;

	QuadTree(int _xl, int _xr, int _yl, int _yr);
	~QuadTree();
	void createSon();
	bool isLeaf() const { return tl == nullptr; }
	bool inNode(int x, int y) const { return x >= xl && x < xr && y >= yl && y < yr; }
	bool inSquare(int x, int y) const { return x >= xl && x <= xr && y >= yl && y <= yr; }
	void updateDepth() 
	{ 
		if (!isLeaf()) 
		{ 
			depth = std::max(std::max(tl->depth, tr->depth), std::max(bl->depth, br->depth)) + 1; 
		} 
	}
	pQuadTree find(int x, int y);
	pQuadTree findNextSon(int x, int y) const;
private:
	
};

class QuadTreeRoot
{
public:
	QuadTreeRoot();
	pQuadTree root;
};

#endif