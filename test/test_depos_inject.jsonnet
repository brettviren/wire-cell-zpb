local wc = import "wirecell.jsonnet";
local zmq = import "zmq.jsonnet";

local engine = "TbbFlow";
//local engine = "Pgrapher";

local engine_pi =
    if engine == "Pgrapher"
    then "WireCellPgraph"
    else "WireCellTbb";


local depo_spewer = function(name, origin)
{
    type:"ZpbDepoSource",
    name: name,
    data: {
        nick: name,
        timeout: 10000,
        headers: {
            greeting: "I am %s.  Feed me!" % name
        },
        attributes: {
            jobname: "test-depos",
            stream: "depos%d" % origin,
        },
        connects: [
            "tcp://127.0.0.1:5678",
        ],
    }
};


[
    {
        type: "wire-cell",
        data: {
            plugins: ["WireCellGen","WireCellApps", engine_pi, "WireCellZpb"],
            apps: [engine]
        }
    },

    {
        type: "TbbDataFlowGraph",
        data: {
            max_threads: 4,
        }
    },

    depo_spewer("depospew1", 41),
    depo_spewer("depospew2", 42),    

    {
        type: "DumpDepos",
        name: "depodump1",
    },

    {
        type: "DumpDepos",
        name: "depodump2",
    },

    {
        type: engine,
        data: {
            edges: [
                {
                    tail: { node: "ZpbDepoSource:depospew1", port: 0},
                    head: { node: "DumpDepos:depodump1", port: 0}
                },
                {
                    tail: { node: "ZpbDepoSource:depospew2", port: 0},
                    head: { node: "DumpDepos:depodump2", port: 0}
                }
                
            ]
        }
    },
]
