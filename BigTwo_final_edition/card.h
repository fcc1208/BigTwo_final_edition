#ifndef CARD_H
#define CARD_H

#include <string>
#include <vector>

class Poker {
public:
    int num;
    int numusedtocal;//用來計算牌型大小的數字，3最小，2最大
    std::string numusedtowatch;//用來顯示牌面的數字，1->A, 11->J, 12->Q, 13->K
    std::string color;
    int value;

    Poker();

    int takevalue();
    std::string getnutw();
    void displaycard(int line);
};

#endif