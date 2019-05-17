

#ifndef __3420utils_H__
#define __3420utils_H__

class Utils {

public:
	
/**
 *  Delays the processor for milli milliseconds
 *  @param  milli
 *          the number of milliseconds to delay
 */
  static void delay(int milli);

/** Represents blank screen */
	static int empty[32][32];

/** Represents properties of Maze 1 */
	static int maze1[32][32];

/**	Represents end game screen */
	static int game_over[32][32];
};

#endif