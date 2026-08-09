#include <catch2/catch_all.hpp>
#include <memory>
#include "hotel.hpp"
#include "standard_room.hpp"
#include "suite_room.hpp"
#include "guest.hpp"
#include "registry.hpp"
#include "concepts.hpp"
#include "hotel_exceptions.hpp"
#include "hotel_state.hpp"
#include "hotel_repository.hpp"
#include "hotel_service.hpp"

// (Q1) Template Registry<T> e concept Calculable
TEST_CASE("Registry<T> generico e concept Calculable") {
    Registry<int> numeros;
    numeros.add(1);
    numeros.add(2);
    REQUIRE(numeros.size() == 2);
    REQUIRE(numeros.at(0) == 1);

    Registry<std::string> nomes;
    nomes.add("a");
    REQUIRE(nomes.size() == 1);

    std::vector<StandardRoom> quartos;
    quartos.emplace_back(1, 100.0f);
    quartos.emplace_back(2, 200.0f);
    REQUIRE(total_price(quartos, 2) == Catch::Approx(600.0));
}

// (Q1-B) CRTP: contagem de instancias sem vtable
TEST_CASE("CRTP Counted rastreia instancias vivas") {
    int antes = Room::alive();
    {
        StandardRoom temp(555, 50.0f);
        REQUIRE(Room::alive() == antes + 1);
    }
    REQUIRE(Room::alive() == antes);
}

// (Q2) Excecao de dominio capturada pela base + optional nos dois casos
TEST_CASE("Excecao de dominio via HotelError (base)") {
    Hotel hotel("Teste");
    auto guest = std::make_shared<Guest>("Fulano", "111.111.111-11");
    hotel.add_guest(guest);

    auto duplicado = std::make_shared<Guest>("Outro", "111.111.111-11");
    REQUIRE_THROWS_AS(hotel.add_guest(duplicado), HotelError);
}

TEST_CASE("optional em busca de quarto: achou e nao achou") {
    Hotel hotel("Teste");
    auto room = std::make_shared<StandardRoom>(10, 100.0f);
    hotel.add_room(room);

    REQUIRE(hotel.find_room(10).has_value());
    REQUIRE_FALSE(hotel.find_room(9999).has_value());
}

TEST_CASE("variant no resultado de create_reservation") {
    Hotel hotel("Teste");
    auto room = std::make_shared<StandardRoom>(20, 100.0f);
    auto guest = std::make_shared<Guest>("Ciclana", "222.222.222-22");
    hotel.add_room(room);
    hotel.add_guest(guest);

    auto resultado = hotel.create_reservation(1, "222.222.222-22", 20, 2);
    REQUIRE(std::holds_alternative<std::shared_ptr<Reservation>>(resultado));

    // Quarto ja ocupado -> variant deve conter mensagem de erro (string)
    auto resultado2 = hotel.create_reservation(2, "222.222.222-22", 20, 1);
    REQUIRE(std::holds_alternative<std::string>(resultado2));
}

// (Q4) Serializacao round-trip + DIP com memory_repository (sem tocar disco)
TEST_CASE("Serializacao round-trip via MemoryHotelRepository") {
    Hotel hotel("Hotel de Teste");
    hotel.add_room(std::make_shared<StandardRoom>(1, 120.0f));
    hotel.add_room(std::make_shared<SuiteRoom>(2, 300.0f, 60.0f));
    hotel.add_guest(std::make_shared<Guest>("Beltrano", "333.333.333-33"));

    MemoryHotelRepository repo; // implementacao de teste, sem I/O
    HotelService service(repo); // logica de alto nivel via DIP

    service.save_state(hotel);
    Hotel carregado = service.load_state();

    REQUIRE(carregado.get_name() == hotel.get_name());
    REQUIRE(carregado.rooms().size() == hotel.rooms().size());
    REQUIRE(carregado.guests().size() == hotel.guests().size());
}