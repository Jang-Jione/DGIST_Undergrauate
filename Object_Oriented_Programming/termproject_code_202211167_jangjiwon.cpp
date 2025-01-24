#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Account;
class Bank;
class ATM;
class World;
ofstream Summary;

//---------Account---------
class Account {
private:
    string name;        // 계좌 주 이름
    string account_num; // 계좌번호
    int password;       // 비밀번호
    int balance;        // 잔액
    string bank;        // 계좌 은행

public:
    Account(string bank_name, string user_name, string account_number,
        int user_password, int available_funds);
    string getBankName();
    string getAccountNum();
    int getPassword();
    int getBalance();
    void setBalance(int update_balance);
    string getName();
};

//---------------Account implementation

Account::Account(string bank_name, string user_name, string account_number,
    int user_password, int available_funds) {
    this->bank = bank_name;
    this->name = user_name;
    this->account_num = account_number;
    this->password = user_password;
    this->balance = available_funds;
    cout << "[Account Constructor] Account ID: " << this->account_num << endl;
}

string Account::getBankName() { return this->bank; }
string Account::getAccountNum() { return this->account_num; }
int Account::getPassword() { return this->password; }
int Account::getBalance() { return this->balance; }
void Account::setBalance(int update_balance) { this->balance = update_balance; }
string Account::getName() { return this->name; }
//---------Bank---------

class Bank {
private:
    string bank_name;

public:
    vector<Account*> Account_list;
    static vector<ATM*> ATM_list; // 모든 ATM 리스트
    static vector<Bank*> Bank_list; // 모든 bank의 list -> atm에서 사용하기 위함
    Bank(const string);
    void addATM(ATM* atm);      // atm_list에 ATM을 추가하는 함수
    void setAccount(Account*); // 계좌 추가
    void getAccount();          // 관리하는 계좌들 출력
    void setDeposit(Account*, int money); // deposit 결과 계좌에 반영
    void setWithdrawal(Account* account,
        int money); // withdrawal 결과 계좌에 반영
    void setTransfer(Account* account, int money); // transfer 결과 계좌에 반영
    bool verifyPassword(Account* account,
        int pw); // pw가 account 비번 맞는지 확인
    Account* findAccount(string ID);
    string getBankName(); // bank name return
    // vector<Bank*> getAllBank();
    void printAllAccounts();
};

//---------------Bank implementation

vector<Bank*> Bank::Bank_list;
vector<ATM*> Bank::ATM_list;

Bank::Bank(const string name) {
    this->bank_name = name;
    Bank_list.push_back(this); // bank_list stack에 생성
    cout << "[Bank Constructor] Bank name: " << this->bank_name << endl;
}
void Bank::addATM(ATM* atm) { // atm 추가, atm constructor에서 알아서 돌아감
    ATM_list.push_back(atm);
}
void Bank::setAccount(Account* account) { Account_list.push_back(account); }

// vector<Bank*> Bank::getAllBank() { return Bank_list; }

void Bank::getAccount() { return; }

void Bank::setDeposit(Account* account, int money) {
    account->setBalance(account->getBalance() + money);
}

void Bank::setWithdrawal(Account* account, int money) {
    account->setBalance(account->getBalance() - money);
}

void Bank::setTransfer(Account* account, int money) {
    account->setBalance(account->getBalance() + money);
}

bool Bank::verifyPassword(Account* account, int pw) {
    if (account->getPassword() == pw) {
        return true;
    }
    return false;
}

Account* Bank::findAccount(string ID) {
    for (int i = 0; i < Account_list.size(); i++) {
        if (Account_list[i]->getAccountNum() == ID) {
            return Account_list[i];
        }
    }
    return nullptr;
}

string Bank::getBankName() { return this->bank_name; }

void Bank::printAllAccounts() {
    cout << "num of accounts: " << Account_list.size() << endl;
    for (int i = 0; i < Account_list.size(); i++) {
        cout << (i + 1) << ". account number: " << Account_list[i]->getAccountNum()
            << endl;
    }
}

//---------ATM---------
class ATM {
private:
    int serial_type;           // single bank 이면 1, multi bank 이면 2
    Bank* serial_primary_bank; // primary bank의 ptr
    int count = 0;
    int num_of_1000;
    int num_of_5000;
    int num_of_10000;
    int num_of_50000;
    string** SessionSummary = new string* [200];
    int trans = 0;
    int withdrawal_count = 0;
public:
    int serial_num;       // serial number
    Bank* current_bank;   // 현재 사용중인 bank
    Account* current_acc; // 현재 사용중인 account
    vector<Bank*> Bank_list;
    int available_cash; // ATM에 남은 돈
    ATM(Bank* primary_bank, int serial_number, int type, int language,
        int initial_1000, int initial_5000, int initial_10000, int initial_50000,
        vector<Bank*> allBanks); // multi bank ATM constructor
    void addBank();
    bool authorization(string account_num);
    void deposit(); // 여기까지 구현함!!
    void withdrawal();
    bool withdrawal_fee();
    void transfer();
    void StartSession();
    bool authorization();
    void admin();
    void ATMHistory(string trans, Account* account, Account* desinationaccount,
        int amount, string type);
    void PrintHistory(int session);
    int serial_lang_type; // unilingual  이면 1, bilingual  이면 2
    int language_you_want;
    void setLanguage();
    void DisplayOfAccountATMEng(); // 이걸 cin 별로 다 만들어 놓기
    void DisplayOfAccountATMKo();
    string type;
};

//---------------ATM implementaion

// Constructor
ATM::ATM(Bank* primary_bank, int serial_number, int type, int language,
    int initial_1000, int initial_5000, int initial_10000,
    int initial_50000, vector<Bank*> allBanks) {
    // primary bank setting
    serial_primary_bank = primary_bank; // atm의 bank
    serial_primary_bank->addATM(
        this); // Bank class의 static ATM_list에 해당 ATM 추가
    //  serial number setting
    serial_num = serial_number;
    // bank type setting
    serial_type = type;
    serial_lang_type = language;
    // initial cash setting
    this->num_of_1000 = initial_1000;   // 지폐 갯수
    this->num_of_5000 = initial_5000;   // 지폐 갯수
    this->num_of_10000 = initial_10000; // 지폐 갯수
    this->num_of_50000 = initial_50000; // 지폐 갯수
    // bank list
    this->Bank_list = allBanks;
    // session summary 저장해놓을 곳 initialize
    cout << "[ATM Constructor] ATM serial number: " << this->serial_num << endl;
    cout << "serial type: " << serial_type << endl;
    cout << "serial lang: " << serial_lang_type << endl;
}

void ATM::setLanguage() {
    if (this->serial_lang_type == 2) {
        cout
            << "Which language do you want to use? 어떤 언어를 사용하고 싶으신가요?"
            << endl;
        cout << "1.Korean/한국어\t2.English/영어";
        cin >> this->language_you_want;
    }
    else {
        this->language_you_want = 2;
    }
}

void ATM::ATMHistory(string tran, Account* account, Account* destinationaccount,
    int amount, string type) {
    SessionSummary[trans] = new string[9];
    SessionSummary[trans][0] = to_string(trans);
    SessionSummary[trans][1] = tran;
    SessionSummary[trans][8] = type;
    if (account != nullptr) {
        SessionSummary[trans][2] = account->getBankName();
        SessionSummary[trans][3] = account->getAccountNum();
    }
    else {
        SessionSummary[trans][2] = "--";
        SessionSummary[trans][3] = "--";
    }
    if (destinationaccount != nullptr) {
        SessionSummary[trans][4] = destinationaccount->getBankName();
        SessionSummary[trans][5] = destinationaccount->getAccountNum();
    }
    else {
        SessionSummary[trans][4] = "--";
        SessionSummary[trans][5] = "--";
    }
    SessionSummary[trans][6] = to_string(amount);
    SessionSummary[trans][7] = to_string(this->available_cash);

    trans++;
}

// StartSession
// void ATM::StartSession(Bank** bank_list) { // bank list를 파라미터로 받아옴
// StartSession
// void ATM::StartSession(Bank** bank_list) { // bank list를 파라미터로 받아옴
void ATM::StartSession() {
    if (this->language_you_want == 2) {
        int check_acc = 0;
        string account_num; // 계좌번호
        // string bank_name; // 이 계좌의 은행이 어디인지 확인하는 변수
        cout << "Insert card: ";
        cin >> account_num; // 계좌번호 입력받음
        if (account_num == "-1") {
            DisplayOfAccountATMEng();
            cout << "Insert card: ";
            cin >> account_num; // 계좌번호 입력받음
        }
        if (account_num == "000000") {
            admin();
            return;
        }

        // for (int i = 0; i < Bank::num_of_bank; i++) { // bank_list가 10개라서
        // 이케함, len 따로 구해도 됨
        for (int i = 0; i < Bank_list.size(); i++) {
            this->current_acc = Bank_list[i]->findAccount(account_num);
            if (current_acc != nullptr) { // 찾으면 break
                current_bank = Bank_list[i];
                check_acc = 1;
                break;
            }
        }

        if (check_acc == 0) {
            cout << "[Error] Such account does not exist." << endl;
            StartSession();
            return;
        }
        if (this->authorization() == false) {
            cout << "Authorization failed" << endl;
            cout << "Session finished" << endl;
            return;
        }
        int action = 0;
        int session = trans;
        while (action != 4) {
            cout << "Choose transaction" << endl;
            cout << "1.Deposit\t 2.Withdrawal\t3.Transfer\t4.Exit : ";
            cin >> action;
            if (action == -1) {
                DisplayOfAccountATMEng();
                cout << "Choose transaction" << endl;
                cout << "1.Deposit\t 2.Withdrawal\t3.Transfer\t4.Exit : ";
                cin >> action;
            }
            cout << endl;
            if (action == 1) {
                deposit();
                count++;
            }
            else if (action == 2) {
                withdrawal();
                count++;
            }
            else if (action == 3) {
                transfer();
                count++;
            }
            else if (action == 4) {
                PrintHistory(session);
                break;
            }
            else {
                cout << "[ERROR] Please enter 1~4" << endl;
            }
        }
        cout << "Session finished" << endl;
        return;
    }
    if (this->language_you_want == 1) {
        // bank list에 item 어떻게 담아 넣을지 모르겠음
        int check_acc = 0;
        string account_num; // 계좌번호
        // string bank_name; // 이 계좌의 은행이 어디인지 확인하는 변수
        cout << "카드를 입력하시오. : ";
        cin >> account_num; // 계좌번호 입력받음
        if (account_num == "-1") {
            DisplayOfAccountATMKo();
            cout << "카드를 입력하시오. : ";
            cin >> account_num; // 계좌번호 입력받음
        }
        if (account_num == "000000") {
            admin();
            return;
        }
        for (int i = 0; i < Bank_list.size(); i++) {
            this->current_acc = Bank_list[i]->findAccount(account_num);
            if (current_acc != nullptr) { // 찾으면 break
                current_bank = Bank_list[i];
                check_acc = 1;
                break;
            }
        }

        if (check_acc == 0) {
            cout << "잘못된 계좌번호입니다." << endl;
            StartSession();
            return;
        }
        if (this->authorization() == false) {
            cout << "인증 오류" << endl;
            cout << "세션이 종료됩니다." << endl;
            return;
        }
        int action = 0;
        int session = trans;
        while (true) {
            cout << "진행하실 거래를 선택해주세요." << endl;
            cout << "1.예금\t 2.출금 \t3.이체 \t4.취소 : ";
            cin >> action;
            if (action == -1) {
                DisplayOfAccountATMKo();
                cout << "진행하실 거래를 선택해주세요." << endl;
                cout << "1.예금\t 2.출금 \t3.이체 \t4.취소 : ";
                cin >> action;
            }
            cout << endl;
            if (action == 1) {
                deposit();
                count++;
            }
            else if (action == 2) {
                withdrawal();
                count++;
            }
            else if (action == 3) {
                transfer();
                count++;
            }
            else if (action == 4) {
                PrintHistory(session);
                break;
            }
            else {
                cout << "[오류] 거래 1~4 중에 선택하시오." << endl;
            }
        }
        cout << "세션이 종료됩니다." << endl;
        return;
    }
}
// stop

