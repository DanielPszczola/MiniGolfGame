#include <SDL.h>
#include <math.h>
#include <stdlib.h> // Dla funkcji rand() i srand()
#include <time.h>   // Dla funkcji time()

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    srand(time(NULL)); // Inicjalizacja generatora liczb losowych

    // Ustaw wymiary okna
    int windowWidth = 800;
    int windowHeight = 600;

    // Stwórz okno
    SDL_Window* window = SDL_CreateWindow("Minigolf", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Główna pętla gry
    bool quit = false;
    bool isMoving = false; // Dodajemy zmienną isMoving
    int moveCount = 0; // Dodajemy licznik ruchów
    double ballSpeedX = 0;
    double ballSpeedY = 0;
    double friction = 0.995; // Zwiększamy wartość tarcia
    double minBounceSpeed = 0.5; // Minimalna prędkość odbicia
    SDL_Rect ballRect = { rand() % (windowWidth - 20), rand() % (windowHeight - 20), 20, 20 };
    SDL_Rect holeRect = { rand() % (windowWidth - 20), rand() % (windowHeight - 20), 20, 20 };
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                // Zapisz kierunek strzału
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                int dx = mouseX - (ballRect.x + ballRect.w / 2);
                int dy = mouseY - (ballRect.y + ballRect.h / 2);
                double distance = sqrt(dx * dx + dy * dy);
                ballSpeedX = dx / distance * 5;
                ballSpeedY = dy / distance * 5;
                isMoving = true; // Ustawiamy isMoving na true, gdy kulka jest w ruchu
                moveCount++; // Zwiększamy licznik ruchów
            }
        }

        // Wyczyść backbuffer (ustaw tło na czarny)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Wyrenderuj kulę (czerwony kwadrat)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &ballRect);

        // Fizyka odbijania się kulki
        ballRect.x += ballSpeedX;
        ballRect.y += ballSpeedY;

        // Odbicie od ścianek
        if (isMoving && (ballRect.x <= 0 || ballRect.x + ballRect.w >= windowWidth)) {
            ballSpeedX = fabs(ballSpeedX) > minBounceSpeed ? -ballSpeedX : (ballSpeedX > 0 ? -minBounceSpeed : minBounceSpeed);
        }
        if (isMoving && (ballRect.y <= 0 || ballRect.y + ballRect.h >= windowHeight)) {
            ballSpeedY = fabs(ballSpeedY) > minBounceSpeed ? -ballSpeedY : (ballSpeedY > 0 ? -minBounceSpeed : minBounceSpeed);
        }

        // Zastosuj tarcie
        ballSpeedX *= friction;
        ballSpeedY *= friction;

        // Sprawdź, czy kulka się zatrzymała
        if (isMoving && fabs(ballSpeedX) < 0.01 && fabs(ballSpeedY) < 0.01) {
            ballSpeedX = 0;
            ballSpeedY = 0;
            isMoving = false; // Ustawiamy isMoving na false, gdy kulka się zatrzymała
        }

        // Sprawdź kolizję z dziurą
        if (ballRect.x < holeRect.x + holeRect.w && ballRect.x + ballRect.w > holeRect.x && ballRect.y < holeRect.y + holeRect.h && ballRect.y + ballRect.h > holeRect.y) {
            // Resetuj grę
            ballSpeedX = 0;
            ballSpeedY = 0;
            ballRect.x = rand() % (windowWidth - 20);
            ballRect.y = rand() % (windowHeight - 20);
            holeRect.x = rand() % (windowWidth - 20);
            holeRect.y = rand() % (windowHeight - 20);
            isMoving = false; // Ustawiamy isMoving na false po resecie gry
            moveCount = 0; // Resetujemy licznik ruchów
        }

        // Jeśli wykonano 3 ruchy, resetujemy grę
        if (moveCount >= 3) {
            ballSpeedX = 0;
            ballSpeedY = 0;
            ballRect.x = rand() % (windowWidth - 20);
            ballRect.y = rand() % (windowHeight - 20);
            holeRect.x = rand() % (windowWidth - 20);
            holeRect.y = rand() % (windowHeight - 20);
            isMoving = false; // Ustawiamy isMoving na false po resecie gry
            moveCount = 0; // Resetujemy licznik ruchów
        }

        // Wyrenderuj dziurę (zielony kwadrat)
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &holeRect);

        // Wyświetl backbuffer na ekranie
        SDL_RenderPresent(renderer);

        // Ogranicz częstotliwość odświeżania ekranu
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
