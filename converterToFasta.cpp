#include <bits/stdc++.h>
#include <string>
#include <stdlib.h>
#include <iomanip>


using namespace std;

FILE *File;
FILE *ofp1;
long lSize;
size_t result;

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

void convertToFasta(string filename)
{
    string outputFileName = "Fasta_Modified/" + filename + ".fasta";
    ofp1 = fopen(outputFileName.c_str(), "w");

    fprintf(ofp1, ">%s\n", filename.c_str());

    string byteCode = parseFile(filename + ".txt");
    vector<string> codeLines = splitter(byteCode, "\n");
    for(int i=0; i<codeLines.size(); i++)
    {
        //cout<<codeLines[i]<<endl;
        string line = codeLines[i];
        for(int j=0; j<line.length(); j++)
        {
            switch(line[j])
            {
            case '0':
                fprintf(ofp1, "%s", "AA");
                break;
            case '1':
                fprintf(ofp1, "%s", "AC");
                break;
            case '2':
                fprintf(ofp1, "%s", "AG");
                break;
            case '3':
                fprintf(ofp1, "%s", "AT");
                break;
            case '4':
                fprintf(ofp1, "%s", "CA");
                break;
            case '5':
                fprintf(ofp1, "%s", "CC");
                break;
            case '6':
                fprintf(ofp1, "%s", "CG");
                break;
            case '7':
                fprintf(ofp1, "%s", "CT");
                break;
            case '8':
                fprintf(ofp1, "%s", "GA");
                break;
            case '9':
                fprintf(ofp1, "%s", "GC");
                break;
            case 'a':
                fprintf(ofp1, "%s", "GG");
                break;
            case 'b':
                fprintf(ofp1, "%s", "GT");
                break;
            case 'c':
                fprintf(ofp1, "%s", "TA");
                break;
            case 'd':
                fprintf(ofp1, "%s", "TC");
                break;
            case 'e':
                fprintf(ofp1, "%s", "TG");
                break;
            case 'f':
                fprintf(ofp1, "%s", "TT");
                break;
            }
        }
        fprintf(ofp1, "\n");
    }

    fclose(ofp1);
}

int main()
{
    string fileList = parseFile("FileList.txt");
    vector<string> fileNames = splitter(fileList, "\n");
    for(int i=0; i<fileNames.size(); i++)
    {
        cout<<fileNames[i]<<endl;
        convertToFasta(fileNames[i]);
        //break;
    }
    
    //cout<<codeLines.size()<<endl;
    return 0;
}