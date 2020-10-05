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
		std::ssub_match sm;
		regex_search(s, m, hhmmss);
		sm = m[0];
		cout << sm.str() << endl;
	}

	return 0;
}
