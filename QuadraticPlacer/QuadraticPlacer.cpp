#include <vector>
#include <numeric>
#include <print>
#include "QuadraticPlacer.h"
#include "solver.h"

pair<double, double> QuadraticPlacer::Parameters::get_new_boundary(
    const pair<double, double>& coord
) const {
    return vertical ? make_pair(midpoint(low.first, high.first), coord.second) :
                      make_pair(coord.first, midpoint(low.second, high.second));
}

pair<double, double> QuadraticPlacer::Parameters::push_to_boundary(
    const pair<double, double>& coordinates
) const {
    return make_pair( clamp( coordinates.first, low.first, high.first ),
                      clamp( coordinates.second, low.second, high.second ) );
}

void QuadraticPlacer::quadratic_placement(
    const Parameters& params, const bool lower
) {
    vector<int> row, column;
    vector<double> data;
    coo_matrix A;
    A.n = params.end - params.begin;
    valarray<double> bx( A.n ), by( A.n ), x( A.n );

    for ( int i = 0; i < A.n; ++i ) {
        const int index = i + params.begin;
        const vector<unsigned>& nets = Gates[index].Nets;

        for ( const unsigned& net_index : nets ) {
            const vector<int>& net = Nets[net_index];
            const double weight = 1 / static_cast<double>( net.size() - 1 );

            for ( const int& gate : net ) {
                double xi, yi;

                if (gate < 0) tie(xi,yi) = params.push_to_boundary(Pins[~gate]);
                else if ( gate >= params.begin and gate < params.end ) {
                    if ( gate != index ) {
                        row.push_back( i );
                        column.push_back( gate - params.begin );
                        data.push_back( -weight );
                    }

                    continue;
                } else {
                    const auto& [xLow, yLow] = params.low;
                    const auto& [xHigh, yHigh] = params.high;
                    tie(xi,yi)=params.push_to_boundary(Gates[gate].coordinates);

                    if (xi > xLow and xi < xHigh and yi > yLow and yi < yHigh) {
                        if ( params.vertical ) xi = lower ? xHigh : xLow;
                        else yi = lower ? yHigh : yLow;
                    }
                }

                bx[i] += weight * xi;
                by[i] += weight * yi;
            }
        }

        row.push_back( i );
        column.push_back( i );
        data.push_back( static_cast<double>( nets.size() ) );
    }

    A.nnz = static_cast<int>( row.size() );
    A.row = valarray<int>( row.data(), A.nnz );
    A.col = valarray<int>( column.data(), A.nnz );
    A.dat = valarray<double>( data.data(), A.nnz );
    A.solve( bx, x );
    for (int i = 0; i < A.n; ++i) Gates[i+params.begin].coordinates.first=x[i];
    A.solve( by, x );
    for (int i = 0; i < A.n; ++i) Gates[i+params.begin].coordinates.second=x[i];
}

QuadraticPlacer::Parameters QuadraticPlacer::partition( Parameters& params ) {
    const int middle = midpoint( params.begin, params.end );
    const vector<Gate>::iterator begin = Gates.begin();
    params.vertical ^= true;
    Parameters params2( params );

    ranges::nth_element(
        begin + params.begin,
        begin + middle,
        begin + params.end,
        [&params](const pair<double, double>& a,const pair<double, double>& b) {
            return params.vertical ? a < b :
                tie( a.second, a.first ) < tie( b.second, b.first );
        },
        &Gate::coordinates
    );

    for ( int i = params.begin; i < params.end; ++i ) {
        Gate& gate = Gates[i];
        for(const unsigned& Net:gate.Nets)*ranges::find(Nets[Net],gate.index)=i;
        gate.index = i;
    }

    params2.begin = middle;
    params2.low = params.get_new_boundary( params.low );
    params.high = params.get_new_boundary( params.high );
    params.end = middle;
    quadratic_placement( params );
    quadratic_placement( params2, false );
    return params2;
}

void QuadraticPlacer::recursiveQP( Parameters& params, int depth ) {
    if ( depth-- ) {
        Parameters params2 = partition( params );
        Parameters params3 = partition( params );
        Parameters params4 = partition( params2 );
        recursiveQP( params, depth );
        recursiveQP( params2, depth );
        recursiveQP( params3, depth );
        recursiveQP( params4, depth );
    }
}

QuadraticPlacer::QuadraticPlacer( ifstream file ) {
    unsigned NumberofGates, NumberofNets, NumberofPads;
    file >> NumberofGates >> NumberofNets;
    Parameters params( NumberofGates );
    Gates.resize( NumberofGates );
    Nets.resize( NumberofNets );

    for ( Gate& gate : Gates ) {
        unsigned NumNetsConnected;
        file >> gate.GateID >> NumNetsConnected;
        gate.index = gate.GateID - 1;
        gate.Nets.resize( NumNetsConnected );

        for ( unsigned& Net : gate.Nets ) {
            file >> Net;
            Nets[--Net].push_back( gate.index );
        }
    }

    file >> NumberofPads;
    Pins.resize( NumberofPads );

    for ( pair<int, int>& Pin : Pins ) {
        int PinID;
        unsigned NetNumberConnectedTo;
        file >> PinID >> NetNumberConnectedTo >> Pin.first >> Pin.second;
        Nets[--NetNumberConnectedTo].push_back( -PinID );
    }

    quadratic_placement( params );
    recursiveQP( params );
    ranges::sort( Gates, {}, &Gate::GateID );

    for ( const Gate& gate : Gates )
        println(
            "{} {:.8f} {:.8f}",
            gate.GateID,
            gate.coordinates.first,
            gate.coordinates.second
        );
}

int main( const int argc, const char *argv[] ) {
    if ( argc != 2 ) return 1;
    QuadraticPlacer quadratic_placer( ifstream{argv[1]} );
}
