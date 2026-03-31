class QuadraticPlacer {
    struct Gate {
        int GateID, index;
        std::vector<unsigned> Nets;
        std::pair<double, double> coordinates;
    };

    struct Parameters {
        int begin{}, end;
        std::pair<double, double> low, high{100, 100};

        Parameters( const int size ) : end( size ) {}

        std::pair<double, double> push_to_boundary(
            const std::pair<double, double>& coordinates
        ) const;

        std::pair<double, double> get_new_boundary(
            const std::pair<double, double>& coord, const bool vertical
        ) const;
    };

    std::vector<Gate> Gates;
    std::vector<std::vector<int>> Nets;
    std::vector<std::pair<int, int>> Pins;

    void quadratic_placement(
        const Parameters& params, const bool vertical, const bool lower = true
    );

    Parameters partition( Parameters& params, const bool vertical = false );
    void recursiveQP( Parameters& params, int depth = 3 );
public:
    QuadraticPlacer( std::ifstream file );
};
