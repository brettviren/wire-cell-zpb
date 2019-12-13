#include <tbb/task_scheduler_init.h>
#include <tbb/flow_graph.h>
#include <czmq.h>

#define NLOOPS 10000
#define NGENS 3

struct TGen {
    int count{0};
    int id;
    TGen(int id) : id(id) {}
    ~TGen() {
        zsys_debug("gen%d: %d destructing", id, count);
    }
    bool operator()(int& num) {
        num = count;
        ++count;
        if (count <= NLOOPS) {
            zsys_debug("gen%d: %d", id, num);
            return true;
        }
        zsys_debug("gen%d: %d exiting", id, num);
        return false;
    }
};

// sink/function node body
struct T2Z {
    zsock_t* sock;
    int id;
    int count{0};
    T2Z(zsock_t* s, int id) : sock(s), id(id) {
        assert(sock);
    }
    ~T2Z() {
        zsys_debug("t2z%d: %d destructing", id, count);
    }
    int operator()(const int& num) {
        assert(sock);
        
        int rc = zsock_send(sock, "i", num);
        ++count;
        if (rc < 0) {
            zsys_error("t2z%d: %d %s", id, errno, strerror(errno));
            return num;
        }
        zsys_debug("t2z%d: %d", id, num);
        return num;
    }
};

// source node body converting zmq message to tbb
struct Z2T {
    zsock_t* sock;
    int id;
    int count{0};
    Z2T(zsock_t* s, int id) : sock(s), id(id) {
        assert(sock);
    }
    ~Z2T() {
        zsys_debug("z2t%d: %d destructing", id, count);
    }
    bool operator()(int& num) {
        assert(sock);
        int rc = zsock_recv(sock, "i", &num);
        if (rc < 0) {
            zsys_error("z2t%d: %d %s", id, errno, strerror(errno));
            return false;
        }
        ++count;
        if (count < NGENS * NLOOPS) {
            zsys_debug("z2t%d: %d [%d]", id, num, count);
            return true;
        }
        zsys_debug("z2t%d: %d [%d] exiting", id, num, count);
        return false;
    }
};

struct Sink {
    int count{0};
    int operator()(const int& num) {
        zsys_debug("snk: %d at %d", num, count);
        ++count;
        return num;
    }
};

int main(int argc, char* argv[])
{
    // NGENS*[gen -tbb-> t2z] =zmq=> z2t -tbb-> sink


    int nthreads = 1;
    if (argc > 1) {
        nthreads = atoi(argv[1]);
    }
    int nconcurs = 1;
    if (argc > 2) {
        nconcurs = atoi(argv[1]);
    }
    const char* what = "pubsub";
    if (argc > 3) {
        what = argv[3];
    }

    zsys_init();


    const char* address = "tcp://127.0.0.1:5678";


    int sender_stype = ZMQ_PUB;
    int recver_stype = ZMQ_SUB;

    if (streq(what, "pubsub")) {
        sender_stype = ZMQ_PUB;
        recver_stype = ZMQ_SUB;
    }    
    else if (streq(what, "pushpull")) {
        sender_stype = ZMQ_PUSH;
        recver_stype = ZMQ_PULL;
    }    
    else if (streq(what, "clientserver")) {
        sender_stype = ZMQ_CLIENT;
        recver_stype = ZMQ_SERVER;
    }    
    else {
        abort();
    }

    std::vector<T2Z> t2zs;
    for (int ind=0; ind<NGENS; ++ind) {
        zsock_t* sock = zsock_new(sender_stype);
        assert(sock);
        int rc = zsock_connect(sock, "%s", address);
        assert (rc >= 0);
        t2zs.emplace_back(sock, ind);
    }

    zsock_t* recv_sock = zsock_new(recver_stype);
    assert(recv_sock);
    if (recver_stype == ZMQ_SUB) {
        zsock_set_subscribe(recv_sock, "");
    }
    int port = zsock_bind(recv_sock, "%s", address);
    assert (port == 5678);
    Z2T z2t(recv_sock, 0);


    // give time for everybody to bind/connect
    zclock_sleep(1000);

    std::vector<TGen> gens;
    for (int ind=0; ind<NGENS; ++ind) {
        gens.emplace_back(ind);
    }
    Sink sink;

    tbb::task_scheduler_init m_sched(nthreads);

    tbb::flow::graph graph;
    std::vector< tbb::flow::source_node<int> > gen_nodes;
    std::vector< tbb::flow::function_node<int,int> > t2z_nodes;
    for (int ind=0; ind<NGENS; ++ind) {
        gen_nodes.emplace_back(graph, gens[ind], false);
        t2z_nodes.emplace_back(graph, nconcurs, t2zs[ind]);
    }
    tbb::flow::source_node<int> z2t_node(graph, z2t, false);    
    tbb::flow::function_node<int,int> sink_node(graph, nconcurs, sink);

    for (int ind=0; ind<NGENS; ++ind) {
        tbb::flow::make_edge(gen_nodes[ind], t2z_nodes[ind]);
    }
    tbb::flow::make_edge(z2t_node, sink_node);

    z2t_node.activate();
    for (auto& gn : gen_nodes) {
        gn.activate();
    }
    graph.wait_for_all();    

    for (auto& t2z : t2zs) {
        zsock_destroy(&t2z.sock);
    }
    zsock_destroy(&z2t.sock);

    return 0;

}
