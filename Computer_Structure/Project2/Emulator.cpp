#include <iostream> 
#include <fstream>   
#include <string>
#include <vector>
#include <bitset>
#include <regex>
using namespace std;

class PROGRAMCOUNTER;
class REGISTER;
class TEXT_MEMORY;
class DATA_MEMORY;

// ============= class implementation

class PROGRAMCOUNTER {
private:
    int program_counter;
public:
    PROGRAMCOUNTER();
    static PROGRAMCOUNTER* PCPointer;
    static int old_PC;
    // 주소 업데이트 -> instruction의 리턴 주소 값으로 업데이트 하기 
    void UpdateProgramCounter(int update_address) {this->program_counter = update_address;}
    int GetPC() {return program_counter;}
};

int PROGRAMCOUNTER::old_PC = 0;
PROGRAMCOUNTER* PROGRAMCOUNTER::PCPointer = nullptr;
PROGRAMCOUNTER::PROGRAMCOUNTER() {
    this->program_counter = 0x00400000;
    PCPointer = this; // PC instance 포인터 선언
}

class REGISTER {
private:
    int register_num;
    int data;   
public:
    REGISTER(int register_num);
    static vector<REGISTER*>register_list;
    int ReadRegNum() {return this->register_num;}
    void UpdateData(int update_data) {this->data = update_data;} // 레지스터 내용 업데이트
    int ReadData() {return this->data;}
};

vector<REGISTER*> REGISTER::register_list; // static 정의
REGISTER::REGISTER(int register_num) {
    this->register_num = register_num;
    register_list.push_back(this); // 레지스터 리스트에 추가
}

class TEXT_MEMORY {
private:
    string line; // line으로 받을지 아니면 int로 받을지 고민해보기 -> 그냥 line으로 받자
    int address;
public:
    TEXT_MEMORY(string line, int address);
    static vector<TEXT_MEMORY*>text_memory_list; // address에는 line이 string으로 저장되어 있다.
    string ReadData() {return this->line;}
    int ReadAddress() {return this->address;}
    void UpdateData(string update_data) {this->line = update_data;}
};

vector<TEXT_MEMORY*> TEXT_MEMORY::text_memory_list;
TEXT_MEMORY::TEXT_MEMORY(string line, int address) {
    this->line = line;
    this->address = address;
    text_memory_list.push_back(this);
}

class DATA_MEMORY {
private:
    string line; // line으로 받을지 아니면 int로 받을지 고민해보기
    int address;
public:
    DATA_MEMORY(string line, int address);
    static vector<DATA_MEMORY*>data_memory_list;
    string ReadData() {return this->line;}
    int ReadAddress() {return this->address;}
    void UpdateData(string update_data) {this->line = update_data;}
};

vector<DATA_MEMORY*> DATA_MEMORY::data_memory_list;
DATA_MEMORY::DATA_MEMORY(string line, int address) {
    this->line = line;
    this->address = address;
    data_memory_list.push_back(this);
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
    if (Hexa_checker == 0) {
        Hexadecimal += '0';
    }
    return Hexadecimal;
}

string Binaryer(int num, int size) { // 10진수 to 2진수 변환 함수
    bitset<32> bit(num); // bitset library 이용, // sign extension 자동 고려
    string binary = bit.to_string().substr(bit.to_string().length()-size);
    return binary; // ex. 5자리만 출력
}

// ============= function implementation (레지스터, 메모리 출력 함수)

void PrintRgisterData(){
    cout << "Current register values: " << endl;
    cout << "------------------------------------" << endl;
    cout << "PC: " << Hexadecimaler(Binaryer(PROGRAMCOUNTER::PCPointer->GetPC(),32)) << endl;
    cout << "Registers:" << endl;
    for (int i=0; i<=31; i++) {
        cout << "R" << i << ": " << Hexadecimaler(Binaryer(REGISTER::register_list[i]->ReadData(),32)) << endl;
    } // 레지스터 클래스의 레지스터 배열 하나씩 읽어오기
    cout << endl;
}

