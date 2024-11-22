#include <iostream> 
#include <fstream>   
#include <string>
#include <vector>
#include <bitset>
#include <regex>
#include <cstring>
#include <cmath>
#include <list> 
#include <deque>

using namespace std;

class BLOCK;
class CACHE;

class BLOCK {
private:
    int index;
    int way; // 몇 번째 way인지
    vector<vector<vector<int>>> block;
public:
    BLOCK(int index, int way) {
        block.resize(index, vector<vector<int>>(way, vector<int>(2, 0))); // [0(index)][1[data, dirty],2,3 ..(way)]
    } // -1로 초기화, tag가 -1이 될 일 없다고 가정,,
    void Set_Tag(int tag, int now_index, int now_way) {block[now_index][now_way][0] = tag;}
    void Set_Dirty(int dirty, int now_index, int now_way) {block[now_index][now_way][1] = dirty;}
    int Get_Tag(int now_index, int now_way) {return block[now_index][now_way][0];}
    int Get_Dirty(int now_index, int now_way) {return block[now_index][now_way][1];}
};

class CACHE {
private:
    int capacity;
    int associativity; // == way, 개수
    int block_size;
    string replacement_policy;
    vector<deque<int>> lru_list; // 각 index에 way의 데이터 있는 지 없는 지에 대한 정보 가지고 있다.
    int dirty_eviction = 0;
    int clean_eviction = 0;
public:
    BLOCK* B;
    void Set_Capacity(int capacity) {this->capacity = capacity;}
    void Set_Associativity(int associativity) {this->associativity = associativity;}
    void Set_Block_Size(int block_size) {this->block_size = block_size;}
    void Set_Replacement_Policy(string policy) {this->replacement_policy = policy;}
    int Get_capacity() {return capacity;} 
    int Get_associativity() {return associativity;} 
    int Get_Index() {return (capacity / (associativity * block_size));} // 개수
    void Initialize_Blocks() {B = new BLOCK(Get_Index(), associativity);}
    void Initialize_LRU() {
        lru_list.resize(Get_Index(), deque<int>(associativity));
        for (int i = 0; i < Get_Index(); ++i) {
            for (int j = 0; j < associativity; ++j) {
                lru_list[i][j] = j;
            }
        }
    }
    int Find_data(int addr, string W);
    void Set_Tag(int addr); 
    int Get_LRU_Victim(int data_index);
    int Get_Random_Victim();
    void Update_LRU(int index, int way); // hit일 때도 update 해주어야 하므로
    void Update_Eviction(int eviction_type);
    int Get_dirty_eviction() {return dirty_eviction;}
    int Get_clean_eviction() {return clean_eviction;}
};

int CACHE::Find_data(int addr, string W) {
    int block_offset = log2(block_size);
    int index_bit = log2(Get_Index());
    int tag = addr >> (block_offset + index_bit);
    int data_index = (unsigned)(addr << (32 - block_offset - index_bit)) >> (32 - index_bit);
    for (int i=0; i<associativity; i++) {
        if (tag == B->Get_Tag(data_index, i)) {
            if (W == "W") {B->Set_Dirty(1, data_index, i);} // W일 떼 dirty bit setting -> Write back
            if (replacement_policy == "lru") {
                Update_LRU(data_index, i);
            } // hit 이므로 lru update
            return 1;
        } // 리턴 값은 hit값
        
    }
    return 0;
}

void CACHE::Set_Tag(int addr) {
    int block_offset = log2(block_size);
    int index_bit = log2(Get_Index());
    int tag = addr >> (block_offset + index_bit);
    int data_index = (unsigned)(addr << (32 - block_offset - index_bit)) >> (32 - index_bit);
    int way;
    int eviction = 0; // dirty = 2, clear = 1, not = 0

    if (replacement_policy == "lru") {
        way = Get_LRU_Victim(data_index);
    } else {
        way = Get_Random_Victim();
    }
    if (B->Get_Dirty(data_index, way) == 1) {
        Update_Eviction(1);
    } else { Update_Eviction(0); } // Eviction memo for write back

    B->Set_Tag(tag, data_index, way);
    B->Set_Dirty(0, data_index, way); // Initially not dirty
    if (replacement_policy == "lru") {
        Update_LRU(data_index, way); // LRU 다시 업데이트
    }
}

int CACHE::Get_LRU_Victim(int data_index) {
    int victim = lru_list[data_index].front(); // 가장 오랫동안 사용되지 않은 데이터가 맨 앞에 위치
    lru_list[data_index].pop_front();
    return victim;
}

int CACHE::Get_Random_Victim() {
    return rand() % associativity;
}

void CACHE::Update_LRU(int index, int way) {
    lru_list[index].push_back(way);
}

void CACHE::Update_Eviction(int eviction_type) {
    if (eviction_type == 1) {
        this->dirty_eviction++;
    } else { this->clean_eviction++; }
}

// class define

