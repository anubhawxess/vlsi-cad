#include <fstream>
#include <cstring>
#include <map>
#include "cubeList.h"

size_t get_variable_index( const long variable ) {
    return ( ( std::abs( variable ) - 1 ) << 1 ) + ( variable < 0 );
}

cubeList AND( const long variable, cubeList& cubelist ) {
    for ( boost::dynamic_bitset<>& cube : cubelist.cube_list )
        cube.set( get_variable_index( variable ) );
    return std::move( cubelist );
}

cubeList OR( cubeList P, cubeList N ) {
    P.cube_list.splice( P.cube_list.end(), N.cube_list );
    return P;
}

cubeList positiveCofactor( cubeList F, const long x ) {
    erase_if( F.cube_list, [&x]( const boost::dynamic_bitset<>& cube ) {
        return cube.test( get_variable_index( -x ) );
    } );

    for ( boost::dynamic_bitset<>& cube : F.cube_list )
        cube.reset( get_variable_index( x ) );

    return F;
}

cubeList negativeCofactor( cubeList& F, const long x ) {
    return positiveCofactor( F, -x );
}

cubeList Complement( cubeList F ) {
    // check if F is simple enough to complement it directly and quit
    if ( F.complement() )
        return F;
    else {
        // do recursion
        const long x = F.get_variable_to_split();
        cubeList P = Complement( positiveCofactor( F, x ) );
        cubeList N = Complement( negativeCofactor( F, x ) );
        P = AND( x, P );
        N = AND( -x, N );
        return OR( P, N );
    }  // end recursion
} // end function

cubeList read_cubes( std::ifstream&& file ) {
    unsigned num_vars, num_cubes;
    file >> num_vars >> num_cubes;
    cubeList cubes( num_vars, num_cubes );

    for ( boost::dynamic_bitset<>& cube : cubes.cube_list ) {
        int num;
        file >> num;

        for ( int j = 0; j < num; ++j ) {
            int variable;
            file >> variable;
            cube.set( get_variable_index( variable ) );
        }
    }

    return cubes;
}

int main( const int argc, const char *argv[] ) {
    constexpr std::string_view EXT = ".pcn";
    std::map<int, cubeList> func_map;
    if ( argc != 2 ) return 1;
    std::ifstream file( argv[1] );

    if ( not strcmp( argv[1] + strlen(argv[1]) - EXT.length(), EXT.data() ) ) {
        cubeList cubes = read_cubes( std::move( file ) );
        cubes = Complement( cubes );
        cubes.print_cubes();
        return 0;
    }

    while ( true ) {
        int func1, func2, func3;
        char cmd;
        file >> cmd;

        switch ( cmd ) {
            case 'r':
                file >> func1;
                func_map.insert( std::make_pair( func1, read_cubes(
                    std::ifstream( std::to_string( func1 ) + EXT.data() )
                ) ) );
                break;
            case '!':
                file >> func1 >> func2;
                func_map.insert_or_assign(func1,Complement(func_map.at(func2)));
                break;
            case '+':
                file >> func1 >> func2 >> func3;
                func_map.insert_or_assign( func1, OR(
                    func_map.at( func2 ), func_map.at( func3 )
                ) );
                break;
            case '&':
                file >> func1 >> func2 >> func3;
                func_map.insert_or_assign( func1, Complement( OR(
                    Complement( func_map.at( func2 ) ),
                    Complement( func_map.at( func3 ) )
                ) ) );
                break;
            case 'p':
                file >> func1;
                func_map.at( func1 ).print_cubes();
                break;
            case 'q':
                return 0;
            default:
                return 1;
        }
    }
}
