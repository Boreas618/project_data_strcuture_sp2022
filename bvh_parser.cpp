#include<fstream>
#include"bvh_parser.h"
#include <iostream>
#include <string>
#include <stack>
#include <sstream>

// a naive bvh parser

using std::ifstream;

/// @brief A helper function for removing whitespace from both sides of the string
/// @param line the string to be processed
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

/// @brief A function fot parsing the offset information
/// @param line the input string
/// @return a vector(length:3) storing the information
vector<double> parseOffset(string line){
    int startIndex = line.find_first_of(' ');
    vector<double> offsetFields(3);
    string info = line.substr(startIndex, line.size());
    std::stringstream ss(info);
    ss >> offsetFields[0] >> offsetFields[1] >> offsetFields[2];
    return offsetFields;
}

/// @brief A function for parsing the channel information
/// @param line the input string
/// @return A vector storing the channel fields
vector<string> parseChannels(string line){
    int startIndex = line.find_first_of(' ');
    int length = 0;
    std::stringstream ss(line.substr(startIndex, line.size()));
    ss >> length;
    vector<string> channelFields(length);
    for(int i = 0; i < length; i++)
        ss >> channelFields[i];
    return channelFields;
}

/// @brief A function for building the skeleton of the data.
/// @param root the root node
/// @param path the file path
void buildSkeleton(joint& root, char* path){
    std::stack<joint*> s;
    joint *p = &root;
    string line;
    ifstream file(path);
    if(file.is_open()){
        int i = 0;
        while(std::getline(file, line)){

            line = trim(line);

            //IMPORTANT: EVERY line ends with a '\r'
            if(line == "MOTION")
                break;

            if(line == "HIERARCHY")
                continue;
            
            if(line == "{"){
                s.push(p);
            } else if (line == "}"){
                s.pop();
            } else {
                if(line.substr(0,line.find_first_of(' ')) == "ROOT"){
                    p->joint_type = "ROOT";
                }
                if(line.substr(0,line.find_first_of(' ')) == "JOINT"){
                    joint* childJoint = new joint();
                    p->children.push_back(childJoint);
                    p = childJoint;
                    childJoint->joint_type = "JOINT";
                }
                if(line.substr(0,line.find_first_of(' ')) == "END"){
                    joint* childJoint = new joint();
                    s.top()->children.push_back(childJoint);
                    p = childJoint;
                    childJoint->joint_type = "END";
                }
                if(line.substr(0,line.find_first_of(' ')) == "OFFSET"){
                    vector<double> offset = parseOffset(line);
                    p->offset_x = offset[0];
                    p->offset_y = offset[1];
                    p->offset_z = offset[2];
                }
                if(line.substr(0,line.find_first_of(' ')) == "CHANNELS"){
                    p->channels = parseChannels(line);
                }
            }
            //std::cout<<trim(line)<<std::endl;

        }
        file.close();
    }
}

int main(int argc, char** argv) {
    joint root;
    META meta_data;
    ifstream file(argv[1]);
    buildSkeleton(root, argv[1]);
    jsonify(root, meta_data);
    file.close();
    return 0;
}