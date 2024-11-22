#include <iostream> 
#include <fstream>   
#include <string>
#include <vector>
#include <bitset>
#include <regex>
using namespace std;

class DATA_LABEL;
class TEXT_LABEL;
class DATA;
class TEXT;

// ============= class implementation


class DATA_LABEL {
private:
    int address;
    vector<DATA*>data_list;
    string line;
public:
    DATA_LABEL(string line);
    static vector<DATA_LABEL*>data_label_list;
    void PushData(DATA *DATA) {data_list.push_back(DATA);} // 자식 데이터 추가
    vector<DATA*> GetDataList() {return data_list;} // 한 라벨의 자식 리턴
    int GetAddress() {return address;}
    string GetLine() {return line;}
    static int forward_address;
};

int DATA_LABEL::forward_address = 0;
vector<DATA_LABEL*> DATA_LABEL::data_label_list; // static 정의
DATA_LABEL::DATA_LABEL(string line) {
    if (!data_label_list.empty()) {
        this->address = forward_address + 4*(data_label_list.back()->data_list.size());
    } 
    else {
        this->address = 0x10000000;
    }
    forward_address = address;
    data_label_list.push_back(this); // 자신을 list에 추가
    this->line = line;
}

class TEXT_LABEL {
private:
    int address;
    vector<TEXT*>text_list;
    string line;
public:
    TEXT_LABEL(string line);
    static vector<TEXT_LABEL*>text_label_list;
    void PushText(TEXT *TEXT) {text_list.push_back(TEXT);} // 자식 데이터 추가
    vector<TEXT*> GetTextList() {return text_list;}
    int GetAddress() {return address;}
    string GetLine() {return line;}
    static int forward_address;
};

int TEXT_LABEL::forward_address = 0;
vector<TEXT_LABEL*> TEXT_LABEL::text_label_list; // static 정의
TEXT_LABEL::TEXT_LABEL(string line) { // 한 라벨의 자식 리턴
    if (!text_label_list.empty()) {
        this->address = forward_address + 4*(text_label_list.back()->text_list.size());
    } 
    else {
        this->address = 0x00400000;
    }
    forward_address = address;
    text_label_list.push_back(this); // 자신을 list에 추가
    this->line = line;
}

class DATA {
private: 
    int address;
    int data;
public:
    DATA(int data);
    static int count_datasec;
    int GetData() {return data;}
    int GetAddress() {return address;}
};

int DATA::count_datasec = 0; // static 정의
DATA::DATA(int data) {
    this->data = data;
    this->address = 0x10000000 + 4*count_datasec;
    count_datasec++;
}

class TEXT {
private:
    int address;
    string line;
public:
    TEXT(string line); // constructor
    static int count_textsec;
    string GetText() {return line;}
    int GetAddress() {return address;}
};

int TEXT::count_textsec = 0; // static 정의
TEXT::TEXT(string line) {
    this->line = line;
    this->address = 0x00400000 + 4*count_textsec;
    count_textsec++;
}

// ============= function implementation (진수 변환 함수)

string Hexadecimaler(string num) { // 2진수 to 16진수 변환 함수
    vector<char>Hexa_arr_table = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    string Hexadecimal = "0x";
    int Hexa_checker = 0; 
    for (int i=0; i<=28; i+=4) // 모든 binary == 32bit 가정
    {
        if (((num[0+i]-'0')*8+(num[1+i]-'0')*4+(num[2+i]-'0')*2+(num[3+i]-'0')) != 0) {
            Hexa_checker = 1; // 자릿 수 고정하지 않기 위한 if문
        } 
        if (Hexa_checker == 1) {
            Hexadecimal += Hexa_arr_table[(num[0+i]-'0')*8+(num[1+i]-'0')*4+(num[2+i]-'0')*2+(num[3+i]-'0')];
        }
    } // 아스키 코드 이슈 때문에 '0'을 감해준다
    return Hexadecimal;
}


string Binaryer(int num, int size) { // 10진수 to 2진수 변환 함수
    bitset<32> bit(num); // bitset library 이용, // sign extension 자동 고려
    string binary = bit.to_string().substr(bit.to_string().length()-size);
    return binary; // ex. 5자리만 출력
}

// ============= function implementation (opcode operation 함수)

// register operand

