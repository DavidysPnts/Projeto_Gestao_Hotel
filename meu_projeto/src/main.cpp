#include <iostream>
#include <memory>
#include <vector>
#include <variant>
#include "hotel.hpp"
#include "guest.hpp"
#include "room.hpp"
#include "standard_room.hpp"
#include "suite_room.hpp"
#include "reservation.hpp"
#include "consumption_product.hpp"
#include "hotel_exceptions.hpp"
#include "registry.hpp"
#include "concepts.hpp"
#include "hotel_repository.hpp"
#include "hotel_service.hpp"

// Função livre (TP2): quarto com maior preço por noite via ponteiro base
const Room* maior_preco(const std::vector<std::unique_ptr<Room>>& quartos) {
    const Room* maior = nullptr;
    for (const auto& q : quartos) {
        if (maior == nullptr || q->get_price_per_night() > maior->get_price_per_night()) {
            maior = q.get();
        }
    }
    return maior;
}

void demo_tp2_resumo() {
    std::cout << "\nRESUMO TP2 (heranca, polimorfismo, interface)\n";

    std::cout << "--- Destrutor virtual ---\n";
    {
        Room* room = new SuiteRoom(201, 400.0f, 80.0f);
        room->display_info();
        delete room; // ~SuiteRoom antes de ~Room
    }

    std::cout << "\n--- Polimorfismo dinamico (vector<unique_ptr<Room>>) ---\n";
    {
        std::vector<std::unique_ptr<Room>> quartos;
        quartos.push_back(std::make_unique<StandardRoom>(301, 120.0f));
        quartos.push_back(std::make_unique<SuiteRoom>(302, 500.0f, 100.0f));
        for (const auto& q : quartos) {
            q->display_info();
            std::cout << "  Preco (2 noites): R$ " << q->calculate_price(2) << "\n";
        }
        const Room* maior = maior_preco(quartos);
        std::cout << "Maior preco/noite: quarto " << maior->get_number() << "\n";
    }

    std::cout << "\n--- Interface pura ICalculable ---\n";
    StandardRoom room_iface(401, 200.0f);
    const ICalculable& calc = room_iface;
    calc.display_price(3);
}

void demo_q1_generica(Hotel& hotel) {
    std::cout << "\nTP3 - Q1: PROGRAMACAO GENERICA\n";

    std::cout << "--- (A) Template Registry<T> com 2 tipos diferentes ---\n";
    Registry<ConsumptionProduct> product_catalog;
    product_catalog.add(ConsumptionProduct("Cafe da manha", 25.0f));
    product_catalog.add(ConsumptionProduct("Servico de quarto", 45.0f));
    std::cout << "Registry<ConsumptionProduct> tem " << product_catalog.size() << " itens\n";

    Registry<std::string> amenities;
    amenities.add("Wi-Fi gratuito");
    amenities.add("Piscina");
    amenities.add("Estacionamento");
    std::cout << "Registry<std::string> tem " << amenities.size() << " itens\n";

    std::cout << "\n--- (B) CRTP: contagem estatica sem vtable ---\n";
    std::cout << "Quartos vivos (Room::alive()): " << Room::alive() << "\n";
    std::cout << "Hospedes vivos (Guest::alive()): " << Guest::alive() << "\n";
    {
        StandardRoom quarto_temporario(999, 90.0f);
        std::cout << "Apos criar quarto temporario: Room::alive() = " << Room::alive() << "\n";
    }
    std::cout << "Apos sair do escopo: Room::alive() = " << Room::alive() << "\n";

    std::cout << "\n--- (C)(D) Concept Calculable restringindo template ---\n";
    std::vector<StandardRoom> quartos_padrao;
    quartos_padrao.emplace_back(501, 100.0f);
    quartos_padrao.emplace_back(502, 150.0f);
    std::cout << "total_price(quartos_padrao, 2 noites) = R$ "
              << total_price(quartos_padrao, 2) << "\n";

    std::cout << "\n--- (E) Pipeline de ranges (filter + transform encadeados) ---\n";
    std::cout << "Numeros de quartos disponiveis: ";
    for (int number : hotel.available_room_numbers()) {
        std::cout << number << " ";
    }
    std::cout << "\n";
}

