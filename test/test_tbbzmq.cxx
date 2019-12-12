#include <tbb/task_scheduler_init.h>
#include <tbb/flow_graph.h>
#include <czmq.h>

#define NLOOPS 10000

struct TGen {
    int count{0};
    bool operator()(int& num) {
        ++count;
        num = count;
        zsys_debug("gen: %d", num);
        return count <= NLOOPS;
    }
};

// sink/function node body
struct T2Z {
    zsock_t* sock;
    std::string name;
    T2Z(zsock_t* s, std::string n) : sock(s), name(n) {
        assert(sock);
    }
    int operator()(const int& num) {
        assert(sock);
        
        int rc = zsock_send(sock, "i", num);
        if (rc < 0) {
            zsys_error("t2z %s: %d %s", name.c_str(), errno, strerror(errno));
            return num;
        }
        zsys_debug("t2z: %d", num);
        return num;
    }
};

// source node body
struct Z2T {
    zsock_t* sock;
    std::string name;
    Z2T(zsock_t* s, std::string n) : sock(s), name(n) {
        assert(sock);
    }
    bool operator()(int& num) {
        assert(sock);
        int rc = zsock_recv(sock, "i", &num);
        if (rc < 0) {
            zsys_error("z2t %s: %d %s", name.c_str(), errno, strerror(errno));
            return false;
        }
        zsys_debug("z2t: %d", num);
        return num < NLOOPS;    // exit on num == NLOOPS
    }
};

struct Sink {
    int operator()(const int& num) {
        zsys_debug("snk: %d", num);
        return num;
    }
};

int main(int argc, char* argv[])
{
    int nthreads = 1;
    if (argc > 1) {
        nthreads = atoi(argv[1]);
    }
    const char* what = "pubsub";
    if (argc > 2) {
        what = argv[2];
    }

    zsys_init();

    const char* address = "tcp://127.0.0.1:5678";
    zsock_t* pub = NULL;
    zsock_t* sub = NULL;

    if (streq(what, "pubsub")) {
        pub = zsock_new_pub(address);
        sub = zsock_new_sub(address, "");
    }    
    else if (streq(what, "pushpull")) {
        pub = zsock_new_push(address);
        sub = zsock_new_pull(address);
    }    
    else {
        abort();
    }
    assert(pub);
    T2Z t2z(pub, "sender");

    assert(sub);
    Z2T z2t(sub, "recver");

    TGen gen;
    Sink sink;

    zclock_sleep(1000);

    tbb::task_scheduler_init m_sched(nthreads);

    tbb::flow::graph graph;
    tbb::flow::source_node<int> gen_node(graph, gen, false);
    tbb::flow::function_node<int,int> t2z_node(graph, 1, t2z);
    tbb::flow::source_node<int> z2t_node(graph, z2t, false);    
    tbb::flow::function_node<int,int> sink_node(graph, 1, sink);

    // gen -tbb-> t2z =zmq=> z2t -tbb-> sink

    tbb::flow::make_edge(gen_node, t2z_node);
    tbb::flow::make_edge(z2t_node, sink_node);

    z2t_node.activate();
    gen_node.activate();
    graph.wait_for_all();    

    zsock_destroy(&sub);
    zsock_destroy(&pub);

    return 0;

}
