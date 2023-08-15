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

string toBase( unsigned i, unsigned base )
{ 
    const string DIGITS = "0123456789abcdef";
    //fprintf(ofp1, "(%d)", i);
    return i ? toBase( i / base, base ) + DIGITS[i%base] : "";
}


void converter(string filename)
{
    string outputFileName = "Modified/" + filename + ".txt";
    ofp1 = fopen(outputFileName.c_str(), "w");
    //string filename = "001";
    unsigned base = 16;

    const int LINELENGTH = 16;

    int width = toBase( 255, base ).size();
    char c;
    int counter = 0;

    ifstream in( filename, ios::binary );
    while ( in.get( c ) )
    {
    //if(c==' ' || c=='\n') continue;
    //cout << setw( width + 1 ) << toBase( (unsigned)c, base );
    //cout << "(" << c << ")";
        if((unsigned)c<16) 
        {
            //cout<<"0";
            fprintf(ofp1, "0");
            if((unsigned)c==0)
            {
                fprintf(ofp1, "0");
            }
        }
        //cout << toBase( (unsigned)c, base );
        /*if(toBase((unsigned)c, base) == "FFFFFF")
        {
            continue;
        }*/
        if(toBase((unsigned)c, base).length()>2)
        {
            string temp = toBase((unsigned)c, base);
            temp = temp.substr(6, temp.length()-6);
            fprintf(ofp1, "%s", temp.c_str());
            //continue;
        }
        else
        {
            fprintf(ofp1, "%s", toBase( (unsigned)c, base ).c_str());
        }
        
        counter++;
        if ( counter%LINELENGTH == 0 ) 
        {
            //cout << '\n';
            fprintf(ofp1, "\n");
        }
    }
    fclose(ofp1);
}


int main( int argc, char *argv[] )
{
    string fileList = parseFile("FileList.txt");
    vector<string> fileNames = splitter(fileList, "\n");
    for(int i=0; i<fileNames.size(); i++)
    {
        cout<<fileNames[i]<<endl;
        converter(fileNames[i]);
        //break;
    }
    return 0;   
}