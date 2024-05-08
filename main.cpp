#include <SDL.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// Dodajemy funkcję do rysowania okręgów
void SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // pozioma odległość od środka
            int dy = radius - h; // pionowa odległość od środka
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    srand(time(NULL));

    int windowWidth = 800;
    int windowHeight = 600;

    SDL_Window* window = SDL_CreateWindow("Minigolf", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;
    bool isMoving = false;
    int moveCount = 0;
    double ballSpeedX = 0;
    double ballSpeedY = 0;
    double friction = 0.01;
    double minBounceSpeed = 0.5;

    // Zmieniamy typ zmiennych na double
    double ballPosX = rand() % (windowWidth - 20);
    double ballPosY = rand() % (windowHeight - 20);
    double holePosX = rand() % (windowWidth - 20);
    double holePosY = rand() % (windowHeight - 20);

    // Dodajemy więcej przeszkód o większych rozmiarach
    const int obstacleCount = 10; // Zwiększamy liczbę przeszkód
    SDL_Rect obstacles[obstacleCount];
    for (int i = 0; i < obstacleCount; i++) {
        obstacles[i].w = rand() % 70 + 30; // Szerokość przeszkody
        obstacles[i].h = rand() % 70 + 30; // Wysokość przeszkody
        do {
            obstacles[i].x = rand() % (windowWidth - obstacles[i].w);
            obstacles[i].y = rand() % (windowHeight - obstacles[i].h);
        } while ((fabs(obstacles[i].x - ballPosX) < 50 && fabs(obstacles[i].y - ballPosY) < 50) ||
                 (fabs(obstacles[i].x - holePosX) < 50 && fabs(obstacles[i].y - holePosY) < 50));
    }

    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                double dx = mouseX - (ballPosX + 10); // Uwzględniamy połowę szerokości kulki
                double dy = mouseY - (ballPosY + 10); // Uwzględniamy połowę wysokości kulki
                double distance = sqrt(dx * dx + dy * dy);
                ballSpeedX = dx / distance * 5;
                ballSpeedY = dy / distance * 5;
                isMoving = true;
                moveCount++;
            }
        }

        // Wyczyść backbuffer (ustaw tło na kolor trawy)
        SDL_SetRenderDrawColor(renderer, 124, 252, 0, 255); // Ustawiamy kolor na kolor trawy
        SDL_RenderClear(renderer);

        // Dodajemy wzór szachownicy do tła
        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255); // Ustawiamy kolor na ciemniejszy odcień zieleni
        for (int i = 0; i < windowWidth; i += 40) {
            for (int j = 0; j < windowHeight; j += 40) {
                if ((i / 40 + j / 40) % 2 == 0) {
                    SDL_Rect rect = { i, j, 40, 40 };
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Ustawiamy kolor na biały
        SDL_RenderDrawCircle(renderer, (int)ballPosX, (int)ballPosY, 10); // Rzutujemy na int do rysowania

        ballPosX += ballSpeedX;
        ballPosY += ballSpeedY;

        if (isMoving && (ballPosX <= 0 || ballPosX + 20 >= windowWidth) && fabs(ballSpeedX) > minBounceSpeed) {
            ballSpeedX = -ballSpeedX;
        }
        if (isMoving && (ballPosY <= 0 || ballPosY + 20 >= windowHeight) && fabs(ballSpeedY) > minBounceSpeed) {
            ballSpeedY = -ballSpeedY;
        }

        // Sprawdź kolizję z przeszkodami
        for (int i = 0; i < obstacleCount; i++) {
            if (ballPosX < obstacles[i].x + obstacles[i].w && ballPosX + 20 > obstacles[i].x &&
                ballPosY < obstacles[i].y + obstacles[i].h && ballPosY + 20 > obstacles[i].y) {
                // Odbij kulke od przeszkody
                ballSpeedX = -ballSpeedX;
                ballSpeedY = -ballSpeedY;
            }
        }

        // Zastosuj tarcie
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
        if (isMoving && fabs(ballSpeedX) < 0.01 && fabs(ballSpeedY) < 0.01) {
            ballSpeedX = 0;
            ballSpeedY = 0;
            isMoving = false;
        }

        // Wyrenderuj dziurę (czarny okrąg)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Ustawiamy kolor na czarny
        SDL_RenderDrawCircle(renderer, (int)holePosX, (int)holePosY, 10); // Rzutujemy na int do rysowania

        if (ballPosX < holePosX + 20 && ballPosX + 20 > holePosX && ballPosY < holePosY + 20 && ballPosY + 20 > holePosY) {
            // Resetuj grę
            ballSpeedX = 0;
            ballSpeedY = 0;
            ballPosX = rand() % (windowWidth - 20);
            ballPosY = rand() % (windowHeight - 20);
            holePosX = rand() % (windowWidth - 20);
            holePosY = rand() % (windowHeight - 20);
            isMoving = false;
            moveCount = 0;
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
        }

        // Wyrenderuj przeszkody
        SDL_SetRenderDrawColor(renderer, 210, 180, 140, 255); // Ustawiamy kolor na kolor piasku
        for (int i = 0; i < obstacleCount; i++) {
            SDL_RenderFillRect(renderer, &obstacles[i]);
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
