#include <unordered_map>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <math.h>
#include <ctype.h>

using namespace std;

ifstream inPreParse("input.txt");
ifstream in("input.txt");

unordered_map <string, int> labelsMap;


const std::string WHITESPACE = " \n\r\t\f\v";

FILE * fout;

union LABEL
{
    unsigned short int SHORT;
    unsigned char BYTE[2];
};

enum class INSTRUCTION_TYPE
{
    LBI, LB, SB, CALL, JUMP, SYSCALL, MOV, ADD, BEQ, BNE, BGE, BLE, BGT, BLT, SBIX, LBIX, JRT, PUSH, POP, RRA, RRB, RRC, RRD, RRE, RRF, XOR, SUB
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
/* 0     1     2      3       4         5        6      7      8      9      10     11     12    13      14     15*/
string INSTRUCTION_TYPE_STR[] = {"LBI", "LB", "SB", "CALL", "JUMP", "SYSCALL", "MOV", "ADD", "BEQ", "BNE", "BGE", "BLE", "BGT", "BLT", "SBIX", "LBIX",
                                 "RRA", "RRB", "RRC", "RRD", "RRE", "RRF", "XOR", "JRT", "PUSH", "POP", "SUB"
                                };
string REG_TYPE_STR[] = {"RA", "RB", "RC", "RD", "RE", "RF", "PC", "SP", "IN", "RET"};

int romData[4000];
int romINSTR[10000];
int romDataPosition = 4;
int romDataCodeStart = 0;

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

/*
 * Case Sensitive Implementation of startsWith()
 * It checks if the string 'mainStr' starts with given string 'toMatch'
 */
bool startsWith(std::string mainStr, std::string toMatch)
{
    // std::string::find returns 0 if toMatch is found at starting
    if(mainStr.find(toMatch) == 0)
        return true;
    else
        return false;
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
    else if (en == "LBI") return (int)INSTRUCTION_TYPE::LBI;
    else if (en == "LB") return (int)INSTRUCTION_TYPE::LB;
    else if (en == "SB") return (int)INSTRUCTION_TYPE::SB;
    else if (en == "CALL") return (int)INSTRUCTION_TYPE::CALL;
    else if (en == "JUMP") return (int)INSTRUCTION_TYPE::JUMP;
    else if (en == "SYSCALL") return (int)INSTRUCTION_TYPE::SYSCALL;
    else if (en == "MOV" ) return (int)INSTRUCTION_TYPE::MOV;
    else if (en == "ADD" ) return (int)INSTRUCTION_TYPE::ADD;
    else if (en == "BEQ") return (int)INSTRUCTION_TYPE::BEQ;
    else if (en == "BNE") return (int)INSTRUCTION_TYPE::BNE;
    else if (en == "BGE") return (int)INSTRUCTION_TYPE::BGE;
    else if (en == "BLE") return (int)INSTRUCTION_TYPE::BLE;
    else if (en == "BGT") return (int)INSTRUCTION_TYPE::BGT;
    else if (en == "BLT") return (int)INSTRUCTION_TYPE::BLT;
    else if (en == "SBIX") return (int)INSTRUCTION_TYPE::SBIX;
    else if (en == "LBIX") return (int)INSTRUCTION_TYPE::LBIX;
    else if (en == "RRA") return (int)INSTRUCTION_TYPE::RRA;
    else if (en == "RRB") return (int)INSTRUCTION_TYPE::RRB;
    else if (en == "RRC") return (int)INSTRUCTION_TYPE::RRC;
    else if (en == "RRD") return (int)INSTRUCTION_TYPE::RRD;
    else if (en == "RRE") return (int)INSTRUCTION_TYPE::RRE;
    else if (en == "RRF") return (int)INSTRUCTION_TYPE::RRF;
    else if (en == "XOR") return (int)INSTRUCTION_TYPE::XOR;
    else if (en == "JRT") return (int)INSTRUCTION_TYPE::JRT;
    else if (en == "PUSH") return (int)INSTRUCTION_TYPE::PUSH;
    else if (en == "POP") return (int)INSTRUCTION_TYPE::POP;
}

void writeToFile(int byte1, int byte2,int byte3,int byte4)
{
    cout << "\n" << "-----------------WRITE TO FILE---------------" << "\n\n";
    char bytes[] = {byte1, byte2, byte3, byte4};
    fwrite(bytes,sizeof(bytes),1,fout);
    cout << " | BYTE1: " << byte1 << " | BYTE2: " << byte2  << " | BYTE3: " << byte3 << " | BYTE4: " << byte4; //TODO REMOVE THIS
    cout << "\n\n" << "-----------------WRITE TO FILE---------------" << "\n";
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

INSTRUCTION parseRowWithSpace(string label, bool isCall)
{
    cout << "FOUND LABEL " << label << "\n";
    if(labelsMap.count(label) == 0) cout << "LABEL NOT FOUND: " << label << "\n";
    else
    {
        cout << "LABEL NUMBER FOUND: " << labelsMap[label] << "\n";
        LABEL myLabel;
        myLabel.SHORT = labelsMap[label];
        std::cout << "HERE: " << (int)myLabel.BYTE[1] << ' ' << (int)myLabel.BYTE[0] << std::endl;
        int byteArray[4] = {0, 0, 0, 0};
        int currentPos = 0;
        byteArray[0] = isCall ? (int)INSTRUCTION_TYPE::CALL : (int)INSTRUCTION_TYPE::JUMP; //CALL | JUMP
        byteArray[1] = 0; //labelsMap[label];
        byteArray[2] = (int)myLabel.BYTE[1];
        byteArray[3] = (int)myLabel.BYTE[0];
        writeToFile(byteArray[0], byteArray[1], byteArray[2], byteArray[3]);
    }
}

INSTRUCTION parseRowWithBEQ(string row)
{

}

INSTRUCTION parseRowWithBNE(string row)
{

}

void parseRowRamLoader(string line)
{
    cout << "RAM LOADER FOUND: " << line << "\n";
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
                    LABEL myLabel;
                    myLabel.SHORT = stoi(cuv);
                    //std::cout << "HERE: " << (int)myLabel.BYTE[1] << ' ' << (int)myLabel.BYTE[0] << std::endl;
                    //byteArray[currentPos] = stoi(cuv);
                    byteArray[2] = (int)myLabel.BYTE[1];
                    byteArray[3] = (int)myLabel.BYTE[0];
                    currentPos++;
                }
                else
                {
                    cout << convertUnknownToTypeByte(result) <<" ";
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
                    LABEL myLabel;
                    myLabel.SHORT = stoi(cuv);
                    //std::cout << "HERE: " << (int)myLabel.BYTE[1] << ' ' << (int)myLabel.BYTE[0] << std::endl;
                    //byteArray[currentPos] = stoi(cuv);
                    byteArray[2] = (int)myLabel.BYTE[1];
                    byteArray[3] = (int)myLabel.BYTE[0];
                    currentPos++;
                    //writeToFile(byteArray[0], byteArray[1], byteArray[2], byteArray[3]);
                    //return;
                }
                else
                {
                    cout << convertUnknownToTypeByte(cuv) <<" ";
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
        //cout << "OUTER IF";
        LABEL myLabel;
        myLabel.SHORT = stoi(cuv);
        //std::cout << "HERE: " << (int)myLabel.BYTE[1] << ' ' << (int)myLabel.BYTE[0] << std::endl;
        //byteArray[currentPos] = stoi(cuv);
        byteArray[2] = (int)myLabel.BYTE[1];
        byteArray[3] = (int)myLabel.BYTE[0];
        currentPos++;
        //writeToFile(byteArray[0], byteArray[1], byteArray[2], byteArray[3]);
        //return;
    }
    else
    {
        cout << convertUnknownToTypeByte(cuv) <<" ";
        byteArray[currentPos] = convertUnknownToTypeByte(cuv);
        currentPos++;
    }
    byteArray[3] = labelsMap[cuv];
    cout << "CHECK ME: " << labelsMap[cuv] << '\n';
    cout << "CHECK ME: " << cuv << '\n';
    writeToFile(byteArray[0], byteArray[1], byteArray[2], byteArray[3]);
}

void parseRowRamLoaderWithLabel(string line)
{
    cout << "RAM LOADER FOUND: " << line << "\n";
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
                    LABEL myLabel;
                    myLabel.SHORT = stoi(cuv);
                    //std::cout << "HERE: " << (int)myLabel.BYTE[1] << ' ' << (int)myLabel.BYTE[0] << std::endl;
                    //byteArray[currentPos] = stoi(cuv);
                    byteArray[2] = (int)myLabel.BYTE[1];
                    byteArray[3] = (int)myLabel.BYTE[0];
                    currentPos++;
                }
                else
                {
                    cout << convertUnknownToTypeByte(result) <<" ";
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
                    LABEL myLabel;
                    myLabel.SHORT = stoi(cuv);
                    //std::cout << "HERE: " << (int)myLabel.BYTE[1] << ' ' << (int)myLabel.BYTE[0] << std::endl;
                    //byteArray[currentPos] = stoi(cuv);
                    byteArray[2] = (int)myLabel.BYTE[1];
                    byteArray[3] = (int)myLabel.BYTE[0];
                    currentPos++;
                }
                else
                {
                    cout << convertUnknownToTypeByte(cuv) <<" ";
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
        //cout << "OUTER IF";
        LABEL myLabel;
        myLabel.SHORT = stoi(cuv);
        //std::cout << "HERE: " << (int)myLabel.BYTE[1] << ' ' << (int)myLabel.BYTE[0] << std::endl;
        //byteArray[currentPos] = stoi(cuv);
        byteArray[2] = (int)myLabel.BYTE[1];
        byteArray[3] = (int)myLabel.BYTE[0];
        currentPos++;
    }
    else
    {
        if(labelsMap.count(cuv) == 0) cout << "LABEL NOT FOUND: " << cuv << "\n";
        else
        {
            LABEL myLabel;
            myLabel.SHORT = labelsMap[cuv];
            //std::cout << "HERE: " << (int)myLabel.BYTE[1] << ' ' << (int)myLabel.BYTE[0] << std::endl;
            //byteArray[currentPos] = stoi(cuv);
            byteArray[2] = (int)myLabel.BYTE[1];
            byteArray[3] = (int)myLabel.BYTE[0];
            currentPos++;
        }
    }
    writeToFile(byteArray[0], byteArray[1], byteArray[2], byteArray[3]);
}

