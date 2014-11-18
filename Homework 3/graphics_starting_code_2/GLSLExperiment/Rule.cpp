#include "Rule.h"
#include "textfile.h"
#include <sstream>
using namespace std;

Rule::Rule(char* fileName)
{
	char* content = textFileRead(fileName);
	stringstream ss(content);
	string line;
	
	do {
		getline(ss, line);
	} while (line[0] == '#');

	string char_buf;
	stringstream slen(line);
	int iter;
	string start;
	map<char, char> rep;
	map<char, string> rules;
	slen >> char_buf >> Rule::len;
	ss >> char_buf >> iter;
	iteration = iter;
	ss >> char_buf >> Rule::rotation.x >> Rule::rotation.y >> Rule::rotation.z;
	getline(ss, line);
	while(getline(ss, line)){
		stringstream ls(line);
		if(line.find("rep") == 0){
			string repStr;
			ls >> char_buf >> repStr;
			if(repStr.length() == 3){
				rep[repStr[0]] = repStr[2];
			} else if(repStr.length() == 2){
				rep[repStr[0]] = '\0';
			}
		} else if (line.find("start") == 0){
			ls >> char_buf >> start;
		} else {
			string from, to;
			ls >> from >> to;
			rules[from[0]] = to;
		}

	}

	pattern = start;
	string newPattern;
	iter = 1;
	for(int i = 0; i < iter; i++){
		newPattern = "";
		for(int c = 0; c < pattern.length(); c++){
			if(rules.find(pattern[c]) != rules.end()){
				newPattern += rules.at(pattern[c]);
			} else {
				newPattern += pattern[c];
			}
		}
		pattern = newPattern;
	}
	
	newPattern = "";
	for(int c = 0; c < pattern.length(); c++){
		if(rep.find(pattern[c]) != rep.end()){
			char newChar = rep.at(pattern[c]);
			if(newChar != '\0'){
				newPattern += newChar;
			}
		} else {
			newPattern += pattern[c];
		}
	}
	
	pattern = newPattern;

	printf("%s	Rotation Values:	%.2f %.2f %.2f\n", fileName, rotation.x, rotation.y, rotation.z);
	printf("Current Pattern: %s\n\n", pattern.c_str());
}


Rule::~Rule(void)
{
}
