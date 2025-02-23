#include <SDL.h>
#include <cmath>
#include <vector>

const int SCREEN_WIDTH = 1366;
const int SCREEN_HEIGHT = 768;
const float TANK_SPEED = 0.1f;
const float BULLET_SPEED = 5.0f;
const float RECOIL_FORCE = 2.0f;

struct Bullet {
    float x, y, dx, dy;
};

struct Tank {
    float x, y;
    float vx, vy;
    float angle;
};

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Tank tank = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 0, 0 };
std::vector<Bullet> bullets;

void handleInput(bool& running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) running = false;
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            float rad = tank.angle * M_PI / 180.0f;
            float bx = tank.x + 20 * cos(rad);
            float by = tank.y + 20 * sin(rad);
            bullets.push_back({ bx, by, BULLET_SPEED * cos(rad), BULLET_SPEED * sin(rad) });
            tank.vx -= RECOIL_FORCE * cos(rad);
            tank.vy -= RECOIL_FORCE * sin(rad);
        }
    }
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    tank.angle = atan2(my - tank.y, mx - tank.x) * 180.0 / M_PI;
}

void update(float deltaTime) {
    tank.x += tank.vx;
    tank.y += tank.vy;
    tank.vx *= 0.98f;
    tank.vy *= 0.98f;

    if (tank.x < 0) tank.x = 0;
    if (tank.x > SCREEN_WIDTH) tank.x = SCREEN_WIDTH;
    if (tank.y < 0) tank.y = 0;
    if (tank.y > SCREEN_HEIGHT) tank.y = SCREEN_HEIGHT;

    for (auto& b : bullets) {
        b.x += b.dx;
        b.y += b.dy;
    }
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect tankBody = { (int)tank.x - 10, (int)tank.y - 10, 20, 20 };
    SDL_RenderFillRect(renderer, &tankBody);
    float rad = tank.angle * M_PI / 180.0f;
    SDL_RenderDrawLine(renderer, tank.x, tank.y, tank.x + 20 * cos(rad), tank.y + 20 * sin(rad));
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    for (auto& b : bullets) {
        SDL_Rect bulletRect = { (int)b.x, (int)b.y, 5, 5 };
        SDL_RenderFillRect(renderer, &bulletRect);
    }
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Tank Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_SetWindowResizable(window, SDL_FALSE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    bool running = true;
    Uint32 lastTime = SDL_GetTicks();
    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        handleInput(running);
        update(deltaTime);
        render();
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
