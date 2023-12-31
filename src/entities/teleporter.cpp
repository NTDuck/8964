#include <entities.hpp>

#include <filesystem>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


Teleporter::Teleporter(SDL_Point const& destCoords) : AbstractAnimatedEntity<Teleporter>(destCoords) {
    destRectModifier = config::teleporter::destRectModifier;
}

void Teleporter::onLevelChange(level::EntityLevelData const& teleporterData) {
    auto data = dynamic_cast<const level::TeleporterLevelData*>(&teleporterData);
    AbstractAnimatedEntity<Teleporter>::onLevelChange(*data);

    targetDestCoords = data->targetDestCoords;
    targetLevel = data->targetLevel;
}


template <>
const std::filesystem::path AbstractEntity<Teleporter>::kTilesetPath = config::teleporter::path;