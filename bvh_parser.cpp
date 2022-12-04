#include<fstream>
#include"bvh_parser.h"

// a naive bvh parser

using std::ifstream;

int main(int argc, char** argv) {
    joint root;
    META meta_data;
    ifstream file(argv[1]);

    //do something


    jsonify(root, meta_data);
    file.close();
    return 0;
}