void PrintMemoryContents(int begin_address, int end_address){ // 메모리 원소 print 하는 함수
    cout << "Memory contents [" << Hexadecimaler(Binaryer(begin_address,32)) << "..";
    cout << Hexadecimaler(Binaryer(end_address,32)) << "]:" << endl;
    cout << "------------------------------------" << endl;
    int else_checker = 0; // 메모리가 할당되지 않았는지 체크 하는 변수
    for (int i=0; i<=(end_address-begin_address)/4; i++) { // 텍스트 메모리 체크
        else_checker = 0;
        for (int j=0; j<TEXT_MEMORY::text_memory_list.size(); j++) {
            if (begin_address+i*4 == TEXT_MEMORY::text_memory_list[j]->ReadAddress()) {
                cout << Hexadecimaler(Binaryer(begin_address+i*4,32)) << ": ";
                cout << TEXT_MEMORY::text_memory_list[j]->ReadData() << endl;
                else_checker = 1;
            }
        }
        for (int j=0; j<DATA_MEMORY::data_memory_list.size(); j++) { // 데이터 메모리 체크
            if (begin_address+i*4 == DATA_MEMORY::data_memory_list[j]->ReadAddress()) {
                cout << Hexadecimaler(Binaryer(begin_address+i*4,32)) << ": ";
                cout << DATA_MEMORY::data_memory_list[j]->ReadData() << endl;
                else_checker = 1;
            }
        }
        if (else_checker == 0) { // 메모리에 아무것도 없으면 0x0출력
            cout << Hexadecimaler(Binaryer(begin_address+i*4,32)) << ": ";
            cout << Hexadecimaler(Binaryer(0,32)) << endl;
        }  
    }
    //i<(end_address-begin_address)/4; 이걸로 바꾸고 -> 마지막꺼만 byte 출력하게 하기
    cout << endl;
}

// ============= function implementation (ALU)

int ALU(string alu_control, int a1, int a2) {
   if (alu_control == "0000") { // and
        return a1&a2;     
   }
   if (alu_control == "0001") { // or
        return a1|a2;     
   }
   if (alu_control == "0010") { // add
        return a1+a2;     
   }
   if (alu_control == "0110") { // subtract
        return a1-a2;     
   }
   if (alu_control == "0111") { // set-less than
        if (a1<a2) {
            return 1;
        }
        else {
            return 0;
        }
   }
   if (alu_control == "1100") { // NOR
        return (~a1)&(~a2);
   }
}

// ============= function implementation (Read Reg, Write Reg)

int ReadReg(int RegNum) {
    int data;
    for (int i = 0; i < REGISTER::register_list.size(); i++) { // 레지스터 찾고 값 저장
        if (RegNum == REGISTER::register_list[i]->ReadRegNum()) {
            data = REGISTER::register_list[i]->ReadData();
        }
    }
    return data;
}

void WriteReg(int RegNum, int Data) {
    for (int i = 0; i < REGISTER::register_list.size(); i++) { // 결과 레지스터에 값 저장
        if (RegNum == REGISTER::register_list[i]->ReadRegNum()) {
            REGISTER::register_list[i]->UpdateData(Data);
        }
    }
}

string ReadMem(int Address) {
    string data;
    /*for (int i = 0; i < TEXT_MEMORY::text_memory_list.size(); i++) { // 레지스터 찾고 값 저장
        if (Address == TEXT_MEMORY::text_memory_list[i]->ReadAddress()) {
            data = TEXT_MEMORY::text_memory_list[i]->ReadData();
        }
    }*/
    for (int i = 0; i < DATA_MEMORY::data_memory_list.size(); i++) { // 레지스터 찾고 값 저장
        if (Address == DATA_MEMORY::data_memory_list[i]->ReadAddress()) {
            data = DATA_MEMORY::data_memory_list[i]->ReadData();
        }
    }
    return data; // data는 string 값으로 저장되어 있음
}

