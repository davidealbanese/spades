#include <iostream>
#include <algorithm>
#include "graph.hpp"
#include "hash.hpp"
#include "graphBuilder.hpp"
#include "parameters.hpp"
#include <ext/hash_map>
#include "logging.hpp"
#include "graphVisualizer.hpp"
#include "iostream"
#include "fstream"

LOGGER("a.graphBuilder");

using namespace std;
using namespace __gnu_cxx;
using namespace gvis;

typedef hash_map< Sequence, int, HashSym<Sequence>, EqSym<Sequence> > SeqCount;
SeqCount seqCount;

typedef hash_map< Sequence, int, HashSym<Sequence>, EqSym<Sequence> > SeqVertice;
SeqVertice seqVertice;

map<long long, int> edges;
//CGraph graph;
GraphPrinter<int> printer("", "output\x.dot");

LoggerPtr logger(Logger::getLogger("a.graphBuilder"));
HashSym<Sequence> hashSym;

void processReadA(Seq<MPSIZE> r) {
	// Processing in O(length * k).
	// Can be done in O(length), provided the hash is polynomial.
	unsigned int h1 = -1;
	int pos1 = -1;
	unsigned int h2 = -1;
	int pos2 = -1;
	for (int i = 0; i + K <= MPSIZE; i++) {
		unsigned int h = hashSym(Sequence(r).Subseq(i, i + K));
		if (h < h1) {
			h2 = h1;
			pos2 = pos1;
			h1 = h;
			pos1 = i;
		} else if (h < h2) {
			h2 = h;
			pos2 = i;
		}
	}
	if (pos1 > pos2) {
		int t = pos1; pos1 = pos2; pos2 = t;
		t = h1; h1 = h2; h2 = t;
	}
	seqCount[Sequence(r).Subseq(pos1, pos1 + K)]++;
	seqCount[Sequence(r).Subseq(pos2, pos2 + K)]++;
	//	if (kmers_map.count(k1)) {
	//		CVertex v(&k1);
	//	} else {
	//
	//	}
	//	CVertex v1 = kmers_map[k1];
	//	v1.hits_++;
	//	graph.AddVertex(v1);
	//	graph.AddVertex(v2);
	//	CEdge e(&v2, &r, pos1, pos2);
	//	v1.AddEdge(e);
}

void GraphBuilder::selectGood()
{
    int i = 1;
    for (SeqCount::iterator p = seqCount.begin(); p != seqCount.end(); ++p) {
    	printer.addVertex(i, p->first.str());
		seqVertice[p->first] = i;
		i++;

	}
    INFO(seqVertice.size() << " vertices");
}

void processReadB(Seq<MPSIZE> r) {
	vector<int> vs;
	for (int i = 0; i + K <= MPSIZE; i++) {
		Sequence s = Sequence(r).Subseq(i, i + K);
		if (seqVertice.count(s) > 0) {
			vs.push_back(seqVertice[s]);
		}
//		TODO WTF?!
//		if (seqVertice.find(s) != seqVertice.end()) {
//			DEBUG("bzz" << seqVertice.find(s).operator *().first.str())
//		}
	}
	for (int i = 0; i < vs.size() - 1; i++) {
//		DEBUG(vs[i] << " " << vs[i + 1])
		long long id = vs[i] * ((long long) 1000000000) + vs[i + 1];
		if (edges[id] > 0) {
			continue;
		}
		printer.addEdge(vs[i], vs[i + 1], "");
		edges[id] = 1;
	}
//	DEBUG(r.str() << " " << vs.size());
}

void GraphBuilder::build() {
	INFO("Building graph...");
	mate_read<MPSIZE>::type mp;
	for (int i = 0; !irs.eof() && i < CUT; i++) {
		irs >> mp;
		processReadA(mp[0]);
		processReadA(mp[1]);
	}
	irs.reset();
    selectGood();
	for (int i = 0; !irs.eof() && i < CUT; i++) {
		irs >> mp;
		processReadB(mp[0]);
		processReadB(mp[1]);
	}
	irs.close();
    printer.output();
}
