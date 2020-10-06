#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <regex>

using namespace std;

stringstream execute(string cmd){
	char line[255];
	FILE *fp;
	stringstream ss;

	const char *sysCommand = cmd.data();
	if ((fp = popen(sysCommand, "r")) == NULL) {
		cout << "error" << endl;
		return ss;
	}
	while (fgets(line, sizeof(line)-1, fp) != NULL){
		ss << line ;
	}
	pclose(fp);
	return ss;
}

vector<string> split(std::string src, std::string pattern)
{
	std::regex ws_re(pattern);
	vector<string> v(sregex_token_iterator(
				 src.begin(),
				 src.end(),
				 ws_re,
				 -1),
			 sregex_token_iterator());
	return v;
}

class record_t
{
public:
	record_t(string tid)
		:tag_id(tid), count(0)
		{}
	string tag_id;
	int count;
	vector<string> time_list;
	void add_time(string time) {
		time_list.push_back(time);
		count++;
	}
};

map<string, record_t> statistic;

void dump_smatch(std::smatch& sm)
{
	int index = 0;
	for (std::ssub_match ssm : sm) {
		cout << "[" << index++ << "]: " << ssm.str() << "\t";
	}
	cout << endl;
}

int main (int argc, char** argv)
{
	// check input
        if (argc != 2) {
		cout << "usage : " << argv[0] << " [gw log file]" << endl;
		return 0;
	}
	// argv[1] should be gateway log file name, we will grep it
	string fname(argv[1]);
	string cmd = string("egrep -A 2 \"gw_common.c\" ") + fname;
	auto buffer = execute(cmd);
	auto result = split(buffer.str(), "--");
	for (auto&& s: result) {
		std::regex hhmmss("\\d\\d:\\d\\d:\\d\\d");
		std::smatch m;
		std::ssub_match time;
		regex_search(s, m, hhmmss);
		time = m[0];
		std::regex tag("(16 04 |16 07 )((\\w\\w\\s){5})");
		std::smatch m2;
		std::ssub_match tid;
		regex_search(s, m2, tag);
		tid = m2[0];

		if (tid.str().size() != 0) {
			// m2[2] is tag id
			dump_smatch(m2);
			string tag_id = m2[2].str();
			// test if tag id existed in map
			//if (!statistic.count(tag_id))
			//	statistic.insert({tag_id, record_t{tag_id}});

			//statistic[tag_id].add_time(time.str());
		}
	}

	return 0;
}
