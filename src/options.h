#ifndef OPTIONS_H
#define OPTIONS_H

enum EOptions {
	OPTION_LETTERS,
	OPTION_EXPERIENCE,
	OPTION_AUTO_AIM,
	OPTION_DONT_LET_SWAP,
	OPTION_CURSOR,
	OPTION_GETALL,
	OPTION_AIM_IF_NO_ENEMY,
	NUMBER_OF_OPTIONS
};

class COptions {
public:
       bool number[NUMBER_OF_OPTIONS];
       COptions();
};

#endif


