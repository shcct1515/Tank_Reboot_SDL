#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
#include <vector>

// Kích thước cửa sổ trò chơi
const int SCREEN_WIDTH = 1366;
const int SCREEN_HEIGHT = 768;

// Tham số tốc độ và lực
const float TANK_SPEED = 0.1f;
const float BULLET_SPEED = 0.7f;
const float RECOIL_FORCE = 3.0f;

// Cấu trúc lưu thông tin viên đạn
struct Bullet {
    float x, y, dx, dy;
};

// Cấu trúc lưu thông tin xe tăng
struct Tank {
    float x, y;
    float vx, vy;
    float angle;
};

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* tankTexture = nullptr;
SDL_Texture* bulletTexture = nullptr;

Tank tank = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 0, 0 };
std::vector<Bullet> bullets;

// Hàm tải hình ảnh và chuyển thành texture
SDL_Texture* loadTexture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        SDL_Log("Failed to load image %s: %s", path, IMG_GetError());
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

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

    if (tank.x < 20) tank.x = 20;
    if (tank.x > SCREEN_WIDTH - 20) tank.x = SCREEN_WIDTH - 20;
    if (tank.y < 20) tank.y = 20;
    if (tank.y > SCREEN_HEIGHT - 20) tank.y = SCREEN_HEIGHT - 20;

    for (auto& b : bullets) {
        b.x += b.dx;
        b.y += b.dy;
    }
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect topWall = { 0, 0, SCREEN_WIDTH, 10 };
    SDL_Rect bottomWall = { 0, SCREEN_HEIGHT - 10, SCREEN_WIDTH, 10 };
    SDL_Rect leftWall = { 0, 0, 10, SCREEN_HEIGHT };
    SDL_Rect rightWall = { SCREEN_WIDTH - 10, 0, 10, SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer, &topWall);
    SDL_RenderFillRect(renderer, &bottomWall);
    SDL_RenderFillRect(renderer, &leftWall);
    SDL_RenderFillRect(renderer, &rightWall);

    SDL_Rect tankRect = { (int)tank.x - 32, (int)tank.y - 32, 64, 64 };
    SDL_Point center = { tankRect.w / 2, tankRect.h / 2 };
    SDL_RenderCopyEx(renderer, tankTexture, nullptr, &tankRect, tank.angle+90, &center, SDL_FLIP_NONE);

    for (auto& b : bullets) {
        SDL_Rect bulletRect = { (int)b.x - 8, (int)b.y - 8, 16, 16 };
        SDL_RenderCopy(renderer, bulletTexture, nullptr, &bulletRect);
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    window = SDL_CreateWindow("Tank Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_SetWindowResizable(window, SDL_FALSE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    tankTexture = loadTexture("assets/default_skin.png");
    bulletTexture = loadTexture("assets/bullet.png");

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

    SDL_DestroyTexture(tankTexture);
    SDL_DestroyTexture(bulletTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