string addiu(string line) {
    line = regex_replace(line , regex("[$,]"), " " );
    istringstream iss(line);
    string opcode;
    int rt;
    int rs;
    int imm; // 0xa이 안읽힌다.
    if (line.find("0x") != std::string::npos) {
        iss >> opcode >> rt >> rs >> hex >> imm;
    }
    else {
        iss >> opcode >> rt >> rs >> imm;
    }
    string binary_instruction = Binaryer(9,6) + Binaryer(rs,5) + Binaryer(rt,5) + Binaryer(imm,16);
    return binary_instruction; 
}

string addu(string line) {
    line = regex_replace(line , regex("[$,]"), " " );
    istringstream iss(line);
    string opcode;
    int rd;
    int rs;
    int rt;
    iss >> opcode >> rd >> rs >> rt;
    string binary_instruction = Binaryer(0,6) + Binaryer(rs,5) + Binaryer(rt,5) + Binaryer(rd,5) + Binaryer(0,5) + Binaryer(0x21,6);
    return binary_instruction; 
}

string andr(string line) {
    line = regex_replace(line , regex("[$,]"), " " );
    istringstream iss(line);
    string opcode;
    int rd;
    int rs;
    int rt;
    iss >> opcode >> rd >> rs >> rt;
    string binary_instruction = Binaryer(0,6) + Binaryer(rs,5) + Binaryer(rt,5) + Binaryer(rd,5) + Binaryer(0,5) + Binaryer(0x24,6);
    return binary_instruction; 
}

string andi(string line) {
    line = regex_replace(line , regex("[$,]"), " " );
    istringstream iss(line);
    string opcode;
    int rt;
    int rs;
    int imm;
    if (line.find("0x") != std::string::npos) {
        iss >> opcode >> rt >> rs >> hex >> imm;
    }
    else {
        iss >> opcode >> rt >> rs >> imm;
    }
    string binary_instruction = Binaryer(0xc,6) + Binaryer(rs,5) + Binaryer(rt,5) + Binaryer(imm,16);
    return binary_instruction; 
}

// memory operand

string beq(string line, int address) { // address는 PC를 가르킨다.
    line = regex_replace(line , regex("[$,]"), " " );
    istringstream iss(line);
    string opcode;
    int rs;
    int rt;
    string label;
    int goal_address;
    iss >> opcode >> rs >> rt >> label;
    for (int i=0; i<TEXT_LABEL::text_label_list.size();i++) { // 목적지 라벨의 주소 가져오기
        string goal_line = TEXT_LABEL::text_label_list[i]->GetLine();
        if (goal_line.find(" " + label + ":") != string::npos) {
            goal_address = TEXT_LABEL::text_label_list[i]->GetAddress();
            break;
        }
    }
    int offset = (goal_address - address - 4)/4; // offset 설정: (goal_address - address(PC))/4
    string binary_instruction = Binaryer(4,6) + Binaryer(rs,5) + Binaryer(rt,5) + Binaryer(offset,16);
    return binary_instruction; 
}

string bne(string line, int address) { // address는 PC를 가르킨다.
    line = regex_replace(line , regex("[$,]"), " " );
    istringstream iss(line);
    string opcode;
    int rs;
    int rt;
    string label;
    int goal_address;
    iss >> opcode >> rs >> rt >> label;
    for (int i=0; i<TEXT_LABEL::text_label_list.size();i++) { // 목적지 라벨의 주소 가져오기
        string goal_line = TEXT_LABEL::text_label_list[i]->GetLine();
        if (goal_line.find(" " + label + ":") != string::npos) {
            goal_address = TEXT_LABEL::text_label_list[i]->GetAddress();
            break;
        }
    }
    int offset = (goal_address - address -4)/4; // offset 설정: (goal_address - address(PC))/4
    string binary_instruction = Binaryer(5,6) + Binaryer(rs,5) + Binaryer(rt,5) + Binaryer(offset,16);
    return binary_instruction; 
}

string jj(string line, int address) { // address는 PC를 가르킨다.
    line = regex_replace(line , regex("[,]"), " " );
    istringstream iss(line);
    string opcode;
    string target;
    int goal_address;
    iss >> opcode >> target;
    for (int i=0; i<TEXT_LABEL::text_label_list.size();i++) { // 목적지 라벨의 주소 가져오기
        string goal_line = TEXT_LABEL::text_label_list[i]->GetLine();
        if (goal_line.find(" " + target + ":") != string::npos) {
            goal_address = TEXT_LABEL::text_label_list[i]->GetAddress();
            break;
        }
    }
    int target_address = (goal_address)/4; // target address 설정: (goal_address)/4
    string binary_instruction = Binaryer(2,6) + Binaryer(target_address,26);
    return binary_instruction; 
}

