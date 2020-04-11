// mac937@nyu.edu
// hw13 

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "simulation.h"
#include <cassert>

using namespace std;


void test() {
    Organism* ant = new Ant(Position(3, 4));
    assert(ant->is_ant());
    assert(!ant->is_doodlebug());
    delete ant;
    Organism* doodlebug = new Doodlebug(Position(3, 4));
    assert(!doodlebug->is_ant());
    assert(doodlebug->is_doodlebug());
    delete doodlebug;
    StandardRandom rng;
    {
        World world(Size(3, 3));
        world.populate(0, 1, rng);
        world.tick(rng);
    }
    {
        World world(Size(3, 3));
        world.populate(1, 0, rng);
        world.tick(rng);
    }
    {
        World world(Size(3, 3));
        world.populate(1, 1, rng);
        world.tick(rng);
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc51-cpp"
int main() {
    test();
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
