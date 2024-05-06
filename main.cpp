#include <SDL.h>

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    // Ustaw wymiary okna
    int windowWidth = 800;
    int windowHeight = 600;

    // Stwórz okno
    SDL_Window* window = SDL_CreateWindow("Minigolf", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

    // Główna pętla gry
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Tutaj dodaj kod renderujący planszę i kulki

        SDL_Delay(16); // Ogranicz częstotliwość odświeżania ekranu
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
