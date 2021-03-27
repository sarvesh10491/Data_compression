#include <iostream>
#include <fstream>
#include <math.h>
#include <string>

#define len(x) ((int)log10(x)+1)
#define charCount 94

using namespace std;

// Character frequencies. {0:a, .. , 26:z, 27:SPACE} etc.
// static int letterFreq [charCount] = {81, 15, 28, 43, 128, 23, 20, 61, 71, 2, 1, 40, 24, 69, 76, 20, 1, 61, 64, 91, 28, 10, 24, 1, 20, 1, 130};
static int letterFreq[charCount] = {
7528,2291,2573,2764,7093,1248,1853,2413,4697,837,1968,3777,2999,4569,5170,2456,346,4960,4611,3874,2102,834,1245,573,1525,633,
// a-z
130,81,66,70,97,42,50,54,71,36,46,78,78,75,73,74,15,85,108,80,35,24,32,14,26,17,
// A-Z
2744,4351,3123,2433,1943,1886,1756,1621,1662,1796,
// 0-9
20,9,31,9,9,10,9,9,9,9,24,9,32,9,9,9,9,24,9,9,9,9,12,9,9,9,9,9,9,9,9,9
//-	SPACE	!	"	#	$	%	&	(	)	*	,	.	/	:	;	?	@	[	\	]	^	_	`	{	|	}	~	+	<	=	>
};


// Node of the huffman tree
struct node{
    int value;
    char letter;
    struct node *left;
    struct node *right;
};
typedef struct node Node;

void buildHuffmanTree(Node **tree);
void buildEncodingTable(long codeTable[], Node *tree);
void fillTable(long codeTable[], Node *tree, long Code);
int findSmaller(Node *array[], int differentFrom);
void printEncodingTable(long codeTable[]);


