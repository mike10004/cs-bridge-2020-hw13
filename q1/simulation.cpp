#include <iostream>
#include <vector>
#include <cassert>
#include <cstdlib>
#include "simulation.h"


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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc30-c"
int StandardRandom::from_range(int minInclusive, int maxExclusive) {
    assert(maxExclusive > minInclusive);
    int width = maxExclusive - minInclusive;
    int offset = rand() % width;
    return minInclusive + offset;
}
#pragma clang diagnostic pop

int Random::from_range(int maxExclusive) {
    return from_range(0, maxExclusive);
}

Size::Size(int rows, int cols) : rows(rows), cols(cols) {
    assert(rows >= 0);
    assert(cols >= 0);
}

int Size::num_cells() const {
    return rows * cols;
}

Position::Position(int row, int col) : row(row), col(col) {

}

void Position::translate(const int delta[]) {
    row += delta[DELTA_ROW];
    col += delta[DELTA_COL];    
}

void Position::update(const Position &source) {
    row = source.row;
    col = source.col;
}

bool Position::equals(const Position &other) const {
    return other.row == row && other.col == col;
}

Position::Position(const Position &other) : row(other.row), col(other.col) {
}

Position::Position() : Position(0, 0) {
}


Organism::Organism(int gestation_period, const Position &position_) : gestation_period(gestation_period), position_(position_), ticks_since_bred(0) {

}

void Organism::move(World &world, Random &rng) {
    int directionIdx = rng.from_range(NUM_DIRECTIONS);
    Position next(position_);
    // std::cerr << "move: " << DIRECTIONS[directionIdx][0] << DIRECTIONS[directionIdx][1] << std::endl;
    next.translate(DIRECTIONS[directionIdx]);
    if (world.contains(next) && !world.occupied(next)) {
        position_.update(next);
    }
}

void Organism::breed(World &world, Random &rng) {
    ticks_since_bred++;
    if (ticks_since_bred >= gestation_period) {
        std::vector<int> indexesOfUnoccupiedDirections;
        for (int i = 0; i < NUM_DIRECTIONS; i++) {
            Position potential(position_);
            if (world.contains(potential) and !world.occupied(potential)) {
                indexesOfUnoccupiedDirections.push_back(i);
            }
        }
        if (!indexesOfUnoccupiedDirections.empty()) {
            int index = rng.from_range(indexesOfUnoccupiedDirections.size());
            Position childPosition(position_);
            childPosition.translate(DIRECTIONS[indexesOfUnoccupiedDirections[index]]);
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

const Position& Organism::position() const {
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
    bool movedWhenHunting = hunt(world, rng);
    if (!movedWhenHunting) {
        Organism::move(world, rng);
    }
}

bool Doodlebug::hunt(World &world, Random &rng) {
    ticks_since_fed++;
    std::vector<int> indexesOfDirectionsWhereAntsAre;
    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        Position potential(position_);
        potential.translate(DIRECTIONS[i]);
        if (world.is_ant(potential)) {
            indexesOfDirectionsWhereAntsAre.push_back(i);
        }
    }
    if (!indexesOfDirectionsWhereAntsAre.empty()) {
        int randomChoice = rng.from_range(indexesOfDirectionsWhereAntsAre.size());
        int dirIndex = indexesOfDirectionsWhereAntsAre[randomChoice];
        Position destination(position_);
        destination.translate(DIRECTIONS[dirIndex]);
        world.kill(destination);
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

void World::populate(int numDoodlebugs, int numAnts, Random &rng) {
    assert(numDoodlebugs + numAnts <= size.num_cells());
    std::vector<Position> allPositions;
    for (int row = 0; row < size.rows; row++) {
        for (int col = 0; col < size.cols; col++) {
            allPositions.push_back(Position(row, col));
        }
    }
    std::vector<Position> organismPositions;
    for (int i = 0; i < numDoodlebugs + numAnts; i++) {
        int posIndex = rng.from_range(allPositions.size());
        Position position(allPositions[posIndex]);
        allPositions.erase(allPositions.begin() + posIndex);
        organismPositions.push_back(position);
    }
    populateDoodlebugs(organismPositions, 0, numDoodlebugs);
    populateAnts(organismPositions, numDoodlebugs, numAnts);
}

void World::populateDoodlebugs(const std::vector<Position> &positions, int offset, int numDoodlebugs) {
    for (int i = offset; i < offset + numDoodlebugs; i++) {
        const Position& position = positions[i];
        spawn(new Doodlebug(position));
    }
}

void World::populateAnts(const std::vector<Position> &positions, int offset, int numAnts) {
    for (int i = offset; i < offset + numAnts; i++) {
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

bool World::is_ant(const Position &position) const {
    Organism* organism = at(position);
    return organism != nullptr && organism->is_ant();
}

bool World::contains(const Position &position) const {
    return position.row >= 0 && position.row < size.rows
            && position.col >= 0 && position.col < size.cols; 
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

int World::count_all() const {
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
