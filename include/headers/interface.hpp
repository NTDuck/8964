#ifndef INTERFACE_H
#define INTERFACE_H

#include <SDL.h>

#include <components.hpp>
#include <entities.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


/* Abstract templates */

template <typename T>
class AbstractInterface : public Singleton<T> {
    public:
        INCL_SINGLETON(T)

        virtual void render() const;
        virtual void onWindowChange();

    protected:
        ~AbstractInterface();

        /**
         * @brief A temporary storage that is rendered every frame. Used to prevent multiple unnecessary calls of `SDL_RenderCopy()`.
         * @note Needs optimization to perfect relative positions of props to entities.
        */
        SDL_Texture* mTexture = nullptr;

        SDL_Point mTextureSize;
};

#define INCL_ABSTRACT_INTERFACE(T) using AbstractInterface<T>::render, AbstractInterface<T>::onWindowChange, AbstractInterface<T>::mTexture, AbstractInterface<T>::mTextureSize;


/* Derived implementations */

/**
 * @brief Handle the in-game map.
*/
class IngameMapHandler final : public AbstractInterface<IngameMapHandler> {
    public:
        INCL_ABSTRACT_INTERFACE(IngameMapHandler)

        IngameMapHandler(const level::Name levelName);
        ~IngameMapHandler();

        static void initialize();
        
        void render() const override;
        void onLevelChange();
        void onWindowChange() override;
        void handleKeyBoardEvent(SDL_Event const& event);

        inline level::Name getLevel() { return mLevelName; }
        void changeLevel(const level::Name levelName);

        bool isOnGrayscale = false;

    private:
        void loadLevel() const;

        void renderToTexture();
        void renderBackground() const;
        void renderLevelTilelayers() const;

        level::Name mLevelName;

        /**
         * @brief Maps a level's name with its corresponding relative file path.
         * @see <src/interface.cpp> Interface.loadLevel() (classmethod)
        */
        static level::Map sLevelMap;

        /**
         * A grayscaled version of `texture`.
        */
        SDL_Texture* mGrayscaleTexture = nullptr;
};


/**
 * @brief Handle in-game camera angles i.e. perspectives.
*/
class IngameViewHandler final : public AbstractInterface<IngameViewHandler> {      
    enum class View : bool {
        kTargetEntity,
        kFullScreen,
    };

    public:
        INCL_ABSTRACT_INTERFACE(IngameViewHandler)

        IngameViewHandler(std::function<void()> const& callable, SDL_Rect& targetedEntityDestRect);
        ~IngameViewHandler() = default;

        void render() const override;
        void onWindowChange() override;
        void onLevelChange();
        void handleKeyBoardEvent(SDL_Event const& event);

        void switchView();

    private:
        friend class IngameInterface;   // Provide access to private member `mTileCountWidth` and `mTileCountHeight`

        /**
         * A function with `void` return type that calls all `render()` method on all dependencies. Dependencies are other entities and the map, and not UI components, for example.
        */
        const std::function<void()> kRenderMethod;
        View mView = View::kTargetEntity;

        SDL_Point mTileDestSize;   // Not to be confused with `globals::tileDestSize`.
        double mTileCountWidth;
        static constexpr double mTileCountHeight = config::interface::tileCountHeight;

        SDL_Rect& mTargetedEntityDestRect;   // Read-only
        mutable SDL_Rect mViewport;   // Prevent warning: `note: the first difference of corresponding definitions is field 'mTargetedEntityDestRect'`
        SDL_Rect mDestRect;
};


/**
 * @brief Represent the in-game interface.
 * @note Governs both `GameState::kIngamePlaying` and `GameState::kIngameDialogue`.
*/
class IngameInterface final : public Singleton<IngameInterface> {
    public:
        INCL_SINGLETON(IngameInterface);

        IngameInterface();
        ~IngameInterface();

        static void initialize();
        static void deinitialize();

        void render() const;
        void onLevelChange() const;
        void onWindowChange() const;

        void handleKeyBoardEvent(SDL_Event const& event) const;
        void handleMouseEvent(SDL_Event const& event) const;

        void handleCustomEventPOST() const;
        void handleCustomEventGET(SDL_Event const& event) const;

        void handleDependencies() const;

    private:
        void handleEntitiesInteraction() const;
        void handleLevelSpecifics() const;
        void handleEntitiesSFX() const;

        void handleCustomEventGET_kResp_Teleport_GTE_Player(SDL_Event const& event) const;
        void handleCustomEventGET_kReq_DeathPending_Player() const;
        void handleCustomEventGET_kReq_DeathFinalized_Player() const;

        void handleLevelSpecifics_kLevelWhiteSpace() const;

        mutable SDL_Point* mCachedTargetDestCoords = nullptr;
};


/**
 * @brief Represent the menu interface.
*/
class MenuInterface final : public AbstractInterface<MenuInterface> {
    public:
        INCL_ABSTRACT_INTERFACE(MenuInterface)

        MenuInterface();
        ~MenuInterface() = default;

        static void initialize();
        static void deinitialize();

        void render() const override;
        void onWindowChange() override;
        void updateAnimation();

        void handleMouseEvent(SDL_Event const& event);

    private:
        void renderBackground() const;
        void renderComponents() const;
};


/**
 * @brief Represent the loading screen.
*/
class LoadingInterface final : public AbstractInterface<LoadingInterface> {
    public:
        INCL_ABSTRACT_INTERFACE(LoadingInterface)

        LoadingInterface();
        ~LoadingInterface() = default;

        inline static void initialize() {}
        static void deinitialize();

        void render() const override;
        void onWindowChange() override;

        void updateAnimation();
        void initiateTransition(GameState const& gameState);
        void handleTransition();

    private:
        void renderBackground() const;
        void renderComponents() const;

        static constexpr int kIdleFramesLimit = config::interface::idleFrames;
        int mCurrIdleFrames = 0;

        GameState mNextGameState;
};


/**
 * @brief Represent whatever shows up when the player should be defeated.
*/
class GameOverInterface final : public AbstractInterface<GameOverInterface> {
    public:
        INCL_ABSTRACT_INTERFACE(GameOverInterface)

        GameOverInterface();
        ~GameOverInterface() = default;
        
        static void initialize();
        static void deinitialize();

        void render() const override;
        void onWindowChange() override;

        void handleMouseEvent(SDL_Event const& event);

    private:
        void renderBackground() const;
        void renderComponents() const;
};



#endif