#include <list>
#include <boost/dynamic_bitset.hpp>

class cubeList {
    unsigned num_bits;

    template<typename Func>
    void get_set_bits( const boost::dynamic_bitset<>& cube, Func f ) const;
public:
    std::list<boost::dynamic_bitset<>> cube_list;

    cubeList( const unsigned nvars, const unsigned ncubes ) :
        num_bits(nvars<<1),cube_list(ncubes,boost::dynamic_bitset<>(num_bits)){}

    void print_cubes() const;
    bool complement();
    long get_variable_to_split() const;
};