using Coordinates = std::array<unsigned, 3>;

class Router {
    enum Tag { N, S, E, W, U, D };

    struct Cell {
        Coordinates coordinates;
        int path_cost;
        Tag pred;
    };

    unsigned row, column;
    int BendPenalty, ViaPenalty;
    boost::dynamic_bitset<> reached;

    unsigned get_index( const Coordinates& coordinates ) const;
    bool check_cell( const Cell& C ) const;
    std::generator<Cell> unreached_neighbours( Cell C ) const;
    void backtrace( const Coordinates& source, Coordinates& target );
public:
    std::vector<std::pair<int, Tag>> grid;

    Router(
        const unsigned X_gridsize,
        const unsigned Y_gridsize,
        const int bend,
        const int via
    ) :
        row( X_gridsize ),
        column( Y_gridsize ),
        BendPenalty( bend ),
        ViaPenalty( via ),
        reached( row * column << 1 ),
        grid( reached.size() )
    {}
    
    void MazeRouting( const Coordinates& source, Coordinates& target );
};