// 3. User Authorization
bool ATM::authorization() {
    if (this->language_you_want == 2) {
        cout << "Authorization Started" << endl;
        int password;
        // REQ3.1 카드가 atm 기기의 타입에 맞는 유효한 카드인지 확인함
        // REQ3.2 유효하지 않으면 -> display error message(ex.”Invalid Card”)
        if ((this->serial_type == 1) &&
            (current_acc->getBankName() != serial_primary_bank->getBankName())) {
            // ATM이 single bank ATM이면서 입력받은 계좌의 은행이 primary bank가 아닐
            // 때
            cout << "[ERROR] Invalid Card: the ATM is a single bank atm, and the "
                "card's bank is not the primary bank"
                << endl;
            return false;
        }
        // REQ3.3 사용자가 비밀번호를 입력하게하여 맞는지 확인 ( bank로 정보 넘겨서
        // 확인받음 ) R EQ3.4 비밀번호 틀렸으면 -> display error message
        cout << "Insert password: ";
        cin >> password;
        if (password == -1) {
            DisplayOfAccountATMEng();
            cout << "Insert password: ";
            cin >> password;
        }
        cout << endl;
        int pw_count = 0;
        while ((current_bank->verifyPassword(current_acc, password)) != true) {
            cout << "[ERROR] Incorrect Password" << endl;
            pw_count++;
            // REQ3.5 연속으로 3번 틀리면 session 끝나야함
            if (pw_count == 3) {
                cout << "Incorrect Password 3 times in a row..." << endl;
                return false; // 여기선 비밀번호 틀림... -> end session
            }
            // 비밀번호 입력 횟수 3번 미만이면 다시 비밀번호 입력받음
            cout << "Insert password: ";
            cin >> password;
            if (password == -1) {
                DisplayOfAccountATMEng();
                cout << "Insert password: ";
                cin >> password;
            }
            cout << endl;
        }
        return true; // 여기선 비밀번호 맞음...
    }
    else if (this->language_you_want == 1) {
        cout << "인증 절차를 시작합니다." << endl;
        int password;
        // REQ3.1 카드가 atm 기기의 타입에 맞는 유효한 카드인지 확인함
        // REQ3.2 유효하지 않으면 -> display error message(ex.”Invalid Card”)
        if ((this->serial_type == 1) &&
            (current_acc->getBankName() != serial_primary_bank->getBankName())) {
            // ATM이 single bank ATM이면서 입력받은 계좌의 은행이 primary bank가 아닐
            // 때
            cout << "[오류] 유효하지 않은 카드입니다: 이 ATM은 단일 거래만 "
                "가능하지만, 카드의 은행은 주 거래 은행이 아닙니다."
                << endl;
            return false;
        }
        // REQ3.3 사용자가 비밀번호를 입력하게하여 맞는지 확인 ( bank로 정보 넘겨서
        // 확인받음 ) R EQ3.4 비밀번호 틀렸으면 -> display error message
        cout << "비밀번호를 입력해주세요." << endl;
        cin >> password;
        if (password == -1) {
            DisplayOfAccountATMKo();
            cout << "비밀번호를 입력해주세요." << endl;
            cin >> password;
        }
        cout << endl;
        int pw_count = 0;
        while ((current_bank->verifyPassword(current_acc, password)) != true) {
            cout << "[오류] 비밀번호가 틀렸습니다." << endl;
            pw_count++;
            // REQ3.5 연속으로 3번 틀리면 session 끝나야함
            if (pw_count == 3) {
                cout << "3회 연속으로 비밀번호가 틀려 세션이 종료됩니다." << endl;
                return false; // 여기선 비밀번호 틀림... -> end session
            }
            // 비밀번호 입력 횟수 3번 미만이면 다시 비밀번호 입력받음
            cout << "비밀번호를 입력해주세요. " << endl;
            cin >> password;
            if (password == -1) {
                DisplayOfAccountATMKo();
                cout << "비밀번호를 입력해주세요." << endl;
                cin >> password;
            }
            cout << endl;
        }
        return true; // 여기선 비밀번호 맞음...
    }
    else {
        return false;
    }
}

