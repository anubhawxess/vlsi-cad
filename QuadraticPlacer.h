class QuadraticPlacer {
    struct Gate {
        int GateID, index;
        std::vector<int> Nets;
        std::pair<double, double> coordinates;
    };

    class Parameters {
        std::pair<double, double> get_new_boundary(
            const std::pair<double, double>& coordinates
        ) const;
    public:
        int begin{}, end;
        std::pair<double, double> low, high{100, 100};
        bool vertical{true}, first;

        Parameters( const int size ) : end( size ) {}
        Parameters partition();

        std::pair<double, double> push_to_boundary(
            const std::pair<double, double>& coordinates
        ) const;
    };

    std::vector<Gate> Gates;
    std::vector<std::vector<int>> Nets;
    std::vector<std::pair<int, int>> Pins;

    void quadratic_placement( const Parameters& params );
    void recursiveQP( Parameters& params, int depth );
public:
    QuadraticPlacer( std::ifstream file, const int depth );
};