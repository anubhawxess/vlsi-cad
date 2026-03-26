#include <vector>
#include <print>
#include "QuadraticPlacer.h"
#include "solver.h"

pair<double, double> QuadraticPlacer::Parameters::get_new_boundary(
    const pair<double, double>& coordinates
) const {
    return vertical ? make_pair( coordinates.first, high.second / 2 ) :
                      make_pair( high.first / 2, coordinates.second );
}

QuadraticPlacer::Parameters QuadraticPlacer::Parameters::partition() {
    const int middle = ( begin + end ) >> 1;
    vertical ^= true;
    Parameters params( *this );
    params.first = false;
    params.begin = middle;
    params.low = get_new_boundary( low );
    high = get_new_boundary( high );
    end = middle;
    first = true;
    return params;
}

pair<double, double> QuadraticPlacer::Parameters::push_to_boundary(
    const pair<double, double>& coordinates
) const {
    return make_pair( clamp( coordinates.first, low.first, high.first ),
                      clamp( coordinates.second, low.second, high.second ) );
}

void QuadraticPlacer::quadratic_placement( const Parameters& params ) {
    vector<int> row, column;
    vector<double> data;
    const vector<Gate>::iterator begin = Gates.begin() + params.begin;
    coo_matrix A;
    A.n = params.end - params.begin;
    valarray<double> bx( A.n ), by( A.n ), x( A.n );

    for ( int i = 0; i < A.n; ++i ) {
        const int index = i + params.begin;
        const vector<int>& nets = Gates[index].Nets;

        for ( const int& Net : nets ) {
            const vector<int>& net = Nets[Net];
            const double weight = 1 / static_cast<double>( net.size() - 1 );

            for ( const int& gate : net ) {
                if ( gate < 0 ) {
                    const auto [xi, yi] = params.push_to_boundary(Pins[~gate]);
                    bx[i] += weight * xi;
                    by[i] += weight * yi;
                } else {
                    if ( gate == index ) continue;

                    if ( gate >= params.begin and gate < params.end ) {
                        row.push_back( i );
                        column.push_back( gate - params.begin );
                        data.push_back( -weight );
                    } else {
                        const auto& [xlo, ylo] = params.low;
                        const auto& [xhi, yhi] = params.high;

                        auto [xi, yi] = params.push_to_boundary(
                            Gates[gate].coordinates
                        );

                        if ( xi > xlo and xi < xhi and yi > ylo and yi < yhi ) {
                            if (params.vertical) yi = params.first ? yhi : ylo;
                            else xi = params.first ? xhi : xlo;
                        }

                        bx[i] += weight * xi;
                        by[i] += weight * yi;
                    }
                }
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

    ranges::nth_element(
        begin,
        begin + ( A.n >> 1 ),
        begin + A.n,
        [&params](const pair<double, double>& a,const pair<double, double>& b) {
            return params.vertical ? a < b :
                tie( a.second, a.first ) < tie( b.second, b.first );
        },
        &Gate::coordinates
    );

    for ( int i = params.begin; i < params.end; ++i ) {
        Gate& gate = Gates[i];
        for (const int& Net : gate.Nets) *ranges::find(Nets[Net], gate.index)=i;
        gate.index = i;
    }
}

void QuadraticPlacer::recursiveQP( Parameters& params, int depth ) {
    quadratic_placement( params );

    if ( depth-- ) {
        Parameters params2 = params.partition();
        quadratic_placement( params );
        quadratic_placement( params2 );
        Parameters params3 = params.partition();
        Parameters params4 = params2.partition();
        recursiveQP( params, depth );
        recursiveQP( params2, depth );
        recursiveQP( params3, depth );
        recursiveQP( params4, depth );
    }
}

QuadraticPlacer::QuadraticPlacer( ifstream file, const int depth ) {
    int NumberofGates, NumberofNets, NumberofPads;
    file >> NumberofGates >> NumberofNets;
    Gates.resize( NumberofGates );
    Nets.resize( NumberofNets );

    for ( Gate& gate : Gates ) {
        int NumNetsConnected;
        file >> gate.GateID >> NumNetsConnected;
        gate.index = gate.GateID - 1;
        gate.Nets.resize( NumNetsConnected );

        for ( int& Net : gate.Nets ) {
            file >> Net;
            Nets[--Net].push_back( gate.index );
        }
    }

    file >> NumberofPads;
    Pins.resize( NumberofPads );

    for ( pair<int, int>& Pin : Pins ) {
        int PinID, NetNumberConnectedTo;
        file >> PinID >> NetNumberConnectedTo >> Pin.first >> Pin.second;
        Nets[--NetNumberConnectedTo].push_back( -PinID );
    }

    Parameters params( NumberofGates );
    recursiveQP( params, depth );
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
    if ( argc != 3 ) return 1;
    QuadraticPlacer quadratic_placer( ifstream( argv[1] ), atoi( argv[2] ) );
}