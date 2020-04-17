// mac937@nyu.edu
// hw13 

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cassert>
#include <string>
#include <climits>
#include <unistd.h>  // stage: cut

const int GESTATION_PERIOD_ANT = 3;
const int GESTATION_PERIOD_DOODLEBUG = 8;
const int STARVATION_PERIOD_DOODLEBUG = 3;
const int NUM_DIRECTIONS = 4;
const int DIRECTIONS[NUM_DIRECTIONS][2] = {
        {-1, 0}, // north
        {0, 1},  // east
        {1, 0},  // south
        {0, -1},  // west
};
const int DELTA_ROW = 0;
const int DELTA_COL = 1;
const char CHAR_ANT = 'o';
const char CHAR_DOODLEBUG = 'X';
const char CHAR_VACANT = '-';

class Random {
public:
    int from_range(int max_exclusive);
    virtual int from_range(int minInclusive, int maxExclusive) = 0;
};

class StandardRandom : public Random {
public:
    int from_range(int min_inclusive, int max_exclusive) override;
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
    Position();
    Position(int row, int col);
    Position(const Position& other);
    void translate(const int delta[]);
    void update(const Position& source);
    bool equals(const Position& other) const;
    int row() const;
    int col() const;
private:
    int row_;
    int col_;
};
class World;

class Organism {
protected:
    const int gestation_period;
    int ticks_since_bred;
    Position position_;
public:
    Organism(int gestation_period, const Position& position_);
    virtual ~Organism();
    const Position& position();
    virtual bool is_doodlebug() const = 0;
    virtual bool is_ant() const = 0;
    virtual bool is_starved() const = 0;
    virtual void move(World& world, Random& rng);
    virtual Organism* give_birth(const Position& position) const = 0;
    void breed(World& world, Random& rng);
    void tick(World& world, Random& rng);
    virtual void render(std::ostream& out) const = 0;
};

class Ant : public Organism {
public:
    explicit Ant(const Position& position_);
    bool is_doodlebug() const override;
    bool is_ant() const override;
    bool is_starved() const override;
    Organism* give_birth(const Position& position) const override;
    void render(std::ostream& out) const override;
};

class Doodlebug : public Organism {
public:
    explicit Doodlebug(const Position& position_);
    bool is_doodlebug() const override;
    bool is_ant() const override;
    bool is_starved() const override;
    Organism* give_birth(const Position& position) const override;
    void render(std::ostream& out) const override;
    void move(World& world, Random& rng) override;
private:
    int ticks_since_fed;
    bool hunt(World& world, Random& rng);
};

class World {
private:
    const Size size;
    std::vector<Organism*> organisms;
    int nticks;
    void check() const;
    void populate_doodlebugs(const std::vector<Position>& positions, int offset, int num_doodlebugs);
    void populate_ants(const std::vector<Position>& positions, int offset, int num_ants);
public:
    explicit World(const Size& size_);
    ~World();
    int num_ticks() const;
    void populate(int num_doodlebugs, int num_ants, Random& rng);
    void render(std::ostream& out) const;
    Organism* at(const Position& position) const;
    bool occupied(const Position& position) const;
    void spawn(Organism* organism);
    void kill(const Position& position);
    bool contains(const Position& position) const;
    int count_ants() const;
    int count_organisms() const;
    void tick(Random& rng);
};

// stage: cut start
std::string GetEnv( const std::string& key)
{
    char * val = std::getenv( key.c_str() );
    return val == nullptr ? std::string("") : std::string(val);
}

int ParseIntV(const std::string& s, int base, bool& valid) {
    valid = false;
    const char* sc = s.c_str();
    char* ep;
    long value = strtol(sc, &ep, base);
    if (ep != sc) {
        if (value >= INT_MIN && value <= INT_MAX) {
            valid = true;
        }
    }
    return (int) value;
}

int ParseInt(const std::string& s, int base, int defaultValue) {
    bool valid = false;
    int value = ParseIntV(s, base, valid);
    return valid ? value : defaultValue;
}

// stage: cut stop