// 4. Deposit
void ATM::deposit() {
    int deposit_type;      // 1 for cash deposit, 2 for check deposit
    int deposit_amount;    // deposit 장수
    int total_deposit = 0; // deposit 총액
    int cash_limit = 30;
    int check_limit = 50;
    int deposit_1000;
    int deposit_5000;
    int deposit_10000;
    int deposit_50000;
    if (this->language_you_want == 2) {
        // REQ4.1 현금/수표 입력받음
        cout << "What are you depositing?";
        cout << "1.cash deposit\t2.check deposit\t3. Exit" << endl;
        cin >> deposit_type;
        if (deposit_type == -1) {
            DisplayOfAccountATMEng();
            cout << "What are you depositing?";
            cout << "1.cash deposit\t2.check deposit\t3. Exit" << endl;
            cin >> deposit_type;
        }
        cout << endl;

        // cash deposit 선택함
        if (deposit_type == 1) {
            type = "Cash Deposit";
            cout << "How much do you want to deposit?" << endl;
            cout << "Insert the amount of 1000won cash : ";
            cin >> deposit_1000;
            if (deposit_1000 == -1) {
                DisplayOfAccountATMEng();
                cout << "How much do you want to deposit?" << endl;
                cout << "Insert the amount of 1000won cash : ";
                cin >> deposit_1000;
            }
            cout << endl;
            if (deposit_1000 < -1) { // 0 이하의 정수 받았을 때
                cout << "Error : Amount can't be negative." << endl;
                cout << endl;
                return;
            }
            cout << "Insert the amount of 5000won cash : ";
            cin >> deposit_5000;
            if (deposit_5000 == -1) {
                DisplayOfAccountATMEng();
                cout << "How much do you want to deposit?" << endl;
                cout << "Insert the amount of 5000won cash : ";
                cin >> deposit_5000;
            }
            cout << endl;
            if (deposit_5000 < -1) { // 0 이하의 정수 받았을 때
                cout << "Error : Amount can't be negative." << endl;
                cout << endl;
                return;
            }
            cout << "Insert the amount of 10000won cash : ";
            cin >> deposit_10000;
            if (deposit_10000 == -1) {
                DisplayOfAccountATMEng();
                cout << "How much do you want to deposit?" << endl;
                cout << "Insert the amount of 10000won cash : ";
                cin >> deposit_10000;
            }
            cout << endl;
            if (deposit_10000 < -1) { // 0 이하의 정수 받았을 때
                cout << "Error : Amount can't be negative." << endl;
                cout << endl;
                return;
            }
            cout << "Insert the amount of 50000won cash : ";
            cin >> deposit_50000;
            if (deposit_50000 == -1) {
                DisplayOfAccountATMEng();
                cout << "How much do you want to deposit?" << endl;
                cout << "Insert the amount of 50000won cash : ";
                cin >> deposit_50000;
            }
            cout << endl;
            if (deposit_50000 < -1) { // 0 이하의 정수 받았을 때
                cout << "Error : Amount can't be negative." << endl;
                cout << endl;
                return;
            }

            deposit_amount =
                deposit_1000 + deposit_5000 + deposit_10000 + deposit_50000;
            total_deposit = deposit_1000 * 1000 + deposit_5000 * 5000 +
                deposit_10000 * 10000 + deposit_50000 * 50000;
            // Exceptional Handling : 이체량이 0원이면 Error Message
            if (deposit_amount == 0) {
                cout << "[ERROR] You should deposit least 1000 for transaction."
                    << endl;
                cout << endl;
                return;
            }
            // REQ4.2 개수 제한 넘으면 display error message
            if (deposit_amount > cash_limit) {
                cout << "[ERROR] Your deposit amount is over 30(cash deposit limit)."
                    << endl;
                return;
            }

            // REQ4.4 이체 수수료 부과될 수 있음 ( non - primary bank deposit일 때)
            if ((this->serial_type == 2) &&
                (current_acc->getBankName() != serial_primary_bank->getBankName())) {
                int confirm_fee = 2;
                cout << "You are charged for deposit fee of 1000won." << endl;
                cout << "1.Insert addtional cash \t 2.Exit \t : ";
                cin >> confirm_fee;
                if (confirm_fee == -1) {
                    DisplayOfAccountATMEng();
                    cout << "You are charged for deposit fee of 1000won." << endl;
                    cout << "1.Insert addtional cash \t 2.Exit \t : ";
                    cin >> confirm_fee;
                }
                if (confirm_fee == 2) {
                    return;
                }
                num_of_1000 += 1;
            }
            // REQ4.3 현금/수표 accept됨 -> 은행 계좌에 거래 결과 반영
            // 계좌 잔액에 총 예금액 더해주는 Bank 클래스 함수
            current_bank->setDeposit(this->current_acc, total_deposit);
            // REQ4.5 현금 -> ATM 기기의 available cash 늘어남
            num_of_1000 += deposit_1000;
            num_of_5000 += deposit_5000;
            num_of_10000 += deposit_10000;
            num_of_50000 += deposit_50000;
        }

        // check deposit 선택함
        else if (deposit_type == 2) {
            type = "Check Deposit";
            cout << "How much do you want to deposit?" << endl;
            cout << "Insert the amount of check : ";
            cin >> deposit_amount;
            if (deposit_amount == -1) {
                DisplayOfAccountATMEng();
                cout << "How much do you want to deposit?" << endl;
                cout << "Insert the amount of check : ";
                cin >> deposit_amount;
            }
            if (deposit_amount < -1) {
                cout << "Error : Amount can't be negative"
                    << endl; // amount가 0보다 낮으면 안됨
                cout << endl;
                return;
            }
            cout << endl;
            int check_amount;
            for (int i = 0; i < deposit_amount; i++) {
                cout << "Insert the amount of money of check " << i << " : ";
                cin >> check_amount;
                if (check_amount == -1) {
                    DisplayOfAccountATMEng();
                    cout << "Insert the amount of money of check " << i << " : ";
                    cin >> check_amount;
                }
                total_deposit += check_amount;
            }
            cout << endl;

            // REQ4.2 개수 제한 넘으면 display error message
            if (deposit_amount > check_limit) {
                cout << "[ERROR] Your deposit amount is over 50(check deposit limit)."
                    << endl;
                return;
            }
            // REQ4.4 이체 수수료 부과될 수 있음 (non-primary bank로 deposit 할 때)
            if ((this->serial_type == 2) &&
                (current_acc->getBankName() != serial_primary_bank->getBankName())) {
                int confirm_fee = 2;
                cout << "You are charged for deposit fee of 1000won." << endl;
                cout << "1.Insert addtional cash \t 2.Exit \t : ";
                cin >> confirm_fee;
                if (confirm_fee == -1) {
                    DisplayOfAccountATMEng();
                    cout << "You are charged for deposit fee of 1000won." << endl;
                    cout << "1.Insert addtional cash \t 2.Exit \t : ";
                    cin >> confirm_fee;
                }
                if (confirm_fee == 2) {
                    return;
                }
                num_of_1000 += 1;
            }
            // REQ4.3 현금/수표 accept됨 -> 은행 계좌에 거래 결과 반영
            // 계좌 잔액에 총 예금액 더해주는 Bank 클래스 함수
            current_bank->setDeposit(this->current_acc, total_deposit);

        }
        // REQ4.6 수표 -> available cash 변화x

        // Exit 선택함
        else if (deposit_type == 3) {
            return;
        }
        // Exception handling
        else {
            cout << "[ERROR] You must choose from 1 to 3." << endl;
            return;
        }

        cout << "Success! You have deposited " << total_deposit
            << " won to your account." << endl;
        cout << endl;
        cout << "The remaining balance in your " << current_bank->getBankName()
            << " account " << current_acc->getAccountNum();
        cout << " is " << current_acc->getBalance() << " won" << endl;
        cout << endl;
        cout << "The remaining 1000 in ATM : " << num_of_1000 * 1000 << endl;
        cout << "The remaining 5000 in ATM : " << num_of_5000 * 5000 << endl;
        cout << "The remaining 10000 in ATM : " << num_of_10000 * 10000 << endl;
        cout << "The remaining 50000 in ATM : " << num_of_50000 * 50000 << endl;
        cout << endl;

        ATMHistory("Deposit", current_acc, nullptr, total_deposit, type);
    }
    if (this->language_you_want == 1) {
        // REQ4.1 현금/수표 입력받음
        cout << "어떤 형태로 예금을 하십니까?";
        cout << "1.화폐 \t 2.수표 \t 3. 취소" << endl;
        cin >> deposit_type;
        if (deposit_type == -1) {
            DisplayOfAccountATMKo();
            cout << "어떤 형태로 예금을 하십니까?";
            cout << "1.화폐 \t 2.수표 \t 3. 취소" << endl;
            cin >> deposit_type;
        }
        cout << endl;

        // cash deposit 선택함
        if (deposit_type == 1) {
            type = "Cash Deposit";
            cout << "예금하고자 하는 양을 각각 입력해주세요" << endl;
            cout << "천원 지폐 개수 : ";
            cin >> deposit_1000;
            if (deposit_1000 == -1) {
                DisplayOfAccountATMKo();
                cout << "예금하고자 하는 양을 각각 입력해주세요" << endl;
                cout << "천원 지폐 개수 : ";
                cin >> deposit_1000;
            }
            cout << endl;
            if (deposit_1000 < -1) { // 0 이하의 정수 받았을 때
                cout << "[오류] 수량은 0보다 작을 수 없습니다." << endl;
                return;
            }
            cout << "오천원 지폐 개수 : ";
            cin >> deposit_5000;
            if (deposit_5000 == -1) {
                DisplayOfAccountATMKo();
                cout << "예금하고자 하는 양을 각각 입력해주세요" << endl;
                cout << "오천원 지폐 개수 : ";
                cin >> deposit_5000;
            }
            cout << endl;
            if (deposit_5000 < -1) { // 0 이하의 정수 받았을 때
                cout << "[오류] 수량은 0보다 작을 수 없습니다." << endl;
                return;
            }
            cout << "만원 지폐 개수 : ";
            cin >> deposit_10000;
            if (deposit_10000 == -1) {
                DisplayOfAccountATMKo();
                cout << "예금하고자 하는 양을 각각 입력해주세요" << endl;
                cout << "만원 지폐 개수 : ";
                cin >> deposit_10000;
            }
            cout << endl;
            if (deposit_10000 < -1) { // 0 이하의 정수 받았을 때
                cout << "[오류] 수량은 0보다 작을 수 없습니다." << endl;
                cout << endl;
                return;
            }
            cout << "오만원 지폐 개수 : ";
            cin >> deposit_50000;
            if (deposit_50000 == -1) {
                DisplayOfAccountATMKo();
                cout << "예금하고자 하는 양을 각각 입력해주세요" << endl;
                cout << "오만원 지폐 개수 : ";
                cin >> deposit_50000;
            }
            cout << endl;
            if (deposit_50000 < -1) { // 0 이하의 정수 받았을 때
                cout << "[오류] 수량은 0보다 작을 수 없습니다." << endl;
                cout << endl;
                return;
            }

            deposit_amount =
                deposit_1000 + deposit_5000 + deposit_10000 + deposit_50000;
            total_deposit = deposit_1000 * 1000 + deposit_5000 * 5000 +
                deposit_10000 * 10000 + deposit_50000 * 50000;
            // Exceptional Handling : 이체량이 0원이면 Error Message
            if (deposit_amount == 0) {
                cout << "[오류] 예금을 위해선 최소 천원을 입력해야 합니다." << endl;
                cout << endl;
                return;
            }
            // REQ4.2 개수 제한 넘으면 display error message
            if (deposit_amount > cash_limit) {
                cout << "[오류] 예금 가능한 화폐의 최대 갯수는 총 30장입니다. (갯수 "
                    "제한)"
                    << endl;
                return; // !!! 에러 발생하면 바로 반환되는가, 아니면 while loop 이용해서
                // 다시 입력받는가..
            }

            // REQ4.4 이체 수수료 부과될 수 있음 ( non - primary bank deposit일 때)
            if ((this->serial_type == 2) &&
                (current_acc->getBankName() != serial_primary_bank->getBankName())) {
                int confirm_fee = 2;
                cout << "예금 수수료로 1000원이 청구됩니다." << endl;
                cout << "1.추가적인 현금 넣기 \t 2.취소 \t : ";
                cin >> confirm_fee;
                if (confirm_fee == -1) {
                    DisplayOfAccountATMKo();
                    cout << "예금 수수료로 1000원이 청구됩니다." << endl;
                    cout << "1.추가적인 현금 넣기 \t 2.취소 \t : ";
                    cin >> confirm_fee;
                }
                if (confirm_fee == 2) {
                    return;
                }
                num_of_1000 += 1;
            }
            // REQ4.3 현금/수표 accept됨 -> 은행 계좌에 거래 결과 반영
            // 계좌 잔액에 총 예금액 더해주는 Bank 클래스 함수
            current_bank->setDeposit(this->current_acc, total_deposit);
            // REQ4.5 현금 -> ATM 기기의 available cash 늘어남
            num_of_1000 += deposit_1000;
            num_of_5000 += deposit_5000;
            num_of_10000 += deposit_10000;
            num_of_50000 += deposit_50000;
        }

        // check deposit 선택함
        else if (deposit_type == 2) {
            type = "Check Deposit";
            cout << "예금하고자 수표의 장 수를 입력해주세요." << endl;
            cin >> deposit_amount;
            if (deposit_amount == -1) {
                DisplayOfAccountATMKo();
                cout << "예금하고자 수표의 장 수를 입력해주세요." << endl;
                cin >> deposit_amount;
            }
            if (deposit_amount < -1) {
                cout << "[오류] 수량은 0보다 작을 수 없습니다."
                    << endl; // amount가 0보다 낮으면 안됨
                cout << endl;
                return;
            }
            cout << endl;
            int check_amount;
            for (int i = 0; i < deposit_amount; i++) {
                cout << (i + 1) << "번째 수표의 금액을 입력해주세요. : ";
                cin >> check_amount;
                if (check_amount == -1) {
                    DisplayOfAccountATMKo();
                    cout << (i + 1) << "번째 수표의 금액을 입력해주세요. : ";
                    cin >> check_amount;
                }
                total_deposit += check_amount;
            }
            cout << endl;

            // REQ4.2 개수 제한 넘으면 display error message
            if (deposit_amount > check_limit) {
                cout
                    << "[오류] 예금 가능한 화폐의 최대 갯수는 총 50(개수 제한)장입니다."
                    << endl;
                return;
            }
            // REQ4.4 이체 수수료 부과될 수 있음 (non-primary bank로 deposit 할 때)
            if ((this->serial_type == 2) &&
                (current_acc->getBankName() != serial_primary_bank->getBankName())) {
                int confirm_fee = 2;
                cout << "예금 수수료로 1000원이 청구됩니다." << endl;
                cout << "1.추가적인 현금 넣기 \t 2.취소 \t : ";
                cin >> confirm_fee;
                if (confirm_fee == -1) {
                    DisplayOfAccountATMKo();
                    cout << "예금 수수료로 1000원이 청구됩니다." << endl;
                    cout << "1.추가적인 현금 넣기 \t 2.취소 \t : ";
                    cin >> confirm_fee;
                }
                if (confirm_fee == 2) {
                    return;
                }
                num_of_1000 += 1;
            }
            // REQ4.3 현금/수표 accept됨 -> 은행 계좌에 거래 결과 반영
            // 계좌 잔액에 총 예금액 더해주는 Bank 클래스 함수
            current_bank->setDeposit(this->current_acc, total_deposit);

        }
        // REQ4.6 수표 -> available cash 변화x

        // Exit 선택함
        else if (deposit_type == 3) {
            return;
        }
        // Exception handling
        else {
            cout << "[오류] 예금 종류 1~3 중에 선택해야 합니다.." << endl;
            return;
        }

        cout << total_deposit << "원이 계좌로 입금되었습니다." << endl;
        cout << endl;
        cout << current_bank->getBankName() << "의 " << current_acc->getAccountNum()
            << "계좌에 남아있는 총 금액은 ";
        cout << current_acc->getBalance() << "원입니다." << endl;
        cout << endl;
        cout << "ATM에 남아있는 천원의 갯수 : " << num_of_1000 * 1000 << endl;
        cout << "ATM에 남아있는 오천원의 갯수 : " << num_of_5000 * 5000 << endl;
        cout << "ATM에 남아있는 만원의 갯수 : " << num_of_10000 * 10000 << endl;
        cout << "ATM에 남아있는 오만원의 갯수 : " << num_of_50000 * 50000 << endl;
        cout << endl;

        ATMHistory("Deposit", current_acc, nullptr, total_deposit, type);
    }
}

