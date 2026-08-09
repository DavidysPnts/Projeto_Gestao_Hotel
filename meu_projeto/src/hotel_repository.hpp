#ifndef HOTEL_REPOSITORY_HPP
#define HOTEL_REPOSITORY_HPP

#include <fstream>
#include <stdexcept>
#include "hotel_state.hpp"

// Abstração de infraestrutura (DIP): classes de alto nível dependem
// desta interface, nunca de uma implementação concreta de persistência.
class HotelRepository {
public:
    virtual void save(const HotelState& state) = 0;
    virtual HotelState load() = 0;
    virtual ~HotelRepository() = default;
};

// Implementação de produção: persiste em arquivo JSON no disco.
class JsonHotelRepository : public HotelRepository {
private:
    std::string file_path_;

public:
    explicit JsonHotelRepository(std::string file_path)
        : file_path_(std::move(file_path)) {}

    void save(const HotelState& state) override {
        json doc = state;
        std::ofstream out(file_path_);
        if (!out) {
            throw std::runtime_error("Nao foi possivel abrir arquivo para escrita: " + file_path_);
        }
        out << doc.dump(2);
    }

    HotelState load() override {
        std::ifstream in(file_path_);
        if (!in) {
            throw std::runtime_error("Nao foi possivel abrir arquivo para leitura: " + file_path_);
        }
        json doc;
        in >> doc;
        return hotel_state_from_json(doc);
    }
};

// Implementação de teste: guarda o estado em memória, sem tocar em disco ou rede.
class MemoryHotelRepository : public HotelRepository {
private:
    HotelState state_;
    bool has_state_ = false;

public:
    void save(const HotelState& state) override {
        state_ = state;
        has_state_ = true;
    }

    HotelState load() override {
        if (!has_state_) {
            throw std::runtime_error("Nenhum estado salvo em memoria ainda");
        }
        return state_;
    }
};

#endif