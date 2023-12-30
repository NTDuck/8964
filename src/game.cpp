#include <game.hpp>

#include <algorithm>
#include <string>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <interaction.hpp>
#include <interface.hpp>
#include <entities.hpp>
#include <auxiliaries.hpp>


Game::Game(GameInitFlag const& flags, SDL_Rect windowDimension, const int frameRate, const std::string title) : flags(flags), windowDimension(windowDimension), frameRate(frameRate), title(title) {}

Game::~Game() {
    if (windowSurface != nullptr) {
        SDL_FreeSurface(windowSurface);
        windowSurface = nullptr;
    }
    
    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    globals::deinitialize();

    IngameInterface::deinitialize();
    MenuInterface::deinitialize();

    Player::deinitialize();
    Teleporter::deinitialize();
    Slime::deinitialize();

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
}

/**
 * @brief The only non-static method accessible at public scope.
 * @note Call this exactly once.
*/
void Game::start() {
    initialize();
    startGameLoop();
}

/**
 * @brief Initialize everything.
 * @note Any `initialize()` methods should be placed here.
*/
void Game::initialize() {
    // Initialize SDL subsystems
    SDL_Init(flags.lSDL);
    IMG_Init(flags.lIMG);
    TTF_Init();

    for (const auto& pair: flags.hints) SDL_SetHint(pair.first.c_str(), pair.second.c_str());

    window = SDL_CreateWindow(title.c_str(), windowDimension.x, windowDimension.y, windowDimension.w, windowDimension.h, flags.window);
    windowID = SDL_GetWindowID(window);
    globals::renderer = SDL_CreateRenderer(window, -1, flags.renderer);

    IngameInterface::initialize();
    MenuInterface::initialize();

    Player::initialize();
    Teleporter::initialize();
    Slime::initialize();

    ingameInterface = IngameInterface::instantiate(config::interface::levelName);
    menuInterface = MenuInterface::instantiate();
    player = Player::instantiate(SDL_Point{ 0, 0 });
}

/**
 * @brief Start the game loop.
 * @todo Manually cap frame rate (VSync disabled)
*/
void Game::startGameLoop() {
    // Serve as partial initialization for certain attributes of certain classes
    onLevelChange();
    onWindowChange();

    while (globals::state != GameState::kExit) {
        handleEntities();
        handleEvents();
        render();
    }
}

/**
 * @brief Handle everything about rendering.
 * 
 * @note The order should be as follows: `interface` i.e. environments -> interactables -> entities -> player
 * @note Any `render()` methods should be placed here.
*/
void Game::render() {
    SDL_RenderClear(globals::renderer);

    switch (globals::state) {
        case GameState::kIngamePlaying:
            ingameInterface->render();
            Teleporter::callOnEach(&Teleporter::render);
            Slime::callOnEach(&Slime::render);
            player->render();
            break;

        case GameState::kMenu:
            menuInterface->render();
            break;

        default: break;
    }

    SDL_RenderPresent(globals::renderer);
}

/**
 * @brief Called when switching to a new level.
*/
void Game::onLevelChange() {
    // Populate `globals::currentLevelData` members
    ingameInterface->onLevelChange();

    // Make changes to dependencies based on populated `globals::currentLevelData` members
    player->onLevelChange(globals::currentLevelData.playerLevelData);
    Teleporter::onLevelChangeAll<level::TeleporterLevelData>(globals::currentLevelData.teleportersLevelData);
    Slime::onLevelChangeAll<level::SlimeLevelData>(globals::currentLevelData.slimesLevelData);
}

/**
 * @brief Called should `window` change e.g. is resized.
*/
void Game::onWindowChange() {
    windowSurface = SDL_GetWindowSurface(window);
    SDL_GetWindowSize(window, &globals::windowSize.x, &globals::windowSize.y);

    // Dependencies that rely on certain dimension-related global variables
    ingameInterface->onWindowChange();
    menuInterface->onWindowChange();

    player->onWindowChange();
    Teleporter::callOnEach(&Teleporter::onWindowChange);
    Slime::callOnEach(&Slime::onWindowChange);

    SDL_UpdateWindowSurface(window);
}

/**
 * @brief Handle everything about entities.
*/
void Game::handleEntities() {
    if (globals::state != GameState::kIngamePlaying) return;
    handleEntitiesMovement();
    handleEntitiesInteraction();
}

/**
 * @brief Handle all entities movements & animation updates.
*/
void Game::handleEntitiesMovement() {
    player->initiateAnimation();
    player->move();
    player->updateAnimation();

    Teleporter::callOnEach(&Teleporter::updateAnimation);

    Slime::callOnEach(&Slime::calculateMove, player->destCoords);
    Slime::callOnEach(&Slime::initiateAnimation);
    Slime::callOnEach(&Slime::move);
    Slime::callOnEach(&Slime::updateAnimation);
}

template <typename Active, typename Passive>
void Game::onEntityCollision(Active& active, Passive& passive) {
    utils::isDerivedFrom<AbstractAnimatedDynamicEntity<Active>, Active>();
    utils::isDerivedFrom<AbstractAnimatedEntity<Passive>, Passive>();
}