string jal(string line, int address) { // address는 PC를 가르킨다.
    line = regex_replace(line , regex("[,]"), " " );
    istringstream iss(line);
    string opcode;
    string target;
    int goal_address;
    iss >> opcode >> target;
    for (int i=0; i<TEXT_LABEL::text_label_list.size();i++) { // 목적지 라벨의 주소 가져오기
        string goal_line = TEXT_LABEL::text_label_list[i]->GetLine();
        if (goal_line.find(" " + target + ":") != string::npos) {
            goal_address = TEXT_LABEL::text_label_list[i]->GetAddress();
            break;
        }
    }
    int target_address = (goal_address)/4; // target address 설정: (goal_address)/4
    string binary_instruction = Binaryer(3,6) + Binaryer(target_address,26);
    return binary_instruction; 
}

string jr(string line) {
    line = regex_replace(line , regex("[$,]"), " " );
    istringstream iss(line);
    string opcode;
    int rs;
    iss >> opcode >> rs;
    string binary_instruction = Binaryer(0,6) + Binaryer(rs,5) + Binaryer(0,15) + Binaryer(8,6);
    return binary_instruction; 
}

string lui(string line) {
    line = regex_replace(line , regex("[$,]"), " " );
    istringstream iss(line);
    string opcode;
    int rt;
    int imm;
    if (line.find("0x") != std::string::npos) {
        string str_imm;
        iss >> opcode >> rt >> str_imm;
        imm = stoi(str_imm, nullptr, 16);
    }
    else {
        iss >> opcode >> rt >> imm;
    }
    string binary_instruction = Binaryer(0xf,6) + Binaryer(0,5) + Binaryer(rt,5) + Binaryer(imm,16);
    return binary_instruction; 
}

string lw(string line) {
    line = regex_replace(line , regex("[$,()]"), " " );
    istringstream iss(line);
    string opcode;
    int rt;
    int offset;
    int rs;
    iss >> opcode >> rt >> offset >> rs;
    string binary_instruction = Binaryer(0x23,6) + Binaryer(rs,5) + Binaryer(rt,5) + Binaryer(offset,16);
    return binary_instruction; 
}

string lb(string line) {
    line = regex_replace(line , regex("[$,()]"), " " );
    istringstream iss(line);
    string opcode;
    int rt;
    int offset;
    int rs;
    iss >> opcode >> rt >> offset >> rs;
    string binary_instruction = Binaryer(0x20,6) + Binaryer(rs,5) + Binaryer(rt,5) + Binaryer(offset,16);
    return binary_instruction; 
}

// register operand

string nor(string line) {
    line = regex_replace(line , regex("[$,]"), " " );
    istringstream iss(line);
    string opcode;
    int rd;
    int rs;
    int rt;
    iss >> opcode >> rd >> rs >> rt;
    string binary_instruction = Binaryer(0,6) + Binaryer(rs,5) + Binaryer(rt,5) + Binaryer(rd,5) + Binaryer(0,5) + Binaryer(0x27,6);
    return binary_instruction; 
}

string orr(string line) {
    line = regex_replace(line , regex("[$,]"), " " );
    istringstream iss(line);
    string opcode;
    int rd;
    int rs;
    int rt;
    iss >> opcode >> rd >> rs >> rt;
    string binary_instruction = Binaryer(0,6) + Binaryer(rs,5) + Binaryer(rt,5) + Binaryer(rd,5) + Binaryer(0,5) + Binaryer(0x25,6);
    return binary_instruction; 
}

string ori(string line) {
    line = regex_replace(line , regex("[$,]"), " " );
    istringstream iss(line);
    string opcode;
    int rt;
    int rs;
    int imm;
    if (line.find("0x") != std::string::npos) {
        string str_imm;
        iss >> opcode >> rt >> rs >> str_imm;
        imm = stoi(str_imm, nullptr, 16);
    }
    else {
        iss >> opcode >> rt >> rs >> imm;
    }
    string binary_instruction = Binaryer(0xd,6) + Binaryer(rs,5) + Binaryer(rt,5) + Binaryer(imm,16);
    return binary_instruction; 
}

