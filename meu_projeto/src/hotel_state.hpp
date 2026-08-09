#ifndef HOTEL_STATE_HPP
#define HOTEL_STATE_HPP

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "room.hpp"
#include "standard_room.hpp"
#include "suite_room.hpp"
#include "guest.hpp"

using json = nlohmann::json;

// Snapshot serializável do estado do hotel: nome, quartos e hóspedes.
struct HotelState {
    std::string hotel_name;
    std::vector<std::shared_ptr<Room>> rooms;
    std::vector<std::shared_ptr<Guest>> guests;

    bool operator==(const HotelState& other) const {
        if (hotel_name != other.hotel_name) return false;
        if (rooms.size() != other.rooms.size()) return false;
        if (guests.size() != other.guests.size()) return false;
        for (std::size_t i = 0; i < rooms.size(); ++i) {
            if (rooms[i]->get_number() != other.rooms[i]->get_number()) return false;
            if (rooms[i]->get_price_per_night() != other.rooms[i]->get_price_per_night()) return false;
        }
        for (std::size_t i = 0; i < guests.size(); ++i) {
            if (guests[i]->get_cpf() != other.guests[i]->get_cpf()) return false;
        }
        return true;
    }
};

// to_json não-intrusivo para um quarto polimórfico: inclui campo
// "type" para permitir recriar o tipo concreto correto na desserialização.
inline void to_json(json& j, const std::shared_ptr<Room>& room) {
    if (auto suite = std::dynamic_pointer_cast<SuiteRoom>(room)) {
        j = json{
            {"type", "suite"},
            {"number", suite->get_number()},
            {"price_per_night", suite->get_price_per_night()},
            {"occupied", suite->is_occupied()},
            {"service_fee", suite->get_service_fee()}
        };
    } else {
        j = json{
            {"type", "standard"},
            {"number", room->get_number()},
            {"price_per_night", room->get_price_per_night()},
            {"occupied", room->is_occupied()}
        };
    }
}

// Reconstrói o tipo concreto correto a partir do campo "type" (polimorfismo preservado na desserialização).
inline std::shared_ptr<Room> room_from_json(const json& j) {
    int number = j.at("number").get<int>();
    float price = j.at("price_per_night").get<float>();
    bool occupied = j.at("occupied").get<bool>();
    std::string type = j.at("type").get<std::string>();

    std::shared_ptr<Room> room;
    if (type == "suite") {
        room = std::make_shared<SuiteRoom>(number, price, j.at("service_fee").get<float>());
    } else if (type == "standard") {
        room = std::make_shared<StandardRoom>(number, price);
    } else {
        throw std::runtime_error("Tipo de quarto desconhecido na desserializacao: " + type);
    }
    room->set_occupied(occupied);
    return room;
}

// to_json para Guest (não-intrusivo)
inline void to_json(json& j, const std::shared_ptr<Guest>& guest) {
    j = json{{"name", guest->get_name()}, {"cpf", guest->get_cpf()}};
}

// Guest não tem construtor default (por design — todo hóspede precisa de nome
// e CPF válidos), então a desserialização usa uma função fábrica em vez do
// from_json(json&, Guest&) padrão, que exigiria um objeto já construído.
inline std::shared_ptr<Guest> guest_from_json(const json& j) {
    return std::make_shared<Guest>(j.at("name").get<std::string>(), j.at("cpf").get<std::string>());
}

// to_json do estado completo, com campo "version" para evolução futura do formato
inline void to_json(json& j, const HotelState& state) {
    json rooms_json = json::array();
    for (const auto& room : state.rooms) {
        rooms_json.push_back(room);
    }
    json guests_json = json::array();
    for (const auto& guest : state.guests) {
        guests_json.push_back(guest);
    }
    j = json{
        {"version", 1},
        {"hotel_name", state.hotel_name},
        {"rooms", rooms_json},
        {"guests", guests_json}
    };
}

inline HotelState hotel_state_from_json(const json& j) {
    int version = j.at("version").get<int>();
    if (version != 1) {
        throw std::runtime_error("Versao de formato nao suportada: " + std::to_string(version));
    }

    HotelState state;
    state.hotel_name = j.at("hotel_name").get<std::string>();
    for (const auto& room_json : j.at("rooms")) {
        state.rooms.push_back(room_from_json(room_json));
    }
    for (const auto& guest_json : j.at("guests")) {
        state.guests.push_back(guest_from_json(guest_json));
    }
    return state;
}

#endif