// Builds tree from all subtrees based on Huffman algorithm
void buildHuffmanTree(Node **tree){
    Node *array[charCount];
    Node *temp;
    int subTrees = charCount;
    int smallOne, smallTwo;

    for(int i=0; i<charCount; i++){
        array[i] = (Node *)malloc(sizeof(Node));
        array[i]->value = letterFreq[i];
        array[i]->letter = i;
        array[i]->left = NULL;
        array[i]->right = NULL;
    }

    while(subTrees > 1){
        smallOne = findSmaller(array, -1);
        smallTwo = findSmaller(array, smallOne);

        temp = array[smallOne];
        array[smallOne] = (Node *)malloc(sizeof(Node));
        array[smallOne]->value = temp->value + array[smallTwo]->value;
        array[smallOne]->letter = 127;
        array[smallOne]->left = array[smallTwo];
        array[smallOne]->right = temp;
        array[smallTwo]->value = -1;

        subTrees--;
    }

    *tree = array[smallOne];
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

    for(i=1; i<charCount; i++){
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
void buildEncodingTable(long codeTable[], Node *tree){
    long n, copy;

    fillTable(codeTable, tree, 0);  // Create encoding table

    for(int i=0; i<charCount; i++){  // Invert the codes
        n = codeTable[i];
        copy = 0;
        while(n > 0){
            copy = copy*10 + n%10;
            n /= 10;
        }
        codeTable[i] = copy;
    }
}

void fillTable(long codeTable[], Node *tree, long Code){
    if(tree->letter < charCount)
        codeTable[(int)tree->letter] = Code;
    else{
        fillTable(codeTable, tree->left, Code*10+1);
        fillTable(codeTable, tree->right, Code*10+2);
    }
}

void printEncodingTable(long codeTable[]){
    for(int i=0; i<charCount; i++){
        cout << codeTable[i] << endl;
    }
}



void compressFile(ifstream &fin, ofstream &fout, long codeTable[]){
    if(!fin || !fout){
        cout << "Error in opening file!\n";
        return;
    }

    char ch, bit, x = 0;
    int codeLen, bitsLeft = 8;
    int originalBits = 0, compressedBits = 0;
    long chCode;

    while(fin){
        if(fin.get(ch)){
            originalBits++;

            if(ch >= 97 && ch <= 122){   // a-z
                codeLen = len(codeTable[ch-97]);
                chCode = codeTable[ch-97];
            }
            if(ch >= 65 && ch <= 90){   // A-Z
                codeLen = len(codeTable[ch-39]);
                chCode = codeTable[ch-39];
            }
            if(ch >= 48 && ch <= 57){   // 0-9
                codeLen = len(codeTable[ch+4]);
                chCode = codeTable[ch+4];
            }
            if(ch == 45){   // -
                codeLen = len(codeTable[62]);
                chCode = codeTable[62];
            }
            if(ch == 32){   // SPACE
                codeLen = len(codeTable[63]);
                chCode = codeTable[63];
            }
            if(ch == 33){   // !
                codeLen = len(codeTable[64]);
                chCode = codeTable[64];
            }
            if(ch == 34){   // "
                codeLen = len(codeTable[65]);
                chCode = codeTable[65];
            }
            if(ch == 35){   // #
                codeLen = len(codeTable[66]);
                chCode = codeTable[66];
            }
            if(ch == 36){   // $
                codeLen = len(codeTable[67]);
                chCode = codeTable[67];
            }
            if(ch == 37){   // %
                codeLen = len(codeTable[68]);
                chCode = codeTable[68];
            }
            if(ch == 38){   // &
                codeLen = len(codeTable[69]);
                chCode = codeTable[69];
            }
            if(ch == 40){   // (
                codeLen = len(codeTable[70]);
                chCode = codeTable[70];
            }
            if(ch == 41){   // )
                codeLen = len(codeTable[71]);
                chCode = codeTable[71];
            }
            if(ch == 42){   // *
                codeLen = len(codeTable[72]);
                chCode = codeTable[72];
            }
            if(ch == 44){   // ,
                codeLen = len(codeTable[73]);
                chCode = codeTable[73];
            }
            if(ch == 46){   // .
                codeLen = len(codeTable[74]);
                chCode = codeTable[74];
            }

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

    // Print compression stats
    fprintf(stderr, "Original bits = %d\n", originalBits*8);
    fprintf(stderr, "Compressed bits = %d\n", compressedBits);
    fprintf(stderr, "Memory consumption to original = %.2f%%\n", ((float)compressedBits/(originalBits*8))*100);

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
                    if(cur->letter < 26)   // a-z
                        fout.put(cur->letter+97);
                    if(cur->letter >= 26 && cur->letter <= 51)   // A-Z
                        fout.put(cur->letter+39);
                    if(cur->letter >= 52 && cur->letter <= 61)   // 0-9
                        fout.put(cur->letter-4);
                    if(cur->letter == 62)   // -
                        fout.put(45);
                    if(cur->letter == 63)   // SPACE
                        fout.put(32);
                    if(cur->letter == 64)   // !
                        fout.put(33);
                    if(cur->letter == 65)   // "
                        fout.put(34);
                    if(cur->letter == 66)   // #
                        fout.put(35);
                    if(cur->letter == 67)   // $
                        fout.put(36);
                    if(cur->letter == 68)   // %
                        fout.put(37);
                    if(cur->letter == 69)   // &
                        fout.put(38);
                    if(cur->letter == 70)   // (
                        fout.put(40);
                    if(cur->letter == 71)   // )
                        fout.put(41);
                    if(cur->letter == 72)   // *
                        fout.put(42);
                    if(cur->letter == 73)   // ,
                        fout.put(44);
                    if(cur->letter == 74)   // .
                        fout.put(46);


                    cur = tree;
                }
            }
        }
    }
}


int main(int argc, char** argv){
    Node *tree;
    long codeTable[charCount];

    ifstream fin;
    ofstream fout;

    buildHuffmanTree(&tree);
    buildEncodingTable(codeTable, tree);
    printEncodingTable(codeTable);

    cout << "\n====================================\n";

    if(std::string(argv[1]) == "-c"){
        cout << "Compressing file..\n\n";

        fin.open("somefile.txt", ios::in);
        fout.open("compressed_somefile.cmpr", ios::out);

        compressFile(fin, fout, codeTable);
    }
    if(std::string(argv[1]) == "-d"){
        cout << "Decompressing file..\n\n";

        fin.open("compressed_somefile.cmpr", ios::in);
        fout.open("decompressed_somefile.txt", ios::out);

        decompressFile(fin, fout, tree);
    }
    cout << "\n====================================\n";

    return 0;
}