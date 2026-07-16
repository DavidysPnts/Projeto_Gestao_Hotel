#ifndef ICALCULABLE_HPP
#define ICALCULABLE_HPP

// Interface pura — contrato para classes que calculam preço
class ICalculable {
public:
    virtual float calculate_price(int nights) const = 0;
    virtual void display_price(int nights) const = 0;
    virtual ~ICalculable() = default;
};

#endif