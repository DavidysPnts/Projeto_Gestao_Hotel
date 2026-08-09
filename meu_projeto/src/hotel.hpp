#ifndef HOTEL_HPP
#define HOTEL_HPP

#include <iostream>
#include <vector>
#include <map>
#include <unordered_set>
#include <string>
#include <memory>
#include <optional>
#include <variant>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <future>
#include <mutex>
#include "room.hpp"
#include "guest.hpp"
#include "reservation.hpp"
#include "hotel_exceptions.hpp"

// Representa o hotel, que agrega quartos e hóspedes
class Hotel {
private:
    std::string name_;
    // Container 1: map ordenado por chave — indice de quartos por numero (busca O(log n) ordenada)
    std::map<int, std::shared_ptr<Room>> rooms_by_number_;
    std::vector<std::shared_ptr<Guest>> guests_;
    // Container 2: unordered_set — garante unicidade de CPF com acesso O(1) medio
    std::unordered_set<std::string> guest_cpfs_;

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
        rooms_by_number_[room->get_number()] = room;
    }

    // Adiciona um hóspede ao hotel (agregação). Lança InvalidReservationError
    // se o CPF já estiver cadastrado (violação de unicidade).
    void add_guest(std::shared_ptr<Guest> guest) {
        if (guest_cpfs_.count(guest->get_cpf()) > 0) {
            throw InvalidReservationError("CPF " + guest->get_cpf() + " ja cadastrado");
        }
        guest_cpfs_.insert(guest->get_cpf());
        guests_.push_back(guest);
    }

    // Busca de quarto que pode falhar — optional em vez de ponteiro nulo
    std::optional<std::shared_ptr<Room>> find_room(int number) const {
        auto it = rooms_by_number_.find(number);
        if (it == rooms_by_number_.end()) {
            return std::nullopt;
        }
        return it->second;
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

    // Cria uma reserva.
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

    // Algoritmos STL: retorna quartos ordenados por preço (sort + comparador/lambda)
    std::vector<std::shared_ptr<Room>> rooms_sorted_by_price() const {
        std::vector<std::shared_ptr<Room>> sorted;
        for (const auto& [number, room] : rooms_by_number_) {
            sorted.push_back(room);
        }
        std::sort(sorted.begin(), sorted.end(),
            [](const std::shared_ptr<Room>& a, const std::shared_ptr<Room>& b) {
                return a->get_price_per_night() < b->get_price_per_night();
            });
        return sorted;
    }

    // count_if: conta quartos ocupados
    std::size_t count_occupied_rooms() const {
        return std::count_if(rooms_by_number_.begin(), rooms_by_number_.end(),
            [](const auto& pair) { return pair.second->is_occupied(); });
    }

    // accumulate + lambda com captura: soma diárias de quartos acima de um limite
    float sum_prices_above(float threshold) const {
        return std::accumulate(rooms_by_number_.begin(), rooms_by_number_.end(), 0.0f,
            [threshold](float acc, const auto& pair) {
                return pair.second->get_price_per_night() > threshold
                    ? acc + pair.second->get_price_per_night()
                    : acc;
            });
    }

    // Pipeline de ranges com 2 adaptadores encadeados: filtra quartos disponíveis e transforma em seus números.
    auto available_room_numbers() const {
        namespace rv = std::ranges::views;
        return rooms_by_number_
            | rv::values
            | rv::filter([](const std::shared_ptr<Room>& r) { return !r->is_occupied(); })
            | rv::transform([](const std::shared_ptr<Room>& r) { return r->get_number(); });
    }

    // Paraleliza o cálculo de preço de todos os quartos
    float calculate_total_revenue_parallel(int nights) const {
        std::mutex mtx;
        float total = 0.0f;
        std::vector<std::future<void>> futures;

        for (const auto& [number, room] : rooms_by_number_) {
            futures.push_back(std::async(std::launch::async, [&mtx, &total, room, nights] {
                float price = room->calculate_price(nights); // independente por quarto
                std::lock_guard<std::mutex> lock(mtx);       // região crítica
                total += price;
            }));
        }

        for (auto& f : futures) {
            f.get(); // garante conclusão de todas as tasks
        }
        return total;
    }

    // Getter para uso em serialização/testes
    const std::map<int, std::shared_ptr<Room>>& rooms() const { return rooms_by_number_; }
    const std::vector<std::shared_ptr<Guest>>& guests() const { return guests_; }

    // Exibe o status geral do hotel
    void display_status() const {
        std::cout << "Hotel " << name_ << " \n";
        std::cout << "Quartos:\n";
        for (const auto& [number, room] : rooms_by_number_) {
            room->display_info();
        }
        std::cout << "Hospedes cadastrados:\n";
        for (const auto& g : guests_) {
            g->display_info();
        }
    }
};

#endif