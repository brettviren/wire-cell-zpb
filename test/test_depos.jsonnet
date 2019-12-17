local wc = import "wirecell.jsonnet";
local zmq = import "zmq.jsonnet";

[
    {
        type: "wire-cell",
        data: {
            plugins: ["WireCellGen","WireCellApps", "WireCellTbb", "WireCellZpb"],
            apps: ["TbbFlow"]
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

    // note: in single threaded mode, things that bind must come first
    // if connects will require discovery.

    {
        type:"ZpbDepoSource",
        data: {
            nick: "depospewer",
            verbose: 1,
            ports: {
                input: {
                    stype: zmq.stype("server"),
                    binds: ["inproc://depo-spigot"],
                }
            }
        }
    },

    {
        type:"ZpbDepoSink",
        data: {
            nick: "depogobbler",
            verbose: 1,
            ports: {
                output: {
                    stype: zmq.stype("client"),
                    connects: [ { nodename: "depospewer", portname: "input" } ],
                }
            }
        }
    },

    {
        type: "DumpDepos",
    },

    {
        type: "TbbFlow",
        data: {
            edges: [

                { tail: { node: "TrackDepos", port: 0},    head: { node: "ZpbDepoSink", port: 0} },
                { tail: { node: "ZpbDepoSource", port: 0}, head: { node: "DumpDepos", port: 0} }
            ]
        }
    },
]
