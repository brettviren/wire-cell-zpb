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
        type:"ZpbDepoSource",
        data: {
            nick: "depospewer",
            verbose: 1,
            timeout: 10000,
            ports: {
                input: {
                    stype: zmq.stype("server"),
                    binds: [ common.address ],
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
                { tail: { node: "ZpbDepoSource", port: 0}, head: { node: "DumpDepos", port: 0} }
            ]
        }
    },
]