string sltiu(string line) {
    line = regex_replace(line , regex("[$,]"), " " );
    istringstream iss(line);
    string opcode;
    int rt;
    int rs;
    int imm;
    if (line.find("0x") != std::string::npos) {
        iss >> opcode >> rt >> rs >> hex >> imm;
    }
    else {
        iss >> opcode >> rt >> rs >> imm;
    }
    string binary_instruction = Binaryer(0xb,6) + Binaryer(rs,5) + Binaryer(rt,5) + Binaryer(imm,16);
    return binary_instruction; 
}

string sltu(string line) {
    line = regex_replace(line , regex("[$,]"), " " );
    istringstream iss(line);
    string opcode;
    int rd;
    int rs;
    int rt;
    iss >> opcode >> rd >> rs >> rt;
    string binary_instruction = Binaryer(0,6) + Binaryer(rs,5) + Binaryer(rt,5) + Binaryer(rd,5) + Binaryer(0,5) + Binaryer(0x2b,6);
    return binary_instruction; 
}

string sll(string line) {
    line = regex_replace(line , regex("[$,]"), " " );
    istringstream iss(line);
    string opcode;
    int rd;
    int rt;
    int shamt;
    iss >> opcode >> rd >> rt >> shamt;
    string binary_instruction = Binaryer(0,6) + Binaryer(0,5) + Binaryer(rt,5) + Binaryer(rd,5) + Binaryer(shamt,5) + Binaryer(0,6);
    return binary_instruction; 
}

string srl(string line) {
    line = regex_replace(line , regex("[$,]"), " " );
    istringstream iss(line);
    string opcode;
    int rd;
    int rt;
    int shamt;
    iss >> opcode >> rd >> rt >> shamt;
    string binary_instruction = Binaryer(0,6) + Binaryer(0,5) + Binaryer(rt,5) + Binaryer(rd,5) + Binaryer(shamt,5) + Binaryer(2,6);
    return binary_instruction; 
}

// memory operand

string sw(string line) {
    line = regex_replace(line , regex("[$,()]"), " " );
    istringstream iss(line);
    string opcode;
    int rt;
    int offset;
    int rs;
    iss >> opcode >> rt >> offset >> rs;
    string binary_instruction = Binaryer(0x2b,6) + Binaryer(rs,5) + Binaryer(rt,5) + Binaryer(offset,16);
    return binary_instruction; 
}

string sb(string line) {
    line = regex_replace(line , regex("[$,()]"), " " );
    istringstream iss(line);
    string opcode;
    int rt;
    int offset;
    int rs;
    iss >> opcode >> rt >> offset >> rs;
    string binary_instruction = Binaryer(0x28,6) + Binaryer(rs,5) + Binaryer(rt,5) + Binaryer(offset,16);
    return binary_instruction; 
}

// register operand

string subu(string line) {
    line = regex_replace(line , regex("[$,]"), " " );
    istringstream iss(line);
    string opcode;
    int rd;
    int rs;
    int rt;
    iss >> opcode >> rd >> rs >> rt;
    string binary_instruction = Binaryer(0,6) + Binaryer(rs,5) + Binaryer(rt,5) + Binaryer(rd,5) + Binaryer(0,5) + Binaryer(0x23,6);
    return binary_instruction; 
}

// ============= function implementation (어셈블러 함수)

