#include "Index_server.h"

#include <cassert>
#include <cmath>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <map>
#include <utility>

#include <unordered_set>

#include "mongoose.h"

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::unordered_set;
using std::unordered_map;

using std::ostream;
using std::ostringstream;
using std::string;
using std::stringstream;
using std::vector;
using std::list;
using std::pair;

map<long, double> prmap;

namespace {
    int handle_request(mg_connection *);
    int get_param(const mg_request_info *, const char *, string&);
    string get_param(const mg_request_info *, const char *);
    string to_json(const vector<Query_hit>&);

    ostream& operator<< (ostream&, const Query_hit&);
}

pthread_mutex_t mutex;

// Runs the index server on the supplied port number.
void Index_server::run(int port)
{
    // List of options. Last element must be NULL
    ostringstream port_os;
    port_os << port;
    string ps = port_os.str();
    const char *options[] = {"listening_ports",ps.c_str(),0};

    // Prepare callback structure. We have only one callback, the rest are NULL.
    mg_callbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.begin_request = handle_request;

    // Initialize the global mutex lock that effectively makes this server
    // single-threaded.
    pthread_mutex_init(&mutex, 0);

    // Start the web server
    mg_context *ctx = mg_start(&callbacks, this, options);
    if (!ctx) {
        cerr << "Error starting server." << endl;
        return;
    }

    pthread_exit(0);
}

// Load index data from the file of the given name.
void Index_server::init(ifstream& infile)
{
    // Fill in this method to load the inverted index from disk.
	string line;
    //word -> idf -> total #occurrences->
    //Doc_id -> number of occurrences in Doc_id -> Doc_id's normalization factor (before sqrt)
	while(std::getline(infile, line)) {
		stringstream iss(line);
        word_info info;
        string word;
        string msg;
		iss >> word >> info.df;
		string ind_data;
        while (iss >> ind_data){
			stringstream colons(ind_data);

			string docidstring;
			getline(colons, docidstring, ':');
			long doc_id = stol(docidstring);
			
			getline(colons, docidstring);
			double tfidf_in = stod(docidstring);

            info.docs[doc_id] = tfidf_in;
        }
        words[word] = info;
	}

	//Load in the pagerank scores
	ifstream prfile("pagerank/output");
	while(prfile >> line) {
		stringstream linedata(line);
		string lineattr;
		getline(linedata, lineattr, ',');
		long id = stol(lineattr);
		getline(linedata, lineattr);
		double pr = stod(lineattr);
		prmap[id] = pr;
	}
}

// Search the index for documents matching the query. The results are to be
// placed in the supplied "hits" vector, which is guaranteed to be empty when
// this method is called.
void Index_server::process_query(const string& query, vector<Query_hit>& hits, double weight) {
    cout << "Processing query '" << query << "'" << endl;

    // Fill this in to process queries.
	// Get list of stop words
	ifstream stop("english.stop");
    if (!stop.is_open()) {
        cerr << "Error opening file 'english.stop' "<< endl;
        return;
    }
    unordered_set<string> stopWord;
    string temp;
    while (stop >> temp){
        stopWord.insert(temp);
    }

	// Get the TF-IDF scores for the query
	// Count the number of times a given word appears in the query
	unordered_map<string, int> query_count; //Words in the query
	stringstream q(query);
	while(q >> temp) {
        //temp is not stopWord
        if (stopWord.find(temp) == stopWord.end())
		  query_count[temp]++;
	}
    
    //needn't to norm weights in query_count
    //doc_id, score
	map<long, double> candidates;
	
    //list<pair<int, double>> candidates;

    for (auto it = query_count.begin(); it != query_count.end(); it++) {
        string word = it->first;
        int freq = it->second;
        //double df = words[word].df;
        //initialize the list candidates
        if (it == query_count.begin()) {
            for (auto iter = words[word].docs.begin(); iter != words[word].docs.end(); iter++) {
				//iter->first = doc_id    iter->second = tfidf score
				candidates[iter->first] = iter->second;
            }
        }
		else {
            auto iter_candiate = candidates.begin();
            auto iter_docs = words[word].docs.begin();
            while (iter_candiate != candidates.end() && iter_docs != words[word].docs.end()) {
                if (iter_candiate->first < iter_docs->first) {
                    iter_candiate = candidates.erase(iter_candiate);
                }
				else if (iter_candiate->first > iter_docs->first) {
                    iter_docs++;
                }
				else {
                    //iter_candiate->second += (idf * iter_docs->occurrences/sqrt(iter_docs->norm)*freq);
					iter_candiate->second += iter_docs->second * freq;
                    iter_candiate++;
                    iter_docs++;
                }
            }         
        }
        //if candidates is empty
        if (candidates.size() == 0)
            return;
    }

	//Factor in the w score
	for(auto it = candidates.begin(); it != candidates.end(); ++it) {
		it->second = it->second * (1.0 - weight);
		it->second += prmap[it->first]*weight;
	}


    //change hits
    pq_type hits_queue;
    while (!candidates.empty()) {
        candidates.begin();
        string str = std::to_string(candidates.begin()->first);
        char * id = new char[str.size()+1];
        strcpy(id, str.c_str());
        Query_hit hit(id, candidates.begin()->second);
        hits_queue.push(hit);
        candidates.erase(candidates.begin());
    }
	
	int i = 0;
    while (!hits_queue.empty() && i < 10){
        hits.push_back(hits_queue.top());
        hits_queue.pop();
		i++;
    }

}

