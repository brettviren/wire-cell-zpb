local wc = import "wirecell.jsonnet";
local zmq = import "zmq.jsonnet";

//local engine = "TbbFlow";
local engine = "Pgrapher";

local engine_pi =
    if engine == "Pgrapher"
    then "WireCellPgraph"
    else "WireCellTbb";

[
    {
        type: "wire-cell",
        data: {
            plugins: ["WireCellGen","WireCellApps", engine_pi, "WireCellZpb"],
            apps: [engine]
        }
    },

    {
        type:"ZpbDepoSource",
        data: {
            nick: "depospewer",
            timeout: 10000,
            headers: {
                greeting: "Feed me!"
            },
            attributes: {
                jobname: "test-depos",
                stream: "depos",
            },
            connects: [
                "tcp://127.0.0.1:5678",
            ],
        }
    },

    {
        type: "DumpDepos",
    },

    {
        type: engine,
        data: {
            edges: [
                {
                    tail: { node: "ZpbDepoSource", port: 0},
                    head: { node: "DumpDepos", port: 0}
                }
            ]
        }
    },
]