void InduceInstance(string line) {
    // format 정의: data 먼저 읽고 -> 라벨 별 저장 / text 먼저 읽고 -> 라벨 별 저장: 더 간결하게 적을 수 있다면...
    regex data_format("^\\s*.data\\s*$");
    regex text_format("^\\s*.text\\s*$");
    regex label("^[^\\s].*$");
    regex nope("^\\s*$");
    static int data_checker;
    static int text_checker;
    if (regex_match(line, data_format)) {  // data 라벨 확인
        text_checker = 0;
        data_checker = 1;
    }
    if (data_checker == 1) {
        if (regex_match(line, label)) {
            DATA_LABEL *new_data_label = new DATA_LABEL(" "+line);
        }
        if (line.find(" .word ") != string::npos) { // 4B word로 뽑아오기: word말고 다른 타입 오면 그것도 추가하기
            istringstream iss(line);
            string data;
            while (iss >> data) { // 마지막 숫자만 가져온다. 
                string data = data;   
            }
            size_t pos;
            int idata = stol(data, &pos, 0);// data 타입 변경 -> 16진수 변환도 고려하기
            DATA *new_data = new DATA(idata);
            DATA_LABEL* current_data_label = DATA_LABEL::data_label_list.back();
            current_data_label->PushData(new_data); // 부모 라벨-자식 데이터 연결
        }
    }
    if (regex_match(line, text_format)) { // text 라벨 확인
        text_checker = 1;
        data_checker = 0;
    }
    if (text_checker == 1) {
        if (regex_match(line, label)) { // text 라벨이 나오면 
            TEXT_LABEL *new_text_label = new TEXT_LABEL(" "+line);
        }
        size_t pos;
        while ((pos = line.find(":")) != std::string::npos) {
            line.replace(0, pos + 1, ""); // line에서 label을 모두 제거 -> label까지 line에 들어가면 안되므로
        }
        if (!regex_match(line, nope)) {
            if (!regex_match(line, text_format)) { // .text 줄이 아닐 때 -> .data에서는 .word만 긁어올 수 있는데, 얘는 그러지 못하므로 이 줄 있어야 함
                if (line.find(" la ") != string::npos) { // la라는 특별한 operand를 처리하기 위한 함수
                    line = regex_replace(line , regex("[$,]"), " " );
                    istringstream iss(line);
                    string opcode;
                    int rs;
                    string label;
                    int goal_address;
                    iss >> opcode >> rs >> label;
                    for (int i=0; i<DATA_LABEL::data_label_list.size();i++) { // 목적지 라벨의 주소 가져오기
                        string goal_line = DATA_LABEL::data_label_list[i]->GetLine();
                        if (goal_line.find(label + ":") != string::npos) {
                            goal_address = DATA_LABEL::data_label_list[i]->GetAddress();
                            break;
                        }
                    }
                    if ((goal_address & 0xffff) == 0) {
                        // lui
                        line = " lui $"+to_string(rs)+", "+ Hexadecimaler(Binaryer((goal_address&0xffff0000)>>16,32));
                        TEXT *new_text = new TEXT(line); 
                        TEXT_LABEL* current_text_label = TEXT_LABEL::text_label_list.back();
                        current_text_label->PushText(new_text); // 부모 라벨-자식 데이터 연결
                    }
                    else {
                        // lui + ori
                        string line1 = " lui $"+to_string(rs)+", " + Hexadecimaler(Binaryer((goal_address&0xffff0000)>>16,32));
                        string line2 = " ori $"+to_string(rs)+", $"+to_string(rs) + ", " + Hexadecimaler(Binaryer(goal_address&0xffff,32));
                        TEXT *new_text1 = new TEXT(line1); 
                        TEXT *new_text2 = new TEXT(line2); 
                        TEXT_LABEL* current_text_label = TEXT_LABEL::text_label_list.back();
                        current_text_label->PushText(new_text1); // 부모 라벨-자식 데이터 연결
                        current_text_label->PushText(new_text2); // 부모 라벨-자식 데이터 연결
                    }
                }
                else {
                    TEXT *new_text = new TEXT(line); 
                    TEXT_LABEL* current_text_label = TEXT_LABEL::text_label_list.back();
                    current_text_label->PushText(new_text); // 부모 라벨-자식 데이터 연결
                }
            }
        }
    }
}

