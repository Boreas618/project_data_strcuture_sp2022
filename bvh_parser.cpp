#include<fstream>
#include"bvh_parser.h"
#include <iostream>
#include <string>
// a naive bvh parser

using std::ifstream;

/// @brief A helper function for removing whitespace from both sides of the string
/// @param string line 
/// @return A string without whitespace at both of its sides
string trim(string line){
    int pointer_forward = 0;
    int pointer_backward = line.size() - 1;

    while(line[pointer_forward] == ' ' || 
    line[pointer_forward] == '\n' || 
    line[pointer_forward] == '\r'|| 
    line[pointer_forward] == '\b'||
    line[pointer_forward] == '\t'||
    line[pointer_forward] == '\f'||
    line[pointer_forward] == '\v'){
        pointer_forward++;
    }

    while(line[pointer_backward] == ' ' || 
    line[pointer_backward] == '\n' || 
    line[pointer_backward] == '\r'|| 
    line[pointer_backward] == '\b'||
    line[pointer_backward] == '\t'||
    line[pointer_backward] == '\f'||
    line[pointer_backward] == '\v'){
        pointer_backward--;
    }

    return line.substr(pointer_forward, pointer_backward+1);
}

void buildSkelenton(joint& root, char* path){
    string line;
    ifstream file(path);
    if(file.is_open()){
        int i = 0;
        while(std::getline(file, line)){

            //IMPORTANT: EVERY line ends with a '\r'
            if(line=="MOTION\r")
                break;

            if(line=="HIERARCHY\r")
                continue;
            std::cout<<trim(line)<<std::endl;

        }
        file.close();
    }
}

int main(int argc, char** argv) {
    joint root;
    META meta_data;
    ifstream file(argv[1]);
    buildSkelenton(root, "sample.bvh");
    //jsonify(root, meta_data);
    file.close();
    return 0;
}