// 5. Withdrawal
void ATM::withdrawal() {
    // check는 withdrawal 불가능
    // int withdrawal_amount; // deposit 장수
    // int total_withdrawal;  // deposit 총액
    int cash_limit = 30;
    int denominationOfBills; // 지폐 종류
    int wrong_input = 0;     // 틀린 입력일 때 종료를 위한 변수
    int sumOfWithdrawal = 0;
    int withdrawal_1000 = 0;
    int withdrawal_5000 = 0;
    int withdrawal_10000 = 0;
    int withdrawal_50000 = 0;
    int fund;
    if (this->language_you_want == 2) {
        // 각 session 당 withdrawal 횟수는 3회까지
        // !!!!!!!!!!!!!!!!!!!
        if (withdrawal_count < 3) {
            // REQ5.1 An ATM shall ask a user to enter the amount of fund to withdra
            cout << "Insert the amount of cash you want to withdrawal." << endl;
            cout << "What denomination of bills would you like to withdraw? (1,000 "
                "won, 5,000 won, 10,000 won, 50,000 won)"
                << endl;
            cout << "please input 1000, 5000, 10000, 50000...: ";
            cin >> denominationOfBills;
            if (denominationOfBills == -1) {
                DisplayOfAccountATMEng();
                cout << "Insert the amount of cash you want to withdrawal." << endl;
                cout << "What denomination of bills would you like to withdraw? (1,000 "
                    "won, 5,000 won, 10,000 won, 50,000 won)"
                    << endl;
                cout << "please input 1000, 5000, 10000, 50000...: ";
                cin >> denominationOfBills;
            }
            cout << endl;

            if (denominationOfBills == 1000) {
                cout << "How many bills of would you like to withdraw?" << endl;
                cout << "please input 1000, 2000, 3000...: ";
                cin >> sumOfWithdrawal;
                if (sumOfWithdrawal == -1) {
                    DisplayOfAccountATMEng();
                    cout << "How many bills of would you like to withdraw?" << endl;
                    cout << "please input 1000, 2000, 3000...: ";
                    cin >> sumOfWithdrawal;
                }
                cout << endl;
                if (sumOfWithdrawal % 1000 == 0) {
                    withdrawal_1000 = sumOfWithdrawal / 1000;
                }
                else {
                    cout << "[Error] Please input the correct amount of bills that you "
                        "want to withdrawal."
                        << endl;
                    wrong_input = 1;
                }
            }
            else if (denominationOfBills == 5000) {
                cout << "How many bills of would you like to withdraw?" << endl;
                cout << "please input 5000, 10000, 15000...: ";
                cin >> sumOfWithdrawal;
                if (sumOfWithdrawal == -1) {
                    DisplayOfAccountATMEng();
                    cout << "How many bills of would you like to withdraw?" << endl;
                    cout << "please input 5000, 10000, 15000...: ";
                    cin >> sumOfWithdrawal;
                }
                cout << endl;
                if (sumOfWithdrawal % 5000 == 0) {
                    withdrawal_5000 = sumOfWithdrawal / 5000;
                }
                else {
                    cout << "[Error] Please input the correct amount of bills that you "
                        "want to withdrawal."
                        << endl;
                    wrong_input = 1;
                }
            }
            else if (denominationOfBills == 10000) {
                cout << "How many bills of would you like to withdraw?" << endl;
                cout << "please input 10000, 20000, 30000...: ";
                cin >> sumOfWithdrawal;
                if (sumOfWithdrawal == -1) {
                    DisplayOfAccountATMEng();
                    cout << "How many bills of would you like to withdraw?" << endl;
                    cout << "please input 10000, 20000, 30000...: ";
                    cin >> sumOfWithdrawal;
                }
                cout << endl;
                if (sumOfWithdrawal % 10000 == 0) {
                    withdrawal_10000 = sumOfWithdrawal / 10000;
                }
                else {
                    cout << "[Error] Please input the correct amount of bills that you "
                        "want to withdrawal."
                        << endl;
                    wrong_input = 1;
                }
            }
            else if (denominationOfBills == 50000) {
                cout << "How many bills of would you like to withdraw?" << endl;
                cout << "please input 50000, 100000, 150000...: ";
                cin >> sumOfWithdrawal;
                if (sumOfWithdrawal == -1) {
                    DisplayOfAccountATMEng();
                    cout << "How many bills of would you like to withdraw?" << endl;
                    cout << "please input 50000, 100000, 150000...: ";
                    cin >> sumOfWithdrawal;
                }
                cout << endl;
                if (sumOfWithdrawal % 50000 == 0) {
                    withdrawal_50000 = sumOfWithdrawal / 50000;
                }
                else {
                    cout << "[Error] Please input the correct amount of bills that you "
                        "want to withdrawal."
                        << endl;
                    wrong_input = 1;
                }
            }
            else {
                cout << "[Error] Please input the correct denomination of bills"
                    << endl;
                wrong_input = 1;
            }

            // REQ5.2 An ATM shall display an appropriate error message if there is
            // insufficient fund in the account or insufficient cash in the ATM.
            if (num_of_1000 < withdrawal_1000 || num_of_5000 < withdrawal_5000 ||
                num_of_10000 < withdrawal_10000 || num_of_50000 < withdrawal_50000) {
                cout << "[Error] There is not enough money in the ATM" << endl;
                cout << endl;
            } // ATM에 돈이 충분하지 않다면 Error 출력
            else if (current_acc->getBalance() < sumOfWithdrawal) {
                cout << "[Error] There is not enough money in your account" << endl;
                cout << endl;
            } // REQ 5.7 The maximum amount of cash withdrawal per transaction is KRW
              // 500,000.
            else if (500000 < sumOfWithdrawal) {
                cout << "[Error] Withdrawals over 500,000 won are not allowed" << endl;
                cout << endl;
            }
            else if (wrong_input == 1) {
                cout << endl;
            }

            //  REQ5.3 Once the withdrawal is successful, the transaction must be
            //  reflected to the bank account
            // as well(i.e., the same amount of fund must be deducted from the
            // corresponding bank account). REQ5.5 The cash withdrawal lower available
            // cash in the ATM that can be used by other users.
            else {
                // REQ5.4 Some withdrawal fee may be charged(See REQ in System Setup).
                if (withdrawal_fee() == true) { // 수수료 낼 수 있으면
                    // ATM에서 돈 빼기
                    num_of_1000 = num_of_1000 - withdrawal_1000;
                    num_of_5000 = num_of_5000 - withdrawal_5000;
                    num_of_10000 = num_of_10000 - withdrawal_10000;
                    num_of_50000 = num_of_50000 - withdrawal_50000;
                    // 계좌에서 돈 빼기
                    current_bank->setWithdrawal(this->current_acc, sumOfWithdrawal);
                    // 계좌/ATM에 남은 금액 출력
                    cout << "Success!" << endl;
                    cout << endl;
                    cout << "The remaining balance in your "
                        << current_bank->getBankName() << " account is ";
                    cout << current_acc->getBalance() << " won" << endl;
                    cout << endl;
                    cout << "The remaining 1000 in ATM : " << num_of_1000 * 1000 << endl;
                    cout << "The remaining 5000 in ATM : " << num_of_5000 * 5000 << endl;
                    cout << "The remaining 10000 in ATM : " << num_of_10000 * 10000
                        << endl;
                    cout << "The remaining 50000 in ATM : " << num_of_50000 * 50000
                        << endl;
                    cout << endl;
                    ATMHistory("Withdrawal", current_acc, nullptr, sumOfWithdrawal, "None");
                    withdrawal_count++;
                }
                else { // 너 수수료 낼 돈 없어,,
                    cout << "[Error] There is not enough money in your account" << endl;
                    cout << endl;
                }
            }
        }
        else {
            cout << "[Error] If you want more withdrawal, Please restart session"
                << endl;
            StartSession();
            cout << endl;
        }
    }

    if (this->language_you_want == 1) {

        // 각 session 당 withdrawal 횟수는 3회까지
        if (withdrawal_count < 3) {
            // REQ5.1 An ATM shall ask a user to enter the amount of fund to withdra
            cout << "인출하고자 하는 금액을 적으시오." << endl;
            cout << "인출하고자 하는 지폐의 종류를 적으시오. (1,000 "
                "원, 5,000원, 10,000원, 50,000원)"
                << endl;
            cout << "[주의] 1000, 5000, 10000, 50000 단위로 적으시오.: ";
            cin >> denominationOfBills;
            if (denominationOfBills == -1) {
                DisplayOfAccountATMKo();
                cout << "인출하고자 하는 금액을 적으시오." << endl;
                cout << "인출하고자 하는 지폐의 종류를 적으시오. (1,000 "
                    "원, 5,000원, 10,000원, 50,000원)"
                    << endl;
                cout << "[주의] 1000, 5000, 10000, 50000 단위로 적으시오.: ";
                cin >> denominationOfBills;
            }
            cout << endl;

            if (denominationOfBills == 1000) {
                cout << "인출하고자 하는 천원의 갯수를 고르시오" << endl;
                cout << "[주의] 1000, 2000, 3000 단위로 적으시오. ";
                cin >> sumOfWithdrawal;
                if (sumOfWithdrawal == -1) {
                    DisplayOfAccountATMKo();
                    cout << "인출하고자 하는 천원의 갯수를 고르시오" << endl;
                    cout << "[주의] 1000, 2000, 3000 단위로 적으시오. ";
                    cin >> sumOfWithdrawal;
                }
                cout << endl;
                if (sumOfWithdrawal % 1000 == 0) {
                    withdrawal_1000 = sumOfWithdrawal / 1000;
                }
                else {
                    cout << "[오류] 인출하고자 하는 단위를 위 사항에 맞게 적으시오. "

                        << endl;
                    wrong_input = 1;
                }
            }
            else if (denominationOfBills == 5000) {
                cout << "인출하고자 하는 오천원의 갯수를 고르시오" << endl;
                cout << "[주의] 5000, 10000, 15000 단위로 적으시오. ";
                cin >> sumOfWithdrawal;
                if (sumOfWithdrawal == -1) {
                    DisplayOfAccountATMKo();
                    cout << "인출하고자 하는 오천원의 갯수를 고르시오" << endl;
                    cout << "[주의] 5000, 10000, 15000 단위로 적으시오. ";
                    cin >> sumOfWithdrawal;
                }
                cout << endl;
                if (sumOfWithdrawal % 5000 == 0) {
                    withdrawal_5000 = sumOfWithdrawal / 5000;
                }
                else {
                    cout << "[오류] 인출하고자 하는 단위를 위 사항에 맞게 적으시오."
                        << endl;
                    wrong_input = 1;
                }
            }
            else if (denominationOfBills == 10000) {
                cout << "인출하고자 하는 만원의 갯수를 고르시오" << endl;
                cout << "[주의] 10000, 20000, 30000 단위로 적으시오. ";
                cin >> sumOfWithdrawal;
                if (sumOfWithdrawal == -1) {
                    DisplayOfAccountATMKo();
                    cout << "인출하고자 하는 만원의 갯수를 고르시오" << endl;
                    cout << "[주의] 10000, 20000, 30000 단위로 적으시오. ";
                    cin >> sumOfWithdrawal;
                }
                cout << endl;
                if (sumOfWithdrawal % 10000 == 0) {
                    withdrawal_10000 = sumOfWithdrawal / 10000;
                }
                else {
                    cout << "[오류] 인출하고자 하는 단위를 위 사항에 맞게 적으시오."
                        << endl;
                    wrong_input = 1;
                }
            }
            else if (denominationOfBills == 50000) {
                cout << "인출하고자 하는 오만원의 갯수를 고르시오" << endl;
                cout << "[주의] 50000, 100000, 150000 단위로 적으시오. ";
                cin >> sumOfWithdrawal;
                if (sumOfWithdrawal == -1) {
                    DisplayOfAccountATMKo();
                    cout << "인출하고자 하는 오만원의 갯수를 고르시오" << endl;
                    cout << "[주의] 50000, 100000, 150000 단위로 적으시오. ";
                    cin >> sumOfWithdrawal;
                }
                cout << endl;
                if (sumOfWithdrawal % 50000 == 0) {
                    withdrawal_50000 = sumOfWithdrawal / 50000;
                }
                else {
                    cout << "[오류] 인출하고자 하는 단위를 위 사항에 맞게 적으시오."
                        << endl;
                    wrong_input = 1;
                }
            }
            else {
                cout << "[오류] 인출하고자 하는 지폐의 종류를 위 사항에 맞게 적으시오."
                    << endl;
                wrong_input = 1;
            }

            // REQ5.2 An ATM shall display an appropriate error message if there is
            // insufficient fund in the account or insufficient cash in the ATM.
            if (num_of_1000 < withdrawal_1000 || num_of_5000 < withdrawal_5000 ||
                num_of_10000 < withdrawal_10000 || num_of_50000 < withdrawal_50000) {
                cout << "[오류] ATM에 들어있는 돈이 부족합니다. " << endl;
                cout << endl;
            } // ATM에 돈이 충분하지 않다면 Error 출력
            else if (current_acc->getBalance() < sumOfWithdrawal) {
                cout << "[오류] 계좌에 충분한 금액이 들어있지 않습니다." << endl;
                cout << endl;
            } // REQ 5.7 The maximum amount of cash withdrawal per transaction is KRW
              // 500,000.
            else if (500000 < sumOfWithdrawal) {
                cout << "[오류] 오십만원이 넘는 인출은 불가능합니다." << endl;
                cout << endl;
            }
            else if (wrong_input == 1) {
                cout << endl;
            }

            //  REQ5.3 Once the withdrawal is successful, the transaction must be
            //  reflected to the bank account
            // as well(i.e., the same amount of fund must be deducted from the
            // corresponding bank account). REQ5.5 The cash withdrawal lower available
            // cash in the ATM that can be used by other users.
            else {
                // REQ5.4 Some withdrawal fee may be charged(See REQ in System Setup).
                if (withdrawal_fee() == true) { // 수수료 낼 수 있으면
                    // ATM에서 돈 빼기
                    num_of_1000 = num_of_1000 - withdrawal_1000;
                    num_of_5000 = num_of_5000 - withdrawal_5000;
                    num_of_10000 = num_of_10000 - withdrawal_10000;
                    num_of_50000 = num_of_50000 - withdrawal_50000;
                    // 계좌에서 돈 빼기
                    current_bank->setWithdrawal(this->current_acc, sumOfWithdrawal);
                    // 계좌/ATM에 남은 금액 출력
                    cout << "성공하였습니다." << endl;
                    cout << endl;
                    cout << current_bank->getBankName() << "의 계좌는 ";
                    cout << current_acc->getBalance() << "원이 되었습니다." << endl;
                    cout << endl;
                    cout << "ATM에 남아있는 천원의 갯수 : " << num_of_1000 << endl;
                    cout << "ATM에 남아있는 오천원의 갯수 : " << num_of_5000 << endl;
                    cout << "The remaining 10000 in ATM : " << num_of_10000 << endl;
                    cout << "The remaining 50000 in ATM : " << num_of_50000 << endl;
                    cout << endl;
                    ATMHistory("Withdrawal", current_acc, nullptr, sumOfWithdrawal, "None");
                    withdrawal_count++;
                }
                else { // 너 수수료 낼 돈 없어,,
                    cout << "[오류] 계좌에 충분한 돈이 존재하지 않습니다." << endl;
                    cout << endl;
                }
            }
        }
        else {
            cout << "[오류] 추가적으로 인출하려면 새로운 세션을 진행하세요." << endl;
            StartSession();
            cout << endl;
        }
    }
}
bool ATM::withdrawal_fee() {
    // Withdrawal fee for a primary bank: KRW 1,000; the fee is paid from the
    // withdrawal account.
    if (current_bank == serial_primary_bank) {
        if (current_acc->getBalance() < 1000) { // 수수료가 통장에 없는 경우
            return false;
        }
        else {
            current_bank->setWithdrawal(this->current_acc, 1000); // 1000원 수수료
            if (language_you_want == 2) {
                cout << "You are charged for withdrawal fee of 1000won." << endl;
            }
            else if (language_you_want == 1) {
                cout << "인출 수수료로 1000원이 청구됩니다." << endl;
            }
            return true;
        }
    }
    else {
        if (current_acc->getBalance() < 2000) {
            return false;
        }
        else {
            current_bank->setWithdrawal(this->current_acc, 2000); // 2000원 수수료
            if (language_you_want == 2) {
                cout << "You are charged for withdrawal fee of 2000won." << endl;
            }
            else if (language_you_want == 1) {
                cout << "인출 수수료로 2000원이 청구됩니다." << endl;
            }
            return true;
        }
    }
}

