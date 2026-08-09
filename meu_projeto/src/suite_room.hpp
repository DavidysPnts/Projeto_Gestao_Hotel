#ifndef SUITE_ROOM_HPP
#define SUITE_ROOM_HPP

#include "room.hpp"

// Suíte — preço por diária com taxa de serviço inclusa
class SuiteRoom final : public Room {
private:
    float service_fee_;

public:
    SuiteRoom(int number, float price_per_night, float service_fee)
        : Room(number, price_per_night), service_fee_(service_fee) {
        std::cout << "SuiteRoom(" << number_ << ") criado\n";
    }

    ~SuiteRoom() {
        std::cout << "~SuiteRoom(" << number_ << ") destruido\n";
    }

    // Calcula o preço total: diárias * preço por noite + taxa de serviço
    float calculate_price(int nights) const override final {
        return (price_per_night_ * nights) + service_fee_;
    }

    // Getter const - necessário para serialização
    float get_service_fee() const { return service_fee_;}

    // Sobrescreve display_info chamando a versão da base primeiro
    void display_info() const override {
        Room::display_info();
        std::cout << "  Tipo: Suite | Taxa de servico: R$ " << service_fee_ << "\n";
    }
};

#endif