namespace {
    int handle_request(mg_connection *conn)
    {
        const mg_request_info *request_info = mg_get_request_info(conn);

        if (!strcmp(request_info->request_method, "GET") && request_info->query_string) {
            // Make the processing of each server request mutually exclusive with
            // processing of other requests.

            // Retrieve the request form data here and use it to call search(). Then
            // pass the result of search() to to_json()... then pass the resulting string
            // to mg_printf.
            string query;
			string weightstring;
            if (get_param(request_info, "q", query) == -1) {
                // If the request doesn't have the "q" field, this is not an index
                // query, so ignore it.
                return 1;
            }

			if(get_param(request_info, "w", weightstring) == -1) {
				// We need the w param
				return 1;
			}

			double weight = stod(weightstring);

            vector<Query_hit> hits;
            Index_server *server = static_cast<Index_server *>(request_info->user_data);

            pthread_mutex_lock(&mutex);
            server->process_query(query, hits, weight);
            pthread_mutex_unlock(&mutex);

            string response_data = to_json(hits);
            int response_size = response_data.length();

            // Send HTTP reply to the client.
            mg_printf(conn,
                      "HTTP/1.1 200 OK\r\n"
                      "Content-Type: application/json\r\n"
                      "Content-Length: %d\r\n"
                      "\r\n"
                      "%s", response_size, response_data.c_str());
        }

        // Returning non-zero tells mongoose that our function has replied to
        // the client, and mongoose should not send client any more data.
        return 1;
    }

    int get_param(const mg_request_info *request_info, const char *name, string& param)
    {
        const char *get_params = request_info->query_string;
        size_t params_size = strlen(get_params);

        // On the off chance that operator new isn't thread-safe.
        pthread_mutex_lock(&mutex);
        char *param_buf = new char[params_size + 1];
        pthread_mutex_unlock(&mutex);

        param_buf[params_size] = '\0';
        int param_length = mg_get_var(get_params, params_size, name, param_buf, params_size);
        if (param_length < 0) {
            return param_length;
        }

        // Probably not necessary, just a precaution.
        param = param_buf;
        delete[] param_buf;

        return 0;
    }

    // Converts the supplied query hit list into a JSON string.
    string to_json(const vector<Query_hit>& hits)
    {
        ostringstream os;
        os << "{\"hits\":[";
        vector<Query_hit>::const_iterator viter;
        for (viter = hits.begin(); viter != hits.end(); ++viter) {
            if (viter != hits.begin()) {
                os << ",";
            }

            os << *viter;
        }
        os << "]}";

        return os.str();
    }

    // Outputs the computed information for a query hit in a JSON format.
    ostream& operator<< (ostream& os, const Query_hit& hit)
    {
        os << "{" << "\"id\":\"";
        int id_size = strlen(hit.id);
        for (int i = 0; i < id_size; i++) {
            if (hit.id[i] == '"') {
                os << "\\";
            }
            os << hit.id[i];
        }
        return os << "\"," << "\"score\":" << hit.score << "}";
    }
}