int main(int argc, char* argv[]) {
    srand(time(nullptr)); // NOLINT(cert-msc32-c)
    StandardRandom rng;
    Size size(20, 20);
    int initial_num_doodlebugs = 5, initial_num_ants = 100;
    int maxTicks = 1000 * 1000 * 1000;
    bool noninteractive = false, quiet = false;
    // stage: cut start
    int sleep_duration = 0;
    {
        const char* ENV_DOODLEBUGS_NONINTERACTIVE = "DOODLEBUGS_NONINTERACTIVE";
        const char* ENV_DOODLEBUGS_MAX_TICKS = "DOODLEBUGS_MAX_TICKS";
        const char* ENV_DOODLEBUGS_SEED = "DOODLEBUGS_SEED";
        const char* ENV_DOODLEBUGS_QUIET = "DOODLEBUGS_QUIET";

        if (argc > 1) {
            std::cerr << "usage error; no arguments are accepted" << std::endl;
            return 1;
        }
        std::string nonInteractiveStr = GetEnv(ENV_DOODLEBUGS_NONINTERACTIVE);
        if (!nonInteractiveStr.empty()) {
            noninteractive = nonInteractiveStr == "1";
        }
        std::string maxTicksStr = GetEnv(ENV_DOODLEBUGS_MAX_TICKS);
        if (!maxTicksStr.empty()) {
            maxTicks = ParseInt(maxTicksStr, 10, maxTicks);
        }
        std::string seedStr = GetEnv(ENV_DOODLEBUGS_SEED);
        if (!seedStr.empty()) {
            bool ok = false;
            int seed = ParseIntV(seedStr, 10, ok);
            if (ok) {
                srand(seed);
            }
        }
        std::string quietStr = GetEnv(ENV_DOODLEBUGS_QUIET);
        if (!quietStr.empty()) {
            quiet = quietStr == "1";
        }
        std::cerr << ENV_DOODLEBUGS_NONINTERACTIVE << "=" << nonInteractiveStr << std::endl;
        std::cerr << ENV_DOODLEBUGS_MAX_TICKS << "=" << maxTicksStr << std::endl;
        std::cerr << ENV_DOODLEBUGS_SEED << "=" << seedStr << std::endl;
        std::cerr << ENV_DOODLEBUGS_QUIET << "=" << quietStr << std::endl;
    }
    // stage: cut stop
    World world(size);
    world.populate(initial_num_doodlebugs, initial_num_ants, rng);
    while (world.num_ticks() < maxTicks) {
        if (!quiet) {
            std::cout << std::endl;
            std::cout << world.num_ticks()
                      << " Ants: " << world.count_ants()
                      << " Doodlebugs: " << world.count_organisms() - world.count_ants()
                      << std::endl;
            world.render(std::cout);
        }
        if (world.count_organisms() == 0) {
            std::cerr << "Everybody's dead." << std::endl;
            break;
        }
        if (size.num_cells() == world.count_ants()) {
            std::cerr << "I for one welcome our new ant overlords." << std::endl;
            break;
        }
        if (!noninteractive) {
            std::cout << "Press enter to continue...";
            std::cin.get();
        }
        world.tick(rng);
        // stage: cut start
        if (sleep_duration > 0) {
            sleep(sleep_duration);
        }
        // stage: cut stop
    }
    std::cout << "Final: "
              << world.num_ticks()
              << " Ants: " << world.count_ants()
              << " Doodlebugs: " << world.count_organisms() - world.count_ants()
              << std::endl;
    return 0;
}

int StandardRandom::from_range(int min_inclusive, int max_exclusive) {
    assert(max_exclusive > min_inclusive);
    int width = max_exclusive - min_inclusive;
    int offset = rand() % width;
    return min_inclusive + offset;
}

int Random::from_range(int max_exclusive) {
    return from_range(0, max_exclusive);
}

Size::Size(int rows, int cols) : rows(rows), cols(cols) {
    assert(rows >= 0);
    assert(cols >= 0);
}

int Size::num_cells() const {
    return rows * cols;
}

Position::Position(int row, int col) : row_(row), col_(col) {

}