void Caching(ifstream& input, ofstream& output, int capacity, int associativity, int block_size, string policy) {
    // output def
    int total_access; int read_access; int write_access;
    int L1_read_miss; int L1_write_miss;
    int L2_read_miss; int L2_write_miss;
    int L1_clean_eviction; int L1_dirty_eviction;
    int L2_clean_eviction; int L2_dirty_eviction;

    // define
    CACHE L1, L2;
    string line;

    // allocate
    L2.Set_Associativity(associativity);
    L2.Set_Capacity(capacity * 1024);
    L2.Set_Block_Size(block_size);
    L2.Set_Replacement_Policy(policy);
    L1.Set_Associativity(associativity == 1 ? 1 : (associativity == 2 ? 2 : associativity / 4));
    L1.Set_Capacity((capacity * 1024) / 4);
    L1.Set_Block_Size(block_size);
    L1.Set_Replacement_Policy(policy);

    // allocate block
    L1.Initialize_Blocks();
    L2.Initialize_Blocks();
    L1.Initialize_LRU();
    L2.Initialize_LRU();

    // caching
    while (getline(input, line)) {
        total_access++; 
        string W = line.substr(0, 1);
        size_t pos;
        int addr = stol(line.substr(2), &pos, 0);

        if (W == "R") {
            read_access++;
            if (L1.Find_data(addr, "R") == 1) { // L1.hit L2.hit
            } else {
                L1_read_miss++;
                if (L2.Find_data(addr, "R") == 1) { // L1.miss, L2.hit
                    L1.Set_Tag(addr);
                } else {
                    L2_read_miss++;
                    // L1.miss, L2.miss 
                    L1.Set_Tag(addr);
                    L2.Set_Tag(addr);
                }
            }
            
        } else { // if (W == "W")
            write_access++;
            if (L1.Find_data(addr, "W") == 1) { // L1.hit L2.hit
            } else {
                L1_write_miss++;
                if (L2.Find_data(addr, "W") == 1) { // L1.miss, L2.hit
                    L1.Set_Tag(addr);
                } else {
                    L2_write_miss++;
                    // L1.miss, L2.miss 
                    L1.Set_Tag(addr);
                    L2.Set_Tag(addr);
                }
            }
        }
    }

    L1_clean_eviction = L1.Get_clean_eviction() - associativity; L1_dirty_eviction = L1.Get_dirty_eviction();
    L2_clean_eviction = L2.Get_clean_eviction() - associativity; L2_dirty_eviction = L2.Get_dirty_eviction();
    // 코드에서 처음에 associativity를 할당해 주었기 때문에 이를 빼준 것이다.

    if(output.is_open()) {
        output << "L1 Capacity: " << L1.Get_capacity()/1024 << endl;
        output << "L1 Way: " << L1.Get_associativity() << endl;
        output << "L2 Capacity: " << L2.Get_capacity()/1024 << endl;
        output << "L2 Way: " << L2.Get_associativity() << endl;
        output << "Block Size: " << block_size << endl;
        output << "Total Accesses: " << total_access << endl;
        output << "Read Accesses: " << read_access << endl;
        output << "Write Accesses: " << write_access << endl;
        output << "L1 Read Misses: " << L1_read_miss << endl;
        output << "L2 Read Misses: " << L2_read_miss << endl;
        output << "L1 Write Misses: " << L1_write_miss << endl;
        output << "L2 Write Misses: " << L2_write_miss << endl;
        output << "L1 Read Miss Rate : " << static_cast<int>((static_cast<double>(L1_read_miss)/read_access)*100) << "%" << endl;
        output << "L2 Read Miss Rate : " << static_cast<int>((static_cast<double>(L2_read_miss)/L1_read_miss)*100) << "%" << endl;
        output << "L1 Write Miss Rate : " << static_cast<int>((static_cast<double>(L1_write_miss)/write_access)*100) << "%" << endl;
        output << "L2 Write Miss Rate : " << static_cast<int>((static_cast<double>(L2_write_miss)/L1_write_miss)*100) << "%" << endl;
        output << "L1 Clean Eviction: " << L1_clean_eviction<< endl;
        output << "L2 Clean Eviction: " << L2_clean_eviction << endl;
        output << "L1 Dirty Eviction: " << L1_dirty_eviction << endl;
        output << "L2 Dirty Eviction: " << L2_dirty_eviction << endl;
    }
    // eviction 고려해서 설계하기 + random에러
}

int main(int argc, char* argv[]) {
    ifstream input(argv[argc-1]);
    int capacity;
    int associativity;
    int block_size;
    string policy; // 1일 때 lru 정책 실행

    for (int i=0; i<argc; i++) {
        if (strcmp(argv[i], "-c") == 0) {
            size_t pos;
            capacity = stol(argv[i+1], &pos, 0);
        }
        if (strcmp(argv[i], "-a") == 0) {
            size_t pos;
            associativity = stol(argv[i+1], &pos, 0);
        }
        if (strcmp(argv[i], "-b") == 0) {
            size_t pos;
            block_size = stol(argv[i+1], &pos, 0);
        }
        if (strcmp(argv[i], "-lru") == 0) {
            policy = "lru";
        }
        if (strcmp(argv[i], "-random") == 0) {
            policy = "random";
        }
    }

    string filename = argv[argc-1];
    string output_filename;
    size_t pos = filename.find(".out");
    if (pos != string::npos) {
        output_filename = filename.substr(0, pos) + "_" + to_string(capacity) + "_" + to_string(associativity) + "_" + to_string(block_size) +".out";
    }
    cout << output_filename;
    ofstream output(output_filename);

    Caching(input, output, capacity, associativity, block_size, policy); 
    
    output.close();

    return 0;
}
