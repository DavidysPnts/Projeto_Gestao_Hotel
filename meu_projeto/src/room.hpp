#ifndef ROOM_HPP
#define ROOM_HPP

#include <iostream>

// Classe base abstrata que representa um quarto do hotel
class Room {
protected:
    int number_;
    float price_per_night_;
    bool occupied_;

public:
    Room(int number, float price_per_night)
        : number_(number), price_per_night_(price_per_night), occupied_(false) {
        std::cout << "Room(" << number_ << ") criado\n";
    }

    // Destrutor virtual obrigatório em classes base
    virtual ~Room() {
        std::cout << "~Room(" << number_ << ") destruido\n";
    }

    // Método virtual puro — cada tipo de quarto calcula o preço à sua maneira
    virtual float calculate_price(int nights) const = 0;

    // Método virtual não-puro — exibição padrão, pode ser sobrescrito
    virtual void display_info() const {
        std::cout << "Quarto " << number_
                  << " | R$ " << price_per_night_ << "/noite"
                  << " | " << (occupied_ ? "Ocupado" : "Disponivel") << "\n";
    }

    // Getters const
    int get_number() const { return number_; }
    float get_price_per_night() const { return price_per_night_; }
    bool is_occupied() const { return occupied_; }
    void set_occupied(bool status) { occupied_ = status; }
};

#endif