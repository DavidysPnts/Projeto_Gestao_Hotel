#ifndef GUEST_HPP
#define GUEST_HPP

#include <string>
#include <iostream>
#include "counted.hpp"

// Representa um hóspede do hotel
class Guest : public Counted<Guest> {
private:
    std::string name_;
    std::string cpf_;

public:
    Guest(std::string name, std::string cpf)
        : name_(name), cpf_(cpf) {
        std::cout << "Guest(\"" << name_ << "\") criado\n";
    }

    ~Guest() {
        std::cout << "~Guest(\"" << name_ << "\") destruido\n";
    }

    // Getters const
    std::string get_name() const { return name_; }
    std::string get_cpf() const { return cpf_; }

    // Exibe informações do hóspede
    void display_info() const {
        std::cout << "Hospede: " << name_ << " | CPF: " << cpf_ << "\n";
    }
};

#endif