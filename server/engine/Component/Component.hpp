/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Component.hpp
*/

#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>
#include <typeindex>

namespace engine {

class Component {
   public:
    virtual ~Component() = default;

    /**
     * @brief Get the type index of this component
     * @return std::type_index representing the component type
     */
    virtual std::type_index getType() const = 0;

    /**
     * @brief Clone the component
     * @return A unique pointer to a new copy of this component
     */
    virtual std::unique_ptr<Component> clone() const = 0;
};

/**
 * @brief Template helper to get a unique type ID for each component type
 * @tparam T The component type
 */
template <typename T>
class ComponentBase : public Component {
   public:
    std::type_index getType() const override {
        return std::type_index(typeid(T));
    }

    std::unique_ptr<Component> clone() const override {
        return std::make_unique<T>(static_cast<const T &>(*this));
    }

    /**
     * @brief Get the static type index for this component type
     * @return std::type_index representing the component type
     */
    static std::type_index getStaticType() {
        return std::type_index(typeid(T));
    }
};

}  // namespace engine