void parseDataRow(string dataRow, int lineNumber)
{
    cout << "\n" << "--------------------------------" << "\n";
    cout << "DATA ROW LABEL " << "\n";
    cout << "ON ROW NUMBER: " << lineNumber << "\n";
    cout << "VALUES: " << dataRow << "\n";
    string label = "";
    string cuv = ""; //IS ACTUALLY THE VALUE BUT I AM USING CUV BECAUSE I COPY PASTED THE CODE FROM ABOVE XD
    int labelPoz = 0;
    for(int i = 0; i < dataRow.size(); i++)
    {
        if(dataRow[i] != ':')
        {
            label = label + dataRow[i];
        }
        else
        {
            labelPoz = i + 1;
            cout << "FOUND LABEL: " << label << "\n";
        }
    }
    //INSERT THE LABEL IN THE HASHMAP WITH THE POSITION OF THE FIRST BYTE IN ROM
    //dataRow.erase(std::remove(dataRow.begin(), dataRow.end(), ':'), dataRow.end());
    pair<std::string,int> instr (label, romDataPosition);
    labelsMap.insert(instr);

    for(int i = labelPoz; i < dataRow.size(); i++)
    {
        if(dataRow[i] != ' ') cuv = cuv + dataRow[i];
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
                    //ADD IT IN THE BYTES DATA VECTOR
                    romData[romDataPosition] =  stoi(cuv);
                    romDataPosition++;
                }
                else
                {

                }
                //cout <<"MY WORD: " << result << "\n";
            }
            else
            {
                if(isdigit(cuv[0]))
                {
                    //ADD IT IN THE BYTES DATA VECTOR
                    romData[romDataPosition] =  stoi(cuv);
                    romDataPosition++;
                }
                else
                {

                }
                //cout <<"MY WORD: " << cuv << "\n";
            }
            cuv = "";
        }
        //cout << "LABEL: " << line << " at lineNumber " << lineNumber << "\n";

    }
    if(isdigit(cuv[0]))
    {
        //ADD IT IN THE BYTES DATA VECTOR
        romData[romDataPosition] =  stoi(cuv);
        romDataPosition++;
    }
    else
    {

    }
    cout << "\n" << "--------------------------------" << "\n";
}

