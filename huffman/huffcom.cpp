#include <iostream>
#include <fstream>
#include <math.h>
#include <string>


#define len(x) ((int)log10(x)+1)

using namespace std;

// 81 = 8.1%, 128 = 12.8% and so on. The 27th frequency is the space.
int letterFreq [27] = {81, 15, 28, 43, 128, 23, 20, 61, 71, 2, 1, 40, 24, 69, 76, 20, 1, 61, 64, 91, 28, 10, 24, 1, 20, 1, 130};

/* Node of the huffman tree */
struct node{
    int value;
    char letter;
    struct node *left;
    struct node *right;
};
typedef struct node Node;

void buildHuffmanTree(Node **tree);
void buildEncodingTable(int codeTable[], Node *tree);
void fillTable(int codeTable[], Node *tree, int Code);
int findSmaller(Node *array[], int differentFrom);
void printEncodingTable(int codeTable[]);


// Builds tree from all subtrees based on Huffman algorithm
void buildHuffmanTree(Node **tree){
    Node *array[27];
    Node *temp;
    int subTrees = 27;
    int smallOne, smallTwo;

    for(int i=0; i<27; i++){
        array[i] = (Node *)malloc(sizeof(Node));
        array[i]->value = letterFreq[i];
        array[i]->letter = i;
        array[i]->left = NULL;
        array[i]->right = NULL;
    }

    // printTrees(array);

    while(subTrees > 1){
        // cout << "\nSubtrees " << subTrees << endl;
        smallOne = findSmaller(array, -1);
        smallTwo = findSmaller(array, smallOne);

        // cout << smallOne << " " << smallTwo << "|" << endl;

        temp = array[smallOne];
        array[smallOne] = (Node *)malloc(sizeof(Node));
        array[smallOne]->value = temp->value + array[smallTwo]->value;
        array[smallOne]->letter = 127;
        array[smallOne]->left = array[smallTwo];
        array[smallOne]->right = temp;
        array[smallTwo]->value = -1;

        // printOneTree(array[smallOne]);

        subTrees--;
    }

    *tree = array[smallOne];
    // printTrees(array);
}

int findSmaller(Node *array[], int differentFrom){  // Finds and returns the small sub-tree in the forest
    int smaller;
    int i = 0;

    while(array[i]->value == -1)
        i++;

    smaller = i;

    if(i == differentFrom){
        i++;
        while(array[i]->value==-1)
            i++;
        smaller = i;
    }

    for(i=1; i<27; i++){
        if(array[i]->value == -1)
            continue;
        if(i == differentFrom)
            continue;
        if(array[i]->value < array[smaller]->value)
            smaller = i;
    }

    return smaller;
}


// Builds the table with the bits for each letter from Humffman tree.
void buildEncodingTable(int codeTable[], Node *tree){
    int n, copy;

    fillTable(codeTable, tree, 0);  // Create encoding table

    for(int i=0;i<27;i++){  // Invert the codes
        n = codeTable[i];
        copy = 0;
        while(n>0){
            copy = copy*10 + n%10;
            n /= 10;
        }
        codeTable[i] = copy;
    }
}

void fillTable(int codeTable[], Node *tree, int Code){
    if(tree->letter < 27)
        codeTable[(int)tree->letter] = Code;
    else{
        fillTable(codeTable, tree->left, Code*10+1);
        fillTable(codeTable, tree->right, Code*10+2);
    }
}

void printEncodingTable(int codeTable[]){
    for(int i=0; i<27; i++){
        cout << codeTable[i] << endl;
    }
}



void compressFile(ifstream &fin, ofstream &fout, int codeTable[]){
    if(!fin || !fout){
        cout << "Error in opening file!\n";
        return;
    }

    char ch, bit, x = 0;
    int chCode, codeLen, bitsLeft = 8;
    int originalBits = 0, compressedBits = 0;

    while(fin){
        if(fin.get(ch)){
            originalBits++;

            if(ch == 32){
                codeLen = len(codeTable[26]);
                chCode = codeTable[26];
            }
            else{
                codeLen = len(codeTable[ch-97]);
                chCode = codeTable[ch-97];
            }
            // cout << ch << " - " << chCode << " - " << codeLen << endl;

            while(codeLen > 0){
                compressedBits++;

                bit = chCode % 10 - 1;  // Strip each bit of code
                chCode /= 10;
                x |= bit;   // Keep ORing to build coded char or original
                codeLen--;
                bitsLeft--;

                if (bitsLeft == 0){     // if no bits left to OR, write x to output file clear & set bitsLeft for next char
                    fout.put(x);
                    x = 0;
                    bitsLeft = 8;
                }
                x <<= 1;
            }
        }
    }

    if (bitsLeft != 8){
        x <<= (bitsLeft-1);
        fout.put(x);
    }

    /*print details of compression on the screen*/
    fprintf(stderr, "Original bits = %d\n", originalBits*8);
    fprintf(stderr, "Compressed bits = %d\n", compressedBits);
    fprintf(stderr, "Saved memory = %.2f%%\n", ((float)compressedBits/(originalBits*8))*100);

}


void decompressFile(ifstream &fin, ofstream &fout, Node *tree){
    if(!fin || !fout){
        cout << "Error in opening file!\n";
        return;
    }

    Node *cur;
    cur = tree;
    char ch, bit;
    char mask = 1 << 7;     // Mask 1000 0000 used ti strip MSBs

    while(fin){
        if(fin.get(ch)){
            for(int i=0; i<8; i++){
                bit = ch & mask;    // Strip MSBs
                ch <<= 1;           // Left shift to remove MSB

                if(bit == 0)
                    cur = cur->left;
                else
                    cur = cur->right;

                if(cur->letter != 127){
                    if(cur->letter == 26)   // SPACE
                        fout.put(32);
                    else
                        fout.put(cur->letter+97);   // a-z
                    cur = tree;
                }
            }
        }
    }
}


int main(int argc, char** argv){
    Node *tree;
    int codeTable[27];

    ifstream fin;
    ofstream fout;

    buildHuffmanTree(&tree);
    buildEncodingTable(codeTable, tree);
    printEncodingTable(codeTable);

    cout << "\n====================================\n";

    if(std::string(argv[1]) == "-c"){
        cout << "Compressing file..\n\n";

        fin.open("somefile.txt", ios::in);
        fout.open("compressed_somefile.txt", ios::out);

        compressFile(fin, fout, codeTable);
    }
    if(std::string(argv[1]) == "-d"){
        cout << "Decompressing file..\n\n";

        fin.open("compressed_somefile.txt", ios::in);
        fout.open("decompressed_somefile.txt", ios::out);

        decompressFile(fin, fout, tree);
    }
    cout << "\n====================================\n";

    return 0;
}