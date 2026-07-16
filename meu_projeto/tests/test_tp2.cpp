#include <catch2/catch_all.hpp>
#include <memory>
#include <vector>
#include "standard_room.hpp"
#include "suite_room.hpp"
#include "icalculable.hpp"

TEST_CASE("Destruicao em cadeia da hierarquia") {
    {
        StandardRoom room(101, 150.0);
        REQUIRE(room.get_number() == 101);
        REQUIRE(room.get_price_per_night() == Catch::Approx(150.0));
    }
    REQUIRE(true);
}

TEST_CASE("Polimorfismo dinamico via unique_ptr") {
    std::vector<std::unique_ptr<Room>> quartos;
    quartos.push_back(std::make_unique<StandardRoom>(101, 100.0));
    quartos.push_back(std::make_unique<SuiteRoom>(102, 200.0, 50.0));

    REQUIRE(quartos[0]->calculate_price(2) == Catch::Approx(200.0));
    REQUIRE(quartos[1]->calculate_price(2) == Catch::Approx(450.0));
    REQUIRE(quartos[0]->calculate_price(2) != quartos[1]->calculate_price(2));
}

TEST_CASE("Interface ICalculable por referencia") {
    StandardRoom room(301, 150.0);
    const ICalculable& calc = room;
    REQUIRE(calc.calculate_price(3) == Catch::Approx(450.0));
}