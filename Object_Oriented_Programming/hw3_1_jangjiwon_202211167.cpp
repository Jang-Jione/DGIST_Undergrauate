#include <iostream>
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
	IArticleSub* sub_list[5];
	int numOfSub = 0;
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
};

class IArticleSub {
private:
	string sub_name;
	int sub_id;
	string recent_article_contents;
	IArticlePub* recent_article_pub;
	static int static_sub_counter;
protected:
	IArticlePub* pub_list[5];
	int numOfPub = 0;
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
	for (int i = 0; i < numOfSub; i++) {
		while (i < 5) {
			if (sub_list[i] != nullptr) {
				sub_list[i]->DetachResponse(this);
				break;
			}
			else {
				i++;
			}
		}
	}
}
void IArticlePub::NotifyAttach(IArticleSub* subscriber) {
	int N = 0;
	for (int i = 0; i < numOfSub; i++) {
		while (i < 5) {
			if (sub_list[i] == subscriber) {
				cout << "Error Message" << endl;
				N = 1;
				break;
			}
			else {
				i++;
			}
		}
	}
	if (N == 0) {
		sub_list[numOfSub] = subscriber;
		numOfSub++;
		subscriber->AttachResponse(this);
		cout << "[Attach] Pub " << "(" << pub_name << "," << pub_id << ")";
		cout << " is attached to Sub " << "(" << subscriber->getSubName() << "," << subscriber->getSubID() << ")" << endl;
	}
}
void IArticlePub::NotifyAttachResponse(IArticleSub* subscriber) {
	int N = 0;
	for (int i = 0; i < numOfSub; i++) {
		while (i < 5) {
			if (sub_list[i] == subscriber) {
				cout << "Error Message" << endl;
				N = 1;
				break;
			}
			else {
				i++;
			}
		}
	}
	if (N == 0) {
		sub_list[numOfSub] = subscriber;
		numOfSub++;
		cout << "[Attach] Pub " << "(" << pub_name << "," << pub_id << ")";
		cout << " is attached to Sub " << "(" << subscriber->getSubName() << "," << subscriber->getSubID() << ")" << endl;
	}
}
void IArticlePub::NotifyDetach(IArticleSub* subscriber) {
	int N = 0;
	for (int i = 0; i < numOfSub; i++) {
		while (i < 5) {
			if (sub_list[i] == subscriber) {
				sub_list[i] = nullptr;
				numOfSub--;
				cout << "[Pub] (" << pub_name << "," << pub_id << ") detach [Sub] (" << subscriber->getSubName();
				cout << "," << subscriber->getSubID() << ")" << endl;
				subscriber->DetachResponse(this);
				N = 1;
				break;
			}
			else {
				i++;
			}
		}
	}
	if (N == 0) {
		cout << "Error Message" << endl;
	}
}
void IArticlePub::NotifyDetachResponse(IArticleSub* subscriber) {
	int N = 0;
	for (int i = 0; i < numOfSub; i++) {
		while (i < 5) {
			if (sub_list[i] == subscriber) {
				sub_list[i] = nullptr;
				numOfSub--;
				cout << "[Pub] (" << pub_name << "," << pub_id << ") detach [Sub] (" << subscriber->getSubName();
				cout << "," << subscriber->getSubID() << ")" << endl;
				N = 1;
				break;
			}
			else {
				i++;
			}
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
	for (int i = 0; i < numOfSub; i++) {
		while (i < 5) {
			if (sub_list[i] != nullptr) {
				sub_list[i]->Update(this, recent_contents);
				cout << endl;
				break;
			}
			else {
				i++;
			}
		}
	}
	cout << endl;
}
int IArticlePub::getSubSize() {
	int size = numOfSub;
	return size;
}
void IArticlePub::PrintAllSub() {
	for (int i = 0; i < numOfSub; i++) {
		while (i < 5) {
			if (sub_list[i] != nullptr) {
				cout << "[" << sub_list[i]->getSubName() << "," << sub_list[i]->getSubID() << "]";
				break;
			}
			else {
				i++;
			}
		}
	}
	cout << endl;
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
	for (int i = 0; i < numOfPub; i++) {
		while (i < 5) {
			if (pub_list[i] != nullptr) {
				pub_list[i]->NotifyDetachResponse(this);
				break;
			}
			else {
				i++;
			}
		}
	}
}
void IArticleSub::Attach(IArticlePub* publisher) {
	int N = 0;
	for (int i = 0; i < numOfPub; i++) {
		while (i < 5) {
			if (pub_list[i] == publisher) {
				cout << "Error Message" << endl;
				N = 1;
				break;
			}
			else {
				i++;
			}
		}
	}
	if (N == 0) {
		pub_list[numOfPub] = publisher;
		numOfPub++;
		publisher->NotifyAttachResponse(this);
		cout << "[Attach] Sub " << "(" << sub_name << "," << sub_id << ")";
		cout << " is attached to Pub " << "(" << publisher->getPubName() << "," << publisher->getPubID() << ")" << endl;
	}
}
void IArticleSub::AttachResponse(IArticlePub* publisher) {
	int N = 0;
	for (int i = 0; i < numOfPub; i++) {
		while (i < 5) {
			if (pub_list[i] == publisher) {
				cout << "Error Message" << endl;
				N = 1;
				break;
			}
			else {
				i++;
			}
		}
	}
	if (N == 0) {
		pub_list[numOfPub] = publisher;
		numOfPub++;
		cout << "[Attach] Sub " << "(" << sub_name << "," << sub_id << ")";
		cout << " is attached to Pub " << "(" << publisher->getPubName() << "," << publisher->getPubID() << ")" << endl;
	}
}
void IArticleSub::Detach(IArticlePub* p_pub) {
	int N = 0;
	for (int i = 0; i < numOfPub; i++) {
		while (i < 5) {
			if (pub_list[i] == p_pub) {
				pub_list[i] = nullptr;
				numOfPub--;
				cout << "[Sub] (" << sub_name << "," << sub_id << ") unsubscribes [Pub] (" << p_pub->getPubName();
				cout << "," << p_pub->getPubID() << ")" << endl;
				p_pub->NotifyDetachResponse(this);
				N = 1;
				break;
			}
			else {
				i++;
			}
		}
	}
	if (N == 0) {
		cout << "Error Message" << endl;
	}
}
void IArticleSub::DetachResponse(IArticlePub* p_pub) {
	int N = 0;
	for (int i = 0; i < numOfPub; i++) {
		while (i < 5) {
			if (pub_list[i] == p_pub) {
				pub_list[i] = nullptr;
				numOfPub--;
				cout << "[Sub] (" << sub_name << "," << sub_id << ") unsubscribes [Pub] (" << p_pub->getPubName();
				cout << "," << p_pub->getPubID() << ")" << endl;
				N = 1;
				break;
			}
			else {
				i++;
			}
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
	int size = numOfPub;
	return size;
}
void IArticleSub::PrintAllPub() {
	for (int i = 0; i < numOfPub; i++) {
		while (i < 5) {
			if (pub_list[i] != nullptr) {
				cout << "[" << pub_list[i]->getPubName() << "," << pub_list[i]->getPubID() << "]";
				break;
			}
			else {
				i++;
			}
		}
	}
	cout << endl;
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

dgist_press::dgist_press():IArticlePub("DGIST") {
	cout << "dgist press constructor1 is called" << endl;
}
dgist_press::dgist_press(const string con):IArticlePub("DGIST", con) {
	cout << "dgist press constructor2 is called" << endl;
}
void dgist_press::Event() {
	cout << "The Event is totally swindle" << endl;
}
void dgist_press::CheerUp() {
	for (int i = 0; i < numOfSub; i++) {
		while (i < 5) {
			if (sub_list[i] != nullptr) {
				cout << "Sub (" << sub_list[i]->getSubName() << "," << sub_list[i]->getSubID() << ")'s latest subscribed news is ";
				cout << '"' << "I Love you guys" << '"' << " by " << getPubName();
				cout << endl;
				break;
			}
			else {
				i++;
			}
		}
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
	for (int i = 0; i < numOfSub; i++) {
		while (i < 5) {
			if (sub_list[i] != nullptr) {
				cout << "The event winner is (" << sub_list[i]->getSubName() << "," << sub_list[i]->getSubID() << ")";
				cout << endl;
				N = 1;
				break;
			}
			else {
				i++;
			}
		}
		if (N == 1) {
			break;
		}
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
	for (int i = 0; i < numOfSub; i++) {
		while (i < 5) {
			if (sub_list[i] != nullptr) {
				cout << "The event winner is (" << sub_list[i]->getSubName() << "," << sub_list[i]->getSubID() << ")";
				cout << endl;
				N = 1;
				break;
				break;
			}
			else {
				i++;
			}
		}
		if (N == 1) {
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
