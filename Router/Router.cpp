#include <stack>
#include <queue>
#include <print>
#include <fstream>
#include <generator>
#include <boost/dynamic_bitset.hpp>
#include "Router.h"

unsigned Router::get_index( const Coordinates& coordinates ) const {
    return row * ( coordinates[2] * column + coordinates[1] ) + coordinates[0];
}

bool Router::check_cell( const Cell& C ) const {
    const unsigned index = get_index( C.coordinates );
    return grid[index].first > 0 and not reached.test( index );
}

std::generator<Router::Cell> Router::unreached_neighbours( Cell C ) const {
    if ( C.coordinates[0]-- and check_cell( C ) ) {
        C.pred = E;
        co_yield C;
    }

    if ( ++ ++C.coordinates[0] < row and check_cell( C ) ) {
        C.pred = W;
        co_yield C;
    }

    --C.coordinates[0];

    if ( C.coordinates[1]-- and check_cell( C ) ) {
        C.pred = N;
        co_yield C;
    }

    if ( ++ ++C.coordinates[1] < column and check_cell( C ) ) {
        C.pred = S;
        co_yield C;
    }

    --C.coordinates[1];
    C.coordinates[2] ^= 1;

    if ( check_cell( C ) ) {
        C.pred = C.coordinates[2] ? D : U;
        C.path_cost += ViaPenalty;
        co_yield C;
    }
}

void Router::backtrace( const Coordinates& source, Coordinates& target ) {
    std::stack<Coordinates> trace;

    while ( target != source ) {
        auto& [cost, pred] = grid[get_index( target )];
        cost = -1;
        trace.push( target );

        switch( pred ) {
            case U:
            case D:
                trace.push( {target[0], target[1], 2} );
                target[2] ^= 1;
                break;
            case N:
                ++target[1];
                break;
            case S:
                --target[1];
                break;
            case E:
                ++target[0];
                break;
            case W:
                --target[0];
        }
    }

    grid[get_index( source )].first = -1;
    trace.push( source );

    for ( ; not trace.empty(); trace.pop() ) {
        auto [Xcoord, Ycoord, LayerInfo] = trace.top();
        std::println( "{} {} {}", ++LayerInfo, Xcoord, Ycoord );
    }
}

void Router::MazeRouting( const Coordinates& source, Coordinates& target ) {
    int& source_cost = grid[get_index( source )].first;
    int& target_cost = grid[get_index( target )].first;
    source_cost = abs( source_cost );
    target_cost = abs( target_cost );

    std::priority_queue wavefront{[]( const Cell& a, const Cell& b ) {
        return a.path_cost > b.path_cost;
    }, std::vector<Cell>{{source, source_cost, D}}};

    for ( ; not wavefront.empty(); wavefront.pop() ) {
        const Cell C = wavefront.top();

        if ( C.coordinates == target ) {
            backtrace( source, target );
            break;
        }

        for ( Cell nb : unreached_neighbours( C ) ) {
            const unsigned index = get_index( nb.coordinates );
            auto& [cost, pred] = grid[index];

            if ((( nb.pred==N or nb.pred==S ) and ( C.pred==E or C.pred==W )) or
                (( nb.pred==E or nb.pred==W ) and ( C.pred==N or C.pred==S )))
                nb.path_cost += BendPenalty;

            nb.path_cost += cost;
            reached.set( index );
            pred = nb.pred;
            wavefront.push( nb );
        }
    }

    reached.reset();
}

int main( const int argc, const char *argv[] ) {
    unsigned X_gridsize, Y_gridsize; 
    int BendPenalty, ViaPenalty, NetNumber;
    if ( argc != 2 ) return 1;
    const std::string file( argv[1] );
    std::ifstream grid_file( file + ".grid" ), netlist( file + ".nl" );
    grid_file >> X_gridsize >> Y_gridsize >> BendPenalty >> ViaPenalty;
    Router router( X_gridsize, Y_gridsize, BendPenalty, ViaPenalty );
    for ( auto& [cost, pred] : router.grid ) grid_file >> cost;
    netlist >> NetNumber;
    std::println( "{}", NetNumber );

    for ( int i = 0; i < NetNumber; ++i ) {
        int NetID;
        Coordinates source, target;

        netlist >> NetID >> source[2] >> source[0] >> source[1]
                         >> target[2] >> target[0] >> target[1];

        --source[2];
        --target[2];
        std::println( "{}", NetID );
        router.MazeRouting( source, target );
        std::println( "0" );
    }
}
