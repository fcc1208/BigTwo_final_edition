#include "card.h"
#include "player.h"

#include <iostream>
#include <algorithm>

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

using std::vector;

void swap(int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}

int getPatternType(vector<Poker>& hand) {
    int n = hand.size();
    if (n == 1) {
        return 1;
    }
    if (n == 2 && hand[0].num == hand[1].num) {
        return 2;
    }
    if (n == 5) {
        //順子 同花順
        bool isStraight = true;
        bool isFlush = true;

        for (int i = 1; i < 5; i++) {
            if (hand[i].numusedtocal != hand[i - 1].numusedtocal + 1) {
                isStraight = false;
            }
            if (hand[i].color != hand[0].color) {
                isFlush = false;
            }
        }

        bool isA2345 = (hand[0].numusedtocal == 0 && hand[1].numusedtocal == 1 &&
            hand[2].numusedtocal == 2 && hand[3].numusedtocal == 11 &&
            hand[4].numusedtocal == 12);

        if (isA2345) {
            isStraight = true; // 強制承認它是順子
        }

        // 排除JQKA2
        if (isStraight && hand[0].numusedtocal == 8) {
            isStraight = false;
        }

        if (isStraight && isFlush) {
            return 6;//同花順
        }
        if (isStraight) {
            return 3;//順子
        }

        //判定鐵支
        if (hand[0].num == hand[3].num || hand[1].num == hand[4].num) {
            return 5;//鐵支
        }

        //葫蘆
        if ((hand[0].num == hand[2].num && hand[3].num == hand[4].num) ||
            (hand[0].num == hand[1].num && hand[2].num == hand[4].num)) {
            return 4;
        }
    }
    return 0;
}

void removeCard(int index, Poker hand[40], int& handCount) {
    for (int i = index; i < handCount - 1; i++) {
        hand[i] = hand[i + 1];
    }
    hand[handCount - 1] = Poker();
    handCount--;
}

void showTableCards(Poker tableCards[], int cardCount) {
    std::cout << "\n=table===========================================================" << std::endl;

    if (cardCount == 0) {
        std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
        std::cout << "===========================================================table=\n" << std::endl;
        return;
    }

    for (int line = 0; line < 7; line++) {
        for (int i = 0; i < cardCount; i++) {
            tableCards[i].displaycard(line);
            std::cout << "   ";
        }
        std::cout << std::endl;
    }
    std::cout << "===========================================================table=\n" << std::endl;
}

void showPlayerHand(Poker hand[], int cardCount) {

    if (cardCount == 0) {
        return;
    }
    int cardsPerRow = 6; //每排最多印幾張
    int totalRows = (cardCount + cardsPerRow - 1) / cardsPerRow; //總共需要幾排

    // 外層：現在印到第幾排
    for (int r = 0; r < totalRows; r++) {
        int startIdx = r * cardsPerRow;
        int endIdx = std::min(startIdx + cardsPerRow, cardCount);

        for (int line = 0; line < 7; line++) {
            for (int i = startIdx; i < endIdx; i++) {
                hand[i].displaycard(line);
                std::cout << "  ";
            }
            std::cout << std::endl;
        }

        //對不齊...
        for (int i = startIdx; i < endIdx; i++) {
            if (i < 10) {
                std::cout << "   [" << i << "]       ";
            }
            else {
                std::cout << "   [" << i << "]      ";
            }
        }
        std::cout << "\n\n";
    }
}

void clearScreen() {
    system("cls");
}