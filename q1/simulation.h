#ifndef HW_SIMULATION_H
#define HW_SIMULATION_H

#include <iostream>
#include <vector>

class Random {
public:
    int from_range(int maxExclusive);
    virtual int from_range(int minInclusive, int maxExclusive) = 0;
};

class StandardRandom : public Random {
public:
    int from_range(int minInclusive, int maxExclusive) override;
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
    Position();
    Position(int row, int col);
    Position(const Position& other);
    void translate(const int delta[]);
    void update(const Position& source);
    bool equals(const Position& other) const;
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
    const Position& position() const;
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
    void populateDoodlebugs(const std::vector<Position>& positions, int offset, int numDoodlebugs);
    void populateAnts(const std::vector<Position>& positions, int offset, int numAnts);
public:
    explicit World(const Size& size_);
    virtual ~World();
    int num_ticks() const;
    void populate(int numDoodlebugs, int numAnts, Random& rng);
    void render(std::ostream& out) const;
    Organism* at(const Position& position) const;
    bool occupied(const Position& position) const;
    void spawn(Organism* organism);
    void kill(const Position& position);
    bool is_ant(const Position& position) const;
    bool contains(const Position& position) const;
    int count_ants() const;
    int count_all() const;
    void tick(Random& rng);
};



#endif //HW_SIMULATION_H
