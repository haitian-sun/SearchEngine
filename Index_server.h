#ifndef INDEX_SERVER_H
#define INDEX_SERVER_H

#include <iosfwd>
#include <map>
#include <stdint.h>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <list>


//word -> idf->total #occurrences->
//Doc_id -> number of occurrences in Doc_id -> Doc_id's normalization factor (before sqrt)
/*struct node{
	long doc_id;
	double tfidf;

	node(int a, double c){
		doc_id = a;
		tfidf = c;
	}
};*/

struct word_info{
	double df;
	std::map<long, double> docs;
};


struct Query_hit {
    Query_hit(const char *id_, double score_)
        : id(id_), score(score_)
    {}

    const char *id;
    double score;
};


class mycomparison{
 public:
  bool operator() (const Query_hit& lhs, const Query_hit& rhs) const{
    return (lhs.score < rhs.score);
  }
};


typedef std::priority_queue<Query_hit,std::vector<Query_hit>,mycomparison> pq_type;

class Index_server {
private:
	std::unordered_map<std::string, word_info> words;
public:
    void run(int port);

    // Methods that students must implement.
    void init(std::ifstream& infile);
    void process_query(const std::string& query, std::vector<Query_hit>& hits, double weight);
};

#endif
