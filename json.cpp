#include <iostream>
#include <fstream>
#include <vector>
#include "bvh_parser.h"

using namespace std;
using std::ofstream;

int count_of_tabs = 0;

void strOutputVector(vector<string> v, ofstream &outfile){
    outfile << "[";
    if(v.size()){
        outfile << "\"" << v[0] << "\"";
        for(int i = 1; i < v.size(); i++)
            outfile << ", \"" << v[i]<<"\"";
    }
    outfile << "]";
}

void lfOutputVector(vector<double> v, ofstream &outfile){
    outfile << "[";
    if(v.size()){
        outfile << v[0];
        for(int i = 1; i < v.size(); i++)
            outfile << ", " << v[i];
    }
    outfile << "]";
}

void PrintTabs(ofstream &outfile){
    for (int i = 0; i < count_of_tabs; i++)
        outfile << "\t";
}

void PrintMetaData(ofstream &outfile, META meta_data){
    count_of_tabs = 0;
    outfile << "{" << endl;
    count_of_tabs ++;
    PrintTabs(outfile);
    outfile << "\"frame\": " << meta_data.frame <<", "<<endl;
    PrintTabs(outfile);
    outfile << "\"frame_time\": " << meta_data.frame_time <<", "<<endl;
    PrintTabs(outfile);
    outfile << "\"joint\": " << endl;
    count_of_tabs++;
}

void PrintJointContent(joint *&cur, ofstream &outfile)
{
    if(cur == NULL){
        cout << "Invalid Joint: a null joint is found in the joint tree";
        return;
    }
    
    //{
    PrintTabs(outfile);
    outfile << "{" << endl;

    //"type": "_"
    count_of_tabs++;
    PrintTabs(outfile);
    outfile << "\"type\": \"" << cur->joint_type << "\"," << endl;

    //"name": "_"
    PrintTabs(outfile);
    outfile << "\"name\": \"" << cur->name << "\"," << endl;

    //"offset": "_"
    PrintTabs(outfile);
    outfile << "\"offset\": [" << cur->offset_x << ", " << cur->offset_y << ", " << cur->offset_z << "]," << endl;

    //"channels": []
    PrintTabs(outfile);
    outfile << "\"channels\": ";
    strOutputVector(cur->channels, outfile);
    outfile << ", " << endl; 

    //"motion": [
    PrintTabs(outfile);
    outfile<<"\"motion\": ["<<endl;

    //[],
    //[],
    //[]
    count_of_tabs++;
    for(int i = 0; i < cur->motion.size() - 1; i ++){
        PrintTabs(outfile);
        lfOutputVector(cur->motion[i], outfile);
        outfile << ", " << endl;
    }
    PrintTabs(outfile);
    lfOutputVector(cur->motion[cur->motion.size()-1], outfile);
    outfile << endl;

    //],
    count_of_tabs--;
    PrintTabs(outfile);
    outfile << "], " << endl;

    //"children":[
    PrintTabs(outfile);
    outfile << "\"children\": [" << endl;

    count_of_tabs++;
    if(cur->children.size()){
        for(int i = 0; i < cur->children.size() - 1; i++){
            PrintJointContent(cur->children[i], outfile);
            outfile << ", " << endl;
        }
        PrintJointContent(cur->children[cur->children.size() - 1], outfile);
    } else {
        PrintTabs(outfile);
    }
    outfile << endl;
    count_of_tabs --;
    PrintTabs(outfile);
    outfile << "]" << endl;
    count_of_tabs--;
    PrintTabs(outfile);
    outfile << "}" << endl;
}

void jsonify(joint root, META meta_data)
{
    ofstream outfile;
    outfile.open("output.json");
    if (!outfile.is_open()){
        cout << "Error: failed to create: output.json" << endl;
    } else {
        joint* cur = &root;
        PrintMetaData(outfile, meta_data);
        PrintJointContent(cur, outfile);
        outfile << "}";
    }
    outfile.close();
}