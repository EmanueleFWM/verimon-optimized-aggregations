#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <vector>
#include <utility>

using namespace std;

int main(int argc, char *argv[]) {
	ifstream infile("tests.txt");
	int xs, ys, ts, events, ftype, ivl_start, ivl_end;
	string type, temporal;


	string line;
	getline(infile, line);

	int last_ts = -1;
	bool done = false;
	vector<string> headers;
	vector<string> results;

	ofstream out;
	out.open("out.csv", ios::out | ios::trunc);

	while (getline(infile, line)) {
		stringstream s(line);
		s >> xs >> ys >> ts >> events >> ivl_start >> ivl_end >> type >> temporal >> ftype;

		if(last_ts == -1) {
			headers.push_back("timestamps");
			last_ts = ts;
		}
		if(ts != last_ts) {
			if(!done) {
				for(int j = 0; j < headers.size() - 1; ++j) {
					out << headers[j] << ",";
				}
				out << headers[headers.size() - 1] << endl;
				done = 1;
			}

			out << last_ts << ",";
			for(int j = 0; j < results.size() -1; ++j) {
				out << results[j] << ",";
			}
			out << results[results.size() - 1] << endl;

			results.clear();
			last_ts = ts;
		}

		ofstream formula;
		formula.open("w.mfotl", ios::out | ios::trunc);

		ofstream signature;
		signature.open("w.sig", ios::out | ios::trunc);
		signature << "P(int, int)" << endl;

		string lside;
		if(ftype == 0) {
			lside = "TRUE";
		} else if(ftype == 1) {
			lside = "NOT Q(x, y)";
			signature << "Q(int, int)" << endl;
		} else {
			lside = "NOT Q(y)";
			signature << "Q(int)" << endl;
		}

		if(ftype == 0 && temporal == "SINCE") {
			formula << "c <- " << type << " x; y(ONCE [" << ivl_start << ", " << ivl_end << "] P(x, y))" << endl;
		} else {
			formula << "c <- " << type << " x; y ("<< lside << " " << temporal << " [" << ivl_start << ", " << ivl_end << "] P(x, y))" << endl;
		}

		ofstream log;
		log.open ("w.log", ios::out | ios::trunc);

		vector<pair<int, int> > prevs;

		for(int i = 0; i < ts; ++i) {
			for(int j = 0; j < events; ++j) {
				if(ftype == 2) {
					if(rand() % (ts * events / ys) == 0) {
						log << "@" << i<< " Q(" << rand() % ys << ")" << endl;
					} else {
						int a = rand() % xs;
						int b = rand() % ys;
						log << "@" << i << " P(" << a << ", " << b << ")" << endl;
					}
				} else if(ftype == 1) {
					if(rand() % 3 == 0 && !prevs.empty()) {
						int idx = rand() % prevs.size();
						auto elem = prevs.at(idx);
						prevs.erase(prevs.begin() + idx); 
						log << "@" << i<< " Q(" << elem.first << ", " << elem.second << ")" << endl;
					} else {
						int a = rand() % xs;
						int b = rand() % ys;
						prevs.push_back(make_pair(a, b));
						log << "@" << i << " P(" << a << ", " << b << ")" << endl;
					}
				} else {
					log << "@" << i << " P(" << rand() % xs << ", " << rand() % ys << ")" << endl;
				}
			}
		}

		string program;
		s >> program;

		string cmd1;

		if(program == "verimon") cmd1 = "./monpoly -verified";
		if(program == "verimon_old") cmd1 = "./monpoly_old -verified";
		if(program == "monpoly") cmd1 = "./monpoly";

		do {
			string command = "timeout 60 ";
			command.append(cmd1);
			command.append(" -formula w.mfotl -log w.log -sig w.sig > output");

			double time = 0;

			int thisrun = 0;

			for(int i = 0; i < 10; ++i) {

				auto start = chrono::high_resolution_clock::now();

				int status = system(&command[0]);

				auto finish = chrono::high_resolution_clock::now();

				if (status != 0) {
					int ms = chrono::duration_cast<chrono::milliseconds>(finish-start).count();
					if(ms > 1000 * 60) {
						thisrun = 1;
					} else {
						thisrun = -1;
					}
					break;
				}

				time += chrono::duration_cast<chrono::milliseconds>(finish-start).count()/1000.0;
			}

			stringstream result;
			if(thisrun == 0) {
				result << fixed << setprecision(2) << (time / 10);
			} else if(thisrun == 1) {
				result << "TO";
			} else if(thisrun == -1) {
				result << "SO";
			}

			results.push_back(result.str());

			if(!done) {
				stringstream header;
				if(program == "verimon") header << "Verimon new - ";
				if(program == "verimon_old") header << "Verimon old - ";
				if(program == "monpoly") header << "Monpoly - ";
				header << temporal << " - " << type << " - Type " << ftype;
				headers.push_back(header.str());
			}

			s >> program;

			if(program == "verimon") cmd1 = "./monpoly -verified";
			if(program == "verimon_old") cmd1 = "./monpoly_old -verified";
			if(program == "monpoly") cmd1 = "./monpoly";
		} while (s);
	}

	if(!done) {
		for(int j = 0; j < headers.size() - 1; ++j) {
			out << headers[j] << ",";
		}
		out << headers[headers.size() - 1] << endl;
		done = 1;
	}

	out << last_ts << ",";
	for(int j = 0; j < results.size() -1; ++j) {
		out << results[j] << ",";
	}
	out << results[results.size() - 1] << endl;

	results.clear();
	last_ts = ts;
}