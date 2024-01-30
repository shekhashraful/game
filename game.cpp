#include "include/SDL.h"
#include "include/SDL_ttf.h"
#include <iostream>
#include <sstream>
#include <vector>
#undef main

class Game {
public:
    Game() : running(true), score(0), currentDirection(RIGHT), nextDirection(RIGHT) {
        Initialize();
        LoadFont();
    }

    ~Game() {
        Cleanup();
    }

    int score;
    void Run() {
        while (running) {
            HandleInput();
            Update();
            Render();
        }
    }

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;

    bool running;

    enum Direction { UP, DOWN, LEFT, RIGHT };
    Direction currentDirection;
    Direction nextDirection;

    struct SnakeSegment {
        int x, y;
    };

    std::vector<SnakeSegment> snake;

    SDL_Point food;

    void Initialize() {
        SDL_Init(SDL_INIT_VIDEO);
        TTF_Init();

        window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        // Initialize snake
        snake.push_back({ 0, 0 });

        // Initialize food
        SpawnFood();
    }

    void LoadFont() {
        font = TTF_OpenFont("font.ttf", 24); // Replace with the path to your font file
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        }
    }

    void HandleInput() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_UP:
                    if (currentDirection != DOWN)
                        nextDirection = UP;
                    break;
                case SDLK_DOWN:
                    if (currentDirection != UP)
                        nextDirection = DOWN;
                    break;
                case SDLK_LEFT:
                    if (currentDirection != RIGHT)
                        nextDirection = LEFT;
                    break;
                case SDLK_RIGHT:
                    if (currentDirection != LEFT)
                        nextDirection = RIGHT;
                    break;
                }
            }
        }
    }

    void Update() {
        // Move the snake
        SnakeSegment newHead = snake.front();
        currentDirection = nextDirection;

        switch (currentDirection) {
        case UP:
            newHead.y -= 1;
            break;
        case DOWN:
            newHead.y += 1;
            break;
        case LEFT:
            newHead.x -= 1;
            break;
        case RIGHT:
            newHead.x += 1;
            break;
        }

        snake.insert(snake.begin(), newHead);

        // Check for collision
        if (CheckCollision()) {
            running = false;
            return;
        }

        // Check if the snake ate the food
        if (newHead.x == food.x && newHead.y == food.y) {
            score += 10;
            SpawnFood();
        }
        else {
            snake.pop_back();
        }
    }

    void Render() {
        SDL_SetRenderDrawColor(renderer, 0, 2, 0, 255);
        SDL_RenderClear(renderer);

        // Render snake
        SDL_SetRenderDrawColor(renderer, 25, 255, 255, 255);
        for (const auto& segment : snake) {
            SDL_Rect rect = { segment.x * 20, segment.y * 20, 20, 20 };
            SDL_RenderFillRect(renderer, &rect);
        }

        // Render food
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect foodRect = { food.x * 20, food.y * 20, 20, 20 };
        SDL_RenderFillRect(renderer, &foodRect);

        // Render score using SDL_ttf
        RenderScore();

        SDL_RenderPresent(renderer);

        SDL_Delay(100);
    }

    void RenderScore() {
        SDL_Color textColor = { 0, 0, 255 };
        std::string scoreText = "Score: " + std::to_string(score);

        SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
        
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        
        SDL_Rect textRect = { 320-surface->w/2 , 10, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &textRect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    void Cleanup() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
    }

    void SpawnFood() {
        food.x = rand() % (640 / 20);
        food.y = rand() % (480 / 20);
    }

    bool CheckCollision() {
        // Check collision with walls
        if (snake.front().x < 0 || snake.front().x >= 640 / 20 ||
            snake.front().y < 0 || snake.front().y >= 480 / 20) {
            return true;
        }

        // Check collision with itself
        for (size_t i = 1; i < snake.size(); ++i) {
            if (snake[i].x == snake.front().x && snake[i].y == snake.front().y) {
                return true;
            }
        }

        return false;
    }
};

int main(int argc, char* args[]) {
    Game snakeGame;
    snakeGame.Run();

    return 0;
}

//g++ -Llib -Iinclude main.cpp -lSDL2 -lSDL2_ttf
