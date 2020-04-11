// mac937@nyu.edu
// hw13 

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "simulation.h"

using namespace std;

int main() {
    srand(time(nullptr)); // NOLINT(cert-msc32-c)
    StandardRandom rng;
    Size size(20, 20);
    int numDoodlebugs = 5, numAnts = 100;
    int maxTicks = 1000 * 1000 * 1000;
    World world(size);
    world.populate(numDoodlebugs, numAnts, rng);
    while (world.num_ticks() < maxTicks) {
        cout << endl;
        cout << world.num_ticks() << "\tA: " << world.count_ants() << "\tD: " << world.count_all() - world.count_ants() << endl;
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
