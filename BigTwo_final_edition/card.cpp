#include "card.h"
#include <iostream>
#include <string>

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

using std::cout;
using std::string;

Poker::Poker() {
    value = 0;
    num = 0;
    numusedtocal = 0;
    color = "";
}

int Poker::takevalue() {
    int val = 0;
    if (num >= 3) {
        numusedtocal = num - 3;
    }
    else {
        numusedtocal = num + 10;
    }

    val += (numusedtocal * 4);
    if (color == "S") {
        val += 3;
    }
    else if (color == "H") {
        val += 2;
    }
    else if (color == "D") {
        val += 1;
    }
    else if (color == "C") {
        val += 0;
    }
    return val;
}

string Poker::getnutw() {
    if (num == 1) {
        numusedtowatch = "A";
    }
    else if (num == 11) {
        numusedtowatch = "J";
    }
    else if (num == 12) {
        numusedtowatch = "Q";
    }
    else if (num == 13) {
        numusedtowatch = "K";
    }
    else {
        numusedtowatch = std::to_string(num);
    }
    return numusedtowatch;
}

void Poker::displaycard(int line) {
    std::string suitStr = "";
    std::string colorCode = "";
    std::string resetCode = "\033[0m";

    if (color == "S") {
        suitStr = "♠";
    }
    else if (color == "H") {
        suitStr = "♥"; colorCode = "\033[31m"; 
    }
    else if (color == "D") {
        suitStr = "♦"; colorCode = "\033[31m"; 
    }
    else if (color == "C") {
        suitStr = "♣"; 
    }

    std::cout << colorCode;

    std::string nutw = getnutw();

    switch (line) {
    case 0:
        std::cout << "┌────────┐";
        break;
    case 1:
        if (nutw.length() == 1) std::cout << "│ " << suitStr << nutw << "     │";
        else                    std::cout << "│ " << suitStr << nutw << "    │";
        break;
    case 2:
        std::cout << "│        │";
        break;
    case 3:
        std::cout << "│   " << suitStr << "    │";
        break;
    case 4:
        std::cout << "│        │";
        break;
    case 5:
        if (nutw.length() == 1) std::cout << "│     " << nutw << suitStr << " │";
        else                    std::cout << "│    " << nutw << suitStr << " │";
        break;
    case 6:
        std::cout << "└────────┘";
        break;
    }

    std::cout << resetCode;
}