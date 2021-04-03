#include "Entity.hpp"

#include "AstrumVK/Textures/Texture2D.hpp"

void Entity::destroy()
{    
    vkDestroyBuffer(GPU::getDevice(), buffer, nullptr);
    vkFreeMemory(GPU::getDevice(), memory, nullptr);
    vkDestroyBuffer(GPU::getDevice(), indexBuffer, nullptr);
    vkFreeMemory(GPU::getDevice(), indexBufferMemory, nullptr);

    delete albedoColor;
}

Entity::Entity()
{

}

Entity::~Entity()
{

}