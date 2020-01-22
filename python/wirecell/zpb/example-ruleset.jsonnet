[
    {
        rule: "(and (= direction 'extract') (or (= type 'depo') (= type 'frame')))",
        rw: "w",
        filepat: "{jobname}.hdf",
        grouppat: "{stream}/{type}",
        attr: {}
    },
    {
        rule: "(and (= direction 'inject') (or (= type 'depo') (= type 'frame')))",
        rw: "r",
        filepat: "{jobname}.hdf",
        grouppat: "{stream}/{type}",
        attr: {}
    },
]
