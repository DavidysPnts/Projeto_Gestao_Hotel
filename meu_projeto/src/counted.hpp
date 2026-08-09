#ifndef COUNTED_HPP
#define COUNTED_HPP

template <typename Derived>
class Counted {
private:
    static inline int count_ = 0;

protected:
    Counted() { ++count_; }
    Counted(const Counted&) { ++count_; }
    ~Counted() { --count_; }

public:
    static int alive() { return count_; }
};

#endif