// 6. transfer
void ATM::transfer() {
    // destination이 보낼 account
    // source이 현재 account
    int transfer_type;  // cash or account 이체
    int total_transfer; // 이체 될 금액 (이건 수수료 포함 X)
    string destination_acc_num;
    int transfer_fee;
    Bank* destination_bank = nullptr;
    Account* destination_acc = nullptr;

    if (this->language_you_want == 2) {
        // REQ 6.1 cash transfer / account fund transfer 중 transfer type 입력받음
        cout << "What is your transfer type? ";
        cout << "1.cash transfer\t2.account transfer\t3. Exit : ";
        cin >> transfer_type;
        if (transfer_type == -1) {
            DisplayOfAccountATMEng();
            cout << "What is your transfer type? ";
            cout << "1.cash transfer\t2.account transfer\t3. Exit : ";
            cin >> transfer_type;
        }
        cout << endl;

        // REQ 6.2 이체해주려는 destination account number 입력받음
        cout << "What is destination account number? : ";
        cin >> destination_acc_num;
        if (destination_acc_num == "-1") {
            DisplayOfAccountATMEng();
            cout << "What is destination account number? : ";
            cin >> destination_acc_num;
        }
        cout << endl;

        // destination account의 account 포인터와 bank 포인터 찾음
        for (int i = 0; i < Bank_list.size();
            i++) { // bank_list가 10개라서 이케함, len 따로 구해도 됨
            destination_acc = Bank_list[i]->findAccount(destination_acc_num);
            if (destination_acc != nullptr) { // 찾으면 break
                destination_bank = Bank_list[i];
                break;
            }
        }

        // cash transfer일 때
        if (transfer_type == 1) {
            type = "Cash Transfer";
            int transfer_1000;
            int transfer_5000;
            int transfer_10000;
            int transfer_50000;
            string confirmed_fee;
            int transfer_amount;

            cout << "How much do you want to tranfer?" << endl;
            cout << "Insert the amount of 1000 won cash : ";
            cin >> transfer_1000;
            if (transfer_1000 == -1) {
                DisplayOfAccountATMEng();
                cout << "How much do you want to tranfer?" << endl;
                cout << "Insert the amount of 1000 won cash : ";
                cin >> transfer_1000;
            }
            cout << endl;
            cout << "Insert the amount of 5000 won cash : ";
            cin >> transfer_5000;
            if (transfer_5000 == -1) {
                DisplayOfAccountATMEng();
                cout << "Insert the amount of 5000 won cash : ";
                cin >> transfer_5000;
            }
            cout << endl;
            cout << "Insert the amount of 10000 won cash : ";
            cin >> transfer_10000;
            if (transfer_10000 == -1) {
                DisplayOfAccountATMEng();
                cout << "Insert the amount of 10000 won cash : ";
                cin >> transfer_10000;
            }
            cout << endl;
            cout << "Insert the amount of 50000 won cash : ";
            cin >> transfer_50000;
            if (transfer_50000 == -1) {
                DisplayOfAccountATMEng();
                cout << "Insert the amount of 50000 won cash : ";
                cin >> transfer_50000;
            }
            cout << endl;
            cout << "The fee is 5,000 won (If confirmed, please press Y) : ";
            cin >> confirmed_fee;
            if (confirmed_fee == "-1") {
                DisplayOfAccountATMEng();
                cout << "The fee is 5,000 won (If confirmed, please press Y) : ";
                cin >> confirmed_fee;
            }
            cout << endl;
            total_transfer = transfer_1000 * 1000 + transfer_5000 * 5000 +
                transfer_10000 * 10000 + transfer_50000 * 50000;
            // REQ 6.6. if cash transfer → available cash 늘어남
            // ATM에 돈 추가
            num_of_1000 += transfer_1000;
            num_of_5000 += transfer_5000 + 1; // 수수료 증가
            num_of_10000 += transfer_10000;
            num_of_50000 += transfer_50000;

            // 계좌로 돈 이체
            destination_bank->setTransfer(destination_acc, total_transfer);
        }
        // account transfer일 때
        else if (transfer_type == 2) {
            type = "Account Transfer";
            int check_source;
            cout << "Is your source account number " << current_acc->getAccountNum()
                << "?" << endl;
            cout << "1.Yes\t2.No : ";
            cin >> check_source;
            if (check_source == -1) {
                DisplayOfAccountATMEng();
                cout << "Is your source account number " << current_acc->getAccountNum()
                    << "?" << endl;
                cout << "1.Yes\t2.No : ";
                cin >> check_source;
            }
            cout << endl;
            if (check_source == 2) {
                cout << "[ERROR] Please restart" << endl;
                return;
            }
            cout << "How much do you want to tranfer?" << endl;
            cin >> total_transfer;
            if (total_transfer == -1) {
                DisplayOfAccountATMEng();
                cout << "How much do you want to tranfer?" << endl;
                cin >> total_transfer;
            }
            cout << endl;

            // REQ 6.5. 이체 수수료 부과될 수 있음 (system setup 확인)
            bool destination_primary = (destination_bank->getBankName() ==
                serial_primary_bank->getBankName())
                ? true
                : false;
            bool source_primary =
                (current_bank->getBankName() == serial_primary_bank->getBankName())
                ? true
                : false;
            // both primary banks
            if (destination_primary && source_primary) {
                transfer_fee = 2000;
            }
            // primary and non-primary banks
            else if (destination_primary || source_primary) {
                transfer_fee = 3000;
            }
            // both non-primary banks
            else {
                transfer_fee = 4000;
            }
            cout << "The transfer fee of " << transfer_fee
                << " won will be excluded from your account." << endl;

            //(REQ 6.7.) 이체 끝나면 잔액 변화 (있다면 source account,) destination
            // account에 반영됨
            // -> the same amount of fund must be deducted from the source bank
            // account,and then added to the destination bank account
            destination_bank->setTransfer(destination_acc, total_transfer);
            current_bank->setWithdrawal(current_acc, total_transfer + transfer_fee);
        }

        // Exit 선택함
        else if (transfer_type == 3) {
            return;
        }
        // Exception handling
        else {
            cout << "[ERROR] You must choose from 1 to 3." << endl;
            transfer();
            return;
        }

        cout << "Success! You have transferred " << total_transfer
            << " won to account " << destination_acc_num << endl;
        if (transfer_type ==
            2) { // account transfer일 경우에 transfer 마치고 다음 것 출력
            cout << "with account transfer." << endl;
            cout << "The remaining balance of the source account "
                << current_bank->getBankName() << " account ";
            cout << current_acc->getAccountNum() << " is "
                << current_acc->getBalance() << " won" << endl;
            cout << "The remaining balance of the destinaton account "
                << destination_bank->getBankName() << " account ";
            cout << destination_acc->getAccountNum() << " is "
                << destination_acc->getBalance() << " won" << endl;
            ATMHistory("Transfer", current_acc, destination_acc, total_transfer, type);
        }
        else if (transfer_type ==
            1) { // cash transfer일 경우에 transfer 마치고 다음 것 출력
            cout << "with cash transfer." << endl;
            cout << "The remaining 1000 in ATM : " << num_of_1000 * 1000 << endl;
            cout << "The remaining 5000 in ATM : " << num_of_5000 * 5000 << endl;
            cout << "The remaining 10000 in ATM : " << num_of_10000 * 10000 << endl;
            cout << "The remaining 50000 in ATM : " << num_of_50000 * 50000 << endl;
            cout << destination_acc->getAccountNum() << " is "
                << destination_acc->getBalance() << " won" << endl;
            cout << "The remaining balance in your "
                << destination_bank->getBankName() << " account ";
            cout << destination_acc->getAccountNum() << " is "
                << destination_acc->getBalance() << " won" << endl;
            ATMHistory("Transfer", nullptr, destination_acc, total_transfer, type);
        }
        cout << "Transfer finished." << endl;
    }

    if (this->language_you_want == 1) {
        // REQ 6.1 cash transfer / account fund transfer 중 transfer type 입력받음
        cout << "이체 종류를 고르시오. ";
        cout << "1.현금 이체\t2.계좌 이체\t3. 취소 ";
        cin >> transfer_type;
        if (transfer_type == -1) {
            DisplayOfAccountATMKo();
            cout << "1.현금 이체\t2.계좌 이체\t3. 취소 ";
            cin >> transfer_type;
        }
        cout << endl;

        // REQ 6.2 이체해주려는 destination account number 입력받음
        cout << "돈을 보내려는 계좌를 고르시오. ";
        cin >> destination_acc_num;
        if (destination_acc_num == "-1") {
            DisplayOfAccountATMKo();
            cout << "돈을 보내려는 계좌를 고르시오. ";
            cin >> destination_acc_num;
        }
        cout << endl;

        for (int i = 0; i < Bank_list.size();
            i++) { // bank_list가 10개라서 이케함, len 따로 구해도 됨
            destination_acc = Bank_list[i]->findAccount(destination_acc_num);
            if (destination_acc != nullptr) { // 찾으면 break
                destination_bank = Bank_list[i];
                break;
            }
        }

        // cash transfer 선택함
        // REQ 6.3. if cash transfer → 수수료 포함한 현금 입력받고 이체 금액
        // 맞는지 확인 후 수수료 제외 나머지 금액 이체 Cash transfer fee to any
        // bank type: KRW 5,000; the fee is paid by inserting additional cash.
        if (transfer_type == 1) {
            type = "Cash Transfer";
            int transfer_1000;
            int transfer_5000;
            int transfer_10000;
            int transfer_50000;
            string confirmed_fee;
            int transfer_amount;

            cout << "이체하고자 하는 금액을 적으시오." << endl;
            cout << "1,000원 지폐 개수 : ";
            cin >> transfer_1000;
            if (transfer_1000 == -1) {
                DisplayOfAccountATMKo();
                cout << "이체하고자 하는 금액을 적으시오." << endl;
                cout << "1,000원 지폐 개수 : ";
                cin >> transfer_1000;
            }
            cout << endl;
            cout << "5,000원 지폐 개수 : ";
            cin >> transfer_5000;
            if (transfer_5000 == -1) {
                DisplayOfAccountATMKo();
                cout << "5,000원 지폐 개수 : ";
                cin >> transfer_5000;
            }
            cout << endl;
            cout << "10,000원 지폐 개수 : ";
            cin >> transfer_10000;
            if (transfer_10000 == -1) {
                DisplayOfAccountATMKo();
                cout << "10,000원 지폐 개수 : ";
                cin >> transfer_10000;
            }
            cout << endl;
            cout << "50,000원 지폐 개수 : ";
            cin >> transfer_50000;
            if (transfer_50000 == -1) {
                DisplayOfAccountATMKo();
                cout << "50,000원 지폐 개수 : ";
                cin >> transfer_50000;
            }
            cout << endl;
            cout << "수수료는 5천원입니다. (동의할 경우, Y를 입력해주세요.) : ";
            cin >> confirmed_fee;
            if (confirmed_fee == "-1") {
                DisplayOfAccountATMKo();
                cout << "수수료는 5천원입니다. (동의할 경우, Y를 입력해주세요.) : ";
                cin >> confirmed_fee;
            }
            cout << endl;
            total_transfer = transfer_1000 * 1000 + transfer_5000 * 5000 +
                transfer_10000 * 10000 + transfer_50000 * 50000;
            // REQ 6.6. if cash transfer → available cash 늘어남
            // ATM에 돈 추가
            num_of_1000 += transfer_1000;
            num_of_5000 += transfer_5000 + 1; // 수수료 증가
            num_of_10000 += transfer_10000;
            num_of_50000 += transfer_50000;

            // 계좌로 돈 이체
            destination_bank->setTransfer(destination_acc, total_transfer);
        }
        else if (transfer_type == 2) { // account transfer
            /*    source account number 새로 입력받는가 or 현재 카드 사용할것인지
            물어보고 넘어가는가????????? double source_acc_num; Account* source_acc;
            Bank* source_bank;
            */
            type = "Account Transfer";
            int check_source;
            cout << "돈을 빼려는 계좌가 " << current_acc->getAccountNum()
                << "가 맞습니까?" << endl;
            cout << "숫자를 입력해주세요 -> 1.네\t2.아니요 : ";
            cin >> check_source;
            if (check_source == -1) {
                DisplayOfAccountATMKo();
                cout << "돈을 빼려는 계좌가 " << current_acc->getAccountNum()
                    << "가 맞습니까?" << endl;
                cout << "숫자를 입력해주세요 -> 1.네\t2.아니요 : ";
                cin >> check_source;
            }
            cout << endl;
            if (check_source == 2) {
                cout << "오류 : 다시 시작하세요 " << endl;
                return;
            }
            cout << "이체를 위한 금액을 적어주세요." << endl;
            cin >> total_transfer;
            if (total_transfer == -1) {
                DisplayOfAccountATMKo();
                cout << "이체를 위한 금액을 적어주세요." << endl;
                cin >> total_transfer;
            }
            cout << endl;

            // REQ 6.5. 이체 수수료 부과될 수 있음 (system setup 확인)
            bool destination_primary = (destination_bank->getBankName() ==
                serial_primary_bank->getBankName())
                ? true
                : false;
            bool source_primary =
                (current_bank->getBankName() == serial_primary_bank->getBankName())
                ? true
                : false;
            if (destination_primary && source_primary) { // both primary banks
                transfer_fee = 2000;
            }
            else if (destination_primary ||
                source_primary) { // primary and non-primary banks
                transfer_fee = 3000;
            }
            else { // both non-primary banks
                transfer_fee = 4000;
            }
            cout << "수수료 " << transfer_fee << " 원이 해당 계좌에서 인출됩니다."
                << endl;

            //(REQ 6.7.) 이체 끝나면 잔액 변화 (있다면 source account,) destination
            // account에 반영됨
            // -> the same amount of fund must be deducted from the source bank
            // account,and then added to the destination bank account
            destination_bank->setTransfer(destination_acc, total_transfer);
            current_bank->setWithdrawal(current_acc, total_transfer + transfer_fee);
        }

        // Exit 선택함
        else if (transfer_type == 3) {
            return;
        }
        // Exception handling
        else {
            cout << "[오류] 1~3 중에서 선택해야합니다." << endl;
            transfer();
            return;
        }

        cout << "성공적으로 " << total_transfer
            << "원을 다음 도착 계좌로 이체하였습니다 -> " << destination_acc_num;
        cout << endl;
        if (transfer_type ==
            2) { // account transfer일 경우에 transfer 마치고 다음 것 출력
            cout << "계좌 이체 결과로," << endl;
            cout << "출발 계좌에 존재하는 금액은 " << current_bank->getBankName()
                << " 계좌, ";
            cout << current_acc->getAccountNum() << " 에서 "
                << current_acc->getBalance() << "원입니다." << endl;
            cout << "도착 계좌에 존재하는 금액은 " << destination_bank->getBankName()
                << " 계좌, ";
            cout << destination_acc->getAccountNum() << " 에서 "
                << destination_acc->getBalance() << "원입니다." << endl;
            ATMHistory("Transfer", current_acc, destination_acc, total_transfer, type);
        }
        else if (transfer_type ==
            1) { // cash transfer일 경우에 transfer 마치고 다음 것 출력
            cout << "현금 이체 결과로," << endl;
            cout << "ATM에 존재하는 천원의 갯수 : " << num_of_1000 * 1000 << endl;
            cout << "ATM에 존재하는 오천원의 갯수 : " << num_of_5000 * 5000 << endl;
            cout << "ATM에 존재하는 만원의 갯수 : " << num_of_10000 * 10000 << endl;
            cout << "ATM에 존재하는 오만원의 갯수 : " << num_of_50000 * 50000 << endl;
            cout << destination_acc->getAccountNum() << " 계좌로 "
                << destination_acc->getBalance() << "원이 입금되고," << endl;
            cout << destination_bank->getBankName() << " 계좌, ";
            cout << destination_acc->getAccountNum() << " 에는  "
                << destination_acc->getBalance() << " 원이 존재합니다." << endl;
            ATMHistory("Transfer", nullptr, destination_acc, total_transfer, type);
        }
        cout << "이체 성공." << endl;
    }
}

