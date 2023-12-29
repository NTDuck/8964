#include <components.hpp>

#include <cstring>
#include <functional>

#include <SDL.h>
#include <SDL_ttf.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename T>
TextArea<T>::TextArea(std::string const& content, SDL_FPoint const& center, TextAreaPreset const& preset) : content(content), kCenter(center), kPreset(preset) {}

template <typename T>
TextArea<T>::~TextArea() {
    SDL_DestroyTexture(outerTexture);
    SDL_DestroyTexture(innerTexture);
    outerTexture = innerTexture = nullptr;
}

template <typename T>
void TextArea<T>::initialize() {
    loadFont();
    // if (font == nullptr) TTF_GetError();
}

template <typename T>
void TextArea<T>::deinitialize() {
    Multiton<T>::deinitialize();
    TTF_CloseFont(font);
    font = nullptr;
}

/**
 * @brief Render the entire text area to the screen.
*/
template <typename T>
void TextArea<T>::render() const {
    SDL_RenderCopy(globals::renderer, outerTexture, nullptr, &outerDestRect);
    SDL_RenderCopy(globals::renderer, innerTexture, nullptr, &innerDestRect);
}

template <typename T>
void TextArea<T>::onWindowChange() {
    // Call `loadFont()` "once"
    static std::size_t counter = 0;
    if (!counter) loadFont();
    ++counter;
    if (counter == instances.size()) counter = 0;

    loadOuterTexture(outerTexture, kPreset);
    loadInnerTexture(innerTexture, kPreset);
}

template <typename T>
void TextArea<T>::editContent(std::string const& nextContent) {
    content = nextContent;
    loadInnerTexture(innerTexture, kPreset);
}

template <typename T>
void TextArea<T>::loadFont() {
    if (font != nullptr) TTF_CloseFont(font);
    destSize = std::min(globals::windowSize.x / kOuterDestRectRatio.x, globals::windowSize.y / kOuterDestRectRatio.y) >> 2;   // The closest power of 2
    destSize *= kDestSizeMultiplier;
    font = TTF_OpenFont(fontPath.c_str(), destSize);
}

/**
 * @brief Populate the "outer" part of the text area.
*/
template <typename T>
void TextArea<T>::loadOuterTexture(SDL_Texture*& texture, TextAreaPreset const& preset) {
    if (texture != nullptr) SDL_DestroyTexture(texture);
    
    outerDestRect.w = destSize * kOuterDestRectRatio.x;
    outerDestRect.h = destSize * kOuterDestRectRatio.y;
    outerDestRect.x = utils::castFloatToInt(globals::windowSize.x * kCenter.x - outerDestRect.w / 2);
    outerDestRect.y = utils::castFloatToInt(globals::windowSize.y * kCenter.y - outerDestRect.h / 2);

    texture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, 
    SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, outerDestRect.w, outerDestRect.h);

    SDL_SetRenderTarget(globals::renderer, texture);
    SDL_Rect arbitraryRect = outerDestRect;
    arbitraryRect.x = arbitraryRect.y = 0;

    auto modifyRect = [](SDL_Rect& rect, float ratio) {
        int delta = utils::castFloatToInt(std::min(rect.w, rect.h) / 2 * ratio);
        rect.x += delta;
        rect.y += delta;
        rect.w -= delta * 2;
        rect.h -= delta * 2;
    };

    auto fillRect = [&](float multiplier, SDL_Color const& color) {
        modifyRect(arbitraryRect, multiplier);
        utils::setRendererDrawColor(globals::renderer, color);
        SDL_RenderFillRect(globals::renderer, &arbitraryRect);
    };

    fillRect(0, preset.backgroundColor);
    fillRect(preset.lineOffset, preset.lineColor);
    fillRect(preset.lineWidth, preset.backgroundColor);

    SDL_SetRenderTarget(globals::renderer, nullptr);
}

/**
 * @brief Populate the "text" part of the text area.
*/
template <typename T>
void TextArea<T>::loadInnerTexture(SDL_Texture*& texture, TextAreaPreset const& preset) {
    if (texture != nullptr) SDL_DestroyTexture(texture);

    // Render text at high quality
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, content.c_str(), preset.textColor);
    if (surface == nullptr) return;

    texture = SDL_CreateTextureFromSurface(globals::renderer, surface);
    
    // Register `innerSurface` dimensions to `innerDestRect`
    innerDestRect.w = surface->w;
    innerDestRect.h = surface->h;
    innerDestRect.x = utils::castFloatToInt(globals::windowSize.x * kCenter.x - innerDestRect.w / 2);
    innerDestRect.y = utils::castFloatToInt(globals::windowSize.y * kCenter.y - innerDestRect.h / 2);

    SDL_FreeSurface(surface);
}

template <typename T>
std::size_t std::hash<TextArea<T>>::operator()(TextArea<T> const*& instance) const {
    return instance == nullptr ? std::hash<std::nullptr_t>{}(instance) : std::hash<SDL_FPoint>(instance->kCenter);
}

template <typename T>
bool std::equal_to<TextArea<T>>::operator()(TextArea<T> const*& first, TextArea<T> const*& second) const {
    return (first == nullptr && second == nullptr) || (first && second && first->kCenter == second->kCenter);
}


/**
 * Determine the physical size of the text area.
*/
template <typename T>
int TextArea<T>::destSize;

/**
 * The global font in the scope of this project.
*/
template <typename T>
TTF_Font* TextArea<T>::font = nullptr;


template class TextArea<Button>;
template class TextArea<Title>;