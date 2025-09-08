/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "MPM-Geomechanics", "index.html", [
    [ "Introduction", "index.html#autotoc_md69", null ],
    [ "Program features", "index.html#autotoc_md70", null ],
    [ "Compiled binaries", "index.html#autotoc_md71", null ],
    [ "Compilation on Windows", "index.html#autotoc_md72", null ],
    [ "Compiling on Linux", "index.html#autotoc_md73", null ],
    [ "Visual Studio Solution", "index.html#autotoc_md74", null ],
    [ "Make Compilation", "index.html#autotoc_md75", null ],
    [ "Documentation", "index.html#autotoc_md76", null ],
    [ "Execution", "index.html#autotoc_md77", null ],
    [ "Development manual", "development_manual.html", [
      [ "Keywords for commit messages", "development_manual.html#development_manual_keywords", [
        [ "Keywords List", "development_manual.html#autotoc_md48", [
          [ "1. doc:", "development_manual.html#autotoc_md49", null ],
          [ "2. feat:", "development_manual.html#autotoc_md50", null ],
          [ "3. fix:", "development_manual.html#autotoc_md51", null ],
          [ "4. refactor:", "development_manual.html#autotoc_md52", null ],
          [ "5. test:", "development_manual.html#autotoc_md53", null ],
          [ "6. perf:", "development_manual.html#autotoc_md54", null ],
          [ "7. todo:", "development_manual.html#autotoc_md55", null ],
          [ "8. hack:", "development_manual.html#autotoc_md56", null ],
          [ "9. warn:", "development_manual.html#autotoc_md57", null ],
          [ "10. debug:", "development_manual.html#autotoc_md58", null ],
          [ "11. chore:", "development_manual.html#autotoc_md59", null ],
          [ "12. deprecated:", "development_manual.html#autotoc_md60", null ]
        ] ]
      ] ],
      [ "Best Practices", "development_manual.html#development_manual_best_practices", null ],
      [ "Parallelization Guidelines", "development_manual.html#development_manual_parallelization_guidelines", [
        [ "Parallelization Guidelines in MPM-Geomechanics", "development_manual.html#autotoc_md61", [
          [ "Use macro-controlled dual versions only when needed", "development_manual.html#autotoc_md63", null ],
          [ "Use a single-version pattern with conditional pragma otherwise", "development_manual.html#autotoc_md65", null ],
          [ "Use of static_cast<int> in for loops", "development_manual.html#autotoc_md66", null ],
          [ "Normalization of OpenMP pragmas", "development_manual.html#autotoc_md67", null ],
          [ "Atomic operations: always take the reference first", "development_manual.html#autotoc_md68", null ]
        ] ]
      ] ]
    ] ],
    [ "Required Programs", "required_programs.html", [
      [ "Required Programs - Windows", "required_programs.html#required_programs_windows", null ],
      [ "Required Programs - Linux", "required_programs.html#required_programs_linux", null ]
    ] ],
    [ "Testing Compilation Manual", "testing_manual.html", [
      [ "Required Programs", "testing_manual.html#testing_manual_required_programs", null ],
      [ "How to Compile the Tests", "testing_manual.html#testing_manual_howto", [
        [ "Windows", "testing_manual.html#testing_manual_windows", null ],
        [ "Linux (Ubuntu/Debian)", "testing_manual.html#testing_manual_linux", null ]
      ] ]
    ] ],
    [ "Theory and Numerical Formulation", "theory_and_numerical_formulation_.html", [
      [ "Introduction to the Material Point Method (MPM)", "theory_and_numerical_formulation_.html#autotoc_md79", null ],
      [ "MPM Formulation", "theory_and_numerical_formulation_.html#autotoc_md80", null ],
      [ "Explicit MPM integration", "theory_and_numerical_formulation_.html#autotoc_md81", null ],
      [ "Central difference Method", "theory_and_numerical_formulation_.html#autotoc_md82", null ],
      [ "Numerical implementation", "theory_and_numerical_formulation_.html#autotoc_md83", null ],
      [ "Stability Requirement", "theory_and_numerical_formulation_.html#autotoc_md84", null ],
      [ "Numerical damping", "theory_and_numerical_formulation_.html#autotoc_md85", [
        [ "Local damping", "theory_and_numerical_formulation_.html#autotoc_md86", null ]
      ] ],
      [ "Explicit MPM Scheme", "theory_and_numerical_formulation_.html#autotoc_md87", null ],
      [ "Update Stress First - USF - Scheme", "theory_and_numerical_formulation_.html#autotoc_md88", null ],
      [ "Update Stress Last - USL - Scheme", "theory_and_numerical_formulation_.html#autotoc_md89", null ],
      [ "Modified Update Stress Last - MUSL - Scheme", "theory_and_numerical_formulation_.html#autotoc_md90", null ],
      [ "References", "theory_and_numerical_formulation_.html#autotoc_md91", null ]
    ] ],
    [ "Verification problems", "verification_problems.html", [
      [ "Bousssinesq's problem", "verification_problems.html#autotoc_md92", [
        [ "Introduction", "verification_problems.html#autotoc_md93", null ],
        [ "Analytical solution", "verification_problems.html#autotoc_md94", null ],
        [ "MPM model and result comparison", "verification_problems.html#autotoc_md95", null ]
      ] ],
      [ "Base acceleration example", "verification_problems.html#autotoc_md96", [
        [ "MPM-Model", "verification_problems.html#autotoc_md97", null ]
      ] ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", "namespacemembers_dup" ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ],
        [ "Enumerator", "namespacemembers_eval.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Enumerator", "functions_eval.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Typedefs", "globals_type.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"BodyCuboid_8h.html",
"Seismic_8h.html#a4aed6a906097df08e741f79131f86f12",
"classMaterial.html#a5790aca903dbc72bbc2d086fbc4b8e74",
"classParticle.html#acd15e9528129cba5244e5a676537f4a5",
"namespaceInput.html#a5de246ace76db47015f0af32c9e7eea6",
"structLoads_1_1NodalPointLoadData.html#a3bcdea5f19060a2e3a9e516686f2606e"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';