int Position::row() const {
    return row_;
}

int Position::col() const {
    return col_;
}

void Position::translate(const int delta[]) {
    row_ += delta[DELTA_ROW];
    col_ += delta[DELTA_COL];
}

void Position::update(const Position &source) {
    row_ = source.row();
    col_ = source.col();
}

bool Position::equals(const Position &other) const {
    return other.row_ == row_ && other.col_ == col_;
}

Position::Position(const Position &other) : row_(other.row_), col_(other.col_) {
}

Position::Position() : Position(0, 0) {
}


Organism::Organism(int gestation_period, const Position &position_) : gestation_period(gestation_period), position_(position_), ticks_since_bred(0) {

}

void Organism::move(World &world, Random &rng) {
    int direction_index = rng.from_range(NUM_DIRECTIONS);
    Position next(position_);
    next.translate(DIRECTIONS[direction_index]);
    if (world.contains(next) && !world.occupied(next)) {
        Position previous(position_);
        position_.update(next);
    }
}

void Organism::breed(World &world, Random &rng) {
    ticks_since_bred++;
    if (ticks_since_bred >= gestation_period) {
        std::vector<int> indexes_of_unoccupied_directions;
        for (int i = 0; i < NUM_DIRECTIONS; i++) {
            Position potential(position_);
            potential.translate(DIRECTIONS[i]);
            if (world.contains(potential) and !world.occupied(potential)) {
                indexes_of_unoccupied_directions.push_back(i);
            }
        }
        if (!indexes_of_unoccupied_directions.empty()) {
            int index = rng.from_range(indexes_of_unoccupied_directions.size());
            Position childPosition(position_);
            childPosition.translate(DIRECTIONS[indexes_of_unoccupied_directions[index]]);
            Organism* child = give_birth(childPosition);
            world.spawn(child);
            ticks_since_bred = 0;
        }
    }
}

void Organism::tick(World &world, Random &rng) {
    move(world, rng);
    breed(world, rng);
}

const Position& Organism::position() {
    return position_;
}

Organism::~Organism() {

}

Ant::Ant(const Position &position_) : Organism(GESTATION_PERIOD_ANT, position_) {
}

bool Ant::is_doodlebug() const {
    return false;
}

bool Ant::is_ant() const {
    return true;
}

bool Ant::is_starved() const {
    return false;
}

Organism *Ant::give_birth(const Position &position) const {
    return new Ant(position);
}

void Ant::render(std::ostream &out) const {
    out << CHAR_ANT;
}

Doodlebug::Doodlebug(const Position &position_) : Organism(GESTATION_PERIOD_DOODLEBUG, position_), ticks_since_fed(0) {
}

bool Doodlebug::is_doodlebug() const {
    return true;
}

bool Doodlebug::is_ant() const {
    return false;
}

bool Doodlebug::is_starved() const {
    return ticks_since_fed >= STARVATION_PERIOD_DOODLEBUG;
}

Organism *Doodlebug::give_birth(const Position &position) const {
    return new Doodlebug(position);
}

void Doodlebug::render(std::ostream &out) const {
    out << CHAR_DOODLEBUG;
}

void Doodlebug::move(World &world, Random &rng) {
    bool moved_when_hunting = hunt(world, rng);
    if (!moved_when_hunting) {
        Organism::move(world, rng);
    }
}

bool Doodlebug::hunt(World &world, Random &rng) {
    ticks_since_fed++;
    std::vector<int> indexes_of_directions_with_ants;
    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        Position potential(position_);
        potential.translate(DIRECTIONS[i]);
        Organism* organism_at_potential = world.at(potential);
        if (organism_at_potential != nullptr && organism_at_potential->is_ant()) {
            indexes_of_directions_with_ants.push_back(i);
        }
    }
    if (!indexes_of_directions_with_ants.empty()) {
        int randomChoice = rng.from_range(indexes_of_directions_with_ants.size());
        int dirIndex = indexes_of_directions_with_ants[randomChoice];
        Position destination(position_);
        destination.translate(DIRECTIONS[dirIndex]);
        world.kill(destination);
        Position previous(position_);
        position_.update(destination);
        ticks_since_fed = 0;
        return true;
    }
    return false;
}

