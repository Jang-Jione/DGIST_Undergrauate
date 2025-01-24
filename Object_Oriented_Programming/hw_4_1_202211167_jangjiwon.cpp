#include <iostream>
#include<vector>

using namespace std;

class IArticlePub;
class IArticleSub;

class IArticlePub {
private:
	string pub_name;
	int pub_id;
	string recent_contents;
	static int static_pub_counter;
protected:
	vector<IArticleSub*> sub_list;
public:
	IArticlePub(const string name);
	IArticlePub(const string name, const string con);
	~IArticlePub();
	void NotifyAttach(IArticleSub* subscriber);
	void NotifyAttachResponse(IArticleSub* subscriber);
	void NotifyDetach(IArticleSub* subscriber);
	void NotifyDetachResponse(IArticleSub* subscriber);
	void updatePubContents(string c);
	void Notify();
	int getPubID() { return pub_id; }
	string getPubName() { return pub_name; }
	int getSubSize();
	void PrintAllSub();
	virtual void Event() = 0; // pure function
	void operator<<(string content);
};

class IArticleSub {
private:
	string sub_name;
	int sub_id;
	string recent_article_contents;
	IArticlePub* recent_article_pub;
	static int static_sub_counter;
protected:
	vector<IArticlePub*> pub_list;
public:
	IArticleSub(string name);
	IArticleSub(string name, IArticlePub* articlePub);
	~IArticleSub();
	void Attach(IArticlePub* publisher);
	void AttachResponse(IArticlePub* publisher);
	void Detach(IArticlePub* p_pub);
	void DetachResponse(IArticlePub* p_pub);
	void Update(IArticlePub* publisher, const string contents);
	void PrintContents();
	string getSubName() { return sub_name; }
	int getSubID() { return sub_id; }
	int getPubSize();
	void PrintAllPub();
	void operator>>(IArticlePub* publisher);
};

// IArticlePub's implements --------------------------------------------------------------------------------------------------
int IArticlePub::static_pub_counter = 1;
IArticlePub::IArticlePub(const string name) {
	pub_name = name;
	pub_id = static_pub_counter++;
	cout << "[Constructor]New Pub Created: " << "(" << pub_name << "," << pub_id << ")" << endl;
}
IArticlePub::IArticlePub(const string name, const string con) {
	pub_name = name;
	pub_id = static_pub_counter++;
	recent_contents = con;
	cout << "[Constructor]New Pub Created: " << "(" << pub_name << "," << pub_id << ")" << endl;
}
IArticlePub::~IArticlePub() {
	cout << "IArticlePub Destructor called" << endl;
	for (int i = 0; i < sub_list.size(); i++) {
		sub_list[i]->DetachResponse(this);
	}
}
void IArticlePub::NotifyAttach(IArticleSub* subscriber) {
	int N = 0;
	for (int i = 0; i < sub_list.size(); i++) {
		if (sub_list[i] == subscriber) {
			cout << "Error Message" << endl;
;			N = 1;
			break;
		}
	}
	if (N == 0) {
		sub_list.push_back(subscriber);
		subscriber->AttachResponse(this);
		cout << "[Attach] Pub " << "(" << pub_name << "," << pub_id << ")";
		cout << " is attached to Sub " << "(" << subscriber->getSubName() << "," << subscriber->getSubID() << ")" << endl;
	}
}
void IArticlePub::NotifyAttachResponse(IArticleSub* subscriber) {
	int N = 0;
	for (int i = 0; i < sub_list.size(); i++) {
		if (sub_list[i] == subscriber) {
			cout << "Error Message" << endl;
			N = 1;
			break;
		}
	}
	if (N == 0) {
		sub_list.push_back(subscriber);
		cout << "[Attach] Pub " << "(" << pub_name << "," << pub_id << ")";
		cout << " is attached to Sub " << "(" << subscriber->getSubName() << "," << subscriber->getSubID() << ")" << endl;
	}
}
void IArticlePub::NotifyDetach(IArticleSub* subscriber) {
	int N = 0;
	for (int i = 0; i < sub_list.size(); i++) {
		if (sub_list[i] == subscriber) {
			sub_list.erase(sub_list.begin()+i);
			cout << "[Pub] (" << pub_name << "," << pub_id << ") detach [Sub] (" << subscriber->getSubName();
			cout << "," << subscriber->getSubID() << ")" << endl;
			subscriber->DetachResponse(this);
			N = 1;
			break;
		}
	}
	if (N == 0) {
		cout << "Error Message" << endl;
	}
}
void IArticlePub::NotifyDetachResponse(IArticleSub* subscriber) {
	int N = 0;
	for (int i = 0; i < sub_list.size(); i++) {
		if (sub_list[i] == subscriber) {
			sub_list.erase(sub_list.begin() + i);
			cout << "[Pub] (" << pub_name << "," << pub_id << ") detach [Sub] (" << subscriber->getSubName();
			cout << "," << subscriber->getSubID() << ")" << endl;
			N = 1;
			break;
		}
	}
	if (N == 0) {
		cout << "Error Message" << endl;
	}
}
void IArticlePub::updatePubContents(string c) {
	recent_contents = c;
	Notify();
}
void IArticlePub::Notify() {
	for (int i = 0; i < sub_list.size(); i++) {
		sub_list[i]->Update(this, recent_contents);
		cout << endl;
	}
	cout << endl;
}
int IArticlePub::getSubSize() {
	int size = sub_list.size();
	return size;
}
void IArticlePub::PrintAllSub() {
	for (int i = 0; i < sub_list.size(); i++) {
		if (sub_list[i] != nullptr) {
		cout << "[" << sub_list[i]->getSubName() << "," << sub_list[i]->getSubID() << "]";
		}
	}
	cout << endl;
}
void IArticlePub::operator<<(string content) {
	for (int i = 0; i < sub_list.size(); i++) {
		sub_list[i]->Update(this, content);
	}
}

