#include <SDL.h>
#include <SDL_ttf.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "headers/circle.h"
#include "headers/collision.h"

int main(int argc, char* argv[]) {
    // Inicjalizacja SDL
    SDL_Init(SDL_INIT_VIDEO);
    srand(time(NULL));

    // Inicjalizacja SDL_ttf
    TTF_Init();

    // Inicjalizacja zmiennych gry
    int moves = 2;
    bool onObstacle = false;
    int score = 0;
    bool quit = false;
    bool isMoving = false;
    int moveCount = 0;
    double ballSpeedX = 0;
    double ballSpeedY = 0;
    double friction = 0.01;
    double minBounceSpeed = 0.5;

    // Ładowanie czcionki
    TTF_Font* font = TTF_OpenFont("font.ttf", 24);

    // Ustawienie koloru tekstu
    SDL_Color textColor = {255, 255, 255, 255};

    // Ustawienia okna
    int windowWidth = 800;
    int windowHeight = 600;
    SDL_Window* window = SDL_CreateWindow("Minigolf", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Inicjalizacja przeszkód
    const int obstacleCount = 10;
    SDL_Rect obstacles[obstacleCount];
    for (int i = 0; i < obstacleCount; i++) {
        obstacles[i].w = rand() % 70 + 30;
        obstacles[i].h = rand() % 70 + 30;
        obstacles[i].x = rand() % (windowWidth - obstacles[i].w);
        obstacles[i].y = rand() % (windowHeight - obstacles[i].h);
    }

    // Inicjalizacja pozycji kulki i dziury
    double ballPosX, ballPosY;
    double holePosX, holePosY;
    bool collision;
    do {
        collision = false;
        ballPosX = rand() % (windowWidth - 20);
        ballPosY = rand() % (windowHeight - 20);
        for (int i = 0; i < obstacleCount; i++) {
            if (checkCollision(obstacles[i], ballPosX, ballPosY, 10) ||
                checkCollision(obstacles[i], ballPosX + 20, ballPosY, 10) ||
                checkCollision(obstacles[i], ballPosX, ballPosY + 20, 10) ||
                checkCollision(obstacles[i], ballPosX + 20, ballPosY + 20, 10)) {
                collision = true;
                break;
            }
        }
    } while (collision);

    do {
        collision = false;
        holePosX = rand() % (windowWidth - 20);
        holePosY = rand() % (windowHeight - 20);
        for (int i = 0; i < obstacleCount; i++) {
            if (checkCollision(obstacles[i], holePosX, holePosY, 10) ||
                checkCollision(obstacles[i], holePosX + 20, holePosY, 10) ||
                checkCollision(obstacles[i], holePosX, holePosY + 20, 10) ||
                checkCollision(obstacles[i], holePosX + 20, holePosY + 20, 10)) {
                collision = true;
                break;
            }
        }
    } while (collision);

    // Główna pętla gry
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_MOUSEBUTTONDOWN && isMoving == false) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                double dx = mouseX - (ballPosX + 10);
                double dy = mouseY - (ballPosY + 10);
                double distance = sqrt(dx * dx + dy * dy);
                ballSpeedX = dx / distance * 5;
                ballSpeedY = dy / distance * 5;
                isMoving = true;
                moveCount++;
                moves--;
            }
        }

        ballPosX += ballSpeedX;
        ballPosY += ballSpeedY;

        // Wyczyść backbuffer (ustaw tło na kolor trawy)
        SDL_SetRenderDrawColor(renderer, 154, 252, 10, 255);
        SDL_RenderClear(renderer);

        // Dodajemy wzór szachownicy do tła
        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
        for (int i = 0; i < windowWidth; i += 40) {
            for (int j = 0; j < windowHeight; j += 40) {
                if ((i / 40 + j / 40) % 2 == 0) {
                    SDL_Rect rect = { i, j, 40, 40 };
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        // Wyrenderuj przeszkody
        SDL_SetRenderDrawColor(renderer, 210, 180, 140, 255);
        for (int i = 0; i < obstacleCount; i++) {
            SDL_RenderFillRect(renderer, &obstacles[i]);
        }

        // Wyrenderuj dziurę (czarny okrąg)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawCircle(renderer, (int)holePosX, (int)holePosY, 10);

        // Renderowanie kulki
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawCircle(renderer, (int)ballPosX, (int)ballPosY, 10);

        if (isMoving && (ballPosX <= 0 || ballPosX + 20 >= windowWidth) && fabs(ballSpeedX) > minBounceSpeed) {
            ballSpeedX = -ballSpeedX;
        }
        if (isMoving && (ballPosY <= 0 || ballPosY + 20 >= windowHeight) && fabs(ballSpeedY) > minBounceSpeed) {
            ballSpeedY = -ballSpeedY;
        }

        // Sprawdź kolizję z przeszkodami
        onObstacle = false;
        for (int i = 0; i < obstacleCount; i++) {
            if (ballPosX < obstacles[i].x + obstacles[i].w && ballPosX + 20 > obstacles[i].x &&
                ballPosY < obstacles[i].y + obstacles[i].h && ballPosY + 20 > obstacles[i].y) {
                onObstacle = true;
            }
        }

        // Zastosuj tarcie
        friction = onObstacle ? 0.1 : 0.01;
        if (ballSpeedX > 0) {
            ballSpeedX = fmax(0, ballSpeedX - friction);
        } else {
            ballSpeedX = fmin(0, ballSpeedX + friction);
        }

        if (ballSpeedY > 0) {
            ballSpeedY = fmax(0, ballSpeedY - friction);
        } else {
            ballSpeedY = fmin(0, ballSpeedY + friction);
        }

        // Sprawdź, czy kulka się zatrzymała
        if (isMoving && fabs(ballSpeedX) < 0.02 && fabs(ballSpeedY) < 0.02) {
            ballSpeedX = 0;
            ballSpeedY = 0;
            isMoving = false;
        }else{
            isMoving = true;
        }

        if (ballPosX < holePosX + 20 && ballPosX + 20 > holePosX && ballPosY < holePosY + 20 && ballPosY + 20 > holePosY) {
            ballSpeedX = 0;
            ballSpeedY = 0;
            ballPosX = rand() % (windowWidth - 20);
            ballPosY = rand() % (windowHeight - 20);
            holePosX = rand() % (windowWidth - 20);
            holePosY = rand() % (windowHeight - 20);
            isMoving = false;
            moveCount = 0;
            moves = 2;

            score++;
        }

        // Jeśli wykonano 2 ruchy, wyświetlamy napis "Click to try again"
        if (moveCount >= 2 && isMoving == false){
            char* message = "Click to try again";
            SDL_Surface* messageSurface = TTF_RenderText_Solid(font, message, textColor);
            SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);

            SDL_Rect messageRect;
            messageRect.x = (windowWidth - messageSurface->w) / 2;
            messageRect.y = (windowHeight - messageSurface->h) / 2;
            messageRect.w = messageSurface->w;
            messageRect.h = messageSurface->h;

            SDL_RenderCopy(renderer, messageTexture, NULL, &messageRect);
        }
        // Jeśli wykonano 3 ruchy, resetujemy grę
        if (moveCount >= 3) {
            ballSpeedX = 0;
            ballSpeedY = 0;
            ballPosX = rand() % (windowWidth - 20);
            ballPosY = rand() % (windowHeight - 20);
            holePosX = rand() % (windowWidth - 20);
            holePosY = rand() % (windowHeight - 20);
            isMoving = false;
            moveCount = 0;
            moves = 2;

            if (score > 0){
                score--;
            }

        }

        //pole tekstowe z wynikiem i ilością punktów
        char scoreText[50];
        sprintf(scoreText, "Score: %d         Moves: %d", score, moves);
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText, textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        SDL_Rect textRect;
        textRect.x = 10;
        textRect.y = 10;
        textRect.w = textSurface->w;
        textRect.h = textSurface->h;

        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);


        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_CloseFont(font);
    TTF_Quit();
    return 0;
}
