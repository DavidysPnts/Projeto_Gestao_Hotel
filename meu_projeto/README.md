# Sistema de Hotel

**Nome:** Davidys Cavalcante de Pontes  
**Matrícula:** 20250019035

## Descrição

Este projeto se resume a um sistema de gerenciamento de hotel. O hotel possui quartos que podem ser reservados por hóspedes, cada reserva pertence a um hóspede e a um quarto específico, e pode conter itens de consumo, como serviços adicionais. O sistema permite realizar reservas e calcular o valor total a pagar.

## Diagrama UML

```mermaid
classDiagram
    class Hotel {
        -string name_
        -vector~shared_ptr~Room~~ rooms_
        -vector~shared_ptr~Guest~~ guests_
        +Hotel(string)
        +~Hotel()
        +add_room(shared_ptr~Room~) void
        +add_guest(shared_ptr~Guest~) void
        +display_status() void
    }

    class ICalculable {
        <<interface>>
        +calculate_price(int nights) float*
        +display_price(int nights) void*
        +~ICalculable()
    }

    class Room {
        <<abstract>>
        #int number_
        #float price_per_night_
        #bool occupied_
        +Room(int, float)
        +~Room()*
        +calculate_price(int nights) float*
        +display_info() void
        +get_number() int
        +get_price_per_night() float
        +is_occupied() bool
        +set_occupied(bool) void
    }

    class StandardRoom {
        +StandardRoom(int, float)
        +~StandardRoom()
        +calculate_price(int nights) float
        +display_price(int nights) void
        +display_info() void
    }

    class SuiteRoom {
        -float service_fee_
        +SuiteRoom(int, float, float)
        +~SuiteRoom()
        +calculate_price(int nights) float
        +display_info() void
    }

    class Guest {
        -string name_
        -string cpf_
        +Guest(string, string)
        +~Guest()
        +get_name() string
        +get_cpf() string
    }

    class Reservation {
        -int id_
        -Guest& guest_
        -Room& room_
        -int nights_
        -vector~ConsumptionProduct~ products_
        +Reservation(int, Guest&, Room&, int)
        +~Reservation()
        +add_product(ConsumptionProduct) void
        +calculate_total() float
        +display_summary() void
    }

    class ConsumptionProduct {
        -string description_
        -float price_
        +ConsumptionProduct(string, float)
        +~ConsumptionProduct()
        +get_description() string
        +get_price() float
    }

    Room <|-- StandardRoom : herda
    Room <|-- SuiteRoom : herda
    ICalculable <|.. StandardRoom : implementa
    Hotel "1" o-- "0..*" Guest : agrega
    Hotel "1" o-- "1..*" Room : agrega
    Reservation "1" *-- "0..*" ConsumptionProduct : contém
    Reservation --> Guest : referencia
    Reservation --> Room : referencia
```
## Herança Avançada

A classe `SuiteRoom` foi marcada como `final` (`class SuiteRoom final : public Room`) para garantir a integridade do design, impedindo que outras classes especializem o comportamento de uma Suite, conforme exigido pelo roteiro do trabalho. Além disso, o método `calculate_price` em `SuiteRoom` também utiliza o qualificador `final`, bloqueando a sobrescrita da lógica de cálculo da taxa de serviço em classes derivadas futuras, garantindo o comportamento esperado e seguro das regras de negócio do sistema.

## Smart Pointers

- `shared_ptr<Room>` no Hotel: quarto é agregado — pode ser compartilhado e existe independentemente do hotel.
- `shared_ptr<Guest>` no Hotel: hóspede é agregado — pode ser compartilhado e existe independentemente do hotel.
- `Guest&` e `Room&` na Reservation: a reserva apenas referencia o hóspede e o quarto, sem posse — referência é suficiente.
- `vector<ConsumptionProduct>` na Reservation: composição por valor — os produtos pertencem à reserva e são destruídos com ela, sem necessidade de ponteiro.

## Como Compilar e Executar

```bash
cmake -B build
cmake --build build
./build/hotel_system
```