void WriteMem(int Address, string Data) {
    /*for (int i = 0; i < TEXT_MEMORY::text_memory_list.size(); i++) { // 결과 레지스터에 값 저장
        if (Address == TEXT_MEMORY::text_memory_list[i]->ReadAddress()) {
            TEXT_MEMORY::text_memory_list[i]->UpdateData(Data);
        }
    }*/
    for (int i = 0; i < DATA_MEMORY::data_memory_list.size(); i++) { // 결과 레지스터에 값 저장
        if (Address == DATA_MEMORY::data_memory_list[i]->ReadAddress()) {
            DATA_MEMORY::data_memory_list[i]->UpdateData(Data);
        }
    }
}

// ============= function implementation (인스트럭션 함수 exclude R format)
// 1. 인스트럭션 실행 -> 레지스터, 메모리 업데이트
// 2. PC값 업데이트

int Addiu(int int_line) {
    int rs = ReadReg((int_line>>21)&0x1F);
    int rt = (int_line>>16)&0x1F; // 여기서는 레지스터 읽으면 안됨 -> just 번호만 저장
    signed int imm = static_cast<signed int>(static_cast<int16_t>(int_line & 0xFFFF));
    WriteReg(rt, ALU("0010",rs,imm)); // 레지스터 쓰기
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

int Andi(int int_line) {
    int rs = ReadReg((int_line>>21)&0x1F);
    int rt = (int_line>>16)&0x1F; // 레지스터 읽기
    int imm = (int_line)&0xFFFF;
    WriteReg(rt, ALU("0000",rs,imm)); // 레지스터 쓰기
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

int Beq(int int_line) {
    int rs = ReadReg((int_line>>21)&0x1F); // 0x1F는 5자리, 즉 11111
    int rt = ReadReg((int_line>>16)&0x1F); // 레지스터 읽기
    signed int offset = static_cast<signed int>(static_cast<int16_t>(int_line & 0xFFFF)); // 0xFFFF는 16자리 (offset은 맨 뒤에 있으므로 shift 안해줘도 된다.)
    if (ALU("0010",rs,rt) == 0) { // 0이라면 분기
        return PROGRAMCOUNTER::PCPointer->GetPC()+4+offset*4; // PC 업데이트 주소 리턴
    }
    else {
        return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
    }
}

int Bne(int int_line) {
    int rs = ReadReg((int_line>>21)&0x1F); // 0x1F는 5자리, 즉 11111
    int rt = ReadReg((int_line>>16)&0x1F); // 레지스터 읽기
    signed int offset = static_cast<signed int>(static_cast<int16_t>(int_line & 0xFFFF)); // 0xFFFF는 16자리 (offset은 맨 뒤에 있으므로 shift 안해줘도 된다.)
    if (ALU("0010",rs,rt) != 0) { // 0이 아니라면 분기
        return PROGRAMCOUNTER::PCPointer->GetPC()+4+offset*4; // PC 업데이트 주소 리턴
    }
    else {
        return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
    }
}

// j instruction

int J(int int_line) { // 다시
    int target = (int_line)&0x3FFFFFF;
    return target*4; // PC 업데이트 주소 리턴
}

int Jal(int int_line) { // 다시
    WriteReg(31, ALU("0010",PROGRAMCOUNTER::PCPointer->GetPC(),4)); // ra 레지스터에 내용(PC+4) 작성   
    int target = (int_line)&0x3FFFFFF;
    return target*4; // PC 업데이트 주소 리턴
}

// j instruction

int Lui(int int_line) {
    int rt = (int_line>>16)&0x1F; // 레지스터 읽기
    signed int imm = static_cast<signed int>(static_cast<int16_t>(int_line & 0xFFFF));
    // (0xFFFF0000과 or 연산 + << 16)
    WriteReg(rt, ALU("0000",0xFFFF0000,imm<<16));
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

int Lw(int int_line) {
    int rs = ReadReg((int_line>>21)&0x1F); // 여기에는 주소가 저장되어 있음
    int rt = (int_line>>16)&0x1F; 
    signed int offset = static_cast<signed int>(static_cast<int16_t>(int_line & 0xFFFF)); 
    size_t pos;
    WriteReg(rt, stol(ReadMem(rs+offset), &pos, 0)); // 주소에는 data가 string 형태로 저장되어 있음
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

int Lb(int int_line) { // 다시 -> 성공
    // lsb에 채우고, sign 32bit 확장하고 하면 되나?
    int rs = ReadReg((int_line>>21)&0x1F); // 여기에는 주소가 저장되어 있음
    int rt = (int_line>>16)&0x1F; 
    signed int offset = static_cast<signed int>(static_cast<int16_t>(int_line & 0xFFFF)); 
    size_t pos;
    int data = stol(ReadMem(rs+4*(offset/4)), &pos, 0); // offset이 6이면 그 다음 주소 읽기, -6이면 그 전 주소 읽기 
    WriteReg(rt, (data>>((3-(offset%4))*8))&0xFF); // byte 단위 계산 -> big endian 고려, offset은 나머지 활용, 자세한 건 ppt 참고
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

int Ori(int int_line) {
    int rs = ReadReg((int_line>>21)&0x1F);
    int rt = (int_line>>16)&0x1F; // 레지스터 읽기
    signed int imm = static_cast<signed int>(static_cast<int16_t>(int_line & 0xFFFF));
    // instruction 표 참고
    WriteReg(rt, ALU("0001",rs,imm));
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

int Sltiu(int int_line) {
    int rs = ReadReg((int_line>>21)&0x1F); // 0x1F는 5자리
    int rt = (int_line>>16)&0x1F; // 레지스터 읽기
    signed int imm = static_cast<signed int>(static_cast<int16_t>(int_line & 0xFFFF));
    WriteReg(rt, ALU("0111", rs, imm));
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

int Sw(int int_line) {
    int rs = ReadReg((int_line>>21)&0x1F); // 0x1F는 5자리
    int rt = ReadReg((int_line>>16)&0x1F);
    signed int offset = static_cast<signed int>(static_cast<int16_t>(int_line & 0xFFFF)); 
    WriteMem(rs+offset, Hexadecimaler(Binaryer(rt,32)));
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

int Sb(int int_line) {  // 다시 -> 성공
    int rs = ReadReg((int_line>>21)&0x1F); // 0x1F는 5자리
    int rt = ReadReg((int_line>>16)&0x1F);
    signed int offset = static_cast<signed int>(static_cast<int16_t>(int_line & 0xFFFF)); 
    size_t pos;
    int data = stol(ReadMem(rs+4*(offset/4)), &pos, 0); // offset이 6이면 그 다음 주소 읽기, -6이면 그 전 주소 읽기
    int new_data = (((data>>((4-offset%4)*8))<<((4-offset%4)*8))|((unsigned int)(data<<((1+offset%4)*8))>>((1+offset%4)*8)))|((rt&0xFF)<<((3-offset%4)*8));
    // ex. 10101010/11110000/11001100/00001111 인데, offset 2이면
    // 101010/11110000/00000000/00000000 or 00000000/00000000/00000000/00001111 or 00000000/00000000/11001100/00000000
    WriteMem(rs+4*(offset/4), Hexadecimaler(Binaryer(new_data,32))); // -> 여기 다시
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

// ============= function implementation (인스트럭션 함수 include R format)
// 1. 인스트럭션 실행 -> 레지스터, 메모리 업데이트
// 2. PC값 업데이트

int Addu(int int_line) {
    int rs = ReadReg((int_line>>21)&0x1F);
    int rt = ReadReg((int_line>>16)&0x1F); 
    int rd = (int_line>>11)&0x1F; 
    WriteReg(rd, ALU("0010",rs,rt)); // 레지스터 쓰기 (add 연산)
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

int And(int int_line) {
    int rs = ReadReg((int_line>>21)&0x1F);
    int rt = ReadReg((int_line>>16)&0x1F); 
    int rd = (int_line>>11)&0x1F; 
    WriteReg(rd, ALU("0000",rs,rt)); // 레지스터 쓰기 (and 연산)
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

int Jr(int int_line) {
    int rs = ReadReg((int_line>>21)&0x1F);
    return rs; // rs 주소로 무조건 분기
}

int Nor(int int_line) { // 다시 -> Nor 연산 제대로 되는지 확인하기 -> 성공
    int rs = ReadReg((int_line>>21)&0x1F);
    int rt = ReadReg((int_line>>16)&0x1F); 
    int rd = (int_line>>11)&0x1F; 
    WriteReg(rd, ALU("1100",rs,rt)); // 레지스터 쓰기 (nor 연산)
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

int Or(int int_line) {
    int rs = ReadReg((int_line>>21)&0x1F);
    int rt = ReadReg((int_line>>16)&0x1F); 
    int rd = (int_line>>11)&0x1F; 
    WriteReg(rd, ALU("0001",rs,rt)); // 레지스터 쓰기 (or 연산)
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

int Sltu(int int_line) {
    int rs = ReadReg((int_line>>21)&0x1F);
    int rt = ReadReg((int_line>>16)&0x1F); 
    int rd = (int_line>>11)&0x1F; 
    WriteReg(rd, ALU("0111",rs,rt)); // 레지스터 쓰기 (set less than 연산)
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

int Sll(int int_line) {
    int rt = ReadReg((int_line>>16)&0x1F); 
    int rd = (int_line>>11)&0x1F; 
    int shamt = (int_line>>6)&0x1F; 
    WriteReg(rd,rt<<shamt); // 레지스터 쓰기 (left shift 연산)
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

int Srl(int int_line) {
    int rt = ReadReg((int_line>>16)&0x1F); 
    int rd = (int_line>>11)&0x1F; 
    int shamt = (int_line>>6)&0x1F; 
    WriteReg(rd,rt>>shamt); // 레지스터 쓰기 (right shift 연산)
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

int Subu(int int_line) {
    int rs = ReadReg((int_line>>21)&0x1F);
    int rt = ReadReg((int_line>>16)&0x1F); 
    int rd = (int_line>>11)&0x1F; 
    WriteReg(rd, ALU("0110",rs,rt)); // 레지스터 쓰기 (substract 연산)
    return PROGRAMCOUNTER::PCPointer->GetPC()+4; // PC 업데이트 주소 리턴
}

int R_format(int int_line) {
    // function field 확인 후 처리할 수 있도록 돕는 함수
    int function_field = (int_line)&0x3F;
    int Update_PC;
    if (function_field == 0x21) { 
        Update_PC = Addu(int_line); // R format
    }
    if (function_field == 0x24) { 
        Update_PC = And(int_line); // R format
    }
    if (function_field == 8) { 
        Update_PC = Jr(int_line); // R format
    }
    if (function_field == 0x27) { 
        Update_PC = Nor(int_line); // R format
    }
    if (function_field == 0x25) { 
        Update_PC = Or(int_line); // R format
    }
    if (function_field == 0x2B) { 
        Update_PC = Sltu(int_line); // R format
    }
    if (function_field == 0) { 
        Update_PC = Sll(int_line); // R format
    }
    if (function_field == 2) { 
        Update_PC = Srl(int_line); // R format
    }
    if (function_field == 0x23) { 
        Update_PC = Subu(int_line); // R format
    }
    return Update_PC; // PC값 리턴
}

// ============= function implementation (인스트럭션 실행 함수)

int Instruction(string line) {
    // line 숫자로 변환
    size_t pos;
    // int_line은 32bit 인스트럭션
    int int_line = stol(line, &pos, 0);
    // shift 26 + 111111과 & bit operation 해서 opcode 구하기
    int opcode = (int_line>>26)&0x3F;
    // 새로운 PC 주소
    int Update_PC;
    // 각 instruction 함수로 보내기
    if (opcode == 0) { 
        Update_PC = R_format(int_line); // R format
    }
    if (opcode == 9) {
        Update_PC = Addiu(int_line); 
    }
    if (opcode == 0xC) {
        Update_PC = Andi(int_line); 
    }
    if (opcode == 4) {
        Update_PC = Beq(int_line); 
    }
    if (opcode == 5) {
        Update_PC = Bne(int_line); 
    }
    if (opcode == 2) {
        Update_PC = J(int_line); 
    }
    if (opcode == 3) {
        Update_PC = Jal(int_line); 
    }
    if (opcode == 0xF) {
        Update_PC = Lui(int_line); 
    }
    if (opcode == 0x23) {
        Update_PC = Lw(int_line); 
    }
    if (opcode == 0x20) {
        Update_PC = Lb(int_line); 
    }
    if (opcode == 0xd) {
        Update_PC = Ori(int_line); 
    }
    if (opcode == 0xb) {
        Update_PC = Sltiu(int_line); 
    }
    if (opcode == 0x2b) {
        Update_PC = Sw(int_line); 
    }
    if (opcode == 0x28) {
        Update_PC = Sb(int_line); 
    }
    return Update_PC;
}

// ============= function implementation (에뮬레이터 함수)

void Emulator(ifstream& input, int d, int n, int num_of_instruction, int begin_address_int, int end_address_int, int m) {
    string line;
    int text_domain;
    int data_domain;
    if (getline(input, line)) { // 텍스트 메모리 개수 얻어오기
        size_t pos;
        text_domain = stol(line, &pos, 0)/4;
    }
    if (getline(input, line)) { // 데이터 메모리 개수 얻어오기 
        size_t pos;
        data_domain = stol(line, &pos, 0)/4;
    }
    for (int i=0; i<=31; i++) {
        REGISTER *new_register = new REGISTER(i); 
        new_register->UpdateData(0x0);
    } // 레지스터 초기화
    int i = 0;
    while (getline(input, line)) { 
        if (i<text_domain) {
            TEXT_MEMORY *new_text_memory = new TEXT_MEMORY(line, 0x00400000+i*4);
        } // 텍스트 인스트럭션 메모리 할당
        if (text_domain<=i && i<text_domain+data_domain) {
            DATA_MEMORY *new_data_memory = new DATA_MEMORY(line, 0x10000000+(i-text_domain)*4);
        } // 데이터 메모리 할당
        i++;
    }
    input.close();
    // Emulator 본격적인 동작 시작
    PROGRAMCOUNTER *new_program_counter = new PROGRAMCOUNTER();
    // PC+4가 last 주소가 되기 전까지 수행
    int instruction_counter = 0;
    int Update_PC;
    if (n == 1) { // n 옵션이 있다면
        while (instruction_counter <= num_of_instruction) {
            if (instruction_counter != 0) { // n이 1이상일 때 부터 실행
                if (new_program_counter->GetPC() != TEXT_MEMORY::text_memory_list.back()->ReadAddress()+4) {
                    for (int i=0; i<TEXT_MEMORY::text_memory_list.size(); i++) {
                        if (new_program_counter->GetPC() == TEXT_MEMORY::text_memory_list[i]->ReadAddress()) {
                            // PC 주소랑 같은 인스트럭션을 찾았을 때 인스트럭션 실행
                            Update_PC = Instruction(TEXT_MEMORY::text_memory_list[i]->ReadData());
                        }
                    }
                    PROGRAMCOUNTER::old_PC = new_program_counter->GetPC();
                    PROGRAMCOUNTER::PCPointer->UpdateProgramCounter(Update_PC);
                    if (d == 1) {
                        PrintRgisterData();
                        if (m == 1) {
                            PrintMemoryContents(begin_address_int, end_address_int);
                        }
                    }
                }
                else {
                    cout << "Warning: PC 주소가 Instruction Mem 범위를 넘어섰습니다." << endl;
                    if (d == 1) {
                        PrintRgisterData();
                        if (m == 1) {
                            PrintMemoryContents(begin_address_int, end_address_int);
                        }
                    }
                    // 만약 PC가 메모리 범위를 넘어서게 되면 아무 업데이트도 진행하지 않는다. + 경고문
                }
            }
            instruction_counter++;
        }
    }
    if (n == 0) {
        int checker = 0;
        while (checker == 0) {
            if (new_program_counter->GetPC() != TEXT_MEMORY::text_memory_list.back()->ReadAddress()+4) {
                for (int i=0; i<TEXT_MEMORY::text_memory_list.size(); i++) {
                    if (new_program_counter->GetPC() == TEXT_MEMORY::text_memory_list[i]->ReadAddress()) {
                        // PC 주소랑 같은 인스트럭션을 찾았을 때 인스트럭션 실행
                        Update_PC = Instruction(TEXT_MEMORY::text_memory_list[i]->ReadData());
                    }
                }
                PROGRAMCOUNTER::old_PC = new_program_counter->GetPC();
                PROGRAMCOUNTER::PCPointer->UpdateProgramCounter(Update_PC);
                if (d == 1) {
                    PrintRgisterData();
                    if (m == 1) {
                        PrintMemoryContents(begin_address_int, end_address_int);
                    }
                }
            }
            else {
                checker = 1;
            }
        }
    }
    if (d == 0) { // d 옵션이 0이라면 마지막꺼만 출력
        // PC 업데이트 된 상태이므로 PC-4 하기
        if (n == 0 || num_of_instruction == 0) { // 0일 때는 PC update 안되므로
            PrintRgisterData();
            if (m == 1) {
                PrintMemoryContents(begin_address_int, end_address_int);
            }
        }
        else { // 이 때는 old PC 사용
            PrintRgisterData(); 
            if (m == 1) {
                PrintMemoryContents(begin_address_int, end_address_int);
            }
        }
    }
}

// ============= function implementation (메인 함수)

int main(int argc, char* argv[]) {
    ifstream input(argv[argc-1]);
    // 사용자가 원하는 시작 주소, 끝 주소, 인스트럭션 갯수 받아오는 변수
    string begin_address;
    string end_address;
    int begin_address_int = 0;
    int end_address_int = 0;
    int num_of_instruction;
    //
    int n = 0; // n 옵션을 알리는 switch
    int d = 0; // d 옵션을 알리는 switch
    int m = 0; // m 옵션을 알리는 switch
    // input 읽기
    for (int i=0; i<argc; i++) {
        if (strcmp(argv[i], "-m") == 0) {
            string address = regex_replace(argv[i+1] , regex("[:]"), " " );
            istringstream iss(address);
            iss >> begin_address >> end_address; 
            size_t pos;
            begin_address_int = stol(begin_address, &pos, 0); // string을 int로 변경
            end_address_int = stol(end_address, &pos, 0);
            m = 1;
        }
        if (strcmp(argv[i], "-d") == 0) {
            d = 1; // d 옵션이 실행되었음을 알리는 switch
        }
        if (strcmp(argv[i], "-n") == 0) {
            size_t pos;
            num_of_instruction = stol(argv[i+1], &pos, 0);
            n = 1;
        }
    }
    Emulator(input, d, n, num_of_instruction, begin_address_int, end_address_int, m); // 파일을 인자로 전달, Emulator 함수 실행 -> Emulator에 인자 하나 더 추가해서 print 인자 만들기 -> ex m=1이면 print
    return 0;
}
