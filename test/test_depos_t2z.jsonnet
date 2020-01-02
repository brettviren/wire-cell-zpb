local wc = import "wirecell.jsonnet";
local zmq = import "zmq.jsonnet";
local common = import "test_depos_common.jsonnet";
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

    {
        type:"ZpbDepoSink",
        data: {
            nick: "depogobbler",
            verbose: 1,
            ports: {
                output: {
                    stype: zmq.stype("client"),
                    connects: [ common.address ],
                }
            }
        }
    },

    {
        type: "TbbFlow",
        data: {
            edges: [

                { tail: { node: "TrackDepos", port: 0},    head: { node: "ZpbDepoSink", port: 0} },
            ]
        }
    },
]
