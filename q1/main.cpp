// mac937@nyu.edu
// hw13 

#include <iostream>
#include <cassert>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>

using namespace std;

class Random {
public:
    virtual int from_range(int minInclusive, int maxExclusive) = 0;
};

class StandardRandom : public Random {
public:
    int from_range(int minInclusive, int maxExclusive);
};

struct Size {
public:
    const int rows;
    const int cols;
    Size(int rows, int cols);
    int num_cells() const;
};

class Position {
public:
    int row;
    int col;
    Position(int row, int col);
    
    void translate(const int delta[]);
    void update(const Position& source);
    bool equals(const Position& other) const;
};

const int DIRECTIONS[4][2] = {
        {-1, 0}, // north
        {0, 1},  // east
        {1, 0},  // south
        {0, 1},  // west
};

const int GESTATION_PERIOD_ANT = 3;
const int GESTATION_PERIOD_DOODLEBUG = 8;
const int STARVATION_PERIOD_DOODLEBUG = 3;

class World;

class Organism {
protected:
    const int gestation_period;
    int ticks_since_bred;
    Position position;
public:
    Organism(int gestation_period, const Position& position_);
    virtual bool is_doodlebug() const = 0;
    virtual bool is_ant() const = 0;
    virtual bool is_starved() const = 0;
    virtual void move(World& world, Random& rng);
    virtual Organism* give_birth(const Position& position) const = 0;
    void breed(World& world, Random& rng) const;
    void tick(World& world, Random& rng);
    virtual void render(ostream& out) const = 0;
};

class Ant : public Organism {
public:
    Ant(const Position& position_);
    virtual bool is_doodlebug() const;
    virtual bool is_ant() const;
    virtual bool is_starved() const;
    virtual Organism* give_birth(const Position& position) const;
}; 

class Doodlebug : public Organism {
public:
    Doodlebug(const Position& position_);
    virtual bool is_doodlebug() const;
    virtual bool is_ant() const;
    virtual bool is_starved() const;
    virtual Organism* give_birth(const Position& position) const;
private:
    bool hunt(World& world, Random& rng);
    void move(World& world, Random& rng);
};

class World {
private:
    const Size size;
    vector<Organism*> organisms;
    int nticks;
    void check() const;
public:
    World(const Size& size_);
    virtual ~World();
    int num_ticks() const;
    void populate(int numDoodlebugs, int numAnts, Random& rng);
    void render(ostream& out) const;
    Organism* at(const Position& position) const;
    bool occupied(const Position& position) const;
    void spawn(Organism* organism);
    void kill(const Position& position);
    bool is_ant(const Position& position) const;
    int count_ants() const;
    int count_all() const;
    void tick(Random& rng);
};


#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc51-cpp"
int main() {
    Size size(20, 20);
    int numDoodlebugs = 5, numAnts = 100;
    int maxTicks = 1000 * 1000 * 1000;
    World world(size);
//    srand (time(nullptr));
    srand(12345); // NOLINT(cert-msc32-c)
    StandardRandom rng;
    world.populate(numDoodlebugs, numAnts, rng);
    while (world.num_ticks() < maxTicks) {
        cout << endl;
        cout << world.num_ticks() << endl;
        world.render(cout);
        if (world.count_all() == 0) {
            cerr << "Everybody's dead." << endl;
            break;
        }
        if (size.num_cells() == world.count_ants()) {
            cerr << "I for one welcome our new ant overlords." << endl;
            break;
        }
        cout << "Press enter to continue...";
        cin.get();
        world.tick(rng);
    }
    return 0;
}
#pragma clang diagnostic pop
