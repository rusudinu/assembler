#include <unordered_map>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <ctype.h>

using namespace std;

ifstream in("input.txt");

unordered_map <string, int> labelsMap;


const std::string WHITESPACE = " \n\r\t\f\v";

FILE * fout;

enum class INSTRUCTION_TYPE
{
    LBI, LB, SB, CALL, JUMP, SYSCALL, MOV, ADD, BEQ, BNE, BGE, BLE, BGT, BLT
};
enum class REG_TYPE
{
    RA, RB, RC, RD, RE, RF, PC, SP, IN, RET
};
struct INSTRUCTION
{
    INSTRUCTION_TYPE type;
    REG_TYPE dest;
    REG_TYPE src;
    int value;
};
/* 0     1     2      3       4         5        6      7      8      9      10     11     12     13*/
string INSTRUCTION_TYPE_STR[] = {"LBI", "LB", "SB", "CALL", "JUMP", "SYSCALL", "MOV", "ADD", "BEQ", "BNE", "BGE", "BLE", "BGT", "BLT"};
string REG_TYPE_STR[] = {"RA", "RB", "RC", "RD", "RE", "RF", "PC", "SP", "IN", "RET"};

string ltrim(const string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == string::npos) ? "" : s.substr(start);
}

string rtrim(const string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == string::npos) ? "" : s.substr(0, end + 1);
}

string trim(const string& s)
{
    return rtrim(ltrim(s));
}

void eraseAllSubStr(std::string & mainStr, const std::string & toErase)
{
    size_t pos = std::string::npos;
    // Search for the substring in string in a loop untill nothing is found
    while ((pos  = mainStr.find(toErase) )!= std::string::npos)
    {
        // If found then erase it from string
        mainStr.erase(pos, toErase.length());
    }
}

INSTRUCTION_TYPE convertStringToInstructionEnum(string en)
{
    if(en == "LBI") return INSTRUCTION_TYPE::LBI;
    else if (en == "LB") return INSTRUCTION_TYPE::LB;
    else if (en == "SB") return INSTRUCTION_TYPE::SB;
    else if (en == "CALL") return INSTRUCTION_TYPE::CALL;
    else if (en == "JUMP") return INSTRUCTION_TYPE::JUMP;
    else if (en == "SYSCALL") return INSTRUCTION_TYPE::SYSCALL;
    else if (en == "MOV" ) return INSTRUCTION_TYPE::MOV;
    else if (en == "ADD" ) return INSTRUCTION_TYPE::ADD;
}

REG_TYPE convertStringToRegType(string en)
{
    if(en == "RA") return REG_TYPE::RA;
    else if (en == "RB") return REG_TYPE::RB;
    else if (en == "RC") return REG_TYPE::RC;
    else if (en == "RD") return REG_TYPE::RD;
    else if (en == "RE") return REG_TYPE::RE;
    else if (en == "RF") return REG_TYPE::RF;
}

int convertStringToInstructionTypeByte(string en)
{
    if(en == "LBI") return 0;
    else if (en == "LB") return 1;
    else if (en == "SB") return 2;
    else if (en == "CALL") return 3;
    else if (en == "JUMP") return 4;
    else if (en == "SYSCALL") return 5;
    else if (en == "MOV" ) return 6;
}

int convertStringToRegTypeByte(string en)
{
    if(en == "RA") return 0;
    else if (en == "RB") return 1;
    else if (en == "RC") return 2;
    else if (en == "RD") return 3;
    else if (en == "RE") return 4;
    else if (en == "RF") return 5;
}

int convertUnknownToTypeByte(string en)
{
    if(en == "RA") return 0;
    else if (en == "RB") return 1;
    else if (en == "RC") return 2;
    else if (en == "RD") return 3;
    else if (en == "RE") return 4;
    else if (en == "RF") return 5;
    else if (en == "PC") return 6;
    else if (en == "SP") return 7;
    else if (en == "IN") return 8;
    else if (en == "RET") return 9;

    else if (en == "LBI") return 0;
    else if (en == "LB") return 1;
    else if (en == "SB") return 2;
    else if (en == "CALL") return 3;
    else if (en == "JUMP") return 4;
    else if (en == "SYSCALL") return 5;
    else if (en == "MOV" ) return 6;
    else if (en == "ADD" ) return 7;
    else if (en == "BEQ") return 8;
    else if (en == "BNE") return 9;
    else if (en == "BGE") return 10;
    else if (en == "BLE") return 11;
    else if (en == "BGT") return 12;
    else if (en == "BLT") return 13;
    //else if (en == "MUL") return 10;
}

void writeToFile(int byte1, int byte2,int byte3,int byte4)
{
    char bytes[] = {byte1, byte2, byte3, byte4};
    fwrite(bytes,sizeof(bytes),1,fout);
    cout << byte1 << byte2 << byte3 << byte4; //TODO REMOVE THIS
}

