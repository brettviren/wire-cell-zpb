// describes rules for the file-server which match
// components used in test_depos_extract.jsonnet 

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
        attr: {extra:"foo"},
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
        attr: {extra:"bar"}
    },
]
