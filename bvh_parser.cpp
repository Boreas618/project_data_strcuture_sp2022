#include <fstream>
#include "bvh_parser.h"
#include <iostream>
#include <string>
#include <stack>
#include <sstream>

// a naive bvh parser

using namespace std;
using std::ifstream;

// Build the tree skeleton of the bvh file
// Read the leading label of each line(e.g. "ROOT", "MOTTION", ...) and parse the corresponding information that follows.
// Use a stack to help build the tree. Whenever a "{" is met, push the current joint into
// the stack. Whenever a "}" is met, pop the top joint from the stack. 
// It keeps reading the file until a "MOTION" is met.
void BuildSkeleton(joint &root, ifstream &file)
{
    stack<joint *> s;
    joint *p = &root;
    string line;
    string parent_name;
    while (getline(file, line))
    {
        stringstream ss(line);
        string leading;
        ss >> leading;
        if (leading == "MOTION")
        {
            break;
        }
        else if (leading == "{")
        {
            s.push(p);
        }
        else if (leading == "}")
        {
            s.pop();
            if (!s.empty())
                p = s.top();
        }
        else if (leading == "ROOT")
        {
            p->joint_type = "ROOT";
            ss >> p->name;
            parent_name = p->name;
        }
        else if (leading == "JOINT")
        {
            joint *child_joint = new joint();
            p->children.push_back(child_joint);
            p = child_joint;
            child_joint->joint_type = "JOINT";
            ss >> child_joint->name;
            parent_name = child_joint->name;
        }
        else if (leading == "End")
        {
            joint *child_joint = new joint();
            p->children.push_back(child_joint);
            p = child_joint;
            p->joint_type = "END";
            p->name = parent_name + "_End";
        }
        else if (leading == "OFFSET")
        {
            ss >> p->offset_x;
            ss >> p->offset_y;
            ss >> p->offset_z;
        }
        else if (leading == "CHANNELS")
        {
            int length;
            ss >> length;
            vector<string> channel_fields(length);
            for (int i = 0; i < length; i++)
                ss >> channel_fields[i];
            p->channels = channel_fields;
        }
    }
}

// Generate meta data from "Frames:	433 Frame Time:	0.00833333"
void GenerateMeta(META &meta_data, ifstream &file)
{
    string line_frames;
    string line_frame_time;
    getline(file, line_frames);
    getline(file, line_frame_time);

    stringstream ss1(line_frames.substr(line_frames.find_first_of(':') + 2, line_frames.size() - line_frames.find_first_of(':') - 2));
    ss1 >> meta_data.frame;

    stringstream ss2(line_frame_time.substr(line_frame_time.find_first_of(':') + 2, line_frame_time.size() - line_frame_time.find_first_of(':') - 2));
    ss2 >> meta_data.frame_time;
}

// Load the motion data
// Tarverse the skeleton that has constrcuted earlier. Load the single-frame data in the form of a vector.
void LoadMotions(ifstream &file, joint &root)
{
    string line;
    joint *p;
    stack<joint *> s;
    while (getline(file, line))
    {
        p = &root;
        s.push(p);

        stringstream ss(line);
        while (!s.empty())
        {
            p = s.top();
            s.pop();

            for (int i = p->children.size() - 1; i >= 0; i--)
                s.push(p->children[i]);

            vector<double> single_frame_channel_data(p->channels.size());

            for (int i = 0; i < p->channels.size(); i++)
                ss >> single_frame_channel_data[i];

            p->motion.push_back(single_frame_channel_data);
        }
    }
}

int main(int argc, char **argv)
{
    joint root;
    META meta_data;
    ifstream file(argv[1]);

    if (!file.is_open())
        cout << "Error: failed to open: " << argv[1] << endl;
    else
        BuildSkeleton(root, file);

    GenerateMeta(meta_data, file);
    LoadMotions(file, root);
    jsonify(root, meta_data);
    file.close();
    return 0;
}