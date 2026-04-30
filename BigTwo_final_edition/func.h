#ifndef FUNC_H
#define FUNC_H

void swap(int& a, int& b);
int getPatternType(std::vector<Poker>& hand);
void removeCard(int index, Poker hand[40], int& handCount);
void showTableCards(Poker tableCards[], int cardCount);
void showPlayerHand(Poker hand[], int cardCount);
void clearScreen();

#endif
