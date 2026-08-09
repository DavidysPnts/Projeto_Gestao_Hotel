#ifndef HOTEL_SERVICE_HPP
#define HOTEL_SERVICE_HPP

#include "hotel.hpp"
#include "hotel_repository.hpp"

class HotelService {
private:
    HotelRepository& repository_;

public:
    explicit HotelService(HotelRepository& repository)
        : repository_(repository) {}

    // Salva o estado atual do hotel usando o repositório injetado
    void save_state(const Hotel& hotel) {
        HotelState state;
        state.hotel_name = hotel.get_name();
        for (const auto& [number, room] : hotel.rooms()) {
            state.rooms.push_back(room);
        }
        for (const auto& guest : hotel.guests()) {
            state.guests.push_back(guest);
        }
        repository_.save(state);
    }

    // Carrega o estado salvo e reconstrói um Hotel a partir dele
    Hotel load_state() {
        HotelState state = repository_.load();
        Hotel hotel(state.hotel_name);
        for (const auto& room : state.rooms) {
            hotel.add_room(room);
        }
        for (const auto& guest : state.guests) {
            hotel.add_guest(guest);
        }
        return hotel;
    }
};

#endif