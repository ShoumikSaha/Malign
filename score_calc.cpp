#include<bits/stdc++.h>
#include <string>
#include <stdlib.h>
#include <dirent.h>

using namespace std;

FILE *mafFile;
FILE *ofp1;
FILE *infile;
long lSize;

string itoa(int a)
{
    string ss="";   //create empty string
    while(a)
    {
        int x=a%10;
        a/=10;
        char i='0';
        i=i+x;
        ss=i+ss;      //append new character at the front of the string!
    }
    return ss;
}

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

bool isFasta(string filename)
{
    vector<string> words = splitter(filename, ".");
    if(words.size()!=2) return false;
    else if(words[1]!="fa") return false;
    return true;
}

vector<int> faFileListing(string path)
{
    vector<int> files;
    struct dirent *entry;
    DIR *dir = opendir(path.c_str());

    if (dir == NULL)
    {
        cout << "Null Directory" << endl;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if(isFasta(entry->d_name))
        {
            printf("%s\n",entry->d_name);
            vector<string> words = splitter(entry->d_name, ".");
            files.push_back(stoi(words[0]));
        }
    }
    closedir(dir);

    return files;
}

bool isMissedBlock(int blockId, vector<int> blocks)
{
    for(int i=0; i<blocks.size(); i++)
    {
        if(blockId==blocks[i]) return true;
    }
    return false;
}

int main(int argc, char** argv)
{
    //string type = argv[1];
    string parentPath = "";

    string path = parentPath + "sibeliaz_out/blocks/";
    //vector<int> blockIds = faFileListing(path);

    string ifilePath = parentPath + "sibeliaz_out/alignment.maf";
    mafFile = fopen(ifilePath.c_str(), "rb");
    if (!mafFile)
    {
        cout << "Unable to open file alignment.maf";
        exit(1);   // call system to stop
    }
    fseek(mafFile, 0, SEEK_END);
    long mafFileSize = ftell(mafFile);
    rewind(mafFile);
    char * buffer;
    buffer = (char*) malloc(sizeof(char)*mafFileSize);
    if(buffer==NULL)
    {
        fputs ("Reading error", stderr);
        exit(3);
    }
    size_t result = fread(buffer, 1, mafFileSize, mafFile);
    fclose(mafFile);

    string str = buffer;
    free(buffer);

    string ofilePath = parentPath + "sibeliaz_out/Score_output.txt";
    ofp1 = fopen(ofilePath.c_str(), "w");
    if(!ofp1)
    {
        cout << "Unable to open output file!! "  << endl;
        return -1;
    }

    vector <string> segments = splitter(str, "a\n");
    //cout<<"Total Segments: "<< segments.size();
    //cout<<endl;

    int count_MissedBlocks=0;  //to skip the id of missed blocks which weren't aligned
    int missed_Block = 0; // 1 if id matches missing block number

    for(int i=2; i<segments.size(); i++)
    {
        vector <string> lines = splitter(segments[i], "\n");
        //cout<<"Total Lines in Segment"<<i<<": "<<lines.size();
        //cout<<endl;
        int total_seq = lines.size()-2;
        string sequences[total_seq];
        int seq_length=0;
        for(int j=0; j<total_seq; j++)
        {
            vector <string> words = splitter(lines[j], " ");
            int length = atoi(words[3].c_str());
            seq_length = max(seq_length, length);
            sequences[j] = words[6];
        }
        //cout<<"Sequence Length: "<<seq_length<<endl;
        float** score_matrix = new float*[5];
        for(int j=0; j<5; j++)
        {
            score_matrix[j] = new float[seq_length];
        }
        //cout<<"Checking for initialization error!"<<endl;
        //float score_matrix[5][seq_length];
        for(int k=0; k<seq_length; k++)
        {
            int countA=0, countC=0, countG=0, countT=0, countGap=0;
            for(int l=0; l<total_seq; l++)
            {
                char res = sequences[l][k];
                if(res=='A') countA++;
                if(res=='C') countC++;
                if(res=='G') countG++;
                if(res=='T') countT++;
                if(res=='-') countGap++;
            }
            score_matrix[0][k] = countA*1.00/total_seq;
            score_matrix[1][k] = countC*1.00/total_seq;
            score_matrix[2][k] = countG*1.00/total_seq;
            score_matrix[3][k] = countT*1.00/total_seq;
            score_matrix[4][k] = countGap*1.00/total_seq;
        }
        //cout<<"Checking for error!"<<endl;
        
        int temp = i+count_MissedBlocks-1;
        if(missed_Block)
        {
            fprintf(ofp1, "a\n");
            string temp = "id" + itoa(i+count_MissedBlocks-2) + "\n";
            fprintf(ofp1, "%s", temp.c_str());
            fprintf(ofp1, "N/A\n");
            missed_Block = 0;
        }
        string start_del = "a\n";
        fprintf(ofp1, "%s", start_del.c_str());
        string info = "id" + itoa(i+count_MissedBlocks-1) + ", " + "length=" + itoa(seq_length) + ", " + "total_seq=" + itoa(total_seq) + "\n";
        fprintf(ofp1,"%s", info.c_str());

        fprintf(ofp1, "A-");
        for(int j=0; j<seq_length; j++)
        {
            fprintf(ofp1, "%.3f", score_matrix[0][j]);
            fprintf(ofp1, ",");
        }
        fprintf(ofp1, "\n");
        fprintf(ofp1, "C-");
        for(int j=0; j<seq_length; j++)
        {
            fprintf(ofp1, "%.3f", score_matrix[1][j]);
            fprintf(ofp1, ",");
        }
        fprintf(ofp1, "\n");
        fprintf(ofp1, "G-");
        for(int j=0; j<seq_length; j++)
        {
            fprintf(ofp1, "%.3f", score_matrix[2][j]);
            fprintf(ofp1, ",");
        }
        fprintf(ofp1, "\n");
        fprintf(ofp1, "T-");
        for(int j=0; j<seq_length; j++)
        {
            fprintf(ofp1, "%.3f", score_matrix[3][j]);
            fprintf(ofp1, ",");
        }
        fprintf(ofp1, "\n");
        fprintf(ofp1, "Gap-");
        for(int j=0; j<seq_length; j++)
        {
            fprintf(ofp1, "%.3f", score_matrix[4][j]);
            fprintf(ofp1, ",");
        }
        fprintf(ofp1, "\n");

        for(int j=0; j<5; j++)
        {
            delete[] score_matrix[j];
        }
        delete[] score_matrix;
    }

    fclose(ofp1);
    //cout<<lines[0]<<endl<<lines[1]<<endl<<lines[lines.size()-1]<<endl<<lines[lines.size()-2]<<endl;


}
