local wc = import "wirecell.jsonnet";
local zmq = import "zmq.jsonnet";
local common = import "test_depos_common.jsonnet";

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
        type:"TrackDepos",
        data: {
            step_size: 1.0 * wc.millimeter,
            tracks: [
                {
                    time: 10.0*wc.us,
                    ray : wc.ray(wc.point(1,0,0,wc.cm), wc.point(2,0,0,wc.cm))
                },
                {
                    time: 20.0*wc.us,
                    ray : wc.ray(wc.point(1,0,0,wc.cm), wc.point(1,0,1,wc.cm))
                },
                {
                    time: 30.0*wc.us,
                    ray : wc.ray(wc.point(1,-1,1,wc.cm), wc.point(1,1,1,wc.cm))
                }
            ],
        }
    },

    {
        type:"ZpbDepoSink",
        data: {
            nodename: "depogobbler",
            origin: 42,
            timeout: 10000,
            headers: {
                greeting: "hello world" // just to show
            },
            attributes: {       // added to BOT's flow object
                jobname: "test-depos",
                stream: "depos",
            },
            connects: [
                "tcp://127.0.0.1:5678",
                // {
                // // Use Zyre to find server by node/port names.
                // // wirecell-zpb file-server -n zpbfiles -p flow [...]
                // nodename: "zpbfiles",
                // portname: "flow"
                // }
            ],
        }
    },

    {
        type: engine,
        data: {
            edges: [
                {
                    tail: { node: "TrackDepos", port: 0},
                    head: { node: "ZpbDepoSink", port: 0}
                },
            ]
        }
    },
]
