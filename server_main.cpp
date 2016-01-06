#include "Index_server.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::string;

Index_server server;

int main(int argc, char *argv[])
{
    if (argc < 3) {
        cerr << "Usage: indexServer <portnum> <inverted-index-filename>" << endl;
        return -1;
    }

    int port = atoi(argv[1]);
    if (port < 1024 || port > 65535) {
        cerr << "Port must be between 1024 and 65535 (exclusive)." << endl;
        return -1;
    }

    const char *fname = argv[2];
    ifstream infile(fname);
    if (!infile.is_open()) {
        cerr << "Error opening file: " << fname << endl;
        return -1;
    }

    cout << "Init server with fname " << fname << endl;
    server.init(infile);
    server.run(port);

    return 0;
}
