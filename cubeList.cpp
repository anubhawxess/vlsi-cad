#include <print>
#include "cubeList.h"

template<typename Func>
void cubeList::get_set_bits(const boost::dynamic_bitset<>& cube, Func f) const {
    for ( size_t i = cube.find_first(); i < num_bits; i = cube.find_next( i ) )
        f( i, i & 1 );
}

void cubeList::print_cubes() const {
    std::println( "{}", num_bits >> 1 );
    std::println( "{}", cube_list.size() );

    for ( const boost::dynamic_bitset<>& cube : cube_list ) {
        std::print( "{}", cube.count() );

        get_set_bits( cube, []( const size_t bit, const bool complement ) {
            const long variable = ( bit >> 1 ) + 1;
            std::print( " {}", complement ? -variable : variable );
        } );

        std::println();
    }
}

bool cubeList::complement() {
    if ( cube_list.empty() ) {
        cube_list.push_back( boost::dynamic_bitset<>( num_bits ) );
        return true;
    } else if ( std::ranges::any_of(
        cube_list, [](const boost::dynamic_bitset<>& cube) {return cube.none();}
    ) ) {
        cube_list.clear();
        return true;
    } else if ( cube_list.size() == 1 ) {
        get_set_bits(cube_list.front(),[this](size_t bit,const bool complement){
            cube_list.push_back( boost::dynamic_bitset<>(
                num_bits, 1 << ( complement ? --bit : ++bit )
            ) );
        } );

        cube_list.pop_front();
        return true;
    }

    return false;
}

long cubeList::get_variable_to_split() const {
    std::vector<std::pair<int, int>> variables( num_bits >> 1 );

    for ( const boost::dynamic_bitset<>& cube : cube_list )
        get_set_bits( cube, [&variables]( size_t bit, const bool complement ) {
            bit >>= 1;
            complement ? ++variables[bit].second : ++variables[bit].first;
        } );

    return 1 + distance( variables.begin(), std::ranges::max_element(
        variables, [](const std::pair<int,int>& a,const std::pair<int,int>& b) {
            const int sumA = a.first + a.second;
            const int sumB = b.first + b.second;

            if ( b.first and b.second ) {
                if ( a.first and a.second ) {
                    const int diffA = abs( a.first - a.second );
                    const int diffB = abs( b.first - b.second );
                    return std::tie( sumA, diffB ) < std::tie( sumB, diffA );
                } else return true;
            } else if ( a.first and a.second ) return false;

            return sumA < sumB;
        }
    ) );
}