void demo_q2_erros(Hotel& hotel) {
    std::cout << "\n=========== TP3 - Q2: TRATAMENTO DE ERROS ===========\n";

    std::cout << "--- (A)(D) Excecao customizada capturada pela base ---\n";
    try {
        auto guest_duplicado = std::make_shared<Guest>("Duplicado", "123.456.789-00");
        hotel.add_guest(guest_duplicado); // CPF ja existe -> lanca InvalidReservationError
    } catch (const HotelError& e) {
        std::cout << "Capturado via HotelError (base): " << e.what() << "\n";
    }

    std::cout << "\n--- (B)(D) optional em busca que pode falhar ---\n";
    auto encontrado = hotel.find_room(101);
    if (encontrado.has_value()) {
        std::cout << "Quarto 101 encontrado: R$ " << encontrado.value()->get_price_per_night() << "/noite\n";
    }
    auto nao_encontrado = hotel.find_room(9999);
    if (nao_encontrado == std::nullopt) {
        std::cout << "Quarto 9999 nao encontrado (nullopt), como esperado\n";
    }

    std::cout << "\n--- (C)(D) variant tratado com std::visit ---\n";
    auto resultado = hotel.create_reservation(10, "123.456.789-00", 999999, 2);
    std::visit([](const auto& valor) {
        using T = std::decay_t<decltype(valor)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<Reservation>>) {
            std::cout << "Reserva criada com sucesso, id=" << valor->get_id() << "\n";
        } else {
            std::cout << "Falha ao reservar (variant string): " << valor << "\n";
        }
    }, resultado);

    std::cout << "\n--- GuestNotFoundError via excecao ---\n";
    try {
        hotel.create_reservation(11, "000.000.000-00", 101, 2);
    } catch (const HotelError& e) {
        std::cout << "Capturado: " << e.what() << "\n";
    }
}

void demo_q3_stl_concorrencia(const Hotel& hotel) {
    std::cout << "\nTP3 - Q3: STL E CONCORRENCIA\n";

    std::cout << "--- (A)(B) map + unordered_set + algoritmos STL ---\n";
    std::cout << "Quartos ordenados por preco (std::sort + lambda):\n";
    for (const auto& room : hotel.rooms_sorted_by_price()) {
        std::cout << "  Quarto " << room->get_number() << ": R$ " << room->get_price_per_night() << "\n";
    }

    std::cout << "Quartos ocupados (count_if): " << hotel.count_occupied_rooms() << "\n";
    std::cout << "Soma de diarias acima de R$150 (accumulate + lambda com captura): R$ "
              << hotel.sum_prices_above(150.0f) << "\n";

    std::cout << "\n--- (C)(D) Paralelizacao com std::async + mutex ---\n";
    float total_paralelo = hotel.calculate_total_revenue_parallel(3);
    std::cout << "Receita total para 3 noites (calculada em paralelo, thread-safe): R$ "
              << total_paralelo << "\n";
}

void demo_q4_serializacao_solid(const Hotel& hotel) {
    std::cout << "\nTP3 - Q4: SERIALIZACAO E SOLID (DIP)\n";

    std::cout << "--- Producao: JsonHotelRepository (grava em disco) ---\n";
    JsonHotelRepository json_repo("estado_hotel.json");
    HotelService service_producao(json_repo); // injecao de dependencia
    service_producao.save_state(hotel);
    Hotel hotel_carregado = service_producao.load_state();
    std::cout << "Estado recarregado do arquivo JSON:\n";
    hotel_carregado.display_status();

    std::cout << "\n--- Teste: MemoryHotelRepository (sem tocar disco) ---\n";
    MemoryHotelRepository mem_repo;
    HotelService service_teste(mem_repo); // mesma logica de alto nivel, outra dependencia
    service_teste.save_state(hotel);
    Hotel hotel_em_memoria = service_teste.load_state();
    std::cout << "Estado recarregado da memoria (nenhum arquivo tocado):\n";
    hotel_em_memoria.display_status();
}

int main() {
    Hotel hotel("Palace Hotel");

    auto room1 = std::make_shared<StandardRoom>(101, 150.0f);
    auto room2 = std::make_shared<SuiteRoom>(102, 200.0f, 50.0f);
    auto guest1 = std::make_shared<Guest>("Davidys Pontes", "123.456.789-00");
    auto guest2 = std::make_shared<Guest>("Carlos Silva", "987.654.321-00");

    hotel.add_room(room1);
    hotel.add_room(room2);
    hotel.add_guest(guest1);
    hotel.add_guest(guest2);

    hotel.display_status();

    demo_tp2_resumo();
    demo_q1_generica(hotel);
    demo_q2_erros(hotel);
    demo_q3_stl_concorrencia(hotel);
    demo_q4_serializacao_solid(hotel);

    std::cout << "\n=========== FIM ===========\n";
    return 0;
}