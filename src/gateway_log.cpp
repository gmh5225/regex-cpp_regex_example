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
	cout << "read size = " << buffer.str().size() << endl;
	return 0;
}