// 7. Display of Transaction History (Admin Menu)
/*
o An administrator of an ATM may request an ATM to display the whole
transaction histories of all users from the beginning of the system start. o
(REQ7.1) When a session is started by an admin by inserting an admin card
(See REQ in System Setup), an ATM displays a menu of “Transaction History”
only. o (REQ7.2) When the “Transaction History” menu is selected, an ATM
display the information of all transactions from all users from the
beginning of the system start.
- Transaction ID, Card Number, Transaction Types, Amount, other
transaction-specific information
- Each transaction may have different types of information, so they need to
be appropriately displayed (e.g., a deposit transaction does not have the
source account information in a transfer transaction). o (REQ7.3) The
“Transaction History” information shall be outputted to the external file
(e.g., txt
*/
void ATM::PrintHistory(int session) {
    if (this->language_you_want == 2) {
        string printhistory;
        for (int i = session; i < trans; i++) {
            if (SessionSummary[i][1] == "Deposit" and SessionSummary[i][8] == "Cash Deposit") {
                printhistory = "Transaction ID: " + SessionSummary[i][0] +
                    "| Cash Deposit | Bank: " + SessionSummary[i][2] +
                    "| Account Number: " + SessionSummary[i][3] +
                    " / Deposit Amount: " + SessionSummary[i][6];

            }

            else if (SessionSummary[i][1] == "Deposit" and SessionSummary[i][8] == "Check Deposit") {
                printhistory = "Transaction ID: " + SessionSummary[i][0] +
                    "| Check Deposit | Bank: " + SessionSummary[i][2] +
                    "| Account Number: " + SessionSummary[i][3] +
                    " / Deposit Amount: " + SessionSummary[i][6];
            }
            else if (SessionSummary[i][1] == "Withdrawal") {

                printhistory = "Transaction ID: " + SessionSummary[i][0] +
                    "| Withdrawal | Bank: " + SessionSummary[i][2] +
                    "| Account Number: " + SessionSummary[i][3] +
                    " / Withdrawal Amount: " + SessionSummary[i][6];
            }

            else if (SessionSummary[i][1] == "Transfer" and SessionSummary[i][8] == "Cash Transfer") {
                printhistory = "Transaction ID: " + SessionSummary[i][0] +
                    "| Cash Transfer | Bank: " + SessionSummary[i][2] + " -> " +
                    SessionSummary[i][4] +
                    "| Account Number: " + SessionSummary[i][3] +
                    "| Destination Account Number: " + SessionSummary[i][5] +
                    " / Transfer Amount: " + SessionSummary[i][6];
            }
            else if (SessionSummary[i][1] == "Transfer" and SessionSummary[i][8] == "Account Transfer") {
                printhistory = "Transaction ID: " + SessionSummary[i][0] +
                    "| Account Transfer | Bank: " + SessionSummary[i][2] + " -> " +
                    SessionSummary[i][4] +
                    "| Account Number: " + SessionSummary[i][3] +
                    "| Destination Account Number: " + SessionSummary[i][5] +
                    " / Transfer Amount: " + SessionSummary[i][6];
            }
            cout << printhistory << endl;
        }
    }
    if (this->language_you_want == 1) {
        string printhistory;
        for (int i = session; i < trans; i++) {
            if (SessionSummary[i][1] == "Deposit" and SessionSummary[i][8] == "Cash Deposit") {
                printhistory = "거래 고유 번호: " + SessionSummary[i][0] +
                    "| 현금 예금 | 은행: " + SessionSummary[i][2] +
                    "| 계좌번호 : " + SessionSummary[i][3] +
                    " / 예금 금액: " + SessionSummary[i][6];
            }
            if (SessionSummary[i][1] == "Deposit" and SessionSummary[i][8] == "Check Deposit") {
                printhistory = "거래 고유 번호: " + SessionSummary[i][0] +
                    "| 수표 예금 | 은행: " + SessionSummary[i][2] +
                    "| 계좌번호 : " + SessionSummary[i][3] +
                    " / 예금 금액: " + SessionSummary[i][6];
            }
            else if (SessionSummary[i][1] == "Withdrawal") {

                printhistory = "거래 고유 번호: " + SessionSummary[i][0] +
                    "| 인출 | 은행: " + SessionSummary[i][2] +
                    "| 계좌번호 : " + SessionSummary[i][3] +
                    " / 인출 금액: " + SessionSummary[i][6];
            }

            else if (SessionSummary[i][1] == "Transfer" and SessionSummary[i][8] == "Cash Transfer") {
                printhistory = "거래 고유 번호: " + SessionSummary[i][0] +
                    "| 현금 이체 | 은행: " + SessionSummary[i][2] + " -> " +
                    SessionSummary[i][4] +
                    "| 계좌번호 : " + SessionSummary[i][3] +
                    "| 도착 계좌번호 : " + SessionSummary[i][5] +
                    " / 이체 금액: " + SessionSummary[i][6];
            }
            else if (SessionSummary[i][1] == "Transfer" and SessionSummary[i][8] == "Account Transfer") {
                printhistory = "거래 고유 번호: " + SessionSummary[i][0] +
                    "| 계좌 이체 | 은행: " + SessionSummary[i][2] + " -> " +
                    SessionSummary[i][4] +
                    "| 계좌번호 : " + SessionSummary[i][3] +
                    "| 도착 계좌번호 : " + SessionSummary[i][5] +
                    " / 이체 금액: " + SessionSummary[i][6];
            }
            cout << printhistory << endl;
        }
    }
}

