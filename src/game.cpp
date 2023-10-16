#include <iostream>
#include <string>

#include <SDL.h>
#include <SDL_image.h>

#include <game.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>


Game::Game(Flags flags, SDL_Rect dims, const int frameRate, const std::string title) : interface(Level::CHAPEL_OF_ANTICIPATION), flags(flags), dims(dims), frameRate(frameRate), title(title) {
    state = GameState::MENU;
}

Game::~Game()
{
    if (windowSurface != nullptr) SDL_FreeSurface(windowSurface);
    if (window != nullptr) SDL_DestroyWindow(window);
    if (renderer != nullptr) SDL_DestroyRenderer(renderer);

    // quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

/**
 * @brief The only method accessible at public scope.
 * @note Call this exactly once.
*/
void Game::start() {
    init();
    gameLoop();
}

/**
 * @brief Initialize everything.
 * @note Any `init()` methods should be placed here.
*/
void Game::init() {
    // Initialize SDL subsystems.
    SDL_Init(flags.init);
    IMG_Init(flags.image);
    for (const auto& pair: flags.hints) SDL_SetHint(pair.first.c_str(), pair.second.c_str());

    window = SDL_CreateWindow(title.c_str(), dims.x, dims.y, dims.w, dims.h, flags.window);
    windowID = SDL_GetWindowID(window);

    renderer = SDL_CreateRenderer(window, -1, flags.renderer);
    // SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);   // throw some arbitrary color hexvalue

    // Initialize member instances.
    interface.init(renderer);
}

/**
 * @brief Start the game loop.
 * @todo Manually cap frame rate (VSync disabled)
*/
void Game::gameLoop() {
    // Call this once.
    blit();

    while (state != GameState::EXIT) {
        handleEvents();
        render();
    }
}

/**
 * @brief Handles the rendering of static graphics or those that rarely update.
 * @note Should also be called when an `SDL_WINDOWEVENT_SIZE_CHANGED` event occurs.
 * @see Game::handleWindowEvent()
*/
void Game::blit() {
    windowSurface = SDL_GetWindowSurface(window);
    SDL_GetWindowSize(window, &windowSize.x, &windowSize.y);
    interface.blit(renderer, windowSize);
    SDL_UpdateWindowSurface(window);
}

/**
 * @brief Handles the rendering of graphics that updates constantly.
 * 
 * @note The order should be as follows:
 * -> `interface` i.e. environments
 * -> interactables
 * -> entities
 * -> player
 * @note Any `render()` methods should be placed here.
*/
void Game::render() {
    SDL_RenderClear(renderer);
    if (state == GameState::INGAME_PLAYING) interface.render(renderer);
    SDL_RenderPresent(renderer);
}

/**
 * @brief Handle all incoming events.
 * @note All `handleEvent()` methods should go here.
*/
void Game::handleEvents() {
    SDL_Event* event = new SDL_Event;
    if (!SDL_PollEvent(event)) return;

    switch (event -> type) {
        case SDL_QUIT:
            state = GameState::EXIT;
            break;
        
        case SDL_WINDOWEVENT:
            handleWindowEvent(event);
            break;
        // track mouse motion & buttons only
        // also invoked when mouse focus regained/lost
        case SDL_MOUSEMOTION: case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:
            handleMouseEvent(event);
            break;
        case SDL_KEYDOWN: case SDL_KEYUP:
            handleKeyBoardEvent(event);
            break;
    }

    delete event;
}

/**
 * @brief Handle a windows event.
*/
void Game::handleWindowEvent(const SDL_Event* event) {
    if (event -> window.windowID != windowID) return;
    switch (event -> window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            blit();
            break;
            
        default: break;
    }
}

/**
 * @brief Handle a keyboard event.
 * @note Scancode denotes physical location and keycode denotes actual meaning (different if remapped)
*/
void Game::handleKeyBoardEvent(const SDL_Event* event) {
    switch (state) {
        case GameState::INGAME_PLAYING:
            if (event -> key.keysym.sym == SDLK_ESCAPE) state = GameState::EXIT;

        default: break;
    }
}

/**
 * @brief Handle a mouse event.
*/
void Game::handleMouseEvent(const SDL_Event* event) {
    switch (state) {
        case GameState::MENU:
            if (event -> type != SDL_MOUSEBUTTONDOWN) break;
            state = GameState::INGAME_PLAYING;
            blit();
            break;

        default: break;
    }
}