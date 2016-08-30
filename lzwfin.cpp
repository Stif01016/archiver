#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <cstring>

#define EOF_CODE 256
typedef unsigned int uint;


class Output_code
{
public:
    Output_code(std::ostream &output, uint max_code)
        :b_output(output),
         b_pending_bit(0),
         b_pending_out(0),
         b_code_size(9),
         b_current_code(256),
         b_next_bump(512),
         b_max_code(max_code)
    {}
    ~Output_code()
    {
        *this<<EOF_CODE;
        flush(0);
    }
    void operator<<(const int &i)
    {
        b_pending_out|=i<<b_pending_bit;
        b_pending_bit+=b_code_size;
        flush(8);
        if(b_current_code<b_max_code){
            b_current_code++;
            if(b_current_code==b_next_bump){
                b_next_bump*=2;
                b_code_size++;
            }
        }
    }
private:
    void flush(const int value)
    {
        while(b_pending_bit>=value){
            b_output.put(b_pending_out & 0xff);
            b_pending_out>>=8;
            b_pending_bit-=8;
        }
    }
    int b_code_size;
    std::ostream &b_output;
    int b_pending_bit;
    uint b_pending_out;
    uint b_current_code;
    uint b_next_bump;
    uint b_max_code;
};

class Input_symbol
{
public:
    Input_symbol(std::istream &input)
        :e_input(input)
    {}
    bool operator>>(char &c)
    {
        if(!e_input.get(c))
            return false;
        else
            return true;
    }
private:
    std::istream &e_input;
};

class Input_code
{
public:
    Input_code(std::istream &input,uint max_code)
        :e_input(input),
         e_available_bit(0),
         e_pending_input(0),
         e_code_size(9),
         e_current_code(256),
         e_next_bump(512),
         e_max_code(max_code)
    {}
    bool operator>>(uint &i)
    {
        while(e_available_bit<e_code_size){
            char c;
            if(!e_input.get(c))
                return false;
            e_pending_input|=(c & 0xff) << e_available_bit;
            e_available_bit+=8;
        }
        i=e_pending_input & ~(~0<<e_code_size);
        e_pending_input>>=e_code_size;
        e_available_bit-=e_code_size;
        if(e_current_code<e_max_code){
            e_current_code++;
            if( e_current_code==e_next_bump){
                e_next_bump*=2;
                e_code_size++;
            }
        }
        if (i==EOF_CODE)
            return false;
        else
            return true;
    }
private:
    int e_code_size;
    std::istream &e_input;
    int e_available_bit;
    uint e_pending_input;
    uint e_current_code;
    uint e_next_bump;
    uint e_max_code;
};

void Compress(std::istream &INPUT, std::ostream &OUTPUT, const uint max_code=32767)
{
    Output_code out(OUTPUT,max_code);
    Input_symbol in(INPUT);
    std::unordered_map <std::string,uint>  codes((max_code*11)/10);
    for (uint i=0; i<256; i++)
        codes[std::string(1,i)]=i;
    uint next_code=257;
    std::string current_string;
    char c;
    while(in>>c){
        current_string=current_string+c;
        if(codes.find(current_string)==codes.end()){
            if(next_code<=max_code)
                codes[current_string]=next_code++;
            current_string.erase(current_string.size()-1);
            out<<codes[current_string];
            current_string=c;
        }
    }
    if(current_string.size())
        out<<codes[current_string];
}


void Decompress(std::istream &INPUT, std::ostream &OUTPUT, const uint max_code=32767)
{
    Input_code in(INPUT,max_code);
    std::unordered_map<uint,std::string> strings((max_code*11)/10);
    for(uint i=0;i<256;i++)
        strings[i]=std::string(1,i);
    std::string previous_string;
    uint code;
    uint next_code=257;
    while(in>>code){
        if(strings.find(code)==strings.end())
            strings[code]=previous_string+previous_string[0];
        OUTPUT<<strings[code];
        if(previous_string.size() && next_code<=max_code)
            strings[next_code++]=previous_string+strings[code][0];
        previous_string=strings[code];
    }
}

int main()
{
    int max_code=32767;
    std::string output_filname;
    std::string input_filename;
    std::cin>>input_filename;
    std::cin>>output_filname;
    std::istream *in=&std::cin;
    std::ostream *out=&std::cout;
    in=new std::ifstream(input_filename);
    out=new std::ofstream(output_filname);
    int choose;
    std::cin>>choose;
    switch(choose)
        {
        case 1:
            Compress(*in,*out,max_code);
        case 2:
            Decompress(*in,*out,max_code);
        }
    delete in;
    delete out;
}
