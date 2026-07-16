#include <iostream>
#include <memory>
#include "hotel.hpp"
#include "guest.hpp"
#include "room.hpp"
#include "standard_room.hpp"
#include "suite_room.hpp"
#include "reservation.hpp"
#include "consumption_product.hpp"

int main() {
    // Criando o hotel
    Hotel hotel("Palace Hotel");

    // Criando quartos
    auto room1 = std::make_shared<StandardRoom>(101, 150.0);
    auto room2 = std::make_shared<SuiteRoom>(102, 200.0, 50.0);

    // Criando hóspedes
    auto guest1 = std::make_shared<Guest>("Davidys Pontes", "123.456.789-00");
    auto guest2 = std::make_shared<Guest>("Carlos Silva", "987.654.321-00");

    // Adicionando quartos e hóspedes ao hotel (agregação)
    hotel.add_room(room1);
    hotel.add_room(room2);
    hotel.add_guest(guest1);
    hotel.add_guest(guest2);

    hotel.display_status();
    std::cout << "\n";

    // Reservation é dona dos ConsumptionProducts
    // Ao sair do bloco, a reserva é destruída junto com seus produtos
    std::cout << "---- Demonstracao de Composicao ----\n";
    {
        Reservation reservation(1, *guest1, *room1, 3);
        reservation.add_product(ConsumptionProduct("Cafe da manha", 25.0));
        reservation.add_product(ConsumptionProduct("Servico de quarto", 45.0));
        reservation.display_summary();
        std::cout << "\n[saindo do bloco - reservation sera destruida]\n";
    }
    std::cout << "[fora do bloco]\n\n";

    // Room e Guest existem independentemente da Reservation
    std::cout << "---- Demonstracao de Agregacao ----\n";
    std::cout << "Quarto e hospede ainda existem apos destruicao da reserva:\n";
    room1->display_info();
    guest1->display_info();
    std::cout << "\n";

    std::cout << "---- Demonstracao de Destrutor Virtual ----\n";
    {
        Room* room = new SuiteRoom(201, 400.0, 80.0);
        room->display_info();
        delete room; // vai chamar ~SuiteRoom antes de ~Room
    }
    std::cout << "\n";

    return 0;
}