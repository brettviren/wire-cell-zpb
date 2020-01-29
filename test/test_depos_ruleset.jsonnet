// describes rules for the file-server which match
// components used in test_depos_extract.jsonnet 

local base_folder = "bar";

[
    {
        rule: |||
            (and
             (eq direction 'extract')
             (or (eq dtype 'wctzpb.Depo') (eq dtype 'wctzpb.Frame')))
        |||,
        rw: "r",             // direction is w.r.t. file point of view
        filepat: "{jobname}.hdf",
        grouppat: "{extra}/{stream}",
        attr: {extra: base_folder},
    },
    {
        rule: |||
            (and
             (eq direction 'inject')
             (or (eq dtype 'wctzpb.Depo') (eq dtype 'wctzpb.Frame')))
        |||,
        rw: "w",             // direction is w.r.t. file point of view
        filepat: "{jobname}.hdf",
        grouppat: "{extra}/{stream}",
        attr: {extra: base_folder}
    },
]