// IArticleSub's implements --------------------------------------------------------------------------------------------------

int IArticleSub::static_sub_counter = 1;
IArticleSub::IArticleSub(const string name) {
	sub_name = name;
	sub_id = static_sub_counter++;
	cout << "[Constructor]New Sub Created: " << "(" << sub_name << "," << sub_id << ")" << endl;
}
IArticleSub::IArticleSub(string name, IArticlePub* articlePub) {
	sub_name = name;
	sub_id = static_sub_counter++;
	cout << "[Constructor]New Sub Created: " << "(" << sub_name << "," << sub_id << ")" << endl;
	Attach(articlePub);
}
IArticleSub::~IArticleSub() {
	cout << "IArticleSub Destructor called" << endl;
	for (int i = 0; i < pub_list.size(); i++){
		pub_list[i]->NotifyDetachResponse(this);
	}
}
void IArticleSub::Attach(IArticlePub* publisher) {
	int N = 0;
	for (int i = 0; i < pub_list.size(); i++) {
		if (pub_list[i] == publisher) {
			cout << "Error Message" << endl;
			N = 1;
			break;
		}
	}
	if (N == 0) {
		pub_list.push_back(publisher);
		publisher->NotifyAttachResponse(this);
		cout << "[Attach] Sub " << "(" << sub_name << "," << sub_id << ")";
		cout << " is attached to Pub " << "(" << publisher->getPubName() << "," << publisher->getPubID() << ")" << endl;
	}
}
void IArticleSub::AttachResponse(IArticlePub* publisher) {
	int N = 0;
	for (int i = 0; i < pub_list.size(); i++) {
		if (pub_list[i] == publisher) {
			cout << "Error Message" << endl;
			N = 1;
			break;
		}
	}
	if (N == 0) {
		pub_list.push_back(publisher);
		cout << "[Attach] Sub " << "(" << sub_name << "," << sub_id << ")";
		cout << " is attached to Pub " << "(" << publisher->getPubName() << "," << publisher->getPubID() << ")" << endl;
	}
}
void IArticleSub::Detach(IArticlePub* p_pub) {
	int N = 0;
	for (int i = 0; i < pub_list.size(); i++) {
		if (pub_list[i] == p_pub) {
			pub_list.erase(pub_list.begin() + i);
			cout << "[Sub] (" << sub_name << "," << sub_id << ") unsubscribes [Pub] (" << p_pub->getPubName();
			cout << "," << p_pub->getPubID() << ")" << endl;
			p_pub->NotifyDetachResponse(this);
			N = 1;
			break;
		}
	}
	if (N == 0) {
		cout << "Error Message" << endl;
	}
}
void IArticleSub::DetachResponse(IArticlePub* p_pub) {
	int N = 0;
	for (int i = 0; i < pub_list.size(); i++) {
		if (pub_list[i] == p_pub) {
			pub_list.erase(pub_list.begin() + i);
			cout << "[Sub] (" << sub_name << "," << sub_id << ") unsubscribes [Pub] (" << p_pub->getPubName();
			cout << "," << p_pub->getPubID() << ")" << endl;
			N = 1;
			break;
		}
	}
	if (N == 0) {
		cout << "Error Message" << endl;
	}
}
void IArticleSub::Update(IArticlePub* publisher, const string contents) {
	recent_article_pub = publisher;
	recent_article_contents = contents;
	PrintContents();
}
void IArticleSub::PrintContents() {
	cout << "Sub" << " (" << sub_name << "," << sub_id << ")";
	cout << "'s lastest subscribed news is " << '"' << recent_article_contents;
	cout << '"' << " by " << recent_article_pub->getPubName() << endl;
}
int IArticleSub::getPubSize() {
	int size = pub_list.size();
	return size;
}
void IArticleSub::PrintAllPub() {
	for (int i = 0; i < pub_list.size(); i++) {
		cout << "[" << pub_list[i]->getPubName() << "," << pub_list[i]->getPubID() << "]";
	}
	cout << endl;
}
void IArticleSub::operator>>(IArticlePub* publisher) {
	this->Attach(publisher);
}

