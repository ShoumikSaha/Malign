#include<bits/stdc++.h>
#include <string>
#include <stdlib.h>
#include <sys/stat.h>

using namespace std;

FILE *File;
FILE *ofp1;
FILE *ofp2;
FILE *infile;
long lSize;

int total_ids = 16118;
int id_match_count[16118]; ///This one for type-6_0
int id_match_score[16118];

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

string parseFile(string fileName)
{
    File = fopen(fileName.c_str(), "rb");
    if (!File)
    {
        cout << "Unable to open file "<<fileName<<endl;
        //exit(1);   // call system to stop
        return "no file";
    }
    fseek(File, 0, SEEK_END);
    long FileSize = ftell(File);
    rewind(File);
    char * buffer;
    buffer = (char*) malloc(sizeof(char)*FileSize);
    if(buffer==NULL)
    {
        fputs ("Reading error", stderr);
        //exit(3);
        return "error";
    }
    size_t result = fread(buffer, 1, FileSize, File);
    if(result!=FileSize)
    {
        return "error";
    }
    fclose(File);

    string str = buffer;
    free(buffer);

    return str;
}

long get_file_size(string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

string getScoreIdSegment(int id_from_maf, vector<string> scoreSegments)
{
    string tempSegment = " ";
    int startSearchId = ((id_from_maf-10)<=0) ? 0 : (id_from_maf-10);
    int endSearchId = ((id_from_maf+10)>=total_ids-2) ? total_ids-2 : (id_from_maf+10);
    for(int i=startSearchId; i<endSearchId; i++)
    {
        tempSegment = scoreSegments[i];
        vector<string> tempSegmentLines = splitter(tempSegment, "\n");
        vector<string> tempSegmentWords = splitter(tempSegmentLines[0], ",");
        if(tempSegmentWords[0].substr(0,2)!="id" || tempSegmentWords[0].length()<=2) continue;
        string idNo = tempSegmentWords[0].substr(2, tempSegmentWords[0].length()-2);
        if(atoi(idNo.c_str())==(id_from_maf)) break;
    }
    return tempSegment;
}

float calc_score(string seq, string idSeq, int idTotal_seq, float arrA[], float arrC[], float arrG[], float arrT[], float arrGap[], string temp_order)
{
    //cout<<"Entering calc score function!"<<endl;
    int order;
    if(temp_order=="+") order=1;
    else order=0;

    float score = 0.00;
    int idxForArray=0;
    for(int i=0; i<seq.length(); i++)
    {

        if(seq[i]=='-')
        {
            if(idSeq[i]!='-')
            {
                score += arrGap[idxForArray];
                idxForArray++;
            }
            continue;
        }
        else if(idSeq[i]=='-') continue;
        else
        {
            if(seq[i]=='A') score += (order==1)? arrA[idxForArray]:arrT[idxForArray];
            if(seq[i]=='C') score += (order==1)? arrC[idxForArray]:arrG[idxForArray];
            if(seq[i]=='G') score += (order==1)? arrG[idxForArray]:arrC[idxForArray];
            if(seq[i]=='T') score += (order==1)? arrT[idxForArray]:arrA[idxForArray];
        }

        if(idSeq[i]!='-') idxForArray++;

    }
    //cout<<"IDXforArray: "<<idxForArray<<endl;
    score = score*idTotal_seq*1.00;
    return score;
}

void dataset_writer(string fasta_name, vector <string> scoreSegments, int type_no)
{

    string directory = "Alignments/" + fasta_name + "/alignment.maf";
    string alignmentString = parseFile(directory.c_str());
    if(alignmentString=="no file" || alignmentString=="error") return;
    long file_size = get_file_size(directory);
    if(file_size<=1100 || file_size==-1) return;

    vector <string> alignmentSegments = splitter(alignmentString, "a\n");
    cout<<"Total Alignment Segments: "<< alignmentSegments.size();
    cout<<endl;

    //int alignmentSegments_to_check[alignmentSegments.size()][2]; ///col1 for the line number, col2 for +/-
    //memset(alignmentSegments_to_check, -1, sizeof(alignmentSegments_to_check));
    int count_normal_reverse_alignmentInSegment[alignmentSegments.size()][2]; ///col1 for + type, col2 for - type
    memset(count_normal_reverse_alignmentInSegment, 0, sizeof(count_normal_reverse_alignmentInSegment));
    vector<vector <string> > new_fasta_seq (alignmentSegments.size());
    int order_to_check_new_fasta[alignmentSegments.size()];  ///to store the '+' / '-' of new fasta
    vector<vector<int> > new_fasta_in_alignmentSegments (alignmentSegments.size());
    //memset(new_fasta_seq, "", sizeof(new_fasta_seq));



    int count_segments_with_new_fasta=0;
    for(int i=2; i<alignmentSegments.size(); i++)
    {
        vector <string> alignmentLines = splitter(alignmentSegments[i], "\n");
        int total_seq = alignmentLines.size()-2;
        for(int j=0; j<total_seq; j++)
        {
            vector <string> words = splitter(alignmentLines[j], " ");
            if(words[1]==fasta_name)
            {
                //alignmentSegments_to_check[i][0] = j;
                new_fasta_in_alignmentSegments[i].push_back(j);
                order_to_check_new_fasta[i] = (words[4]=="+") ? 1 : -1;
                new_fasta_seq[i].push_back(words[6]);
                count_segments_with_new_fasta++;
                if(words[4]=="+") count_normal_reverse_alignmentInSegment[i][0]++;
                else if(words[4]=="-") count_normal_reverse_alignmentInSegment[i][1]++;
                //cout<<new_fasta_seq[i]<<endl;
                continue;
            }

        }
    }
    cout << "Initial steps done" << endl;
    float Final_Score = 0.00;
    for(int i=2; i<alignmentSegments.size(); i++)
    {
        if(new_fasta_in_alignmentSegments[i].size()>0)
        {
            vector <string> alignmentLines = splitter(alignmentSegments[i], "\n");
            int total_seq = alignmentLines.size()-2;
            float score = 0.00;
            if(new_fasta_in_alignmentSegments[i].size()>=total_seq) continue;  ///only new fasta's blocks in the segment

            for(int j=0; j<total_seq; j++)
            {
                int isNewFastaLine = 0;
                for(int m=0; m<new_fasta_in_alignmentSegments[i].size(); m++)
                {
                    if(new_fasta_in_alignmentSegments[i][m]==j)
                    {
                        isNewFastaLine=1;
                        break;
                    }
                }
                if(isNewFastaLine==1) continue;

                vector <string> words = splitter(alignmentLines[j], " ");
                if(words.size()<7)
                {
                    continue;
                }
                else
                {
                    cout << words[1] << ",";
                }
                if(words[1].substr(0, 2)!="id" || words[1].length()<=2) continue;
                string idNo = words[1].substr(2, words[1].length()-2);
                cout << idNo << endl;
                //string scoreIdSegment = scoreSegments[atoi(idNo.c_str())];
                string scoreIdSegment = getScoreIdSegment(atoi(idNo.c_str()), scoreSegments);
                //cout << scoreIdSegment << endl;
                //if(scoreIdSegment==" "||"N/A\n") continue;

                string idSeq = words[6];
                int startIdx, endIdx, length;

                if(count_normal_reverse_alignmentInSegment[i][1]==0 && words[4]=="-") continue;  ///new fasta in normal but id in reverse
                if(count_normal_reverse_alignmentInSegment[i][0]==0 && words[4]=="+") continue;  ///new fasta in reverse but id in normal

                id_match_count[atoi(idNo.c_str())] += new_fasta_in_alignmentSegments[i].size();  ///added for ML dataset
                cout << id_match_count[atoi(idNo.c_str())] << endl;

                if(words[4]=="-")
                {
                    endIdx = atoi(words[5].c_str()) - atoi(words[2].c_str());
                    length = atoi(words[3].c_str());
                    startIdx = endIdx-length;
                }
                else if(words[4]=="+")
                {
                    startIdx = atoi(words[2].c_str());
                    length = atoi(words[3].c_str());
                    endIdx = startIdx + length;
                }

                vector <string> scoreLines = splitter(scoreIdSegment, "\n");
                //cout<<scoreLines[0]<<endl;
                vector <string> commaDelim = splitter(scoreLines[0], ",");
                vector <string> equalDelim = splitter(commaDelim[2], "=");
                int idTotal_seq = atoi(equalDelim[1].c_str());
                /*cout<<idTotal_seq<<endl;
                cout<<startIdx<<"  "<<endIdx<<endl;*/
                float scores[5][length];
                for(int k=1; k<=5; k++)
                {
                    vector <string> dashDelim = splitter(scoreLines[k], "-");
                    vector <string> commaScores = splitter(dashDelim[1], ",");
                    /*if(k==1) cout<<"A-";
                    else if(k==2) cout<<"C-";
                    else if(k==3) cout<<"G-";
                    else if(k==4) cout<<"T-";
                    else if(k==5) cout<<"Gap-";*/
                    if(words[4]=="+")
                    {
                        for(int l=startIdx; l<endIdx; l++)
                        {
                            //cout<<commaScores[l]<<" ";
                            scores[k-1][l-startIdx] = atof(commaScores[l].c_str());
                        }
                    }
                    else if(words[4]=="-")
                    {
                        for(int l=endIdx-1; l>=startIdx; l--)
                        {
                            //cout<<commaScores[l]<<" ";
                            scores[k-1][endIdx-1-l] = atof(commaScores[l].c_str());
                        }
                    }

                }

                for(int m=0; m<new_fasta_in_alignmentSegments[i].size(); m++)
                {
                    /*cout<<new_fasta_seq[i][m]<<endl;
                    cout<<idSeq<<endl;
                    cout<<idTotal_seq<<endl;
                    cout<<words[4]<<endl;*/
                    float temp_score = calc_score(new_fasta_seq[i][m], idSeq, idTotal_seq, scores[0], scores[1], scores[2], scores[3], scores[4], words[4]);
                    score = max(score, temp_score);
                    //cout<<"SCORE: "<<score<<endl;
                }

                //if(words[4]=="+" && score!=0 && count_normal_reverse_alignmentInSegment[i][0]!=0) score = score / (count_normal_reverse_alignmentInSegment[i][0]*1.00);
                //if(words[4]=="-" && score!=0 && count_normal_reverse_alignmentInSegment[i][1]!=0) score = score / (count_normal_reverse_alignmentInSegment[i][1]*1.00);
                //fprintf(ofp1, "id%s: ", idNo.c_str());
                //fprintf(ofp1, "%f\n", score);
                id_match_score[atoi(idNo.c_str())] += (int) score;
                cout << id_match_score[atoi(idNo.c_str())] << endl;
                score = 0.00;
            }

        }

    }
    cout << "Score Calculation Done" << endl;
    if(type_no==6)
    {
        fprintf(ofp1, "%s,",fasta_name.c_str());
        for(int i=1; i<total_ids; i++)
        {
            fprintf(ofp1, "%d,", id_match_count[i]);
            fprintf(ofp1, "%d,", id_match_score[i]);
        }
        fprintf(ofp1, "\n");
    }
    else
    {
        fprintf(ofp2, "%s,",fasta_name.c_str());
        for(int i=1; i<total_ids; i++)
        {
            fprintf(ofp2, "%d,", id_match_count[i]);
            fprintf(ofp2, "%d,", id_match_score[i]);
        }
        fprintf(ofp2, "\n");
    }
}

int main()
{

    string scoreString = parseFile("sibeliaz_out/Score_output.txt");
    vector <string> scoreSegments = splitter(scoreString, "a\n");
    cout<<"Total Score Segments: "<< scoreSegments.size();
    cout<<endl;
    for(int i=0; i<5; i++)
    {
        cout<<scoreSegments[i]<<endl;
    }


    string pos_output_csv_file = "output/dataset_pos.csv";
    ofp1 = fopen(pos_output_csv_file.c_str(), "w");
    if(!ofp1)
    {
        cout << "Unable to open output file!! "  << endl;
        return -1;
    }

    string neg_output_csv_file = "output/dataset_neg.csv";
    ofp2 = fopen(neg_output_csv_file.c_str(), "w");
    if(!ofp2)
    {
        cout << "Unable to open output file!! "  << endl;
        return -1;
    }


    string cmdFile_name = "cmd_for_alignment.sh";
    string cmdFile_string = parseFile(cmdFile_name.c_str());
    //cout << cmdFile_string << endl;
    vector<string> cmdFile_lines = splitter(cmdFile_string, "\n");
    for(int i=0; i<cmdFile_lines.size(); i++)
    {
        //if(i<=125) continue;
        //cout << cmdFile_lines[i] << endl;
        vector<string> cmd_File_line_file = splitter(cmdFile_lines[i], " ");
        string file_name = cmd_File_line_file[4];
        cout << file_name << endl;
        vector<string> file_name_splitted = splitter(file_name, "/");
        file_name = file_name_splitted[2];
        file_name = splitter(file_name, ".")[0];
        cout << file_name << endl;
        string family_no = file_name_splitted[0];
        cout<<family_no<<endl;
        dataset_writer(file_name, scoreSegments, atoi(family_no.c_str()));
    }
    fclose(ofp1);
    fclose(ofp2);

}
