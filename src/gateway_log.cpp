#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <regex>
#include <iomanip>
#include <algorithm>

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
	record_t()
		{}
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

	for (auto blk_iter = result.begin(); blk_iter != result.end(); blk_iter++) {
		// search time stamp
		std::regex hhmmss("\\d\\d:\\d\\d:\\d\\d");
		std::smatch m;
		std::ssub_match time;
		regex_search(*blk_iter, m, hhmmss);
		time = m[0];

		// search tag id pattern
		std::regex tag("(16 04 |16 07 )((\\w\\w\\s){5})");
		std::smatch m2;
		regex_search(*blk_iter, m2, tag);
		string tag_id = m2[0].str();

		// add valid tag_id to statistic
		if (tag_id.size() != 0) {
			// dump_smatch(m2);
			// test if tag id existed in map
			if (!statistic.count(tag_id)) {
				statistic.insert({tag_id, record_t{tag_id}});
			}
			record_t& r = statistic[tag_id];
			r.add_time(time.str());
		}
		// print progress
		int percent = distance(result.begin(), blk_iter) * 100 / result.size();
		cout << '\r'
		     << std::setw(3) << percent << '%' << flush;
	}
	cout << endl << "statistic : " << statistic.size() << " entries." << endl;

	// move map to vector
	vector<pair<string, record_t>> vec;
	for (auto it = statistic.begin(); it != statistic.end(); ++it) {
		vec.push_back(*it);
	}
	// sort vector
	sort(vec.begin(),
	     vec.end(),
	     [](pair<string, record_t> x, pair<string, record_t> y) {
		     return x.second.count > y.second.count;
	     });
	for (auto it = vec.begin(); it != vec.end(); ++it) {
		cout << it->first << ": " << it->second.count << endl;
	}

	return 0;
}