void World::check() const {
    for (int i = 0; i < organisms.size(); i++) {
        assert(contains(organisms[i]->position()));
        for (int j = i + 1; j < organisms.size(); j++) {
            assert(!organisms[i]->position().equals(organisms[j]->position()));
        }
    }
}

World::World(const Size& size_) : size(size_), nticks(0) {
}

World::~World() {
    while (!organisms.empty()) {
        Organism* organism = organisms.back();
        delete organism;
        organisms.pop_back();
    }
}

int World::num_ticks() const {
    return nticks;
}

void World::populate(int num_doodlebugs, int num_ants, Random &rng) {
    assert(num_doodlebugs + num_ants <= size.num_cells());
    std::vector<Position> all_positions;
    for (int row = 0; row < size.rows; row++) {
        for (int col = 0; col < size.cols; col++) {
            all_positions.emplace_back(row, col);
        }
    }
    std::vector<Position> organism_positions;
    for (int i = 0; i < num_doodlebugs + num_ants; i++) {
        int posIndex = rng.from_range(all_positions.size());
        Position position(all_positions[posIndex]);
        all_positions.erase(all_positions.begin() + posIndex);
        organism_positions.push_back(position);
    }
    populate_doodlebugs(organism_positions, 0, num_doodlebugs);
    populate_ants(organism_positions, num_doodlebugs, num_ants);
}

void World::populate_doodlebugs(const std::vector<Position> &positions, int offset, int num_doodlebugs) {
    for (int i = offset; i < offset + num_doodlebugs; i++) {
        const Position& position = positions[i];
        spawn(new Doodlebug(position));
    }
}

void World::populate_ants(const std::vector<Position> &positions, int offset, int num_ants) {
    for (int i = offset; i < offset + num_ants; i++) {
        const Position& position = positions[i];
        spawn(new Ant(position));
    }
}

void World::render(std::ostream &out) const {
    for (int row = 0; row < size.rows; row++) {
        for (int col = 0; col < size.cols; col++) {
            Organism* organism = at(Position(row, col));
            if (organism == nullptr) {
                out << CHAR_VACANT;
            } else {
                organism->render(out);
            }
        }
        out << std::endl;
    }
}

Organism *World::at(const Position &position) const {
    for (Organism* organism : organisms) {
        if (position.equals(organism->position())) {
            return organism;
        }
    }
    return nullptr;
}

bool World::occupied(const Position &position) const {
    return at(position) != nullptr;
}

void World::spawn(Organism *organism) {
    assert(contains(organism->position()));
    organisms.push_back(organism);
}

void World::kill(const Position &position) {
    int index = -1;
    for (int i = 0; i < organisms.size(); i++) {
        if (position.equals(organisms[i]->position())) {
            index = i;
            break;
        }
    }
    assert(index >= 0);
    Organism* organism = organisms[index];
    delete organism;
    organisms.erase(organisms.begin() + index);
}

bool World::contains(const Position &position) const {
    return position.row() >= 0 && position.row() < size.rows
           && position.col() >= 0 && position.col() < size.cols;
}

int World::count_ants() const {
    int count = 0;
    for (Organism* organism : organisms){
        if (organism->is_ant()) {
            count++;
        }
    }
    return count;
}

int World::count_organisms() const {
    return organisms.size();
}

void World::tick(Random &rng) {
    nticks++;
    std::vector<Organism*> doodlebugs;
    for (Organism* organism : organisms) {
        if (organism->is_doodlebug()){
            doodlebugs.push_back(organism);
        }
    }
    for (Organism* doodlebug : doodlebugs) {
        doodlebug->tick(*this, rng);
        if (doodlebug->is_starved()) {
            kill(doodlebug->position());
        }
    }
    std::vector<Organism*> ants;
    for (Organism* organism : organisms) {
        if (organism->is_ant()) {
            ants.push_back(organism);
        }
    }
    for (Organism* ant : ants) {
        ant->tick(*this, rng);
    }
    check();
}
