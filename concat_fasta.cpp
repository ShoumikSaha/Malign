#include<bits/stdc++.h>
#include <string>
#include <stdlib.h>

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

int findMax(int arr[], int len)    ///version 2 is used to find 2nd max (case of gap)
{
    int max = 0;
    int idx = -1;
    int max2 = 0;
    int idx2 = -1;
    for(int i=0; i<len; i++)
    {
        if(arr[i]>max)
        {
            max = arr[i];
            idx = i;
            max2 = max;
            idx2 = idx;
        }
    }
    /*if(idx==(len-1) && idx2!=-1)
    {
        return idx2;
    }*/
    return idx;
}

int main()
{
    mafFile = fopen("sibeliaz_out/alignment.maf", "rb");
    if (!mafFile)
    {
        cout << "Unable to open file alignment_test.maf";
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

    ofp1 = fopen("merged_sequence.fasta", "w");
    if(!ofp1)
    {
        cout << "Unable to open output file!! "  << endl;
        return -1;
    }

    vector <string> segments = splitter(str, "a\n");
    cout<<"Total Segments: "<< segments.size();
    cout<<endl;

    int count_MissedBlocks=0;  //to skip the id of missed blocks which weren't aligned

    for(int i=2; i<segments.size(); i++)
    {
        vector <string> lines = splitter(segments[i], "\n");
        cout<<"Total Lines in Segment"<<i<<": "<<lines.size();
        cout<<endl;
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
        cout<<"Sequence Length: "<<seq_length<<endl;
        int *idx_for_seq = new int [seq_length];
        //char *merged_seq = new char [seq_length];
        string merged_seq;

        for(int k=0; k<seq_length; k++)
        {
            //int countA=0, countC=0, countG=0, countT=0, countGap=0;
            int countElement[5];
            memset(countElement, 0, sizeof(countElement));

            for(int l=0; l<total_seq; l++)
            {
                char res = sequences[l][k];
                if(res=='A') countElement[0]++;
                if(res=='C') countElement[1]++;
                if(res=='G') countElement[2]++;
                if(res=='T') countElement[3]++;
                if(res=='-') countElement[4]++;
            }

            idx_for_seq[k] = findMax(countElement, 5);
            //cout<<idx_for_seq[k]<<endl;
            /*if(idx_for_seq[k]==0) merged_seq[k]='A';
            if(idx_for_seq[k]==1) merged_seq[k]='C';
            if(idx_for_seq[k]==2) merged_seq[k]='G';
            if(idx_for_seq[k]==3) merged_seq[k]='T';
            if(idx_for_seq[k]==4) merged_seq[k]='-';*/

            if(idx_for_seq[k]==0) merged_seq.append("A");
            if(idx_for_seq[k]==1) merged_seq.append("C");
            if(idx_for_seq[k]==2) merged_seq.append("G");
            if(idx_for_seq[k]==3) merged_seq.append("T");
            if(idx_for_seq[k]==4) merged_seq.append("N");
        }

        //cout<<"Checking for error!"<<endl;
        //cout<<merged_seq<<endl;
        int temp = i+count_MissedBlocks-1;
        string start_del = ">id" + itoa(i+count_MissedBlocks-1) + "\n";   
        fprintf(ofp1, "%s", start_del.c_str());
        string info = merged_seq + "\n";
        fprintf(ofp1,"%s", info.c_str());

        delete[] idx_for_seq;
        //delete[] merged_seq;
    }

    fclose(ofp1);
    //cout<<lines[0]<<endl<<lines[1]<<endl<<lines[lines.size()-1]<<endl<<lines[lines.size()-2]<<endl;


}