INSTRUCTION parseRow(string line)
{
    INSTRUCTION ins = INSTRUCTION();

    //ins.type = INSTRUCTION_TYPE::CALL;
    //LBI RA, 0

    int byteArray[4] = {0, 0, 0, 0};
    int currentPos = 0;
    string cuv = "";
    for(int i = 0; i < line.size(); i++)
    {
        if(line[i] != ' ') cuv = cuv + line[i];
        else
        {
            //parse it
            if(cuv.find(",") != std::string::npos)  // has a comma
            {
                string result;
                //result = cuv.substr(0, myString.size()-1);
                remove_copy(cuv.begin(), cuv.end(), back_inserter(result), ','); //remove the ','
                if(isdigit(result[0]))
                {
                    //cout << result;
                    byteArray[currentPos] = stoi(result);
                    currentPos++;
                }
                else
                {
                    //cout << convertUnknownToTypeByte(result) <<" ";
                    byteArray[currentPos] = convertUnknownToTypeByte(result);
                    currentPos++;
                }
                //cout <<"MY WORD: " << result << "\n";
            }
            else
            {
                if(isdigit(cuv[0]))
                {
                    //cout << cuv;
                    byteArray[currentPos] = stoi(cuv);
                    currentPos++;
                }
                else
                {
                    //cout << convertUnknownToTypeByte(cuv) <<" ";
                    byteArray[currentPos] = convertUnknownToTypeByte(cuv);
                    currentPos++;
                }
                //cout <<"MY WORD: " << cuv << "\n";
            }
            cuv = "";
        }
    }
    if(isdigit(cuv[0]))
    {
        //cout << cuv << " ";
        byteArray[currentPos] = stoi(cuv);
        currentPos++;
    }
    else
    {
        //cout << convertUnknownToTypeByte(cuv) <<" ";
        byteArray[currentPos] = convertUnknownToTypeByte(cuv);
        currentPos++;
    }
    writeToFile(byteArray[0], byteArray[1], byteArray[2], byteArray[3]);
}

INSTRUCTION parseRowWithSpace(string label)
{
    cout << "FOUND LABEL " << label << "\n";
    if(labelsMap.count(label) == 0) cout << "LABEL NOT FOUND: " << label << "\n";
    else
    {
        cout << "LABEL FOUND: " << labelsMap[label] << "\n";
        int byteArray[4] = {0, 0, 0, 0};
        int currentPos = 0;
        byteArray[0] = 4; //JUMP
        byteArray[1] = labelsMap[label];
        writeToFile(byteArray[0], byteArray[1], byteArray[2], byteArray[3]);
    }
}

INSTRUCTION parseRowWithBEQ(string row)
{

}

INSTRUCTION parseRowWithBNE(string row)
{

}

int main()
{
    fout = fopen("game.rom","wb");
    int lineNumber = 0;

    if (in.is_open())
    {
        std::string line;
        while (std::getline(in, line))
        {
            //using printf() in all tests for consistency
            //printf("%s", line.c_str());
            //line.c_str()
            //string toParse = line;
            //TODO TRIM THE START OF THE WORD
            line = ltrim(line); //TRIMS THE INDENTATION
            if(line.rfind("CALL", 0) == 0)
            {
                eraseAllSubStr(line, "CALL ");
                parseRowWithSpace(line);
            }
            else if(line.rfind("JUMP", 0) == 0)
            {
                //THE LINE IS A LABEL
                eraseAllSubStr(line, "JUMP ");
                parseRowWithSpace(line);
            }
            else if (line.rfind("BEQ", 0) == 0)
            {
                parseRowWithBEQ(line);
            }
            else if (line.rfind("BNE", 0) == 0)
            {
                parseRowWithBNE(line);
            }
            else if(line.find(":") != std::string::npos)  // has a ":"{
            {
                cout << "was label" << "\n";
                //remove_copy(line.begin(), line.end(), back_inserter(line), ':');
                line.erase(std::remove(line.begin(), line.end(), ':'), line.end());
                pair<std::string,double> instr (line, lineNumber);
                labelsMap.insert(instr);
                cout << "LABEL: " << line << "\n"; // << " value: " << lineNumber << "\n";
            }

            else if(trim(line) != "" )
            {
                cout << "line read: " << line << "\n";//<< "LINE NUMBER: " << lineNumber << "\n";
                parseRow(line);
                lineNumber++;
            }
            else
            {
                cout << "was empty line" << "\n";
            }
        }

    }
    in.close();

    /*
    parseRow("LBI RA, 4");
     cout << "\n"; //TODO REMOVE THOSE
     //parse("ADD RD, RC, RB");
     cout << "\n"; //TODO REMOVE THOSE
     //parse("MOV RA, RC");
    */
    fclose(fout);
    return 0;
}


/*
RA, RB, RC, RD, RE, RF -- registri
+ LBI REGISTER, VALUE
 + SB REGISTER, ADDRESS(REGISTER_OFFSET)
+ CALL address
+ JUMP address
+ SYSCALL  ( no arguments here )
*/
