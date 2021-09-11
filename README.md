This repository contains the formally verified implementation of the optimized aggregation algorithm described in the bachelor thesis "Verified Incremental Evaluation of Aggregation Operators in Metric First-Order Temporal Logic", and all the scripts used for its empirical evaluation.

Setting up Isabelle
-------------------

The formalization uses the latest development version of Isabelle (September 11, 2011), available [here](https://isabelle.sketis.net/devel/release_snapshot/). Installation instructions and requirements can be found [here](https://isabelle.in.tum.de/website-Isabelle2021/installation.html). 

It also requires the latest version of the [Archive of Formal Proofs](https://foss.heptapod.net/isa-afp/afp-devel/), installed as described [here](https://www.isa-afp.org/using.html).

In order to run Isabelle and view/edit the proofs, run the following command from the project root:

```
isabelle jedit -d '$AFP' -d ./thys ./thys/MFODL_Monitor_Devel/Monitor_Code.thy
```

The implementation and proofs of the `aggaux` data structure can be found in the `./thys/MFODL_Monitor_Devel/Optimized_Agg.thy` theory, while the integration into the temporal operator's algorithm can be found in the `./thys/MFODL_Monitor_Devel/Optimized_Agg_Temporal.thy` theory.

Exporting the verified code
---------------------------

In order to export the verified code directly via the Isabelle GUI, open the `Monitor_Code.thy` theory as described earlier, wait for all the proofs to complete (progress can be seen in the Theories tab on the right hand side), and then pressing the `theory exports` button in the bottom output tab. Afterwards, copy the exported code and replace the contents of the `src/verified.ml` file with it.

In order to export the verified code via command line, run the following command from the root directory:

```
isabelle build -d '$AFP' -o browser_info -c -e -v -D thys/
```

Then again as before replace the contents of the `src/verified.ml` file with the generated `verified.ocaml` code.

Building MonPoly
----------------

As VeriMon is part of MonPoly, in order to use it you must first build MonPoly. Installation instructions and requirements can be found [here](monpoly.md).

MonPoly can then be invoked using `monpoly -log w.log -sig w.sig -formula w.mfotl`, where `w.log` stores the trace, `w.sig` the type signature of the events, and `w.mfotl` the formula we are monitoring. VeriMon can be invoked by passing the `-verified` flag.

Running the experiments
-----------------------

The random experiment generator can be found in the `experiments/gen.cpp` file, which can be built with any c++ compiler, e.g. with:

```
g++ experiments/gen.cpp -o experiments/gen
```

and then by executing `gen`, it will generate logs for the experiments described in the `experiments/tests.txt` file (note that the `gen` command has to be run from the `experiments` folder in order to work). The parameters for the log generator are described in the thesis. The last column (commands) describes which versions of VeriMon/MonPoly to run. Supported are `monpoly`, `verimon` (the optimized version, shorthand for `monpoly -verified`) and `verimon_old` (shorthand for `monpoly_old -verified`). The test results are then written to the `experiments/out.csv` file.

The three experiments shown in the thesis can be found in the `experiments/tests/` folder.

To run the experiments with your own version of VeriMon/MonPoly, replace the `experiments/MonPoly` file with your own compiled version.