// new class --------------------------------------------------------------------------------------------------
// dgist_press's --------------------------------------------------------------------------------------------------

class dgist_press : public IArticlePub {
public:
	dgist_press();
	dgist_press(const string con);
	void Event();
	void CheerUp();
	~dgist_press();
};

dgist_press::dgist_press() :IArticlePub("DGIST") {
	cout << "dgist press constructor1 is called" << endl;
}
dgist_press::dgist_press(const string con) :IArticlePub("DGIST", con) {
	cout << "dgist press constructor2 is called" << endl;
}
void dgist_press::Event() {
	cout << "The Event is totally swindle" << endl;
}
void dgist_press::CheerUp() {
	for (int i = 0; i < sub_list.size(); i++) {
		cout << "Sub (" << sub_list[i]->getSubName() << "," << sub_list[i]->getSubID() << ")'s latest subscribed news is ";
		cout << '"' << "I Love you guys" << '"' << " by " << getPubName();
		cout << endl;
	}
}
dgist_press::~dgist_press() {
	cout << "DGIST PRESS is destructed" << endl;
}


// BBC's --------------------------------------------------------------------------------------------------

class BBC : public IArticlePub {
public:
	BBC();
	BBC(const string con);
	void Advertisement();
	void Event();
	~BBC();
};

BBC::BBC() :IArticlePub("BBC") {
	cout << "BBC constructor1 is called" << endl;
}
BBC::BBC(const string con) :IArticlePub("BBC", con) {
	cout << "BBC constructor2 is called" << endl;
}
void BBC::Advertisement() {
	cout << "BBC NEWS is coming soon!!" << endl;
}
void BBC::Event() {
	int N = 0;
	for (int i = 0; i < sub_list.size(); i++) {
		cout << "The event winner is (" << sub_list[i]->getSubName() << "," << sub_list[i]->getSubID() << ")";
		cout << endl;
		N = 1;
		break;
	}
	if (N == 0) {
		cout << "Error Message" << endl;
	}
}
BBC::~BBC() {
	cout << "BCC is destructed" << endl;
}

