#ifndef STANDARD_ROOM_HPP
#define STANDARD_ROOM_HPP

#include "room.hpp"
#include "icalculable.hpp"

// Quarto padrão — preço simples por diária
class StandardRoom : public Room, public ICalculable {
public:
    StandardRoom(int number, float price_per_night)
        : Room(number, price_per_night) {
        std::cout << "StandardRoom(" << number_ << ") criado\n";
    }

    ~StandardRoom() {
        std::cout << "~StandardRoom(" << number_ << ") destruido\n";
    }

    // Calcula o preço total: diárias * preço por noite
    float calculate_price(int nights) const override {
        return price_per_night_ * nights;
    }

    void display_price(int nights) const override {
        std::cout << " Preco Standard (" << nights << " noites): R$ " << calculate_price(nights) << "\n";
    }

    // Sobrescreve display_info chamando a versão da base primeiro
    void display_info() const override {
        Room::display_info();
        std::cout << "  Tipo: Standard\n";
    }
};

#endif