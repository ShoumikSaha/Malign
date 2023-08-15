#include<bits/stdc++.h>
#include <string>
#include <stdlib.h>

using namespace std;

FILE *File;
FILE *ofp1;
FILE *infile;
long lSize;

vector <string> splitter(string str, string delim)
{
    int count = 0;
    size_t pos = 0;
    string token;
    vector <string> Tokens;
    while((pos = str.find(delim)) != std::string::npos)
    {
        token = str.substr(0, pos);
        str.erase(0, pos + delim.length());
        count++;
        Tokens.push_back(token);
    }
    Tokens.push_back(str);
    return Tokens;
}

string parseFile(string fileName)
{
    File = fopen(fileName.c_str(), "rb");
    if (!File)
    {
        cout << "Unable to open file"<<fileName<<endl;
        exit(1);   // call system to stop
    }
    fseek(File, 0, SEEK_END);
    long FileSize = ftell(File);
    rewind(File);
    char * buffer;
    buffer = (char*) malloc(sizeof(char)*FileSize);
    if(buffer==NULL)
    {
        fputs ("Reading error", stderr);
        exit(3);
    }
    size_t result = fread(buffer, 1, FileSize, File);
    fclose(File);

    string str = buffer;
    free(buffer);

    return str;
}

int main()
{
    ofp1 = fopen("cmd_for_alignment.sh", "w");
    if(!ofp1)
    {
        cout << "Unable to open output file!! "  << endl;
        return -1;
    }

    string str = parseFile("pathListTrain.txt");
    vector<string> lines = splitter(str, "\r\n");
    cout<<lines.size()<<endl;

    string common_cmd = "./sibeliaz -o Alignments/";

    for(int i=0; i<lines.size(); i++)
    {
        vector<string> slash_delims = splitter(lines[i], "/");
        string fasta_name = slash_delims[1];
        vector<string> dot_delims = splitter(fasta_name, ".");
        string fasta_name_before_dot = dot_delims[0];

        fprintf(ofp1, "%s", common_cmd.c_str());
        fprintf(ofp1, "%s", fasta_name_before_dot.c_str());
        fprintf(ofp1, " merged_seq_withoutA.fasta ");
        fprintf(ofp1, "%s\n", lines[i].c_str());
    }

    fclose(ofp1);
}
