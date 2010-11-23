#ifndef __MOVE_H__
#define __MOVE_H__

#include <boost/array.hpp>

using namespace std;

extern const char pieceType[32];
extern const char pieceSymbol[7];

struct Move {
	char index;
	char to;
	char from;
	char xindex;

	bool operator==(const Move &rhs) const;
	void setNull();
	bool isNull();
	string dump() const;
	string toString() const;
	bool parse(string s);
};

// forward declations
struct MoveNode;
struct MoveList;

struct MoveNode {
	Move move;
	int score;
	bool check;
};

struct MoveList {
	boost::array<MoveNode, 320> list;
	int size;

	MoveList() : size(0) {}
};

struct ScoreSort {
	bool operator()(const MoveNode &a, const MoveNode &b)
	{
		return a.score > b.score;
	}
};

string printLoc(const char loc);

#endif