string Assembler() { // Assembler
    string fin_binary_code;
    fin_binary_code = Hexadecimaler(Binaryer(TEXT::count_textsec*4, 32)); // 텍스트 섹션 영역 출력
    fin_binary_code += "\n";
    fin_binary_code += Hexadecimaler(Binaryer(DATA::count_datasec*4, 32)); // 데이터 섹션 영역 출력
    fin_binary_code += "\n";
    for (int i=0; i<TEXT_LABEL::text_label_list.size();i++){
        for (int j=0; j<TEXT_LABEL::text_label_list[i]->GetTextList().size();j++){
            // line과 address, address는 memory operand를 위해서 필요하다.
            string line = TEXT_LABEL::text_label_list[i]->GetTextList()[j]->GetText();
            int address = TEXT_LABEL::text_label_list[i]->GetTextList()[j]->GetAddress(); // address = PC
            // register operand
            if (line.find(" addiu ") != string::npos) { // addiu
            fin_binary_code += Hexadecimaler(addiu(line));
            }
            if (line.find(" addu ") != string::npos) { // addu
                fin_binary_code += Hexadecimaler(addu(line));
            }
            if (line.find(" and ") != string::npos) { // and
                fin_binary_code += Hexadecimaler(andr(line)); // and로 쓰면 오류 생겨서 andr 사용
            }
            if (line.find(" andi ") != string::npos) { // andi
                fin_binary_code += Hexadecimaler(andi(line)); 
            }
            // memory operand
            if (line.find(" beq ") != string::npos) { // beq
                fin_binary_code += Hexadecimaler(beq(line, address)); 
            }
            if (line.find(" bne ") != string::npos) { // bne
                fin_binary_code += Hexadecimaler(bne(line, address)); 
            }
            if (line.find(" j ") != string::npos) { // j
                fin_binary_code += Hexadecimaler(jj(line, address)); // j로 쓰면 오류 생겨서 jj 사용
            }
            if (line.find(" jal ") != string::npos) { // jal
                fin_binary_code += Hexadecimaler(jal(line, address)); 
            }
            if (line.find(" jr ") != string::npos) { // jr
                fin_binary_code += Hexadecimaler(jr(line)); 
            }
            if (line.find(" lui ") != string::npos) { // lui
                fin_binary_code += Hexadecimaler(lui(line)); 
            }
            if (line.find(" lw ") != string::npos) { // lw
                fin_binary_code += Hexadecimaler(lw(line)); 
            }
            if (line.find(" lb ") != string::npos) { // lb
                fin_binary_code += Hexadecimaler(lb(line)); 
            }
            // register operand
            if (line.find(" nor ") != string::npos) { // nor
                fin_binary_code += Hexadecimaler(nor(line)); 
            }
            if (line.find(" or ") != string::npos) { // or
                fin_binary_code += Hexadecimaler(orr(line)); // or로 쓰면 오류 생겨서 andr 사용
            }
            if (line.find(" ori ") != string::npos) { // ori
                fin_binary_code += Hexadecimaler(ori(line)); 
            }
            if (line.find(" sltiu ") != string::npos) { // sltiu
                fin_binary_code += Hexadecimaler(sltiu(line)); 
            }
            if (line.find(" sltu ") != string::npos) { // sltu
                fin_binary_code += Hexadecimaler(sltu(line)); 
            }
            if (line.find(" sll ") != string::npos) { // sll
                fin_binary_code += Hexadecimaler(sll(line)); 
            }
            if (line.find(" srl ") != string::npos) { // srl
                fin_binary_code += Hexadecimaler(srl(line)); 
            }
            // memory operand
            if (line.find(" sw ") != string::npos) { // sw
                fin_binary_code += Hexadecimaler(sw(line)); 
            }
            if (line.find(" sb ") != string::npos) { // sb
                fin_binary_code += Hexadecimaler(sb(line)); 
            }
            // register operand
            if (line.find(" subu ") != string::npos) { // subu
                fin_binary_code += Hexadecimaler(subu(line)); 
            }
            // la: special operand: induceinstance 함수에서 처리
            fin_binary_code += "\n";
        }    
    }
    for (int i=0; i<DATA_LABEL::data_label_list.size();i++){
        for (int j=0; j<DATA_LABEL::data_label_list[i]->GetDataList().size();j++){
            fin_binary_code += Hexadecimaler(Binaryer((DATA_LABEL::data_label_list[i]->GetDataList()[j]->GetData()),32)); 
            fin_binary_code += "\n";
        }
    }
    return fin_binary_code;
}

// ============= function implementation (메인 함수)

int main(int argc, char* argv[]) {

    string line;
    ifstream input(argv[1]); // 파일 객체 불러오기
    while (getline(input, line)) { // 첫 번째 루프: 인스트럭션의 모든 인스턴스 생성 + 섹션 카운트
        InduceInstance(line);
    }
    input.close();

    string filename = argv[1];
    string output_filename;
    size_t pos = filename.find(".s");
    if (pos != string::npos) {
        output_filename = filename.substr(0, pos) + ".o";
    }

    ofstream output(output_filename);
    if(output.is_open()) {
        output << Assembler();
    }
    output.close();

   return 0;
}
