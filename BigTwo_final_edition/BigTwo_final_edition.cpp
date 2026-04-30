#include "card.h"
#include "player.h"
#include "func.h"

#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include <thread>
#include <chrono>
#include <windows.h>

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
// 強制轉UTF8

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;
using std::swap;
using std::sort;
using std::greater;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;
using std::stringstream;
using std::ws;

int main() {//----------------------------------------------------------main

    // -----------解鎖 Windows 終端機的彩色顯示------------------------------
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= 0x0004;
    SetConsoleMode(hOut, dwMode);

    SetConsoleOutputCP(CP_UTF8);//花色 邊框
    //-------------------------------------------------------反正跟紅色牌有關

    Player player[4];//玩家
    Poker deck[52];//牌

    srand(time(NULL));

    // ================== 遊戲大廳 ===============================
    int playercount = 0, round = 0, curround = 1;

    while (true) {
        clearScreen();
        cout << "\n=================================================================\n";
        cout << "                     大 老 二   B I G   T W O                    \n";
        cout << "=================================================================\n\n";
        cout << "  請輸入遊玩人數 (2~4 人): ";

        if (!(cin >> playercount)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n  [錯誤] 請輸入有效的數字！\n\n";
            system("pause");
            continue;
        }

        if (playercount >= 2 && playercount <= 4) {
            break;
        }
        cout << "\n  [錯誤] 玩家人數必須在 2 到 4 之間！\n\n";
        system("pause");
    }

    while (true) {
        clearScreen();
        cout << "\n=================================================================\n";
        cout << "                     大 老 二   B I G   T W O                    \n";
        cout << "=================================================================\n\n";
        cout << "  已選擇模式: " << playercount << " 人對戰\n\n";
        cout << "  請選擇遊玩局數 (例如 1, 3, 5): ";

        if (!(cin >> round) || round <= 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n  [錯誤] 請輸入大於 0 的有效局數！\n\n";
            system("pause");
            continue;
        }
        break;
    }

    clearScreen();
    cout << "\n=================================================================\n";
    cout << "                     大 老 二   B I G   T W O                    \n";
    cout << "=================================================================\n\n";
    cout << "  [ 遊戲設定完成 ]\n";
    cout << "  玩家人數 : " << playercount << " 人\n";
    cout << "  遊玩局數 : " << round << " 局\n\n";
    cout << "  正在準備牌局，請稍候...\n\n";
    cout << "=================================================================\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    // ================== 遊戲大廳 ===============================

    for (int i = 0;i < playercount;i++) {
        player[i].totscore = 0;
    }

    while (round--) {
        cout << "=第" << curround << "輪======================================================開始= " << endl << endl;

        //用來紀錄這局歷史
        vector<string> actionLog;

        //-------------------------------------------------------初始化牌組v
        for (int j = 0;j < 4;j++) {
            for (int i = 0;i < 13;i++) {
                if (j == 0) {
                    deck[j * 13 + i].color = "S";
                }
                else if (j == 1) {
                    deck[j * 13 + i].color = "H";
                }
                else if (j == 2) {
                    deck[j * 13 + i].color = "D";
                }
                else if (j == 3) {
                    deck[j * 13 + i].color = "C";
                }
                deck[j * 13 + i].num = i + 1;
            }
        }

        for (int i = 0;i < 52;i++) {//讓3最小，2最大->用numusedtocal比大小
            if (deck[i].num >= 3) {
                deck[i].numusedtocal = deck[i].num - 3;
            }
            else {
                deck[i].numusedtocal = deck[i].num + 10;
            }
        }

        //-------------------------------------------------------洗牌v
        for (int i = 0; i < 52; i++) {
            int r = rand() % 52;
            swap(deck[i], deck[r]);
        }
        //-------------------------------------------------------洗牌^

        //-------------------------------------------------------發牌v
        Poker playerc[4][40];//每位玩家的手牌
        int handCounts[4] = { 0 };//每位玩家手牌數量
        int c3p = 0;//梅花三玩家

        if (playercount != 3) {
            int k = 0;
            for (int i = 0;i < playercount;i++) {
                for (int j = 0;j < 52 / playercount;j++) {
                    playerc[i][j] = deck[k];

                    if (playerc[i][j].numusedtocal == 0 && playerc[i][j].color == "C") {//找梅花三玩家
                        c3p = i;
                    }

                    k++;
                    handCounts[i]++;
                }
            }
        }
        else {//三人發牌：每人17張，剩下一張給有梅花三的
            int k = 0;
            for (int i = 0;i < playercount;i++) {
                for (int j = 0;j < 51 / playercount;j++) {
                    playerc[i][j] = deck[k];
                    k++;
                    handCounts[i]++;
                }
            }

            for (int i = 0;i < playercount;i++) {
                for (int j = 0;j < 51 / playercount;j++) {
                    if (playerc[i][j].numusedtocal == 0 && playerc[i][j].color == "C") {//找梅花三玩家
                        c3p = i;
                        break;
                    }
                }
            }

            playerc[c3p][17] = deck[51];//剩下的牌給有梅花三的玩家
            handCounts[c3p]++;
        }
        //-------------------------------------------------------發牌^

        //-------------------------------------------------------理牌v
        for (int pID = 0;pID < playercount;pID++) {
            for (int i = 0; i < handCounts[pID] - 1; i++) {
                for (int j = i + 1; j < handCounts[pID]; j++) {
                    if (playerc[pID][i].takevalue() > playerc[pID][j].takevalue()) {
                        Poker temp = playerc[pID][i];
                        playerc[pID][i] = playerc[pID][j];
                        playerc[pID][j] = temp;
                    }
                }
            }
        }
        //-------------------------------------------------------理牌^

        //-------------------------------------------------------遊戲進行v
        bool firstroundfirsthand = true;//第一輪第一手必須出梅花三
        int currentPlayer, lastPlayer, tableType = 0, tableValue = -1, lastPlayPlayer = -1;
        Poker tableCards[5];//桌面牌
        int currentTableCardCount = 0;

        bool gameOver = false;
        int curpoint = 4;
        int passCount = 0;
        bool hasscored[4] = { false };
        int currentRank = 1;

        currentPlayer = c3p;//梅花三玩家先出

        cout << "本輪出牌順序: ";
        for (int i = 0; i < playercount; i++) {
            int orderID = (c3p + i) % playercount;
            cout << "玩家" << orderID + 1 << (i == playercount - 1 ? "" : " -> ");
        }
        cout << endl << " p.s.您是 玩家1 ";
        cout << "\n\n";
        system("pause");

        while (!gameOver) {
            int finishedPlayers = 0;
            for (int i = 0; i < playercount; i++) {
                if (hasscored[i]) {
                    finishedPlayers++;
                }
            }

            if (finishedPlayers == playercount - 1) {
                for (int i = 0; i < playercount; i++) {
                    if (hasscored[i] == false) {
                        cout << "玩家 " << i + 1 << " 是最後一名，本局結束！" << endl;
                        player[i].totscore += curpoint;

                        player[i].lastRoundRank = currentRank;

                        cout << "玩家 " << i + 1 << " 獲得 " << curpoint << " 分！" << endl;
                        hasscored[i] = true;
                    }
                }
                gameOver = true;

                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                continue;
            }

            if (passCount >= (playercount - finishedPlayers - 1)) {//如果連續過牌數達到剩下玩家數-1 表示其他玩家都過了 出牌權回到最後出牌的玩家 牌桌清空
                actionLog.push_back("--- 所有對手皆 pass，桌面清空！輪到玩家 " + std::to_string(currentPlayer + 1) + " 自由出牌 ---");
                tableType = 0;
                tableValue = -1;
                passCount = 0;
                currentTableCardCount = 0;

                for (int i = 0; i < 5; i++) {
                    tableCards[i] = Poker();
                }

                cout << "=============================" << endl;
                cout << "所有對手皆 pass，桌面清空！" << endl;
                cout << "=============================" << endl;
            }


            clearScreen();
            cout << "\n=================================================================\n";
            cout << "                     大 老 二   B I G   T W O                    \n";
            cout << "=================================================================\n";
            cout << "  局數: " << curround << " / " << (curround + round) << "    \n";

            // 印出所有玩家剩餘牌數
            for (int i = 0; i < playercount; i++) {
                if (i == 0) {
                    cout << "[ 你剩 " << handCounts[i] << " 張 ]    ";
                }
                else {
                    cout << "[ 玩家" << i + 1 << " 剩 " << handCounts[i] << " 張 ]    ";
                }
            }
            cout << "\n-----------------------------------------------------------------\n";

            if (tableType == 0) {
                cout << "【 桌面目前為空，請自由出牌 】\n";
            }
            else {
                cout << "【 桌面牌型：由 玩家" << lastPlayPlayer + 1 << " 打出 】\n";
            }

            // 畫出公共牌桌
            showTableCards(tableCards, currentTableCardCount);

            cout << "輪到玩家 " << currentPlayer + 1 << " 出牌" << endl;

            if (currentPlayer == 0) { // 助教 (玩家 1)
                showPlayerHand(playerc[currentPlayer], handCounts[currentPlayer]);
                cout << endl;
                cout << "請輸入索引(如 0 2 4)、輸入 -1 Pass、輸入 -2 歷史、輸入 -3 規則說明、輸入 -4 投降: ";

                //輸入要出的牌
                string input;
                getline(cin >> ws, input);
                stringstream ss(input);
                vector<int> indices;
                int idx;
                while (ss >> idx) {
                    indices.push_back(idx);
                }
                sort(indices.begin(), indices.end());

                indices.erase(unique(indices.begin(), indices.end()), indices.end()); //unique-><algorithm> 刪掉重複元素 回傳指標

                if (indices.size() == 1 && indices[0] == -2) {
                    clearScreen();
                    cout << "=歷史紀錄============================================================\n\n";
                    if (actionLog.empty()) {
                        cout << "  (目前尚無紀錄)\n";
                    }
                    else {
                        for (int i = 0; i < actionLog.size(); i++) {
                            cout << actionLog[i] << endl;
                        }
                    }
                    cout << "\n============================================================歷史紀錄=\n";

                    system("pause");

                    continue;
                }

                if (indices.size() == 1 && indices[0] == -3) {
                    clearScreen();
                    cout << "\n=================================================================\n";
                    cout << "                     大 老 二   遊 戲 說 明                      \n";
                    cout << "=================================================================\n\n";
                    cout << "  【 牌 面 大 小 】\n";
                    cout << "  * 點數：3 < 4 < 5 < ... < K < A < 2\n";
                    cout << "  * 花色：梅花(C) < 方塊(D) < 紅心(H) < 黑桃(S)\n\n";

                    cout << "  【 出 牌 種 類 】\n";
                    cout << "  1. 單張：先比點數，點數相同再比花色。\n";
                    cout << "  2. 一對：兩張點數相同，比該對中最大的花色。\n";
                    cout << "  3. 五張 (順子 < 葫蘆 < 鐵支 < 同花順)：\n";
                    cout << "     - 順子：5張連號，含 A-2-3-4-5，不含 J-Q-K-A-2 (比最大點)\n";
                    cout << "     - 葫蘆：3張同點 + 1對 (比三張那組的點數)\n";
                    cout << "     - 鐵支：4張同點 + 1張單牌 (比四張那組的點數)\n";
                    cout << "     - 同花順：5張同花色且連號 (比大點數)\n\n";

                    cout << "  【 遊 戲 規 則 】\n";
                    cout << "  * 第一手由持有「梅花 3」的玩家出牌，且出牌必須包含梅花 3。\n";
                    cout << "  * 每次出牌必須跟桌面【張數相同】且【牌型/點數更大】。\n";
                    cout << "  * 若所有對手皆 Pass，最後出牌者將獲得出牌權。\n";
                    cout << "  * 最先出完牌者得 4 分，依序遞減，最後一名得 1 分。\n\n";
                    cout << "\033[31m  * 依照作業規定，玩家出牌必須跟桌面張數相同，所以本遊戲鐵支與\n    同花順將不能用來壓單張或一對。\033[0m\n";
                    cout << "=================================================================\n";

                    system("pause");

                    continue;
                }

                if (indices.size() == 1 && indices[0] == -4) {
                    clearScreen();
                    cout << "\n\n\n";
                    cout << "=\033[31m要拚\033[0m==============================================================\n";
                    cout << "\033[31m                                                                 \033[0m\n";
                    cout << "\033[31m                  舒 服 是 留 給 死 人 的 ！！！                 \033[0m\n";
                    cout << "\033[31m                                                                 \033[0m\n";
                    cout << "==============================================================\033[31m要猛\033[0m=\n\n";

                    cout << "\033[31m禁止投降 乖乖回去把牌打完！\033[0m\n\n";

                    system("pause");

                    continue;
                }

                if (indices.size() == 1 && indices[0] == -1) {//選擇pass
                    if (firstroundfirsthand == true) {
                        cout << "第一手不可 pass !" << endl;
                    }
                    else {
                        passCount++;
                        cout << "玩家 " << currentPlayer + 1 << " 選擇 pass" << endl;
                        cout << "-----------------------------------------------------------------" << endl;

                    }
                }
                else {//選擇出牌
                    if (indices.size() == 1) {//出一張
                        if (tableType == 0 || (tableType == 1 && playerc[currentPlayer][indices[0]].takevalue() > tableValue)) {//牌桌沒牌或牌桌是一張牌且出牌點數更大
                            if (firstroundfirsthand == true && playerc[currentPlayer][indices[0]].takevalue() != 0) {
                                cout << "第一手必須出梅花三！" << endl;

                                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                                continue;
                            }
                            else {
                                cout << "你出了一張牌: " << playerc[currentPlayer][indices[0]].color << "-" << playerc[currentPlayer][indices[0]].getnutw() << endl;
                                //更新牌桌狀態
                                firstroundfirsthand = false;
                                lastPlayPlayer = currentPlayer;
                                tableType = 1;
                                tableValue = playerc[currentPlayer][indices[0]].takevalue();
                                passCount = 0;
                                tableCards[0] = playerc[currentPlayer][indices[0]];
                                currentTableCardCount = 1;
                                removeCard(indices[0], playerc[currentPlayer], handCounts[currentPlayer]);//出牌後從手牌移除
                            }
                        }
                        else {
                            cout << "牌型不合或點數不夠大！" << endl;

                            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                            continue;
                        }
                    }
                    else if (indices.size() == 2) {//出一對
                        if (playerc[currentPlayer][indices[0]].num == playerc[currentPlayer][indices[1]].num) {//兩張牌點數相同才有可能是一對
                            if (tableType == 0 || (tableType == 2 && playerc[currentPlayer][indices[1]].takevalue() > tableValue)) {//牌桌沒牌或牌桌是一對且出牌點數更大
                                if (firstroundfirsthand == true && (playerc[currentPlayer][indices[0]].takevalue() != 0 && playerc[currentPlayer][indices[1]].takevalue() != 0)) {
                                    cout << "第一手必須出梅花三！" << endl;

                                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                                    continue;
                                }
                                else {
                                    cout << "你出了一對: " << playerc[currentPlayer][indices[0]].color << "-" << playerc[currentPlayer][indices[0]].getnutw() << " " <<
                                        playerc[currentPlayer][indices[1]].color << "-" << playerc[currentPlayer][indices[1]].getnutw() << std::endl;

                                    //更新牌桌狀態
                                    firstroundfirsthand = false;
                                    lastPlayPlayer = currentPlayer;
                                    tableType = 2;
                                    tableValue = playerc[currentPlayer][indices[1]].takevalue();
                                    passCount = 0;
                                    tableCards[0] = playerc[currentPlayer][indices[0]];
                                    tableCards[1] = playerc[currentPlayer][indices[1]];
                                    currentTableCardCount = 2;

                                    int firstIdx = indices[0];
                                    int secondIdx = indices[1];
                                    removeCard(secondIdx, playerc[currentPlayer], handCounts[currentPlayer]);
                                    removeCard(firstIdx, playerc[currentPlayer], handCounts[currentPlayer]);
                                }
                            }
                            else {
                                cout << "牌型不合或點數不夠大！" << endl;

                                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                                continue;
                            }
                        }
                        else {
                            cout << "這兩張點數不同，不是一對！" << endl;

                            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                            continue;
                        }
                    }
                    else if (indices.size() == 5) {//出五張牌
                        vector<Poker> selected;

                        for (int idx : indices) {
                            selected.push_back(playerc[currentPlayer][idx]);
                        }

                        int currentPattern = getPatternType(selected);
                        int currentMaxValue = -1;

                        if (currentPattern == 0) {
                            cout << "無效的五張牌組合！" << endl;

                            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                            continue;
                        }

                        //根據牌型決定最大點數
                        if (currentPattern == 4 || currentPattern == 5) {
                            currentMaxValue = selected[2].takevalue();
                        }
                        else {
                            currentMaxValue = selected[4].takevalue();
                        }

                        bool canPlay = false;

                        if (tableType == 0) {
                            canPlay = true;
                        }
                        else if (currentPattern > tableType && tableType >= 3) {//五張牌牌型只能壓五張牌牌型
                            canPlay = true;
                        }
                        else if (currentPattern == tableType && currentMaxValue > tableValue) {//牌型相同 點數更大
                            canPlay = true;
                        }

                        if (canPlay && firstroundfirsthand) {//第一輪第一手必須包含梅花三
                            bool hasClub3 = false;
                            //檢查選擇的牌中是否有梅花三
                            for (int i = 0; i < selected.size(); i++) {
                                if (selected[i].value == 0) {
                                    hasClub3 = true;
                                    break;
                                }
                            }
                            if (hasClub3 == false) {
                                cout << "第一手必須包含梅花 3！" << endl;

                                std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 讓你看清楚警告
                                continue;
                            }
                        }

                        if (canPlay) {
                            if (currentPattern == 3) {
                                cout << "你出了順子" << endl;
                            }
                            else if (currentPattern == 4) {
                                cout << "你出了葫蘆" << endl;
                            }
                            else if (currentPattern == 5) {
                                cout << "你出了鐵支" << endl;
                            }
                            else if (currentPattern == 6) {
                                cout << "你出了同花順" << endl;
                            }

                            firstroundfirsthand = false;
                            lastPlayPlayer = currentPlayer;
                            tableType = currentPattern;
                            tableValue = currentMaxValue;
                            passCount = 0;

                            for (int i = 0; i < 5; i++) {
                                tableCards[i] = selected[i];
                            }
                            currentTableCardCount = 5;

                            sort(indices.begin(), indices.end(), greater<int>());//改成從大到小排序，以免移除牌時影響未移除的牌
                            for (int i = 0; i < indices.size(); i++) {
                                int targetIdx = indices[i];
                                removeCard(targetIdx, playerc[currentPlayer], handCounts[currentPlayer]);
                            }
                        }
                        else {
                            cout << "牌型等級不足或點數不夠大！" << endl;

                            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                            continue;
                        }
                    }
                    else {//亂按
                        cout << "無效的牌型，請重新輸入！" << endl;

                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                        continue;
                    }
                }
            }
            else { // 電腦玩家 (玩家 2, 3, 4)
                cout << "玩家 " << currentPlayer + 1 << " 思考中..." << endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1500));

                vector<int> aiPlayIndices; //儲存AI決定要出的牌的索引

                if (tableType == 0) {// s1：牌桌沒牌，AI可以出任何牌
                    if (firstroundfirsthand) {
                        int c3Idx = -1;
                        //找梅花 3 位置 (takevalue)
                        for (int i = 0; i < handCounts[currentPlayer]; i++) {
                            if (playerc[currentPlayer][i].takevalue() == 0) {
                                c3Idx = i;
                                break;
                            }
                        }

                        if (c3Idx != -1) {//找到梅花 3
                            vector<int> possiblePairs;
                            vector<int> possibleStraights;
                            vector<int> possibleFullHouses;

                            //找包含梅花 3 對子
                            if (c3Idx + 1 < handCounts[currentPlayer] &&
                                playerc[currentPlayer][c3Idx].num == playerc[currentPlayer][c3Idx + 1].num) {
                                possiblePairs.push_back(c3Idx);
                                possiblePairs.push_back(c3Idx + 1);
                            }

                            //找包含梅花 3 順子
                            vector<int> tempIdx = { c3Idx };
                            int expectedNum = playerc[currentPlayer][c3Idx].numusedtocal + 1;//順子下一張牌是梅花 3 的點數+1
                            for (int j = c3Idx + 1; j < handCounts[currentPlayer]; j++) {
                                if (playerc[currentPlayer][j].numusedtocal == expectedNum) {
                                    tempIdx.push_back(j);
                                    expectedNum++;
                                    if (tempIdx.size() == 5) {
                                        break;
                                    }
                                }
                            }
                            if (tempIdx.size() == 5 && playerc[currentPlayer][tempIdx[0]].numusedtocal != 8) { //排除JQKA2
                                possibleStraights = tempIdx;
                            }

                            //找包含梅花 3 葫蘆
                            if (!possiblePairs.empty()) {
                                for (int i = c3Idx + 2; i <= handCounts[currentPlayer] - 3; i++) {
                                    if (playerc[currentPlayer][i].num == playerc[currentPlayer][i + 1].num &&
                                        playerc[currentPlayer][i].num == playerc[currentPlayer][i + 2].num) {
                                        possibleFullHouses = { c3Idx, c3Idx + 1, i, i + 1, i + 2 };
                                        sort(possibleFullHouses.begin(), possibleFullHouses.end());
                                        break;
                                    }
                                }
                            }

                            // 第一局第一手抽籤
                            vector<int> availableTypes;
                            availableTypes.push_back(1); // 單出梅花 3
                            if (!possiblePairs.empty()) {
                                for (int i = 0; i < 3; i++) availableTypes.push_back(2);
                            }
                            if (!possibleStraights.empty()) {
                                for (int i = 0; i < 4; i++) availableTypes.push_back(3);
                            }
                            if (!possibleFullHouses.empty()) {
                                for (int i = 0; i < 5; i++) availableTypes.push_back(4);
                            }

                            int choice = availableTypes[rand() % availableTypes.size()];

                            if (choice == 1) {
                                aiPlayIndices.push_back(c3Idx);
                            }
                            else if (choice == 2) {
                                aiPlayIndices = possiblePairs;
                            }
                            else if (choice == 3) {
                                aiPlayIndices = possibleStraights;
                            }
                            else if (choice == 4) {
                                aiPlayIndices = possibleFullHouses;
                            }
                        }
                    }
                    else {//AI隨機 單張 對子 五張
                        vector<int> possiblePairs;
                        vector<int> possibleStraights;//順
                        vector<int> possibleFullHouses;//葫
                        vector<int> possibleFourOfAKinds;//鐵

                        for (int i = 0; i < handCounts[currentPlayer] - 1; i++) {//嘗試找手上最小的一對
                            if (playerc[currentPlayer][i].num == playerc[currentPlayer][i + 1].num) {
                                possiblePairs.push_back(i);
                                possiblePairs.push_back(i + 1);

                                i++; //跳過下一張以免重複抓到同一對
                            }
                        }

                        for (int i = 0; i < handCounts[currentPlayer]; i++) {//嘗試找手上最小的順子
                            vector<int> tempIdx;
                            tempIdx.push_back(i);
                            int expectedNum = playerc[currentPlayer][i].numusedtocal + 1;

                            for (int j = i + 1; j < handCounts[currentPlayer]; j++) {
                                if (playerc[currentPlayer][j].numusedtocal == expectedNum) {
                                    tempIdx.push_back(j);
                                    expectedNum++;

                                    if (tempIdx.size() == 5) {
                                        break;
                                    }
                                }
                            }
                            // 排除 JQKA2
                            if (tempIdx.size() == 5 && playerc[currentPlayer][tempIdx[0]].numusedtocal != 8) {
                                possibleStraights = tempIdx;
                            }
                        }

                        int threeNum = -1;//紀錄葫蘆中三張的點數
                        vector<int> threeIdx;
                        for (int i = 0; i <= handCounts[currentPlayer] - 3; i++) { //嘗試找手上最小的葫蘆
                            if (playerc[currentPlayer][i].num == playerc[currentPlayer][i + 1].num &&
                                playerc[currentPlayer][i + 1].num == playerc[currentPlayer][i + 2].num) {
                                threeNum = playerc[currentPlayer][i].num;
                                threeIdx.push_back(i);
                                threeIdx.push_back(i + 1);
                                threeIdx.push_back(i + 2);
                                break;
                            }
                        }
                        if (threeNum != -1) {//如果找到了三張相同的牌 繼續找有沒有一對
                            for (int i = 0; i <= handCounts[currentPlayer] - 2; i++) {
                                if (playerc[currentPlayer][i].num != threeNum &&
                                    playerc[currentPlayer][i].num == playerc[currentPlayer][i + 1].num) {//防抓到三條以為對子
                                    possibleFullHouses = threeIdx;
                                    possibleFullHouses.push_back(i);
                                    possibleFullHouses.push_back(i + 1);
                                    sort(possibleFullHouses.begin(), possibleFullHouses.end());//排序以免在getpattern出錯
                                    break;
                                }
                            }
                        }

                        vector<int> fourIdx;
                        for (int i = 0; i <= handCounts[currentPlayer] - 4; i++) {//嘗試找手上最小的鐵支
                            if (playerc[currentPlayer][i].num == playerc[currentPlayer][i + 1].num &&
                                playerc[currentPlayer][i].num == playerc[currentPlayer][i + 2].num &&
                                playerc[currentPlayer][i].num == playerc[currentPlayer][i + 3].num) {
                                fourIdx.push_back(i);
                                fourIdx.push_back(i + 1);
                                fourIdx.push_back(i + 2);
                                fourIdx.push_back(i + 3);
                                break;
                            }
                        }
                        if (!fourIdx.empty()) {
                            for (int i = 0; i < handCounts[currentPlayer]; i++) {
                                if (playerc[currentPlayer][i].num != playerc[currentPlayer][fourIdx[0]].num) {
                                    possibleFourOfAKinds = fourIdx;
                                    possibleFourOfAKinds.push_back(i);
                                    sort(possibleFourOfAKinds.begin(), possibleFourOfAKinds.end());
                                    break;
                                }
                            }
                        }

                        //把可行的出牌選項抽籤
                        vector<int> availableTypes;

                        if (!possiblePairs.empty()) {
                            for (int i = 0; i < 3; i++) availableTypes.push_back(2);
                        }
                        if (!possibleStraights.empty()) {
                            for (int i = 0; i < 4; i++) availableTypes.push_back(3);
                        }
                        if (!possibleFullHouses.empty()) {
                            for (int i = 0; i < 5; i++) availableTypes.push_back(4);
                        }
                        if (!possibleFourOfAKinds.empty()) {
                            for (int i = 0; i < 5; i++) availableTypes.push_back(5);
                        }

                        //完全沒有其他組合時 才放單張進籤筒
                        if (possiblePairs.empty() && possibleStraights.empty() && possibleFullHouses.empty()) {
                            availableTypes.push_back(1);
                        }

                        //隨機抽籤
                        int choice = availableTypes[rand() % availableTypes.size()];

                        if (choice == 1) {
                            aiPlayIndices.push_back(0);//出最小的單張
                        }
                        else if (choice == 2) {
                            aiPlayIndices = possiblePairs;//出最小的一對
                        }
                        else if (choice == 3) {
                            aiPlayIndices = possibleStraights;//出最小的順子
                        }
                        else if (choice == 4) {
                            aiPlayIndices = possibleFullHouses;//出最小的葫蘆
                        }
                        else if (choice == 5) {
                            aiPlayIndices = possibleFourOfAKinds;//出最小的桐花順
                        }
                    }
                }
                else if (tableType == 1) {//s2：牌桌一張牌，AI出一張點數更大的牌
                    for (int i = 0; i < handCounts[currentPlayer]; i++) {
                        if (playerc[currentPlayer][i].takevalue() > tableValue) {
                            aiPlayIndices.push_back(i);
                            break; //找到能壓過的最小牌就停下來
                        }
                    }
                }
                else if (tableType == 2) {//s3：牌桌一對，AI出一對更大的牌
                    for (int i = 0; i < handCounts[currentPlayer] - 1; i++) {
                        if (playerc[currentPlayer][i].num == playerc[currentPlayer][i + 1].num &&
                            playerc[currentPlayer][i + 1].takevalue() > tableValue) {
                            aiPlayIndices.push_back(i);
                            aiPlayIndices.push_back(i + 1);
                            break;
                        }
                    }
                }
                else if (tableType == 0 || tableType >= 3) {//s4：牌桌五張牌，AI出五張牌型更大的牌

                    //嘗試找順子
                    if (aiPlayIndices.empty() && (tableType == 0 || tableType == 3)) {
                        for (int i = 0; i < handCounts[currentPlayer]; i++) {
                            vector<int> tempIdx;
                            tempIdx.push_back(i);
                            int expectedNum = playerc[currentPlayer][i].numusedtocal + 1;

                            for (int j = i + 1; j < handCounts[currentPlayer]; j++) {
                                // 找到下一個連號的牌
                                if (playerc[currentPlayer][j].numusedtocal == expectedNum) {
                                    tempIdx.push_back(j);
                                    expectedNum++;
                                    if (tempIdx.size() == 5) {
                                        break;
                                    }
                                }
                            }

                            if (tempIdx.size() == 5) {
                                //排除 JQKA2
                                if (playerc[currentPlayer][tempIdx[0]].numusedtocal != 8) {
                                    int maxVal = playerc[currentPlayer][tempIdx[4]].takevalue();
                                    if (tableType == 0 || maxVal > tableValue) {
                                        aiPlayIndices = tempIdx;
                                        break; // 找到能出的順子就停
                                    }
                                }
                            }
                        }
                    }

                    //嘗試找葫蘆
                    if (aiPlayIndices.empty() && (tableType == 0 || tableType == 3 || tableType == 4)) {
                        int threeNum = -1;
                        vector<int> threeIdx;

                        //先找三條
                        for (int i = 0; i <= handCounts[currentPlayer] - 3; i++) {
                            if (playerc[currentPlayer][i].num == playerc[currentPlayer][i + 1].num &&
                                playerc[currentPlayer][i + 1].num == playerc[currentPlayer][i + 2].num) {

                                int maxVal = playerc[currentPlayer][i + 2].takevalue();
                                // 葫蘆壓順子，或同葫蘆時比大小
                                if (tableType == 0 || tableType == 3 || (tableType == 4 && maxVal > tableValue)) {
                                    threeNum = playerc[currentPlayer][i].num;
                                    threeIdx = { i, i + 1, i + 2 };
                                    break;
                                }
                            }
                        }

                        // 如果有三條，去找一對(數字不能跟三條一樣)
                        if (threeNum != -1) {
                            for (int i = 0; i <= handCounts[currentPlayer] - 2; i++) {
                                if (playerc[currentPlayer][i].num != threeNum &&
                                    playerc[currentPlayer][i].num == playerc[currentPlayer][i + 1].num) {

                                    aiPlayIndices = threeIdx;
                                    aiPlayIndices.push_back(i);
                                    aiPlayIndices.push_back(i + 1);
                                    sort(aiPlayIndices.begin(), aiPlayIndices.end());
                                    break;
                                }
                            }
                        }
                    }

                    //嘗試找鐵支
                    if (aiPlayIndices.empty() && (tableType == 0 || tableType <= 5)) {
                        vector<int> fourIdx;
                        int singleIdx = -1;

                        // 找四條
                        for (int i = 0; i <= handCounts[currentPlayer] - 4; i++) {
                            if (playerc[currentPlayer][i].num == playerc[currentPlayer][i + 1].num &&
                                playerc[currentPlayer][i].num == playerc[currentPlayer][i + 2].num &&
                                playerc[currentPlayer][i].num == playerc[currentPlayer][i + 3].num) {

                                int maxVal = playerc[currentPlayer][i + 3].takevalue();
                                // 鐵支可以壓任何5張以下的牌型，或同鐵支時比大小
                                if (tableType == 0 || tableType < 5 || (tableType == 5 && maxVal > tableValue)) {
                                    fourIdx = { i, i + 1, i + 2, i + 3 };
                                    break;
                                }
                            }
                        }

                        // 隨便配一張不是鐵支數字的單牌
                        if (!fourIdx.empty()) {
                            for (int i = 0; i < handCounts[currentPlayer]; i++) {
                                if (playerc[currentPlayer][i].num != playerc[currentPlayer][fourIdx[0]].num) {
                                    singleIdx = i;
                                    break;
                                }
                            }
                            if (singleIdx != -1) {
                                aiPlayIndices = fourIdx;
                                aiPlayIndices.push_back(singleIdx);
                                sort(aiPlayIndices.begin(), aiPlayIndices.end());
                            }
                        }
                    }

                    //嘗試找同花順
                    if (aiPlayIndices.empty() && (tableType == 0 || tableType >= 3)) {
                        for (int i = 0; i < handCounts[currentPlayer]; i++) {
                            vector<int> tempIdx;
                            tempIdx.push_back(i);
                            int expectedNum = playerc[currentPlayer][i].numusedtocal + 1;
                            string targetColor = playerc[currentPlayer][i].color; // 鎖定第一張牌的花色

                            for (int j = i + 1; j < handCounts[currentPlayer]; j++) {
                                // 條件 1.連號 2.花色完全一樣
                                if (playerc[currentPlayer][j].numusedtocal == expectedNum && playerc[currentPlayer][j].color == targetColor) {
                                    tempIdx.push_back(j);
                                    expectedNum++;
                                    if (tempIdx.size() == 5) {
                                        break;
                                    }
                                }
                            }

                            if (tempIdx.size() == 5) {
                                // 排除JQKA2
                                if (playerc[currentPlayer][tempIdx[0]].numusedtocal != 8) {
                                    int maxVal = playerc[currentPlayer][tempIdx[4]].takevalue();

                                    if (tableType == 0 || tableType < 6 || (tableType == 6 && maxVal > tableValue)) {
                                        aiPlayIndices = tempIdx;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                if (aiPlayIndices.empty()) {// AI pass
                    actionLog.push_back("玩家 " + std::to_string(currentPlayer + 1) + " (電腦) : Pass");
                    passCount++;
                    cout << "玩家 " << currentPlayer + 1 << " (電腦) 選擇 pass" << endl;
                    cout << "-----------------------------------------------------------------" << endl;
                }
                else {// AI出牌
                    if (aiPlayIndices.size() == 1) {//出一張
                        actionLog.push_back("玩家 " + std::to_string(currentPlayer + 1) + " (電腦) : 單張 " + playerc[currentPlayer][aiPlayIndices[0]].color + "-" + playerc[currentPlayer][aiPlayIndices[0]].getnutw());
                        cout << "玩家 " << currentPlayer + 1 << " (電腦) 出了一張牌: "
                            << playerc[currentPlayer][aiPlayIndices[0]].color << "-"
                            << playerc[currentPlayer][aiPlayIndices[0]].getnutw() << endl;

                        firstroundfirsthand = false;
                        lastPlayPlayer = currentPlayer;
                        tableType = 1;
                        tableValue = playerc[currentPlayer][aiPlayIndices[0]].takevalue();
                        passCount = 0;
                        tableCards[0] = playerc[currentPlayer][aiPlayIndices[0]];

                        currentTableCardCount = 1;

                        removeCard(aiPlayIndices[0], playerc[currentPlayer], handCounts[currentPlayer]);
                    }
                    else if (aiPlayIndices.size() == 2) {//出一對
                        actionLog.push_back("玩家 " + std::to_string(currentPlayer + 1) + " (電腦) : 一對 " + playerc[currentPlayer][aiPlayIndices[0]].color + "-" + playerc[currentPlayer][aiPlayIndices[0]].getnutw() + " / " + playerc[currentPlayer][aiPlayIndices[1]].color + "-" + playerc[currentPlayer][aiPlayIndices[1]].getnutw());
                        cout << "玩家 " << currentPlayer + 1 << " (電腦) 出了一對: "
                            << playerc[currentPlayer][aiPlayIndices[0]].color << "-" << playerc[currentPlayer][aiPlayIndices[0]].getnutw() << " "
                            << playerc[currentPlayer][aiPlayIndices[1]].color << "-" << playerc[currentPlayer][aiPlayIndices[1]].getnutw() << endl;

                        firstroundfirsthand = false;
                        lastPlayPlayer = currentPlayer;
                        tableType = 2;
                        tableValue = playerc[currentPlayer][aiPlayIndices[1]].takevalue();
                        passCount = 0;
                        tableCards[0] = playerc[currentPlayer][aiPlayIndices[0]];
                        tableCards[1] = playerc[currentPlayer][aiPlayIndices[1]];

                        currentTableCardCount = 2;

                        removeCard(aiPlayIndices[1], playerc[currentPlayer], handCounts[currentPlayer]);
                        removeCard(aiPlayIndices[0], playerc[currentPlayer], handCounts[currentPlayer]);
                    }
                    else if (aiPlayIndices.size() == 5) {//五張牌
                        vector<Poker> selected;
                        for (int idx : aiPlayIndices) {
                            selected.push_back(playerc[currentPlayer][idx]);
                        }
                        int pattern = getPatternType(selected);

                        cout << "玩家 " << currentPlayer + 1 << " (電腦) 出了 ";
                        if (pattern == 3) {
                            cout << "順子: ";
                        }
                        else if (pattern == 4) {
                            cout << "葫蘆: ";
                        }
                        else if (pattern == 5) {
                            cout << "鐵支: ";
                        }

                        for (int idx : aiPlayIndices) {
                            cout << playerc[currentPlayer][idx].color << "-" << playerc[currentPlayer][idx].getnutw() << " ";
                        }
                        cout << endl;

                        firstroundfirsthand = false;
                        lastPlayPlayer = currentPlayer;
                        tableType = pattern;

                        if (pattern == 4 || pattern == 5) {
                            tableValue = selected[2].takevalue();
                        }
                        else {
                            tableValue = selected[4].takevalue();
                        }

                        passCount = 0;
                        for (int i = 0; i < 5; i++) {
                            tableCards[i] = selected[i];
                        }

                        currentTableCardCount = 5;

                        string detail = "";
                        for (int idx : aiPlayIndices) {
                            detail += playerc[currentPlayer][idx].color + "-" + playerc[currentPlayer][idx].getnutw() + " ";
                        }
                        actionLog.push_back("玩家 " + std::to_string(currentPlayer + 1) + " (電腦) : 五張牌 (" + detail + ")");

                        sort(aiPlayIndices.begin(), aiPlayIndices.end(), greater<int>());//從大到小排序，以免移除牌時影響未移除的牌位置
                        for (int i = 0; i < aiPlayIndices.size(); i++) {
                            removeCard(aiPlayIndices[i], playerc[currentPlayer], handCounts[currentPlayer]);
                        }
                    }
                    cout << "-----------------------------------------------------------------" << endl;
                }
            }

            //-----------------------------------------------------------有人出完
            for (int i = 0;i < playercount;i++) {
                if (handCounts[i] == 0 && hasscored[i] == false) {
                    cout << "玩家 " << i + 1 << " 已經出完牌了！" << endl;
                    player[i].totscore += curpoint;

                    player[i].lastRoundRank = currentRank++;

                    cout << "玩家 " << i + 1 << " 獲得 " << curpoint << " 分！" << endl;
                    curpoint--;
                    hasscored[i] = true;
                }
            }

            currentPlayer = (currentPlayer + 1) % playercount;
            while (hasscored[currentPlayer] == true) {//如果下一位玩家已經出完牌了，就繼續往下找下一位玩家
                currentPlayer = (currentPlayer + 1) % playercount;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }

        cout << "=本輪結束=======================================================累積總分=" << endl;
        for (int i = 0; i < playercount; i++) {
            cout << "玩家 " << i + 1 << ": " << player[i].totscore << " 分" << endl;
        }
        curround++;
    }

    clearScreen();
    cout << "\n=================================================================\n";
    cout << "                    遊 戲 結 束  -  最 終 排 行 榜                 \n";
    cout << "=================================================================\n\n";

    //氣泡排序法
    Player* rankBoard[4];
    for (int i = 0; i < playercount; i++) rankBoard[i] = &player[i];

    for (int i = 0; i < playercount - 1; i++) {
        for (int j = 0; j < playercount - i - 1; j++) {
            if (rankBoard[j]->totscore < rankBoard[j + 1]->totscore ||
                (rankBoard[j]->totscore == rankBoard[j + 1]->totscore && rankBoard[j]->lastRoundRank > rankBoard[j + 1]->lastRoundRank)) {

                Player* temp = rankBoard[j];
                rankBoard[j] = rankBoard[j + 1];
                rankBoard[j + 1] = temp;
            }
        }
    }

    //印出名次
    for (int i = 0; i < playercount; i++) {
        int originalID = 0;
        for (int k = 0; k < playercount; k++) {
            if (rankBoard[i] == &player[k]) {
                originalID = k + 1;
                break;
            }
        }
        cout << "  第 " << i + 1 << " 名： 玩家 " << originalID << " (總分: " << rankBoard[i]->totscore << " 分)\n";
    }
    cout << "\n=================================================================\n";
    system("pause");

    return 0;
}