void ATM::admin() {
    if (this->language_you_want == 2) {
        int select;
        string printhistory;
        cout << "Please select your transaction" << endl;
        cout << "1. Transaction History: " << endl;
        cin >> select;
        if (select == 1) {
            ofstream letsgo;
            letsgo.open("ATM_Transaction.txt", ios::trunc);
            for (int i = 0; i < trans; i++) {
                if (SessionSummary[i][1] == "Deposit" and SessionSummary[i][8] == "Cash Deposit") {
                    printhistory = "Transaction ID: " + SessionSummary[i][0] +
                        "| Cash Deposit | Bank: " + SessionSummary[i][2] +
                        "| Account Number: " + SessionSummary[i][3] +
                        " / Deposit Amount: " + SessionSummary[i][6];

                }

                else if (SessionSummary[i][1] == "Deposit" and SessionSummary[i][8] == "Check Deposit") {
                    printhistory = "Transaction ID: " + SessionSummary[i][0] +
                        "| Check Deposit | Bank: " + SessionSummary[i][2] +
                        "| Account Number: " + SessionSummary[i][3] +
                        " / Deposit Amount: " + SessionSummary[i][6];
                }
                else if (SessionSummary[i][1] == "Withdrawal") {

                    printhistory = "Transaction ID: " + SessionSummary[i][0] +
                        "| Withdrawal | Bank: " + SessionSummary[i][2] +
                        "| Account Number: " + SessionSummary[i][3] +
                        " / Withdrawal Amount: " + SessionSummary[i][6];
                }

                else if (SessionSummary[i][1] == "Transfer" and SessionSummary[i][8] == "Cash Transfer") {
                    printhistory = "Transaction ID: " + SessionSummary[i][0] +
                        "| Cash Transfer | Bank: " + SessionSummary[i][2] + " -> " +
                        SessionSummary[i][4] +
                        "| Account Number: " + SessionSummary[i][3] +
                        "| Destination Account Number: " + SessionSummary[i][5] +
                        " / Transfer Amount: " + SessionSummary[i][6];
                }
                else if (SessionSummary[i][1] == "Transfer" and SessionSummary[i][8] == "Account Transfer") {
                    printhistory = "Transaction ID: " + SessionSummary[i][0] +
                        "| Account Transfer | Bank: " + SessionSummary[i][2] + " -> " +
                        SessionSummary[i][4] +
                        "| Account Number: " + SessionSummary[i][3] +
                        "| Destination Account Number: " + SessionSummary[i][5] +
                        " / Transfer Amount: " + SessionSummary[i][6];
                }
                cout << printhistory << endl;
                letsgo << printhistory << endl;
            }
            letsgo << endl;
            letsgo.close();
        }
        return;
    }

    if (this->language_you_want == 1) {
        int select;
        string printhistory;
        cout << "거래를 선택해주세요." << endl;
        cout << "1. 거래 내역: " << endl;
        cin >> select;
        if (select == 1) {
            ofstream letsgo;
            letsgo.open("ATM_Transaction.txt", ios::trunc);
            for (int i = 0; i < trans; i++) {
                if (SessionSummary[i][1] == "Deposit" and SessionSummary[i][8] == "Cash Deposit") {
                    printhistory = "거래 고유 번호: " + SessionSummary[i][0] +
                        "| 현금 예금 | 은행: " + SessionSummary[i][2] +
                        "| 계좌번호 : " + SessionSummary[i][3] +
                        " / 예금 금액: " + SessionSummary[i][6];
                }
                if (SessionSummary[i][1] == "Deposit" and SessionSummary[i][8] == "Check Deposit") {
                    printhistory = "거래 고유 번호: " + SessionSummary[i][0] +
                        "| 수표 예금 | 은행: " + SessionSummary[i][2] +
                        "| 계좌번호 : " + SessionSummary[i][3] +
                        " / 예금 금액: " + SessionSummary[i][6];
                }
                else if (SessionSummary[i][1] == "Withdrawal") {

                    printhistory = "거래 고유 번호: " + SessionSummary[i][0] +
                        "| 인출 | 은행: " + SessionSummary[i][2] +
                        "| 계좌번호 : " + SessionSummary[i][3] +
                        " / 인출 금액: " + SessionSummary[i][6];
                }

                else if (SessionSummary[i][1] == "Transfer" and SessionSummary[i][8] == "Cash Transfer") {
                    printhistory = "거래 고유 번호: " + SessionSummary[i][0] +
                        "| 현금 이체 | 은행: " + SessionSummary[i][2] + " -> " +
                        SessionSummary[i][4] +
                        "| 계좌번호 : " + SessionSummary[i][3] +
                        "| 도착 계좌번호 : " + SessionSummary[i][5] +
                        " / 이체 금액: " + SessionSummary[i][6];
                }
                else if (SessionSummary[i][1] == "Transfer" and SessionSummary[i][8] == "Account Transfer") {
                    printhistory = "거래 고유 번호: " + SessionSummary[i][0] +
                        "| 계좌 이체 | 은행: " + SessionSummary[i][2] + " -> " +
                        SessionSummary[i][4] +
                        "| 계좌번호 : " + SessionSummary[i][3] +
                        "| 도착 계좌번호 : " + SessionSummary[i][5] +
                        " / 이체 금액: " + SessionSummary[i][6];
                }
                cout << printhistory << endl;
                letsgo << printhistory << endl;
            }

            letsgo << endl;
            letsgo.close();
        }
    }
}