void makeRomHeader()
{
    LABEL myLabel;
    myLabel.SHORT = ((romDataPosition / 4));
    if(romDataPosition%4!=0) myLabel.SHORT++;
    //std::cout << "HERE: " << (int)myLabel.BYTE[1] << ' ' << (int)myLabel.BYTE[0] << std::endl;
    //byteArray[currentPos] = stoi(cuv);
    romData[0] = (int)myLabel.BYTE[1];
    romData[1] = (int)myLabel.BYTE[0];
    romData[2] = 0;
    romData[3] = 0;
}

void preParseLabels()
{
    int lineNumber = 0;
    if (inPreParse.is_open())
    {
        cout << "PRE-PARSING STARTED" << "\n";
        std::string line;
        while (std::getline(inPreParse, line))
        {
            //line.c_str()
            //string toParse = line;
            line = ltrim(line); //TRIMS THE INDENTATION

            //REMOVE THE COMMENTS FROM THE CODE MARKED WITH "#"
            line = line.substr(0, line.find(" #", 0));

            if(startsWith(line, "#") || startsWith(line, " #"))
            {
                cout << "COMMENTED LINE" << "\n";
            }
            else
            {
                if(line.rfind(".data", 0) == 0)  //is now reading the data for the rom
                {
                    //lineNumber--;
                }
                else if(line.rfind(".text", 0) == 0)  //is now reading the code
                {
                    //lineNumber--;
                }
                else
                {
                    if(line.find(":") != std::string::npos)  // has a ":"
                    {
                        cout << "was label" << "\n";
                        //remove_copy(line.begin(), line.end(), back_inserter(line), ':');
                        line.erase(std::remove(line.begin(), line.end(), ':'), line.end());
                        pair<std::string,int> instr (line, lineNumber);
                        labelsMap.insert(instr);
                        cout << "LABEL (PRE-PARSE): " << line << " at lineNumber " << lineNumber << "\n"; // << " value: " << lineNumber << "\n";
                        //lineNumber++;
                    }
                    else if(trim(line) != "" )
                    {
                        //cout << "line read: " << line << "\n";//<< "LINE NUMBER: " << lineNumber << "\n";
                        //parseRow(line);
                        cout << "INCREMENT: " << line << "\n";
                        lineNumber++;
                    }
                    else
                    {
                        cout << "was empty line" << "\n";
                        //lineNumber--;
                    }
                    //lineNumber++;

                }
            }
        }
    }
    inPreParse.close();
}

