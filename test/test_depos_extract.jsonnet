local wc = import "wirecell.jsonnet";
local zmq = import "zmq.jsonnet";

//local engine = "TbbFlow";
local engine = "Pgrapher";

local engine_pi =
    if engine == "Pgrapher"
    then "WireCellPgraph"
    else "WireCellTbb";

local track_depos = function(name) {
    type:"TrackDepos",
    name: name,
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
};

local depo_gobbler = function(name, origin) {
    type:"ZpbDepoSink",
    name: name,
    data: {
        nodename: name,
        origin: origin,
        timeout: 10000,
        headers: {
            greeting: "hello world I am %s"%name // just to show something
        },
        attributes: {       // added to BOT's flow object
            jobname: "test-depos",
            stream: "depos%d"%origin,
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
};

[
    {
        type: "wire-cell",
        data: {
            plugins: ["WireCellGen","WireCellApps", engine_pi, "WireCellZpb"],
            apps: [engine]
        }
    },

    track_depos("depomaker1"),
    track_depos("depomaker2"),

    depo_gobbler("depogobbler1", 41),
    depo_gobbler("depogobbler2", 42),

    {
        type: engine,
        data: {
            edges: [
                {
                    tail: { node: "TrackDepos:depomaker1", port: 0},
                    head: { node: "ZpbDepoSink:depogobbler1", port: 0}
                },
                {
                    tail: { node: "TrackDepos:depomaker2", port: 0},
                    head: { node: "ZpbDepoSink:depogobbler2", port: 0}
                },
            ]
        }
    },
]