template <>
void Game::onEntityCollision<Player, Teleporter>(Player& player, Teleporter& teleporter) {
    // state = GameState::kIngamePaused;
    ingameInterface->changeLevel(teleporter.targetLevel);
    globals::currentLevelData.playerLevelData.destCoords = teleporter.targetDestCoords;
    onLevelChange(); onWindowChange();
    // state = GameState::kIngamePlaying;
}

template <>
void Game::onEntityCollision<Player, Slime>(Player& player, Slime& slime) {
    // state = GameState::kExit;
    // player.onDeath();
}

/**
 * @brief Called when the `active` entity initiate an animation (possibly caused by the `passive` entity).
*/
template <typename Active, typename Passive>
void Game::onEntityAnimation(AnimationType animationType, Active& active, Passive& passive) {
    utils::isDerivedFrom<AbstractAnimatedEntity<Active>, Active>();
    utils::isDerivedFrom<AbstractAnimatedEntity<Passive>, Passive>();

    // Handle `kDamaged` case differently
    if (animationType == AnimationType::kDamaged && passive.currAnimationType == AnimationType::kAttack) {
        active.secondaryStats.HP -= EntitySecondaryStats::calculateFinalizedPhysicalDamage(passive.secondaryStats, active.secondaryStats);
        if (active.secondaryStats.HP <= 0) animationType = AnimationType::kDeath;
    }

    tile::NextAnimationData::update(active.nextAnimationData, animationType);
}

template void Game::onEntityAnimation<Player, Slime>(const AnimationType animationType, Player& player, Slime& slime);
template void Game::onEntityAnimation<Slime, Player>(const AnimationType animationType, Slime& slime, Player& player);

/**
 * @brief Handle interactions between entities.
*/
void Game::handleEntitiesInteraction() {
    auto teleporter = utils::checkEntityCollision<Player, Teleporter>(*player, InteractionType::kCoords); if (teleporter != nullptr) onEntityCollision<Player, Teleporter>(*player, *teleporter);
    auto slime = utils::checkEntityCollision<Player, Slime>(*player, InteractionType::kRect); if (slime != nullptr) onEntityCollision<Player, Slime>(*player, *slime);

    for (auto& slime : slimes) {
        if (slime == nullptr || slime->currAnimationType == AnimationType::kDeath) continue;
        if (utils::checkEntityAttackInitiate<Slime, Player>(*slime, *player)) onEntityAnimation<Slime, Player>(AnimationType::kAttack, *slime, *player);
        if (utils::checkEntityAttackRegister<Player, Slime>(*player, *slime)) onEntityAnimation<Player, Slime>(AnimationType::kDamaged, *player, *slime);
        if (utils::checkEntityAttackRegister<Slime, Player>(*slime, *player)) onEntityAnimation<Slime, Player>(AnimationType::kDamaged, *slime, *player);
    }
}

/**
 * @brief Handle all incoming events.
 * @note All `handleEvent()` methods should go here.
*/
void Game::handleEvents() {
    SDL_Event* event = new SDL_Event;
    if (!SDL_PollEvent(event)) return;

    switch (event->type) {
        case SDL_QUIT:
            globals::state = GameState::kExit;
            delete instance;   // ?
            break;
        
        case SDL_WINDOWEVENT:
            handleWindowEvent(*event);
            break;

        // track mouse motion & buttons only
        // also invoked when mouse focus regained/lost
        case SDL_MOUSEMOTION: case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:
            handleMouseEvent(*event);
            break;

        case SDL_KEYDOWN: case SDL_KEYUP:
            handleKeyBoardEvent(*event);
            break;
    }

    delete event;
}

/**
 * @brief Handle a windows event.
 * @bug Event `SDL_WINDOWEVENT_SIZE_CHANGED` causes temporary undefined behavior for non-stationary entities.
*/
void Game::handleWindowEvent(const SDL_Event& event) {
    if (event.window.windowID != windowID) return;
    switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            onWindowChange();
            break;
        default: break;
    }
}

/**
 * @brief Handle a keyboard event.
 * @note Scancode denotes physical location and keycode denotes actual meaning (different if remapped)
*/
void Game::handleKeyBoardEvent(const SDL_Event& event) {
    switch (globals::state) {
        case GameState::kIngamePlaying:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                globals::state = GameState::kExit;
                break;
            }
            player->handleKeyboardEvent(event);

        default: break;
    }
}

/**
 * @brief Handle a mouse event.
*/
void Game::handleMouseEvent(const SDL_Event& event) {
    SDL_GetMouseState(&globals::mouseState.x, &globals::mouseState.y);
    switch (globals::state) {
        case GameState::kMenu:
            menuInterface->handleMouseEvent(event);
            break;

        case GameState::kIngamePlaying:
            if (event.type != SDL_MOUSEBUTTONDOWN) break;
            onLevelChange(); onWindowChange();
            break;

        default: break;
    }
}