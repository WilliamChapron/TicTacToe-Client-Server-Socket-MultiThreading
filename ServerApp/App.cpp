// App.cpp
#include "App.h"
#include "JSON.h"
#include "ServerSocket.h"
#include "Morpion.h"

App* App::instance = nullptr;

App::App() : pServer(nullptr), myMorpion(nullptr), turnCounter(0), player1(nullptr), player2(nullptr), playerNumber(0) {
}


bool App:: turn(sf::Vector2i mousePosition, int turnIndex, SOCKET inputSocket) {
    // check is good player turn
    if (!pServer->isSocketAtIndex(inputSocket, turnIndex)) {
        return false;
    }
    else {
        int placeState = myMorpion->placeSymbol(mousePosition, 640, 480);
        if (placeState == 1) {
            std::cout << "Erreur de placement du symbole. Impossible de placer � cet emplacement." << std::endl;
            return false;
        }
        // Increment turn and change current player
        myMorpion->currentPlayer = (turnCounter % 2 == 0) ? player2 : player1;
        turnCounter++;

        // Broadcast board to all clients
        json boardJson = CreateJsonTable("Message", myMorpion->board);
        pServer->BroadcastMessage(boardJson);
    }


    return true;
}


App::~App() {
    delete pServer;
    delete myMorpion;
    delete player1;
    delete player2;
}

App* App::GetInstance() {
    if (instance == nullptr) {
        instance = new App();
    }

    return instance;
}

void App::Init(HINSTANCE hInstance) {

    //PRINT("APP INIT");
    pServer = new ServerSocket(80, hInstance);
    if (!pServer->StartAsyncListening()) {
        return;
    }

    myMorpion = new Morpion();
}

void App::Run() {
    MSG msg = {};
    while (true) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

    }
}