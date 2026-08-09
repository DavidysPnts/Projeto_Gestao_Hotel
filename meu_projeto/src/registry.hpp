#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <vector>
#include <algorithm>
#include <cstddef>
#include <stdexcept>

template <typename T>
class Registry {
private:
    std::vector<T> items_;

public:
    Registry() = default;

    // Adiciona um item ao registro (move quando possível)
    void add(T item) {
        items_.push_back(std::move(item));
    }

    // Acesso com verificação de limites
    const T& at(std::size_t index) const {
        return items_.at(index);
    }

    std::size_t size() const {
        return items_.size();
    }

    bool empty() const {
        return items_.empty();
    }

    // Remove todos os itens que satisfazem o predicado, retorna quantos foram removidos
    template <typename Predicate>
    std::size_t remove_if(Predicate pred) {
        auto original_size = items_.size();
        items_.erase(std::remove_if(items_.begin(), items_.end(), pred), items_.end());
        return original_size - items_.size();
    }

    // Iteradores para permitir uso em range-based for e com <algorithm>/<ranges>
    auto begin() { return items_.begin(); }
    auto end() { return items_.end(); }
    auto begin() const { return items_.begin(); }
    auto end() const { return items_.end(); }
};

#endif