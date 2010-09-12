#ifndef __MOVE_LOOKUP_H__
#define __MOVE_LOOKUP_H__

#define ATCK_CMP(A, B)		((A^B) >> 3) - ((A^(-B)) >> 3)
#define CAPTURE_MOVE(A, B)	(A * B <  0)
#define ANY_MOVE(A, B)		(A * B <= 0)
#define NOT_EMPTY(A, B)		(A * B != 0)
#define EMPTY_MOVE(A, B)	(A * B == 0)
#define NOT_CAPTURE(A, B)	(A * B >= 0)
#define OWN_PIECE(A, B)		(A * B >  0)

class MoveLookup {
private:
	static char mailbox[120];

	static char mailbox64[64];

	static char offsets[7][8];

	char *square;

public:
	MoveLookup(char *Square) : square(Square) {}

	char* genAll(const char &from) const;

	char* genCapture(const char &from) const;

	char* genMove(const char &from) const;

	bool fromto(const char &From, const char &To) const;

	bool isAttacked(const char &from) const;
};

#endif
