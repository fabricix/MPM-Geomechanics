/*
 * Warning.cpp
 *
 *  Created on: 21 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#if defined (_WIN64) || defined(_WIN32)
#include <windows.h>
#endif

#include "Warning.h"

#include <iostream>
using std::cout;
using std::endl;

namespace Warning
{
    // format
    string none("0");
    string bold("1");
    string underline("4");
    string inverse("7");

    // color
    string black("30m");
    string red("31");
    string green("32");
    string yellow("33");
    string blue("34");
    string magenta("35");
    string cyan("36");
    string white("37");

    // reset and scape
    string reset("\033[0m");
    string esc("\033[");

    // current text format and color
    string currentColor=yellow;
    string currentFormat=none;

    void setColor(string color)
    {
        if(color=="black") currentColor=black;
        if(color=="red") currentColor=red;
        if(color=="green") currentColor=green;
        if(color=="blue") currentColor=blue;
        if(color=="magenta") currentColor=magenta;
        if(color=="cyan") currentColor=cyan;
        if(color=="white") currentColor=white;
        if(color=="yellow") currentColor=yellow;
    }

    void setFormat(string format)
    {
        if(format=="bold") currentFormat=bold;
        if(format=="underline") currentFormat=underline;
        if(format=="inverse") currentFormat=inverse;
    }

    void printMessage(string what)
    {
		bool enable_virtual_terminal_processing = true;
#if 0 
		#if defined (_WIN64) || defined(_WIN32)
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		enable_virtual_terminal_processing=SetConsoleMode(hStdout, ENABLE_VIRTUAL_TERMINAL_PROCESSING);

		if (!enable_virtual_terminal_processing){
			CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
			GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
			SetConsoleTextAttribute(hStdout,12);
			cout << "Warning: ";
			SetConsoleTextAttribute(hStdout, 14);
			cout << what << endl;
			SetConsoleTextAttribute(hStdout, csbiInfo.wAttributes);
		}
		#endif
#endif
		if (enable_virtual_terminal_processing) {
			string color_set_1 = esc + bold + ";" + red + "m";
			string color_set_2 = esc + currentFormat + ";" + currentColor + "m";
			cout << color_set_1 << "Warning: " << color_set_2 << what << reset << endl;
		}
    }
}
