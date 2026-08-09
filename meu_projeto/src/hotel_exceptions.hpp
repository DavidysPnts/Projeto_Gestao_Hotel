#ifndef HOTEL_EXCEPTIONS_HPP
#define HOTEL_EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

// Exceção base do domínio: toda exceção específica do hotel herda daqui,
// permitindo captura genérica via catch (const HotelError&).
class HotelError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Lançada quando se tenta reservar um quarto que já está ocupado
class RoomNotAvailableError : public HotelError {
public:
    explicit RoomNotAvailableError(int room_number)
        : HotelError("Quarto " + std::to_string(room_number) + " nao esta disponivel para reserva") {}
};

// Lançada quando um hóspede referenciado não existe no cadastro do hotel
class GuestNotFoundError : public HotelError {
public:
    explicit GuestNotFoundError(const std::string& cpf)
        : HotelError("Hospede com CPF " + cpf + " nao encontrado") {}
};

// Lançada quando dados de uma reserva são inválidos (ex: número de noites <= 0)
class InvalidReservationError : public HotelError {
public:
    explicit InvalidReservationError(const std::string& reason)
        : HotelError("Reserva invalida: " + reason) {}
};

#endif