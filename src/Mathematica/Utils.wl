(* ::Package:: *)

(* Marc B. Reynolds, 2017 *)
(* Public Domain under http://unlicense.org, see link for details. *)

BeginPackage["dumbUtils`"]

(* printf %e output to a list *)
fromPrintfE[a_ b_ + c_] := a 10^c;
SetAttributes[fromPrintfE, {Listable, HoldAll}];

(* printf %a output to a list *)
fromPrintfA0[Hold[Times[a_, Dot[b_, c_]]]] := ToExpression[
  "16^^" <> StringDrop[SymbolName[b], 1] <> "." <> 
   StringDrop[SymbolName[c], -1]];
fromPrintfA[Plus[a__, b__]] := fromPrintfA0[Hold[a]] 2^b;
SetAttributes[fromPrintfA, {Listable, HoldAll}];


EndPackage[]