// CNN's --------------------------------------------------------------------------------------------------

class CNN : public IArticlePub {
public:
	CNN();
	CNN(const string con);
	void Event();
	~CNN();
};
CNN::CNN() :IArticlePub("CNN") {
	cout << "CNN constructor1 is called" << endl;
}
CNN::CNN(const string con) :IArticlePub("CNN", con) {
	cout << "CNN constructor2 is called" << endl;
}
void CNN::Event() {
	int N = 0;
	for (int i = 0; i < sub_list.size(); i++) {
		if (sub_list[i] != nullptr) {
			cout << "The event winner is (" << sub_list[i]->getSubName() << "," << sub_list[i]->getSubID() << ")";
			cout << endl;
			N = 1;
			break;
		}
	}
	if (N == 0) {
		cout << "Error Message" << endl;
	}
}
CNN::~CNN() {
	cout << "CNN is destructed" << endl;
}

// main --------------------------------------------------------------------------------------------------

int main() {
	dgist_press* dgistPub = new dgist_press(string("this is dgist pub"));
	BBC* bbcPub = new BBC();
	CNN* cnnPub = new CNN();
	//Jenny subscribe DGIST, BBC
	IArticleSub* jennySub = new IArticleSub("Jenny", dgistPub);
	*jennySub >> bbcPub; //Operator Overloading
	//Tom subscribe BBC, CNN
	IArticleSub* tomSub = new IArticleSub("Tom");
	*tomSub >> bbcPub; //Operator Overloading
	*tomSub >> cnnPub; //Operator Overloading
	//Kate subscribe DGIST, BBC, CNN
	IArticleSub* kateSub = new IArticleSub("Kate", dgistPub);
	*kateSub >> bbcPub; //Operator Overloading
	*kateSub >> bbcPub; //Operator Overloading
	*kateSub >> cnnPub; //Operator Overloading
	cout << "All Sub of (" << dgistPub->getPubName() << "," << dgistPub->getPubID() << "): ";
	dgistPub->PrintAllSub();

	cout << "All Sub of (" << bbcPub->getPubName() << "," << bbcPub->getPubID() << "): ";
	bbcPub->PrintAllSub();

	cout << "All Sub of (" << cnnPub->getPubName() << "," << cnnPub->getPubID() << "): ";
	cnnPub->PrintAllSub();

	bbcPub->Event();
	dgistPub->CheerUp();
	kateSub->Detach(bbcPub);
	cout << "All Pub of (" << jennySub->getSubName() << "," << jennySub->getSubID() << "): ";
	jennySub->PrintAllPub();

	cout << "All Pub of (" << tomSub->getSubName() << "," << tomSub->getSubID() << "): ";
	tomSub->PrintAllPub();

	cout << "All Pub of (" << kateSub->getSubName() << "," << kateSub->getSubID() << "): ";
	kateSub->PrintAllPub();

	cout << "=========DGIST Notify ===========" << endl;
	*dgistPub << "Welcome New DGIST students"; //Operator Overloading

	cout << "=========BBC Notify ===========" << endl;
	*bbcPub << "Mr. Son scored at Tottenham"; //Operator Overloading

	cout << "=========CNN Notify ===========" << endl;
	*cnnPub << "New York city celebrates Christmas"; //Operator Overloading
	cout << "=========DELETING [tomSub]===========" << endl;
	delete tomSub;
	cout << "=========DGIST Notify ===========" << endl;
	*dgistPub << "Welcome New DGIST students"; //Operator Overloading

	cout << "=========BBC Notify ===========" << endl;
	*bbcPub << "Mr. Son scored at Tottenham"; //Operator Overloading

	cout << "=========CNN Notify ===========" << endl;
	*cnnPub << "New York city celebrates Christmas"; //Operator Overloading

	cout << "=========Delete all others ===========" << endl;
	delete dgistPub;
	delete bbcPub;
	delete cnnPub;
	delete jennySub;
	//delete tomSub;
	delete kateSub;

	return 0;
}