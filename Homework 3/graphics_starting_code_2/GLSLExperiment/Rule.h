#pragma once

#include "Angel.h"
#include <string>
#include <map>

using namespace std;

class Rule
{
public:
	float len;
	int iteration;
	vec3 rotation;
	string pattern;
	Rule(char* fileName);
	~Rule(void);
};

