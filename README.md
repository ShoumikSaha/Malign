# Malign
Malign is a static malware family classification tool that adopts multiple sequence alignment (MSA) with necessary pre-processing and leverages simple machine-learning model. It takes in hex-byte code of executable files. It not only classifies the malware family but also can find potentially malicious code segments. Read our paper for more details - https://arxiv.org/pdf/2111.14185.pdf

## Requirements
- C++11
- Python 3.8
- SibeliaZ

## Installing with conda

SibeliaZ can be installed using [bioconda](https://bioconda.github.io/). 
After installing bioconda, run this command - 
```
conda install sibeliaz
```

For more details, read the documentation of [SibeliaZ](https://github.com/medvedevgroup/SibeliaZ).

## Pipeline
We will be explaining each step in the pipeline of Malign.

### Phase 1

- **Preprocessing** 

  Run `converter.cpp` which takes any `.txt` file containing the file path that needed to be converted. It will generate new files in a folder named 'Modified'. Then run `convertedToFasta.cpp` on these newly generated files. After this, you will get the `.fasta` version of your hex files and you will use these files in the next steps.

- **Running MSA**

  We will apply multiple sequence alignment on the fasta files using SibeliaZ. Run this command -
  ```
  sibeliaz -o sibeliaz_out <fasta_file1> <fasta_file2> <fasta_file3>
  ```
  These fasta files have to be the malware files from the family of interest. Running this command will generate `blocks` folder,  `alignment.maf` and `blocks_coords.gff` files inside the 'sibeliaz_out' folder.

- **Consensus Sequence and Score Generation**

  Run `concat_fasta.cpp` file takes in the `alignment.maf` file and generates the `merged_sequence.fasta` file that we will be using as the *consensus sequence* for that malware family.

  Run `score_calc.cpp` file that takes in the same `alignment.maf` file and  generates the `Score_output.txt` file that stores the scores of all coordinates for each consensus sequence. 


### Phase 2

- **Alignment with Consensus Sequences**

  Run `sibeliaz_cmd_maker.cpp` file that takes in the list of files that you want to align with the generated *consensus sequences* from Phase 1. This will generate a script named `cmd_for_alignment.sh` that will contain multiple sibeliaz alignment commands each having an alignment command between the `merged_sequence.fasta` file and file from your list.
  Then run the generated `cmd_for_alignment.sh` script. It will automatically create folder for each files from your list and store the alignment output.

- **Input Generation for Classification Model**
  
  Run `csv_file_maker.cpp` file that will take in the `score_output.txt` file generated in Phase 1. It will iterate through every `alignment.maf` file from the previous step, and generate *alignment score* for each of them. These scores will be saved in a `.csv` file with the file name.

- **Classification**

  Now you can use any classification model as your choice on the generated `.csv` file. We have uploaded python notebooks for Logisitic Regression, Decision Tree and SVM models.


# Citation

If you use Malign in your research, please cite:
```
@article{saha2021malign,
  title={MALIGN: Adversarially Robust Malware Family Detection using Sequence Alignment},
  author={Saha, Shoumik and Afroz, Sadia and Rahman, Atif},
  journal={arXiv preprint arXiv:2111.14185},
  year={2021}
}
```
  
