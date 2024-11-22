#include <iostream> 
#include <fstream>   
#include <string>
#include <vector>
#include <bitset>
#include <regex>
using namespace std;

class PROGRAMCOUNTER;
class REGISTER;
class STATE_REGISTER;
class TEXT_MEMORY;
class DATA_MEMORY;

// ============= class implementation

class PROGRAMCOUNTER {
private:
    int program_counter;
public:
    PROGRAMCOUNTER();
    static PROGRAMCOUNTER* PCPointer;
    static int old_PC; // 과거의 PC값
    // 주소 업데이트 -> instruction의 리턴 주소 값으로 업데이트 하기 
    void UpdateProgramCounter(int update_address) {this->program_counter = update_address;}
    int GetPC() {return program_counter;}
};

int PROGRAMCOUNTER::old_PC = 0x00400000;
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

class STATE_REGISTER { // state reg class
private:
    int register_num;
    int data;   
public:
    STATE_REGISTER(int register_num);
    static vector<STATE_REGISTER*>register_list;
    int ReadRegNum() {return this->register_num;}
    void UpdateData(int update_data) {this->data = update_data;} // 레지스터 내용 업데이트
    int ReadData() {return this->data;}
};

vector<STATE_REGISTER*> STATE_REGISTER::register_list; // static 정의
STATE_REGISTER::STATE_REGISTER(int register_num) {
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

// control은 bubble일 때 쓰는 거..
vector<int> pipelining_control = {0, 0, 0, 0, 0}; // 실행되고 있는 인스트럭션의 주소 값들이 담겨있는 벡터, 디폴트는 0 (어차피 주소에 0 올 리 X)
vector<int> pipelining = {0, 0, 0, 0, 0};

int PC_buffer = 0; // 업데이트 될 PC 값을 담고 있는 버퍼
int line_buffer = 0; // 라인을 담고 있는 버퍼
int Load_use_Hazard = 0; // 스테이지 유지 (stall) 신호
int Load_use_Hazard_forwarding = 0; // 포워딩 (forward) 신호
int Load_use_Hazard_pipe = 0; // load use pipe 신호 

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

void PrintPipeLineContents(int a, int b, int c, int d, int e) {
    cout << "Current Pipiline PC State:" << endl;
    string a_1 = (a == 0) ? "" : Hexadecimaler(Binaryer(a, 32)); 
    string b_1 = (b == 0) ? "" : Hexadecimaler(Binaryer(b, 32)); 
    string c_1 = (c == 0) ? "" : Hexadecimaler(Binaryer(c, 32)); 
    string d_1 = (d == 0) ? "" : Hexadecimaler(Binaryer(d, 32)); 
    string e_1 = (e == 0) ? "" : Hexadecimaler(Binaryer(e, 32)); 
    cout << "{" << a_1 << "|" << b_1 << "|" << c_1 << "|" << d_1 << "|" << e_1 << "}" << endl;
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
    for (int i = 0; i < DATA_MEMORY::data_memory_list.size(); i++) { // 레지스터 찾고 값 저장
        if (Address == DATA_MEMORY::data_memory_list[i]->ReadAddress()) {
            data = DATA_MEMORY::data_memory_list[i]->ReadData();
        }
    }
    return data; // data는 string 값으로 저장되어 있음
}

void WriteMem(int Address, string Data) {
    for (int i = 0; i < DATA_MEMORY::data_memory_list.size(); i++) { // 결과 레지스터에 값 저장
        if (Address == DATA_MEMORY::data_memory_list[i]->ReadAddress()) {
            DATA_MEMORY::data_memory_list[i]->UpdateData(Data);
        }
    }
}

int ReadStateReg(int RegNum) {
    int data;
    for (int i = 0; i < STATE_REGISTER::register_list.size(); i++) { // 레지스터 찾고 값 저장
        if (RegNum == STATE_REGISTER::register_list[i]->ReadRegNum()) {
            data = STATE_REGISTER::register_list[i]->ReadData();
        }
    }
    return data;
}

void WriteStateReg(int RegNum, int Data) {
    for (int i = 0; i < STATE_REGISTER::register_list.size(); i++) { // 결과 레지스터에 값 저장
        if (RegNum == STATE_REGISTER::register_list[i]->ReadRegNum()) {
            STATE_REGISTER::register_list[i]->UpdateData(Data);
        }
    }
}

// ============= function implementation (인스트럭션 함수 exclude R format)
// 1. 인스트럭션 실행 -> 레지스터, 메모리 업데이트
// 2. PC값 업데이트

void R_format() {
    // function field 확인 후 처리할 수 있도록 돕는 함수
    int function_field = ReadStateReg(10);
    if (function_field == 0x21) { 
        // Addu
        int result = ALU("0010",ReadStateReg(4),ReadStateReg(5)); // rs:4, rt:5
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
    if (function_field == 0x24) { 
        // And
        int result = ALU("0000",ReadStateReg(4),ReadStateReg(5)); // rs:4, rt:5
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
    if (function_field == 8) { 
        // Jr
    }
    if (function_field == 0x27) { 
        // Nor
        int result = ALU("1100",ReadStateReg(4),ReadStateReg(5)); // rs:4, rt:5
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
    if (function_field == 0x25) { 
        // Or
        int result = ALU("0001",ReadStateReg(4),ReadStateReg(5)); // rs:4, rt:5
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
    if (function_field == 0x2B) { 
        // Sltu
        int result = ALU("0111",ReadStateReg(4),ReadStateReg(5)); // rs:4, rt:5
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
    if (function_field == 0) { 
        // Sll
        int result = ReadStateReg(5)<<ReadStateReg(8); // rt:5, shamt: 8
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
    if (function_field == 2) { 
        // Srl
        int result = ReadStateReg(5)>>ReadStateReg(8); // rt:5, shamt: 8
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
    if (function_field == 0x23) { 
        // Subu
        int result = ALU("0110",ReadStateReg(4),ReadStateReg(5)); // rs:4, rt:5
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
}

// ============= function implementation (인스트럭션 실행 함수)

void Instruction() {
    int opcode = ReadStateReg(3); 
    if (opcode == 0) { 
        R_format(); // R format
    }
    if (opcode == 9) {
        // Addiu
        int result = ALU("0010",ReadStateReg(4),static_cast<signed int>(static_cast<int16_t>(ReadStateReg(6))));
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
    if (opcode == 0xC) {
        // Andi
        int result = ALU("0000",ReadStateReg(4),ReadStateReg(6)); // unsigned extentsion
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
    if (opcode == 4) {
        // Beq 
        int offset = static_cast<signed int>(static_cast<int16_t>(ReadStateReg(6)));
        WriteStateReg(11, ReadStateReg(2)+4+offset*4); // 11. EX_MEM.ALU_OUT 기록
    }
    if (opcode == 5) {
        // Bne
        int offset = static_cast<signed int>(static_cast<int16_t>(ReadStateReg(6)));
        WriteStateReg(11, ReadStateReg(2)+4+offset*4); // 11. EX_MEM.ALU_OUT 기록
    }
    if (opcode == 2) {
        // J 
    }
    if (opcode == 3) {
        WriteStateReg(11, ReadStateReg(1)); // 다음 PC 주소 기록
        // Jal
    }
    if (opcode == 0xF) {
        // Lui
        int result = ALU("0000",0xFFFF0000,(static_cast<signed int>(static_cast<int16_t>(ReadStateReg(6))))<<16);
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
    if (opcode == 0x23) {
        // Lw -> 메모리 어디 접근할 지에 관한 연산
        int result = ALU("0010",ReadStateReg(4),static_cast<signed int>(static_cast<int16_t>(ReadStateReg(6))));
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
    if (opcode == 0x20) {
        // Lb -> 메모리 어디 접근할 지에 관한 연산
        // 주소 원래대로 읽되, offset 다르게 읽기 -> 어케 읽었는지는 HW2 참고
        int result = ALU("0010",ReadStateReg(4),4*(static_cast<signed int>(static_cast<int16_t>(ReadStateReg(6)))/4));
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
    if (opcode == 0xd) {
        // Ori 
        int result = ALU("0001",ReadStateReg(4),ReadStateReg(6)); // unsigned extentsion
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
    if (opcode == 0xb) {
        // Sltiu
        int result = ALU("0111",ReadStateReg(4),ReadStateReg(6)); // unsigned extentsion
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
    if (opcode == 0x2b) {
        // Sw -> 메모리 어디 접근할 지에 관한 연산
        int result = ALU("0010",ReadStateReg(4),static_cast<signed int>(static_cast<int16_t>(ReadStateReg(6))));
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
    if (opcode == 0x28) {
        // Sb -> 메모리 어디 접근할 지에 관한 연산
        // 주소 원래대로 읽되, offset 다르게 읽기 -> 어케 읽었는지는 HW2 참고
        int result = ALU("0010",ReadStateReg(4),4*(static_cast<signed int>(static_cast<int16_t>(ReadStateReg(6)))/4));
        WriteStateReg(11, result); // 11. EX_MEM.ALU_OUT 기록
    }
}

// ============= function implementation (Pipeline State)

int IF_stage(string line) { // PC 주소의 line 받아온 후 -> 32bit binary return 하기
// line 32bit 숫자로 변환
    size_t pos;
    int line_32bit = stol(line, &pos, 0);
    WriteStateReg(0,line_32bit); // 0. IF_ID.Instr에 인스트럭션이 무엇인지 기록
    WriteStateReg(1,PROGRAMCOUNTER::PCPointer->GetPC()); // NPC 기록
    if (Load_use_Hazard == 1) {
        PC_buffer = ReadStateReg(1); // IF stage 유지
        Load_use_Hazard = 0;
    }
}

vector<int> ID_stage(int line_32bit, int atp) { // 받아온 32bit instruction 디코딩 하기 -> control signal return
    // 버퍼 확인
    if (line_buffer != 0) {
        line_32bit = line_buffer; // 버퍼에 값이 있다면 값 유지 (lw hazard에서의 stall 때문에 일어난 값 손실 막기 위한 유지)
        line_buffer = 0; // 라인 버퍼 초기화
    }
    // DataHazard: load use
    if (ReadStateReg(3) == 0x23 || ReadStateReg(3) == 0x20) { // Ex stage load 명령어가 있을 때
        if ((((line_32bit>>26)&0x3F) != 0x2b) && (((line_32bit>>26)&0x3F) != 0x28)) { // sb, sw일 때는 예외
            if ((ReadStateReg(20) == ((line_32bit>>21)&0x1F)) || (ReadStateReg(21) == ((line_32bit>>16)&0x1F))) { // load use의 rt와 ID stage 명령의 rs, 또는 rt가 같다면
                pipelining_control[1] = 0; // flush => 왜 1 index를 nope 하는지는 '파이프라인 미루기' 부분 참고
                Load_use_Hazard = 1; // IF stage 유지 신호
                Load_use_Hazard_forwarding = 1; // forwarding 신호
                Load_use_Hazard_pipe = 1;
                line_buffer = line_32bit; // ID stage 유지
            }
        }
    }
    // Control Hazard: j
    // bit 연산자 괄호 주의하기
    if ((((line_32bit>>26)&0x3F) == 2) || (((line_32bit>>26)&0x3F) == 3) || ((((line_32bit>>26)&0x3F) == 0) && (((line_32bit)&0x3F) == 8))) { // Id stage에 jump 명령어가 있을 때
        pipelining[0] = 0; // flush
        if (((line_32bit>>26)&0x3F) == 2) { // j  
            PC_buffer = ((line_32bit)&0x3FFFFFF)*4;
        }
        if ((((line_32bit>>26)&0x3F) == 0) && (((line_32bit)&0x3F) == 8)) {// jr
            PC_buffer = ReadReg((line_32bit>>21)&0x1F);
        }
        if (((line_32bit>>26)&0x3F) == 3) { // jal
            PC_buffer = ((line_32bit)&0x3FFFFFF)*4;
            WriteStateReg(15, 31); // 31번에 기록
        }
    }
    // Control Hazard: Brch
    if ((((line_32bit>>26)&0x3F) == 4) || (((line_32bit>>26)&0x3F) == 5)) {
        int offset = (line_32bit)&0xFFFF;
        if (atp == 1) { // Always Taken
            pipelining[0] = 0; // 1 cycle stall
            PC_buffer = ReadStateReg(1)+4+offset*4;
        }
        else { // Always not Taken
            // 그냥 진행
        }
    }
    // ID stage
    // reg read, opcode read
    // opcode 확인
    WriteStateReg(3,(line_32bit>>26)&0x3F); // 3. IF_ID.opcode에 opcode가 무엇인지 기록
    WriteStateReg(2, ReadStateReg(1)); // NPC 기록
    // if R format이면 function field도 입력
    if (((line_32bit>>26)&0x3F) == 0) { // R format일 경우
        WriteStateReg(10,(line_32bit)&0x3F); // 10. ID.EX.function_field에 function_field가 무엇인지 기록
        WriteStateReg(20,(line_32bit>>21)&0x1F); // 20. rs 번호 무엇인지 기록
        WriteStateReg(21,(line_32bit>>16)&0x1F); // 21. rt 번호 무엇인지 기록
        WriteStateReg(4,ReadReg((line_32bit>>21)&0x1F)); // 4. rs의 값이 무엇인지 기록
        WriteStateReg(5,ReadReg((line_32bit>>16)&0x1F)); // 5. rt의 값이 무엇인지 기록
        WriteStateReg(7,(line_32bit>>11)&0x1F); // 7. rd의 번호 무엇인지 기록
        WriteStateReg(8,(line_32bit>>6)&0x1F); // 8. shamt가 무엇인지 기록
        if (((line_32bit)&0x3F) == 8) { // jr
            return {-1,0,0,0,0,0,0,1,-1,-1};
        }
        else {
            WriteStateReg(15, ReadStateReg(7)); // WB reg 기록
            return {1,0,0,0,0,1,0,0,-1,-1}; // {RegDst, ALUSrc, Brch, MemRead, MemWrite, RegWrite, MemToReg, PCSrc, WordByteCheck, BeqBneChecker}
        }
    }
    else { 
        WriteStateReg(20,(line_32bit>>21)&0x1F); // 20. rs 번호 무엇인지 기록
        WriteStateReg(21,(line_32bit>>16)&0x1F); // 21. rt 번호 무엇인지 기록
        WriteStateReg(4,ReadReg((line_32bit>>21)&0x1F)); // 4. rs의 값이 무엇인지 기록
        WriteStateReg(5,ReadReg((line_32bit>>16)&0x1F)); // 5. rt의 값이 무엇인지 기록
        WriteStateReg(6,(line_32bit)&0xFFFF); // 6. imm, offset, target가 무엇인지 기록
        if ((((line_32bit>>26)&0x3F) == 0x23) || (((line_32bit>>26)&0x3F) == 0x20)) {
            if (((line_32bit>>26)&0x3F) == 0x23) {
                WriteStateReg(15, ReadStateReg(21)); // WB reg '번호' 기록
                return {0,1,0,1,0,1,0,0,1,-1}; // lw -> WordByteCheck = 1
            }
            else {
                WriteStateReg(15, ReadStateReg(21)); // WB reg 기록
                return {0,1,0,1,0,1,0,0,0,-1}; // lb
            }
        }
        else if ((((line_32bit>>26)&0x3F) == 0x2B) || (((line_32bit>>26)&0x3F) == 0x28)) {
            if (((line_32bit>>26)&0x3F) == 0x2B) {
                return {-1,1,0,0,1,0,-1,0,1,-1}; // -1은 신호 없음, sw
            }
            else {
                return {-1,1,0,0,1,0,-1,0,0,-1}; // -1은 신호 없음, sb
            }
        }
        else if (((line_32bit>>26)&0x3F) == 4) {
            return {-1,0,1,0,0,0,-1,1,-1,1}; //  beq
        }
        else if (((line_32bit>>26)&0x3F) == 5) {
            return {-1,0,1,0,0,0,-1,1,-1,0}; // bne
        }
        else if (((line_32bit>>26)&0x3F) == 2) {
            return {-1,0,0,0,0,0,0,1,-1,-1}; // -1은 신호 없음, j
        }
        else if (((line_32bit>>26)&0x3F) == 3) {
            return {1,0,0,0,0,1,0,1,-1,-1}; // -1은 신호 없음, jal
        }
        else {
            WriteStateReg(15, ReadStateReg(21)); // WB reg가 무엇인지 reg num 기록
            return {0,0,0,0,0,1,0,0,0,-1}; // 나머지들
            // return 뒤에 뭐 쓰지 말기
        }
    }
}

void EX_stage(vector<int> control_signal) { // ALU 연산
    // DataHazard: Load use Forwarding
    if ((ReadStateReg(16) == ReadStateReg(20)) && (Load_use_Hazard_forwarding == 1)) { // 16: EX_MEM.WB, 4: ID_EX.rs & WB일 때
        // WB될 rt, 또는 rd가 rs와 같다면 -> ALU 값을 forwarding
        WriteStateReg(4, ReadStateReg(14));
        // Forwarding
    } 
    if ((ReadStateReg(16) == ReadStateReg(21)) && (Load_use_Hazard_forwarding == 1)) { // 16: EX_MEM.WB, 5: ID_EX.rt & WB일 때
        // WB될 rt, 또는 rd가 rt와 같다면
        WriteStateReg(5, ReadStateReg(14));
        // Forwarding
    }
    Load_use_Hazard_forwarding = 0;
    // EX_stage
    WriteStateReg(16, ReadStateReg(15)); // WB reg 기록
    Instruction();
    WriteStateReg(18, ReadStateReg(6)); // IMM reg 기록
    WriteStateReg(19, ReadStateReg(5)); // rt에 있는 값 기록
    WriteStateReg(16, ReadStateReg(15)); // WB reg 기록
    WriteStateReg(22, ReadStateReg(20)); // rs 번호 기록
    WriteStateReg(23, ReadStateReg(21)); // rt 번호 기록
    WriteStateReg(24, ReadStateReg(2)); // NPC 기록
}

void MEM_stage(vector<int> control_signal, int atp) {
    // DataHazard: EX/MEM to EX Forwarding
    if (control_signal[5] == 1) {
        if (ReadStateReg(16) == ReadStateReg(20)) { // 16: EX_MEM.WB, 4: ID_EX.rs & WB일 때
            // WB될 rt, 또는 rd가 rs와 같다면 -> ALU 값을 forwarding
            WriteStateReg(4, ReadStateReg(11));
            // Forwarding
        } 
        if (ReadStateReg(16) == ReadStateReg(21)) { // 16: EX_MEM.WB, 5: ID_EX.rt & WB일 때
            // WB될 rt, 또는 rd가 rt와 같다면
            WriteStateReg(5, ReadStateReg(11));
            // Forwarding
        }
    }
    // Brch
    if (control_signal[2] == 1) {
        // 이 때는 conditional branch (=beq, bne)
        if (control_signal[9] == 1) { // beq
            if (ReadReg(ReadStateReg(22)) == ReadReg(ReadStateReg(23))) { // 분기O
                if (atp == 1) { // 맞춤
                    // 굿
                }
                else { // 틀림
                    pipelining[0] = 0;
                    pipelining[1] = 0;
                    pipelining[2] = 0;
                    PC_buffer = ReadStateReg(11);
                }
            }
            else { // 분기X
                if (atp == 1) { // 틀림
                    pipelining[0] = 0;
                    pipelining[1] = 0;
                    pipelining[2] = 0;
                    PC_buffer = ReadStateReg(24)+4;
                }
                else {
                    // 굿
                }
            }
        }
        if (control_signal[9] == 0) { // bne
            if (ReadReg(ReadStateReg(22)) == ReadReg(ReadStateReg(23))) { // 분기X
                if (atp == 1) { // 틀림
                    pipelining[0] = 0;
                    pipelining[1] = 0;
                    pipelining[2] = 0;
                    PC_buffer = ReadStateReg(24)+4;
                }
                else {
                    // 굿
                }
            }
            else { // 분기O
                if (atp == 1) { // 맞춤
                    // 굿
                }
                else { // 틀림
                    pipelining[0] = 0;
                    pipelining[1] = 0;
                    pipelining[2] = 0;
                    PC_buffer = ReadStateReg(11);
                }
            }
        }
    }
    // MEM stage
    if (control_signal[3] == 1) {
        // 이 때는 memory_read (=Lw, Lb)
        // state reg 11에 저장되어 있던 값 빼와서 그 메모리 읽어서 state reg 14로 전송
        if (control_signal[8] == 1) { // Lw
            size_t pos;
            WriteStateReg(14, stol(ReadMem(ReadStateReg(11)), &pos, 0)); 
        }
        if (control_signal[8] == 0) { // Lb
            int offset = ReadStateReg(18);
            size_t pos;
            int data = stol(ReadMem(ReadStateReg(11)), &pos, 0); // offset이 6이면 그 다음 주소 읽기, -6이면 그 전 주소 읽기 
            WriteStateReg(14, (data>>((3-(offset%4))*8))&0xFF);
        }
    }
    if (control_signal[4] == 1) {
        // 이 때는 memory_write (=Sw, Sb)
        if (control_signal[8] == 1) { // Sw
            WriteMem(ReadStateReg(11), Hexadecimaler(Binaryer(ReadStateReg(19),32)));
        }
        if (control_signal[8] == 0) { // Sb -> 주석은 HW2 참고
            int offset = ReadStateReg(18);
            size_t pos;
            int data = stol(ReadMem(ReadStateReg(11)), &pos, 0);
            int new_data = (((data>>((4-offset%4)*8))<<((4-offset%4)*8))|((unsigned int)(data<<((1+offset%4)*8))>>((1+offset%4)*8)))|(((ReadStateReg(19))&0xFF)<<((3-offset%4)*8));
            WriteMem(ReadStateReg(11), Hexadecimaler(Binaryer(new_data,32)));
        }
    }
    // ====== 여기까지가 연산 -> 이 밑은 WB하기 위해 레지스터 값 옮기기
    // 기존에 ALU 연산 저장되어 있던 11 reg 그대로 13 reg에 옮기기
    // 이 때 WB하지 않는 inst는 13reg 채우지 않게 하기
    // WB하지 않는 inst: R<-jr, RX<-Beq,Bne,J,Jal,Sw,Sb
    if (control_signal[5] == 1 && control_signal[3] == 0) {
        WriteStateReg(13, ReadStateReg(11));
        // lw, lb 제외 13레지에 옮겨 적기
    }
    WriteStateReg(17, ReadStateReg(16)); // WB reg 기록
    // Forwarding
}

void WB_stage(vector<int> control_signal) {
    // DataHazard: MEM/WB to EX Forwarding
    if(control_signal[5] == 1) {
        if (ReadStateReg(17) == ReadStateReg(20)) { // 17: MEM_WB.WB, WB일 때
            if (ReadStateReg(16) != ReadStateReg(20)) { // 20은 굳이 forwarding될 필요 없는 레지스터 -> state reg는 다음 cycle에 값 바뀌는 걸 보존 하기 위한 레지이기 때문!
                WriteStateReg(4, ReadStateReg(13));
                // Forwarding
            }
        } 
        if (ReadStateReg(17) == ReadStateReg(21)) { // 17: MEM_WB.WB, WB일 때
            if (ReadStateReg(16) != ReadStateReg(21)) {
                WriteStateReg(5, ReadStateReg(13));
                // Forwarding
            }
        } 
    }
    // DataHazard: MEM/WB to MEM Forwarding
    if(control_signal[3] == 1) {
        if (ReadStateReg(17) == ReadStateReg(23)) { // rt 같을 때
            WriteStateReg(19, ReadStateReg(14));
            // Forwarding
        } 
    }
    // WB stage
    // rd에 쓸 애들, rt에 쓸 애들 나눠서 적기 rt일 때 lw, lb는 reg14읽게 하기 -> rd, rt는 ID 부분에서 이미 나눠서 저장
    // 여기까지 쓰고, 한 번 돌려보고 hazard 처리하기
    // rt: addiu, andi, lui, lw, lb, ori, sltiu
    // rd: addu, and, nor, or, sltu, sll, srl, subu
    if (control_signal[5] == 1) {
        if (control_signal[3] == 1) {
            WriteReg(ReadStateReg(17), ReadStateReg(14));
        }
        else {
            WriteReg(ReadStateReg(17), ReadStateReg(13));
        }
    }
}

// ============= function implementation (에뮬레이터 함수)

void Emulator(ifstream& input, int d, int n, int m, int p, int atp, int num_of_instruction, int begin_address_int, int end_address_int) {
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
    // 레지스터 초기화
    for (int i=0; i<=31; i++) {
        REGISTER *new_register = new REGISTER(i); 
        new_register->UpdateData(0x0);
    } 
    // 메모리 초기화
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
    // 읽는 건 이제 끝!!!!
    input.close();
    // 파이프 라이닝을 위한 state reg 값 초기화
    for (int i=0; i<=24; i++) { // 레지스터 갯수 21개
        STATE_REGISTER *new__state_register = new STATE_REGISTER(i); 
        // initializing X
    } 
    // 0. IF_ID.Instr O
    // 1. IF_ID.NPC: PC값
    // 2. ID_EX.NPC: PC값
    // 3. ID_EX.Opcode O
    // 4. ID_EX.rs O -> EX stage에서 레지스터를 바탕으로 업데이트 하면 WB 스테이지 전에 값을 아예 업데이트 할 수 있기 때문에
    // 5. ID_EX.rt O 
    // 6. ID_EX.IMM O 
    // 7. ID_EX.rd O
    // 8. ID_EX.shamt O
    // 10. ID.EX.function_field O
    // 11. EX_MEM.ALU_OUT O
    // 12. EX_MEM.BR_TARGET
    // 13. MEM_WB.ALU_OUT -> rd에 저장하기 위해 그 전에 연산 되어 있던 값을 WB로 옮기기
    // 14. MEM_WB.MEM_OUT -> 메모리에 저장되어 있던 값을 WB로 옮기기
    // 15. 추가 state reg ID.EX.WB
    // 16. 추가 state reg EX.MEM.WB
    // 17. 추가 state reg MEM.WB.WB -> WB reg 뭔지 알기 위한 signal
    // 18. 추가 state reg EX.MEM.IMM -> mem sw, lw 연산에 필요한 것들
    // 19. 추가 state reg EX.MEM.rt -> mem sw, lw 연산에 필요한 것들
    // 20. 추가 state reg ID.EX.rs -> data hazard 확인하기 위한 reg
    // 21. 추가 state reg ID.EX.rt -> data hazard 확인하기 위한 reg
    // 22. 추가 state reg EX.MEM.rs -> mem data hazard 확인하기 위한 reg (여기는 rs 번호)
    // 23. 추가 state reg EX.MEM.rt -> mem data hazard 확인하기 위한 reg (여기는 rt 번호)
    // 24. 추가 state reg EX.MEM.NPC: PC값 (brch 예측에 쓰이는)
    // 4,5와 20,21 만든 이유 보고서에 적기
    // ========================================
    // Emulator 본격적인 동작 시작
    vector<int> control_signal_ID = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    vector<int> control_signal_EX = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    vector<int> control_signal_MEM = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    vector<int> control_signal_WB = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int end_checker = 0; // 출력 마무리 신호
    int inst; // PC 값을 바탕으로 instruction memory list에 어디있는지 찾기 위한 변수
    int inst_end_checker = 0; // instruction이 다 끝났는지 확인하는 변수
    int instruction_counter = 0; // 실행된 instruction의 개수를 세는 변수
    int cycle_counter = 0;
    PROGRAMCOUNTER *new_program_counter = new PROGRAMCOUNTER(); 
    if (n == 0) {
        while (end_checker == 0) {
            PC_buffer = 0; // PC 버퍼 초기화
            vector<int> pipelining_finish = {0, 0, 0, 0, 0}; // instruction이 다 끝났는지 확인하는 벡터
            if (new_program_counter->GetPC() != TEXT_MEMORY::text_memory_list.back()->ReadAddress()+4) {
                for (int i=0; i<TEXT_MEMORY::text_memory_list.size(); i++) {
                    if (new_program_counter->GetPC() == TEXT_MEMORY::text_memory_list[i]->ReadAddress()) {
                        // PC 주소랑 같은 인스트럭션을 찾았을 때 인스트럭션 실행
                        inst = i;
                    }
                }
            }
            else {
                inst_end_checker = 1;
            }
            int pipeline_keep = pipelining[1]; // bubble일 때 keep
            if (pipelining_control[0] == 0) { // stall
                pipelining[0] = 0;
            }
            if (pipelining_control[1] == 0) {
                pipelining[1] = 0;
            }
            if (pipelining_control[2] == 0) {
                pipelining[2] = 0;
            }
            if (pipelining_control[3] == 0) {
                pipelining[3] = 0;
            }
            if (pipelining_control[4] == 0) {
                pipelining[4] = 0;
            }
            pipelining_control = {1,1,1,1,1};
            if (Load_use_Hazard_pipe == 1) { // 버블일 때는 뒤에꺼만 미루기 -> 약간 하드 코딩...
                pipelining[4] = pipelining[3]; // 파이프 한 칸씩 미루기
                pipelining[3] = pipelining[2]; // 파이프 한 칸씩 미루기
                pipelining[2] = pipelining[1]; // 파이프 한 칸씩 미루기
                pipelining[1] = pipeline_keep; // 얜 미루지 말기
                Load_use_Hazard_pipe--;
                // k++ 같은 신호 줘서 멈추게 하기 
            }
            else {
                pipelining[4] = pipelining[3]; // 파이프 한 칸씩 미루기
                pipelining[3] = pipelining[2]; // 파이프 한 칸씩 미루기
                pipelining[2] = pipelining[1]; // 파이프 한 칸씩 미루기
                pipelining[1] = pipelining[0]; // 파이프 한 칸씩 미루기
            }
            if (inst_end_checker == 1) {
                pipelining[0] = 0;
            } 
            else {
                pipelining[0] = new_program_counter->GetPC(); // 0은 PC값으로 update
            }
            // stage 시작
            control_signal_WB = control_signal_MEM;
            if (pipelining[4] != 0) { // ==0인 경우는 noop가 들어온 상황 (= stall)
                WB_stage(control_signal_WB);
                pipelining_finish[4] = 1;
                instruction_counter++;
            }
            control_signal_MEM = control_signal_EX;
            if (pipelining[3] != 0) { // ==0인 경우는 noop가 들어온 상황 (= stall)
                MEM_stage(control_signal_MEM, atp);
                pipelining_finish[3] = 1;
            }
            control_signal_EX = control_signal_ID;
            if (pipelining[2] != 0) { // ==0인 경우는 noop가 들어온 상황 (= stall)
                EX_stage(control_signal_EX);
                pipelining_finish[2] = 1;
            }
            if (pipelining[1] != 0) { // ==0인 경우는 noop가 들어온 상황 (= stall)
                control_signal_ID = ID_stage(ReadStateReg(0), atp);
                pipelining_finish[1] = 1;
            }
            if (inst_end_checker == 0) { // instruction 메모리 끝나면 동작 X -> 그 전까지만 동작
                IF_stage(TEXT_MEMORY::text_memory_list[inst]->ReadData());
                // 파이프 라이닝 벡터 첫 번째 원소 이거 주소로 바꾸기
                pipelining_finish[0] = 1;
            }
            if (pipelining_finish[0] == 0 && pipelining_finish[1] == 0 && pipelining_finish[2] == 0 && pipelining_finish[3] == 0 && pipelining_finish[4] == 0) {
                end_checker = 1;
                // endchecker에 신호가 오면 종료
            }
            cycle_counter++;
            // 버퍼에 값이 들어있으면 버퍼 값으로 PC 업데이트
            if (PC_buffer == 0) {
                if (inst_end_checker == 1) {
                }
                else {    
                    PROGRAMCOUNTER::PCPointer->UpdateProgramCounter(PROGRAMCOUNTER::PCPointer->GetPC()+4);
                }
            }
            else {
                PROGRAMCOUNTER::PCPointer->UpdateProgramCounter(PC_buffer);
                inst_end_checker = 0;
            }
            if (d == 1) {
                if (end_checker == 1) {
                    cout << "===== Completion cycle: " << cycle_counter << " =====" << endl;
                    cout << endl;
                }
                else {
                    cout << "===== Cycle " << cycle_counter << " =====" << endl;
                    cout << endl;
                }
                cout << endl;
                if (m == 1 && p == 1) {
                    PrintPipeLineContents(pipelining[0], pipelining[1], pipelining[2], pipelining[3], pipelining[4]);
                    PrintRgisterData();
                    PrintMemoryContents(begin_address_int, end_address_int);
                }
                if (m == 1 && p == 0) {
                    PrintRgisterData();
                    PrintMemoryContents(begin_address_int, end_address_int);
                }
                if (m == 0 && p == 1) {
                    PrintPipeLineContents(pipelining[0], pipelining[1], pipelining[2], pipelining[3], pipelining[4]);
                    PrintRgisterData();
                }
                if (m == 0 && p == 0) {
                    PrintRgisterData();
                }
            }
        }
    }
    if (n == 1) {
        if (num_of_instruction != 0) { // -n 1부터 제대로 된 시작하기!
            while (num_of_instruction != instruction_counter) {
                PC_buffer = 0; // PC 버퍼 초기화
                if (end_checker == 0) {
                    vector<int> pipelining_finish = {0, 0, 0, 0, 0}; // instruction이 다 끝났는지 확인하는 벡터
                    if (new_program_counter->GetPC() != TEXT_MEMORY::text_memory_list.back()->ReadAddress()+4) {
                        for (int i=0; i<TEXT_MEMORY::text_memory_list.size(); i++) {
                            if (new_program_counter->GetPC() == TEXT_MEMORY::text_memory_list[i]->ReadAddress()) {
                                // PC 주소랑 같은 인스트럭션을 찾았을 때 인스트럭션 실행
                                inst = i;
                            }
                        }
                    }
                    else {
                        inst_end_checker = 1;
                    }
                    int pipeline_keep = pipelining[1]; // bubble일 때 keep
                    if (pipelining_control[0] == 0) { // stall
                        pipelining[0] = 0;
                    }
                    if (pipelining_control[1] == 0) {
                        pipelining[1] = 0;
                    }
                    if (pipelining_control[2] == 0) {
                        pipelining[2] = 0;
                    }
                    if (pipelining_control[3] == 0) {
                        pipelining[3] = 0;
                    }
                    if (pipelining_control[4] == 0) {
                        pipelining[4] = 0;
                    }
                    pipelining_control = {1,1,1,1,1};
                    if (Load_use_Hazard_pipe == 1) { // 버블일 때는 뒤에꺼만 미루기
                        pipelining[4] = pipelining[3]; // 파이프 한 칸씩 미루기
                        pipelining[3] = pipelining[2]; // 파이프 한 칸씩 미루기
                        pipelining[2] = pipelining[1]; // 파이프 한 칸씩 미루기
                        pipelining[1] = pipeline_keep; // 얜 미루지 말기
                        Load_use_Hazard_pipe--;
                    }
                    else {
                        pipelining[4] = pipelining[3]; // 파이프 한 칸씩 미루기
                        pipelining[3] = pipelining[2]; // 파이프 한 칸씩 미루기
                        pipelining[2] = pipelining[1]; // 파이프 한 칸씩 미루기
                        pipelining[1] = pipelining[0]; // 파이프 한 칸씩 미루기
                    }
                    if (inst_end_checker == 1) {
                        pipelining[0] = 0;
                    } 
                    else {
                        pipelining[0] = new_program_counter->GetPC(); // 0은 PC값으로 update
                    }
                    // stage 시작
                    control_signal_WB = control_signal_MEM;
                    if (pipelining[4] != 0) { // ==0인 경우는 noop가 들어온 상황 (= stall)
                        WB_stage(control_signal_WB);
                        pipelining_finish[4] = 1;
                        instruction_counter++;
                    }
                    control_signal_MEM = control_signal_EX;
                    if (pipelining[3] != 0) { // ==0인 경우는 noop가 들어온 상황 (= stall)
                        MEM_stage(control_signal_MEM, atp);
                        pipelining_finish[3] = 1;
                    }
                    control_signal_EX = control_signal_ID;
                    if (pipelining[2] != 0) { // ==0인 경우는 noop가 들어온 상황 (= stall)
                        EX_stage(control_signal_EX);
                        pipelining_finish[2] = 1;
                    }
                    if (pipelining[1] != 0) { // ==0인 경우는 noop가 들어온 상황 (= stall)
                        control_signal_ID = ID_stage(ReadStateReg(0), atp);
                        pipelining_finish[1] = 1;
                    }
                    if (inst_end_checker == 0) { // instruction 메모리 끝나면 동작 X -> 그 전까지만 동작
                        IF_stage(TEXT_MEMORY::text_memory_list[inst]->ReadData());
                        // 파이프 라이닝 벡터 첫 번째 원소 이거 주소로 바꾸기
                        pipelining_finish[0] = 1;
                    }
                    if (pipelining_finish[0] == 0 && pipelining_finish[1] == 0 && pipelining_finish[2] == 0 && pipelining_finish[3] == 0 && pipelining_finish[4] == 0) {
                        end_checker = 1;
                        // endchecker에 신호가 오면 종료
                    }
                    cycle_counter++;
                    // 버퍼에 값이 들어있으면 버퍼 값으로 PC 업데이트
                    if (PC_buffer == 0) {
                        if (inst_end_checker == 1) {
                        }
                        else {    
                            PROGRAMCOUNTER::PCPointer->UpdateProgramCounter(PROGRAMCOUNTER::PCPointer->GetPC()+4);
                        }
                    }
                    else {
                        PROGRAMCOUNTER::PCPointer->UpdateProgramCounter(PC_buffer);
                        inst_end_checker = 0;
                    }
                    if (d == 1) {
                        if (end_checker == 1) {
                            cout << "===== Completion cycle: " << cycle_counter << " =====" << endl;
                            cout << endl;
                        }
                        else {
                            cout << "===== Cycle " << cycle_counter << " =====" << endl;
                            cout << endl;
                        }
                        if (m == 1 && p == 1) {
                            PrintPipeLineContents(pipelining[0], pipelining[1], pipelining[2], pipelining[3], pipelining[4]);
                            PrintRgisterData();
                            PrintMemoryContents(begin_address_int, end_address_int);
                        }
                        if (m == 1 && p == 0) {
                            PrintRgisterData();
                            PrintMemoryContents(begin_address_int, end_address_int);
                        }
                        if (m == 0 && p == 1) {
                            PrintPipeLineContents(pipelining[0], pipelining[1], pipelining[2], pipelining[3], pipelining[4]);
                            PrintRgisterData();
                        }
                        if (m == 0 && p == 0) {
                            PrintRgisterData();
                        }
                    }
                }
                else {
                    instruction_counter++;
                    if (d == 1) {
                        cout << "===== Completion cycle: " << cycle_counter << " =====" << endl;
                        cout << endl;
                        if (m == 1 && p == 1) {
                            PrintPipeLineContents(pipelining[0], pipelining[1], pipelining[2], pipelining[3], pipelining[4]);
                            PrintRgisterData();
                            PrintMemoryContents(begin_address_int, end_address_int);
                        }
                        if (m == 1 && p == 0) {
                            PrintRgisterData();
                            PrintMemoryContents(begin_address_int, end_address_int);
                        }
                        if (m == 0 && p == 1) {
                            PrintPipeLineContents(pipelining[0], pipelining[1], pipelining[2], pipelining[3], pipelining[4]);
                            PrintRgisterData();
                        }
                        if (m == 0 && p == 0) {
                            PrintRgisterData();
                        }
                    }
                }
            }
        }
    }
    if (d == 0) {
        if (end_checker == 1) {
            cout << "===== Completion cycle: " << cycle_counter << " =====" << endl;
            cout << endl;
        }
        else {
            cout << "===== Cycle " << cycle_counter << " =====" << endl;
            cout << endl;
        }
        if (m == 1 && p == 1) {
            PrintPipeLineContents(pipelining[0], pipelining[1], pipelining[2], pipelining[3], pipelining[4]);
            PrintRgisterData();
            PrintMemoryContents(begin_address_int, end_address_int);
        }
        if (m == 1 && p == 0) {
            PrintRgisterData();
            PrintMemoryContents(begin_address_int, end_address_int);
        }
        if (m == 0 && p == 1) {
            PrintPipeLineContents(pipelining[0], pipelining[1], pipelining[2], pipelining[3], pipelining[4]);
            PrintRgisterData();
        }
        if (m == 0 && p == 0) {
            PrintRgisterData();
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
    int p = 0; // p 옵션을 알리는 switch
    int atp = 0; // atp일 때는 1, antp일 때는 0 .. default = 0
    // input 읽기
    for (int i=0; i<argc; i++) {
        if (strcmp(argv[i], "-atp") == 0) {
            atp = 1; // atp일 때는 1, antp일 때는 0
        }
        if (strcmp(argv[i], "-antp") == 0) {
            atp = 0; // atp일 때는 1, antp일 때는 0
        }
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
        if (strcmp(argv[i], "-p") == 0) {
            p = 1; // p 옵션이 실행되었음을 알리는 switch
        }
    }
    Emulator(input, d, n, m, p, atp, num_of_instruction, begin_address_int, end_address_int); // 파일을 인자로 전달, Emulator 함수 실행 -> Emulator에 인자 하나 더 추가해서 print 인자 만들기 -> ex m=1이면 print
    return 0;
}