void ATM::DisplayOfAccountATMEng() {
    cout << endl;
    cout << "==== all ATM, accounts info ====" << endl;
    cout << endl;
    for (int i = 0; i < Bank::ATM_list.size(); i++) {
        cout << "ATM [SN: " << Bank::ATM_list[i]->serial_num;
        int remaining_cash = Bank::ATM_list[i]->num_of_1000 * 1000 +
            Bank::ATM_list[i]->num_of_5000 * 5000 +
            Bank::ATM_list[i]->num_of_10000 * 10000 +
            Bank::ATM_list[i]->num_of_50000 * 50000;
        cout << "] remaining cash: " << remaining_cash << endl;
        cout << endl;
    }
    for (int k = 0; k < Bank::Bank_list.size(); k++) {
        for (int j = 0; j < Bank_list[k]->Account_list.size(); j++) {
            cout << "Account [Bank: " << Bank::Bank_list[k]->getBankName()
                << ", No: ";
            cout << Bank::Bank_list[k]->Account_list[j]->getAccountNum()
                << ", Owner: ";
            cout << Bank::Bank_list[k]->Account_list[j]->getName();
            cout << "] balance: " << Bank::Bank_list[k]->Account_list[j]->getBalance()
                << endl;
            cout << endl;
        }
    }
    cout << "================================" << endl;
    cout << endl;
}

void ATM::DisplayOfAccountATMKo() {
    cout << endl;
    cout << "==== 모든 ATM, accounts의 정보 ====" << endl;
    cout << endl;
    for (int i = 0; i < Bank::ATM_list.size(); i++) {
        cout << "ATM [고유 ATM 번호: " << Bank::ATM_list[i]->serial_num;
        int remaining_cash = Bank::ATM_list[i]->num_of_1000 * 1000 +
            Bank::ATM_list[i]->num_of_5000 * 5000 +
            Bank::ATM_list[i]->num_of_10000 * 10000 +
            Bank::ATM_list[i]->num_of_50000 * 50000;
        cout << "] ATM에 남아있는 잔액: " << remaining_cash << endl;
        cout << endl;
    }
    for (int k = 0; k < Bank::Bank_list.size(); k++) {
        for (int j = 0; j < Bank_list[k]->Account_list.size(); j++) {
            cout << "계좌 [은행: " << Bank::Bank_list[k]->getBankName()
                << ", 계좌 번호: ";
            cout << Bank::Bank_list[k]->Account_list[j]->getAccountNum()
                << ", Owner: ";
            cout << Bank::Bank_list[k]->Account_list[j]->getName();
            cout << "] 계좌에 잔액: "
                << Bank::Bank_list[k]->Account_list[j]->getBalance() << endl;
            cout << endl;
        }
    }
    cout << "================================" << endl;
    cout << endl;
}

//---------main---------
Bank* findBank(string bank_name, vector<Bank*> Bank_list) {
    Bank* bank_ptr = nullptr;
    for (int i = 0; i < Bank_list.size(); i++) {
        if (bank_name == Bank_list[i]->getBankName()) {
            bank_ptr = Bank_list[i];
            break;
        }
    }
    return bank_ptr;
}

ATM* findATM(int ATM_id, vector<ATM*> ATM_list) {
    ATM* ATM_ptr = nullptr;
    for (int i = 0; i < ATM_list.size(); i++) {
        if (ATM_id == ATM_list[i]->serial_num) {
            ATM_ptr = ATM_list[i];
            break;
        }
    }
    return ATM_ptr;
}

void Bank_Creation() {
    string bank_name;
    cout << "Insert the bank name: ";
    cin >> bank_name;
    Bank* new_bank = new Bank(bank_name);
}

void Account_Creation() {
    string account_owner;
    string account_bank_name;
    Bank* account_bank_ptr;
    string account_id;
    int account_pw;
    int account_balance;
    cout << "Insert the account's bank name : ";
    cin >> account_bank_name;
    account_bank_ptr = findBank(
        account_bank_name, Bank::Bank_list); // account_bank_ptr 찾아야함 !!!!
    while (account_bank_ptr == nullptr) {
        cout << "Bank " << account_bank_name << " does not exist." << endl;
        cout << "Insert the account's bank name again : ";
        cin >> account_bank_name;
        account_bank_ptr = findBank(account_bank_name, Bank::Bank_list);
    }
    cout << "Insert the account's owner name : ";
    cin >> account_owner;
    cout << "Insert the account number : ";
    cin >> account_id;
    cout << "Insert the account password : ";
    cin >> account_pw;
    cout << "Insert the account balance : ";
    cin >> account_balance;
    Account* new_account = new Account(account_bank_name, account_owner,
        account_id, account_pw, account_balance);
    account_bank_ptr->setAccount(new_account);
    cout << endl;
    cout << "Bank " << account_bank_name
        << "'s account list is updated as following." << endl;
    account_bank_ptr->printAllAccounts();
}

void ATM_Creation() {
    string ATM_bank_name;
    Bank* ATM_bank_ptr;
    int ATM_id;
    int ATM_type;
    int ATM_lang;
    int ATM_1000;
    int ATM_5000;
    int ATM_10000;
    int ATM_50000;
    cout << "Insert the ATM's primary bank name : ";
    cin >> ATM_bank_name;
    ATM_bank_ptr =
        findBank(ATM_bank_name, Bank::Bank_list); // ATM_bank_ptr 찾아야함 !!!!
    while (ATM_bank_ptr == nullptr) {
        cout << "Bank " << ATM_bank_name << " does not exist." << endl;
        cout << "Insert the ATM's primary bank name again : ";
        cin >> ATM_bank_name;
        ATM_bank_ptr = findBank(ATM_bank_name, Bank::Bank_list);
    }
    cout << "Insert the ATM's serial number : ";
    cin >> ATM_id;
    cout << "Insert the ATM type : " << endl;
    cout << "1.Single Bank\t2.Multi-Bank" << endl;
    cin >> ATM_type;
    cout << "Insert the ATM language type : " << endl;
    cout << "1.Unillingual\t2.Bilingual" << endl;
    cin >> ATM_lang;
    cout << "Insert the number of 1000 won cash in the ATM : ";
    cin >> ATM_1000;
    cout << "Insert the number of 5000 won cash in the ATM : ";
    cin >> ATM_5000;
    cout << "Insert the number of 10000 won cash in the ATM : ";
    cin >> ATM_10000;
    cout << "Insert the number of 50000 won cash in the ATM : ";
    cin >> ATM_50000;
    ATM* new_ATM = new ATM(ATM_bank_ptr, ATM_id, ATM_type, ATM_lang, ATM_1000,
        ATM_5000, ATM_10000, ATM_50000, Bank::Bank_list);
}

int main() {
    // Bank들 생성함
    int create_bank = 1;
    cout << "--Start making Banks--" << endl;
    while (create_bank == 1) {
        Bank_Creation();
        cout << endl;
        cout << "Do you want to create another bank?" << endl;
        cout << "1.Yes\t2.No" << endl;
        cin >> create_bank;
    }
    cout << "--Finish making Banks--" << endl << endl;

    // Account들 생성함
    int create_account = 1;
    cout << "--Start making Accounts--" << endl;
    while (create_account == 1) {
        Account_Creation();
        cout << endl;
        cout << "Do you want to create another account?" << endl;
        cout << "1.Yes\t2.No" << endl;
        cin >> create_account;
    }
    cout << "--Finish making Accounts--" << endl << endl;

    // ATM들 생성함
    int create_ATM = 1;
    cout << "--Start making ATMs--" << endl;
    while (create_ATM == 1) {
        ATM_Creation();
        cout << endl;
        cout << "Do you want to create another ATM?" << endl;
        cout << "1.Yes\t2.No" << endl;
        cin >> create_ATM;
    }
    cout << "--Finish making ATMs--" << endl << endl;

    vector<Bank*> allBanks = Bank::Bank_list;
    vector<ATM*> allATMs = Bank::ATM_list;

    // Bank 생성 결과 확인함
    cout << "num of banks: " << allBanks.size() << endl;
    for (int i = 0; i < allBanks.size(); i++) {
        cout << "Bank #" << (i + 1) << " : " << allBanks[i]->getBankName() << endl;
    }
    cout << endl;

    // ATM 생성 결과 확인함
    cout << "num of ATMs: " << allATMs.size() << endl;
    for (int i = 0; i < allATMs.size(); i++) {
        cout << "ATM #" << (i + 1) << " : " << allATMs[i]->serial_num << endl;
    }
    cout << endl;

    // Use ATMs
    int start_session;
    cout << "Would you like to use a ATM machine?" << endl;
    cout << "1.Yes\t2.No" << endl;
    cin >> start_session;
    while (start_session == 1) {
        int ATM_id;
        ATM* ATM_ptr;
        cout << "Insert the serial number of the ATM you want to use : ";
        cin >> ATM_id;
        ATM_ptr = findATM(ATM_id, allATMs); // ATMs에서 A
        ATM_ptr->setLanguage();
        cout << "Your session will be started." << endl;
        ATM_ptr->StartSession();
        cout << "Would you like to use a ATM machine?" << endl;
        cout << "1.Yes\t2.No" << endl;
        cin >> start_session;
    }
    return 0;
}