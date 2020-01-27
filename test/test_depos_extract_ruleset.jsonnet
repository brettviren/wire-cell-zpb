// describes rules for the file-server which match
// components used in test_depos_extract.jsonnet 

[
    {
        rule: "(and (= direction 'extract') (or (= dtype 'depo') (= dtype 'frame')))",
        rw: "w",
        filepat: "{jobname}.hdf",
        grouppat: "{extra}/{stream}/{dtype}",
        attr: {extra:"foo"}
    },
    {
        rule: "(and (= direction 'inject') (or (= dtype 'depo') (= dtype 'frame')))",
        rw: "r",
        filepat: "{jobname}.hdf",
        grouppat: "{extra}/{stream}/{dtype}",
        attr: {extra:"bar"}
    },
]