int main()
{

    fout = fopen("game.rom","wb");
    int lineNumber = 0;
    bool isDataRow = false;

    preParseLabels();

    if (in.is_open())
    {
        std::string line;
        while (std::getline(in, line))
        {
            //line.c_str()
            //string toParse = line;
            line = ltrim(line); //TRIMS THE INDENTATION

            //REMOVE THE COMMENTS FROM THE CODE MARKED WITH "#"
            line = line.substr(0, line.find(" #", 0));

            if(startsWith(line, "#") || startsWith(line, " #"))
            {
                cout << "COMMENTED LINE" << "\n";
            }
            else
            {
                if(line.rfind(".data", 0) == 0)  //is now reading the data for the rom
                {
                    cout << "FOUND DATA ROW" << "\n";
                    isDataRow = true;
                }
                else if(line.rfind(".text", 0) == 0)  //is now reading the code
                {
                    makeRomHeader();

                    for(int i = 0; i < romDataPosition; i+=4)
                    {
                        if(i == 0)
                        {
                            //PRINTS THE HEADER
                            cout << romData[i]<< romData[i+1] << romData[i+2] << romData[i+3];
                        }
                        writeToFile(romData[i], romData[i+1], romData[i+2], romData[i+3]);
                        //cout << romData[i] << " " << romData[i+1] << " " << romData[i+2]  << " " << romData[i+3] << "\n";
                    }


                    cout << "FOUND TEXT ROW" << "\n";
                    isDataRow = false;
                }
                else
                {
                    if(isDataRow)
                    {
                        parseDataRow(line, lineNumber);
                        lineNumber++;
                    }
                    else
                    {
                        if(line.rfind("CALL", 0) == 0)
                        {
                            eraseAllSubStr(line, "CALL ");
                            parseRowWithSpace(line, true);
                        }
                        else if(line.rfind("JUMP", 0) == 0)
                        {
                            //THE LINE IS A LABEL
                            eraseAllSubStr(line, "JUMP ");
                            parseRowWithSpace(line, false);
                        }
                        else if(line.rfind("RRA", 0) == 0)
                        {
                            parseRowRamLoader(line);
                        }
                        else if(line.rfind("RRB", 0) == 0)
                        {
                            parseRowRamLoader(line);
                        }
                        else if(line.rfind("RRC", 0) == 0)
                        {
                            parseRowRamLoader(line);
                        }
                        else if(line.rfind("RRD", 0) == 0)
                        {
                            parseRowRamLoader(line);
                        }
                        else if(line.rfind("RRE", 0) == 0)
                        {
                            parseRowRamLoader(line);
                        }
                        else if(line.rfind("RRF", 0) == 0)
                        {
                            parseRowRamLoader(line);
                        }
                        /*
                        else if (line.rfind("BEQ", 0) == 0)
                        {
                            parseRowWithBEQ(line);
                        }
                        else if (line.rfind("BNE", 0) == 0)
                        {
                            parseRowWithBNE(line);
                        }
                        */
                        else if(line.find(":") != std::string::npos)  // has a ":"
                        {
                            /*
                            cout << "was label" << "\n";
                            //remove_copy(line.begin(), line.end(), back_inserter(line), ':');
                            line.erase(std::remove(line.begin(), line.end(), ':'), line.end());
                            pair<std::string,int> instr (line, lineNumber);
                            labelsMap.insert(instr);
                            cout << "LABEL: " << line << " at lineNumber " << lineNumber << "\n"; // << " value: " << lineNumber << "\n";
                            */
                        }
                        else if(trim(line) != "" )
                        {
                            cout << "line read: " << line << "\n";//<< "LINE NUMBER: " << lineNumber << "\n";
                            parseRow(line);
                        }
                        else
                        {
                            cout << "was empty line" << "\n";
                            lineNumber--;
                        }
                        lineNumber++;
                    }
                }
            }
        }

    }



    //FOR TESTING PURPOSES PRINTS THE ROM DATA TO THE CONSOLE
    for(int i = 0; i < romDataPosition; i++)
    {
        if(i % 4 == 0 && i != 0) cout << "\n";
        cout << romData[i] << " ";
    }

    if(romDataPosition % 4 != 0)  //THIS SIMULATES THE ACUTAL MEMMORY, BECAUSE THE ROM DATA VECTOR IS GLOBAL SO IT IS ALREADY FILLED WITH "0"
    {
        for(int i = 0; i < romDataPosition % 4; i++)
        {
            cout << "0 ";
        }
    }

    //PRINTS THE ROM DATA TO THE ROM FILE

    in.close();
    fclose(fout);
    return 0;
}
