#ifndef HOTEL_HPP
#define HOTEL_HPP

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <variant>
#include <algorithm>
#include "room.hpp"
#include "guest.hpp"
#include "reservation.hpp"
#include "hotel_exceptions.hpp"

// Representa o hotel, que agrega quartos e hóspedes
class Hotel {
private:
    std::string name_;
    std::vector<std::shared_ptr<Room>> rooms_;
    std::vector<std::shared_ptr<Guest>> guests_;

public:
    Hotel(std::string name)
        : name_(name) {
        std::cout << "Hotel(\"" << name_ << "\") criado\n";
    }

    ~Hotel() {
        std::cout << "~Hotel(\"" << name_ << "\") destruido\n";
    }

    // Getters const
    std::string get_name() const { return name_; }

    // Adiciona um quarto ao hotel (agregação)
    void add_room(std::shared_ptr<Room> room) {
        rooms_.push_back(room);
    }

    // Adiciona um hóspede ao hotel (agregação)
    void add_guest(std::shared_ptr<Guest> guest) {
        guests_.push_back(guest);
    }

    // Busca de quarto que pode falhar — optional em vez de ponteiro nulo
    std::optional<std::shared_ptr<Room>> find_room(int number) const {
        auto it = std::find_if(rooms_.begin(), rooms_.end(),
            [number](const std::shared_ptr<Room>& r) { return r->get_number() == number; });
        if (it == rooms_.end()) {
            return std::nullopt;
        }
        return *it;
    }

    // Busca de hóspede por CPF — também via optional
    std::optional<std::shared_ptr<Guest>> find_guest(const std::string& cpf) const {
        auto it = std::find_if(guests_.begin(), guests_.end(),
            [&cpf](const std::shared_ptr<Guest>& g) { return g->get_cpf() == cpf; });
        if (it == guests_.end()) {
            return std::nullopt;
        }
        return *it;
    }

    // Cria uma reserva
    std::variant<std::shared_ptr<Reservation>, std::string>
    create_reservation(int id, const std::string& guest_cpf, int room_number, int nights) {
        if (nights <= 0) {
            throw InvalidReservationError("numero de noites deve ser positivo");
        }

        auto guest_opt = find_guest(guest_cpf);
        if (!guest_opt.has_value()) {
            throw GuestNotFoundError(guest_cpf);
        }

        auto room_opt = find_room(room_number);
        if (!room_opt.has_value()) {
            return std::string("Quarto " + std::to_string(room_number) + " nao existe neste hotel");
        }
        if (room_opt.value()->is_occupied()) {
            return std::string("Quarto " + std::to_string(room_number) + " ja esta ocupado");
        }

        return std::make_shared<Reservation>(id, *guest_opt.value(), *room_opt.value(), nights);
    }

    // Exibe o status geral do hotel
    void display_status() const {
        std::cout << "Hotel " << name_ << " \n";
        std::cout << "Quartos:\n";
        for (const auto& r : rooms_) {
            r->display_info();
        }
        std::cout << "Hospedes cadastrados:\n";
        for (const auto& g : guests_) {
            g->display_info();
        }
    }
};

#endif