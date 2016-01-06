#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <set>
#include <cstdlib>
#include <cmath>

using namespace std;

// EECS 485 - Project 5
// PageRank Algorithm

// Node struct
// Keeps track of incoming/outgoing nodes
struct node {
	//int id;
	//string name;
	
	double rank[2];

	set<int> children;
	set<int> parents;

	bool sink;

	node(int total_nodes) : sink(false) {
		rank[0] = rank[1] = 1.0 / total_nodes;
	}

	node(): sink(false) {} // To make map happy

	int get_o() {
		return static_cast<int>(children.size());
	}

	int get_i() {
		return static_cast<int>(parents.size());
	}

	void set_rank(int total_nodes){
		rank[0] = rank[1] = 1.0 / total_nodes;
	}
};

int main(int argc, char *argv[]) {
	// Global application variables
	double d;					// Damping factor
	bool   k = false;			// Stop condition #1
	int    iterations;			// Condition #1 value
	double conv;				// Condition #2 value
	string infile;				// File to read from
	string outfile;				// File to write to
	int    N;					// Total number of nodes
	double fexpr;				// Equal to (1-d)/N

	// Process the command line arguments
	d = atof(argv[1]);
	if(strcmp(argv[2], "-k") == 0) {
		k = true;
		iterations = atoi(argv[3]);
	} else {
		conv = atof(argv[3]);
	}

	infile =  argv[4];
	outfile = argv[5];
	
	unordered_map<int, node> nodes;
	//unordered_map<int, node> nodes_past;

	// Read from the input file
	ifstream is(infile);

	// Write to output file
	ofstream os;
	os.precision(4);
	
	string buf;
	int m, n;
	getline(is, buf);
	while (true){
 		getline(is, buf);
		if (buf != "<eecs485_edge>")
			break;
		getline(is, buf, '>');
		is >> m;
		getline(is, buf);

		getline(is, buf, '>');
		is >> n;
		getline(is, buf);

		getline(is, buf);

		if(m != n) {
			nodes[m].children.insert(n);
			nodes[n].parents.insert(m);
		}
	}
	N = nodes.size();
	fexpr = (1 - d) / N;

	for (auto it = nodes.begin(); it != nodes.end(); it ++){
		it->second.set_rank(N);
	}
	
	// Set sink nodes
	vector<int> sinks;
	for(auto it = nodes.begin(); it != nodes.end(); ++it) {
		if(it->second.children.empty()) {
			it->second.sink = true;
			sinks.push_back(it->first);
		}
	}

	// Start calculating the pagerank
	// Copy the initial value
	
	// For iteration count
	if(k) {
		for(int i = 0; i < iterations; ++i) {
			int np = ((i % 2 == 0) ? 0 : 1);
			int nc = ((i % 2 == 0) ? 1 : 0);

			if(i == iterations - 1) {
				os.open(outfile);
				//os << scientific;
			}

			// We only need to calculate the sink node contribution once every iteration, rather than after every node in the loop.
			double sinksum = 0;
			for(auto jt = sinks.begin(); jt != sinks.end(); ++jt) {
				sinksum += nodes[*jt].rank[np] / (N - 1);
			}


			// Calculate the pagerank for every node
			for(auto it = nodes.begin(); it != nodes.end(); ++it) {
				double summation = 0;

				for(auto jt = it->second.parents.begin(); jt != it->second.parents.end(); ++jt) {
					summation += nodes[*jt].rank[np] / nodes[*jt].children.size();
				}

				summation += sinksum;

				it->second.rank[nc] = fexpr + d * summation;

				if(os.is_open() && i == iterations - 1) {
					os << it->first << "," << std::scientific << it->second.rank[nc] << "\n";
				}
			}
		}

		os.close();
	}
	else {
		bool con = false;
		int i = 0;
		do {
			con = false;
			int np = ((i % 2 == 0) ? 0 : 1);
			int nc = ((i % 2 == 0) ? 1 : 0);

			// We only need to calculate the sink node contribution once every iteration, rather than after every node in the loop.
			double sinksum = 0;
			for(auto jt = sinks.begin(); jt != sinks.end(); ++jt) {
				sinksum += nodes[*jt].rank[np] / (N - 1);
			}


			// Calculate the pagerank for every node
			for(auto it = nodes.begin(); it != nodes.end(); ++it) {
				double summation = 0;

				for(auto jt = it->second.parents.begin(); jt != it->second.parents.end(); ++jt) {
					summation += nodes[*jt].rank[np] / nodes[*jt].children.size();
				}

				summation += sinksum;

				it->second.rank[nc] = fexpr + d * summation;

				double diffPer = abs(it->second.rank[nc] - it->second.rank[np]) / it->second.rank[np];
				if(diffPer > conv)
					con = true;
			}

			i++;

		} while(con);

		int fval = ((i % 2 == 0) ? 1 : 0);

		os.open(outfile);

		//os << scientific;

		for(auto it = nodes.begin(); it != nodes.end(); ++it) {
			os << it->first << "," << std::scientific << it->second.rank[fval] << "\n";
		}
		
		os.close();
	}
	return 0;
}
