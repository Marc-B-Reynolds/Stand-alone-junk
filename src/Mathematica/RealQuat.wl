(* ::Package:: *)

(* Marc B. Reynolds, 2017 *)
(* Public Domain under http://unlicense.org, see link for details. *)
(* Stripped down quanterions over Reals package, has bugs oh well *)


BeginPackage["RealQuat`"];

modifiyMessage[func_, extra_] := func::"usage" = MessageName[func, "usage"] <> " " <> extra;

RealQuaternions::"usage" = "Implements quaternion over reals.";

RealQuat::"usage"    = "General forms:\n" <>
                       "\tRealQuat[{x,y,z},s] represents the bivector {x,y,z} + scalar s.\n" <>
                       "\tRealQuat[b, s]      represents the bivector b + scalar s\n" <>
                       "Polar coordinate forms:\n" <>
                       "\tRealQuat[Sin[\[Theta]]b, Cos[\[Theta]]]\n" <>
                       "\tRealQuat[Sin[\[Theta]]{x,y,z}, Cos[\[Theta]]]\n";

RealQuatQ::"usage"   = "RealQuatQ[x] test if 'x' is a RealQuat object.";
AssertUnitary::usage = "AssertUnitary[Q] creates an assumption defination that the L2 norm of RealQuat object 'Q' is one.";
AssertPure::usage    = "AssertPure[Q] creates an assumption defination that the scalar component of 'Q' is zero.";

NonCommutativeMultiply::usage = "A**B is the axiomic product, which is associative but not commutative.";


RealQuatPolar::usage  = "Set to 'True' or 'False' to enable/disable polar coordinate assumptions.";

RealQuatRandom::usage ="RealQuatRandom[m] generates a random quaternion of magnitude m, default is one.";


(***** Dealing with reducing vector operations *****)

(* A container for marking symbols as bivectors and another for scalars *)
If[Head[RealQuatB]==Symbol, RealQuatB=Alternatives[]];
If[Head[RealQuatS]==Symbol, RealQuatS=Alternatives[]];

(* functions to manage the containers *)
DeclareBivector[x_] := If[!MemberQ[RealQuatB,x], AppendTo[RealQuatB,x];];
DeclareScalar[x_] := If[!MemberQ[RealQuatS,x], AppendTo[RealQuatS,x];];
SetAttributes[DeclareBivector, {Listable}]
SetAttributes[DeclareScalar, {Listable}]
DeclareBivector::"usage" := "marks a symbol or list of symbols as representing bivectors." ;
DeclareScalar::"usage" := "marks a symbol or list of symbols as representing scalars." ;

(* unmark symbols *)
DeleteBivector[x_Symbol]:=RealQuatB=Complement[RealQuatB,Alternatives[x]];
DeleteScalar[x_Symbol]  :=RealQuatS=Complement[RealQuatS,Alternatives[x]];
DeleteBivector[x_List]  :=RealQuatB=Complement[RealQuatB, x /. List -> Alternatives];
DeleteScalar[x_List]    :=RealQuatS=Complement[RealQuatS, x /. List -> Alternatives];


(*  Older version...clean this up
DeclareScalar[x_] := x /: NumericQ[x] := True;
DeclareScalar::usage = "Tags a symbol to be treated as a scalar value for \
transformation rules.";
SetAttributes[DeclareScalar, {Listable}]

DeleteScalar[x_] := TagUnset[x, NumericQ[x]];
DeleteScalar::usage = "
Remove the tag to treat the symbol as a scalar value.";
SetAttributes[DeleteScalar, {Listable}]
*)



(* Wrapper for TensorReduce using the containers *)
RealQuat /: RealQuatReduce[RealQuat[Q_,q_]] := RealQuat[
  TensorReduce[Q, Assumptions->Element[RealQuatB,Vectors[3,Reals]] && Element[RealQuatS,Reals]],	
  TensorReduce[q, Assumptions->Element[RealQuatB,Vectors[3,Reals]] && Element[RealQuatS,Reals]]];

(* reduction helper to ID scalar values *)
ScalarQ[x_] := MemberQ[RealQuatS,x]||(NumericQ[x] && Head[x] =!= Complex);
ScalarQ[x_Dot] := True;

(* reduction helper to ID bivector values *)
BivectorQ[x_] := MemberQ[RealQuatB,x]||VectorQ[x];
BiVectorQ[x_Cross] := True;

(* Enable/Disable reductions based on potential presences of Hadamard vector \
products. *)
NoHadamardProduct = True;
NoHadamardProduct::"usage" := "Enables/Disables reductions based on the \
assumption of presence of Hadamard products in an expression."; 



(*********************************************************)

(* L2 norm of the bivector (basis) portion *)
RealQuat /: bivectorAbs[RealQuat[Q_,q_]] := Sqrt[Dot[Q,Q]];
bivectorAbs[v_]  := Sqrt[Dot[v,v]];


(* Cayley transforms variants as per http://marc-b-reynolds.github.io/quaternions/2016/05/30/QuatHDAngleCayley.html *)
RealQuat /: fct[RealQuat[Q_,q_]] := RealQuat[Q/(1+q),0];
RealQuat /: ict[RealQuat[Q_,0]]  := RealQuat[2/(1+Dot[Q,Q]) Q, (2/(1+Dot[Q,Q]))-1];
ict[v_]  := RealQuat[2/(1+Dot[v,v]) v, (2/(1+Dot[v,v]))-1];
fct::"usage" := "special case Cayley transform: (Q-1)/(Q+1), unit quaterion (positive scalar) to bivector";
ict::"usage" := "special case Cayley transform: bivector back to H";


(*********************************************************)

(*
    TODO: PULL OUT MORE COMMON SCALAR EXPRESSIONS, AS PER THE POLAR FORM
*)

(* Utility functions *)

RealQuat /: UnitQ[RealQuat[Q_,q_]] := Dot[Q,Q]+q^2===1;
RealQuat /: Element[RealQuat[Q_,q_],e_]   := Element[Insert[Q,q,4],e];

AssertUnitary[RealQuat[Q_,q_]] := Dot[Q,Q]+t^2==1;
AssertPure[RealQuat[Q_,q_]]    := q==0;
RealQuat /: Arg[RealQuat[     Q_,     q_]] := ArcTan[q, Sqrt[Dot[Q,Q]]];

(* extraction using GA grade notation *)
Subscript[AngleBracket[RealQuat[Q_, q_]], 0] := q;
Subscript[AngleBracket[RealQuat[Q_, q_]], 2] := RealQuat[Q, 0];



RealQuat /: NonCommutativeMultiply[RealQuat[L_, l_], RealQuat[R_, r_]] := RealQuat[Cross[L,R] + l R + r L, r l - Dot[L,R]]

(* handle left/right products which reduce to scalars (and do commute) *)
RealQuat /: s_?ScalarQ ** RealQuat[Q_,q_]  := RealQuat[s Q, s q];
RealQuat /: RealQuat[Q_,q_] ** s_?ScalarQ  := RealQuat[s Q, s q];


(*********************************************************)
(* 
  SPECIAL CASING: assume all symbols are scalars if the don't have RealQuat as a header
*)

(* if the bivector portion is zero: it is simply a scalar *)
RealQuat[{0,0,0},s_] := s;
RealQuat[0, s_] := s;

(* addition *)
RealQuat /: Plus[RealQuat[ L_,l_], RealQuat[R_,r_]] := RealQuat[L+R, l+r];
RealQuat /: s_ * RealQuat[Q_,q_] := RealQuat[s Q, s q]; /; ScalarQ[s];
RealQuat /: s_ + RealQuat[Q_,q_] := RealQuat[Q,   s+q]; /; ScalarQ[s];

(* complex conjugate *)
RealQuat /: SuperStar[RealQuat[Q_,q_]] := RealQuat[-Q, q];
RealQuat /: Conjugate[Q:RealQuat[__]]  := SuperStar[Q];

RealQuat /: Norm[RealQuat[     Q_,     q_]] := q^2 + Dot[Q,Q];
RealQuat /: Norm[RealQuat[(s__)Q_,(s__)q_]] := s s Norm[RealQuat[Q,q]];

RealQuat /: Det[Q:RealQuat[__]]             := Norm[Q]^2;
RealQuat /: Abs[Q:RealQuat[__]]             := Sqrt[Norm[Q]];
RealQuat /: Sign[Q:RealQuat[__]]            := Q/Abs[Q];

RealQuat /: Im[RealQuat[Q_,q_]]             := Q;
RealQuat /: Re[RealQuat[Q_,q_]]             := q;


(* NUKE THE LIMIT STUPIDITY *)
RealQuat /: Log[RealQuat[Q_,q_]] := Module[{x},RealQuat[ArcTan[q,bivectorAbs[Q]] Limit[Q/x, x->Sqrt[Dot[Q,Q]]], Log[q q + Dot[Q,Q]]/2]];

(* More rules below: polar and special case of integer *)
RealQuat /: Power[Q:RealQuat[__], N:RealQuat[__]] := Exp[N**Log[Q]];

(* Raise quaternion to a real power -- this defination is way overcomplex, should clean it up *)
RealQuat /: Power[Q:RealQuat[__], 1]  :=Q
RealQuat /: Power[Q:RealQuat[__], -1] :=SuperStar[Q]/Norm[Q]
RealQuat /: Power[Q:RealQuat[__], n_] :=RealQuat[Abs[Q]^n/bivectorAbs[Q] Sin[n Arg[Q]] Im[Q], Abs[Q]^n Cos[n Arg[Q]]]


(* Using the limit is a little silly...*)
TrigScale[B_] := Module[{x}, Limit[Sinh[x]/x, x->bivectorAbs[B]]];
ExpScale[B_]  := Module[{x}, Limit[Sin[x]/x,  x->bivectorAbs[B]]];

RealQuat /: Exp[RealQuat[Q_,q_]] := Exp[q] RealQuat[ExpScale[Q]Q,  Cos[bivectorAbs[Q]]];

(* Reduce *)
RealQuat /: Sqrt[RealQuat[Sin[a_]v_, Cos[a_]] m_:1] := RealQuat[Q/Sqrt[Q.Q] Sinh[2 Sqrt[Q.Q]],Sin[2q]]/(Cos[2q]+Cosh[2 Sqrt[Q.Q]]); 


(* Square root of a quaternion - using the trig half angle identity produces more compact results
  then simply allow this to be promoted to Power. If this defination is made, a specal case version
  needs to be defined below to keep orientation formats returning compact results. *)
  
RealQuat /: Sqrt[Q:RealQuat[__]] := RealQuat[Im[Q]/Sqrt[2(Re[Q]+Abs[Q])], Sqrt[2(Re[Q]+Abs[Q])]/2]

(* square root of unit quaternion -- Sqrt should direct here if known *)
usqrt[RealQuat[B_,s_]]:= RealQuat[1/Sqrt[2+2s] B,(1+s)/Sqrt[2+2s]];

(*************************************************
 * Additional products - direct from definations - should be reduced here..started commenting out some
 *************************************************)

(* Euclidean Inner (Dot) product *)
RealQuat /: Dot[Q:RealQuat[__],Q:RealQuat[__]] := Norm[Q]; (* self dot is norm *)
RealQuat /: Dot[RealQuat[L_,l_], RealQuat[R_,r_]] := (Dot[L,R]+l r);

(*
RealQuat /: Dot[RealQuat[(s__)L_,(s__)l_], RealQuat[(t__)R_,(t__)r_]] := s t (Dot[L,R]+l r);
RealQuat /: Dot[RealQuat[     L_,     l_], RealQuat[(t__)R_,(t__)r_]] :=   t (Dot[L,R]+l r);
RealQuat /: Dot[RealQuat[(s__)L_,(s__)l_], RealQuat[     R_,     r_]] := s   (Dot[L,R]+l r);
*)

(* Cross product = Same as GrassmanOuter - just directly defined *)
RealQuat /: Cross[RealQuat[ L_,l_], RealQuat[R_,r_]] := RealQuat[Cross[L,R], 0];
(*
RealQuat /: Cross[RealQuat[(s__)L_,(s__)l_], RealQuat[(t__)R_,(t__)r_]] := s t RealQuat[Cross[L,R], 0];
RealQuat /: Cross[RealQuat[     L_,     l_], RealQuat[(t__)R_,(t__)r_]] :=   t RealQuat[Cross[L,R], 0];
RealQuat /: Cross[RealQuat[(s__)L_,(s__)l_], RealQuat[ \[AliasDelimiter]    R_,     r_]] := s   RealQuat[Cross[L,R], 0];
*)

(* GrassmanOuter - use wedge as well *)
RealQuat /: Wedge[L:RealQuat[__],R:RealQuat[__]] := Cross[L, R];
RealQuat /: Wedge[A:RealQuat[__], B:RealQuat[__], C:__RealQuat] := Wedge[Wedge[A,B],C];

(* GrassmanInner - direct defination *)
RealQuat /: Vee[A:RealQuat[__], B:RealQuat[__]] := GrassmannInner[A,B];
RealQuat /: Vee[A:RealQuat[__], B:RealQuat[__], C:__RealQuat] := Vee[Vee[A,B],C];

(* Euclidean - symbolic operator *)
RealQuat /: SmallCircle[A:RealQuat[__], B:RealQuat[__]] := Euclidean[A,B];
RealQuat /: SmallCircle[A:RealQuat[__], B:RealQuat[__], C:__RealQuat] := CircleTimes[CircleTimes[A,B],C];

(* EuclideanOuter - symbolic operator *)
RealQuat /: CircleTimes[A:RealQuat[__], B:RealQuat[__]] := EuclideanOuter[A,B];
RealQuat /: CircleTimes[A:RealQuat[__], B:RealQuat[__], C:__RealQuat] := CircleTimes[CircleTimes[A,B],C];


(* 
  Test if 'x' is a valid RealQuat object...this is far from bulletproof. (this is borked)
  Should this maybe be changed to just checking the Head and number of components?

  RealQuat[Fv[t],Fs[t]]  <- this will return false, for example, 
*)

RealQuatQ[x_] := If[(Head[x] === RealQuat)        &&   (* Head is RealQuat *)
                    (Dimensions[x]      === {2})  &&   (* Composed of two parts - this okay? *)
                    (VectorQ[x[[1]]]    === True) &&   (* First part is a Vector *)
                    (Dimensions[x[[1]]] === {3})  &&   (*   composed of three parts - this okay? *)
                    (VectorQ[x[[2]]]    === False)     (* Second part is not a Vector *)
                    ,True, False];


(* One of many *)
RealQuat /: ToRealMatrix[RealQuat[{x_, y_, z_}, t_]] := 
  {{ t, x,-z,-y},
   {-x, t,-y, z},
   { z, y, t, x},
   { y,-z,-x, t}};

RealQuat[{{t_, x_,-z_,-y_},{-x_, t_,-y_, z_},{ z_, y_, t_, x_},{ y_,-z_,-x_, t_}}] := RealQuat[{x,y,z},t];



SymbolicQ[a_]         := ! NumericQ[a];
SymbolicQ[q_RealQuat] := True;
SymbolicQ[RealQuat[{x_, y_, z_}, s_]] := ! (NumericQ[s] && NumericQ[x] && NumericQ[y] && NumericQ[z])


(*
  These special rules should be commented out if attempting to generalize the package, or if incorrect
  results are being generated. These assume that all symbols are of real scalar values.
*)

(**************************************************************************************************************
   SPECIAL RULE: Exp of quaternion in log (Euler identity) form:
     Exp[a v, Log[m]] = Sin[a] v + m
 *)

(*
RealQuat /: Exp[RealQuat[a_ {x_,y_,z_}, m_]] := Exp[m] RealQuat[Sin[a]{x,y,z}, Cos[a]] /; NumericQ[a]
RealQuat /: Exp[RealQuat[a_ v_        , m_]] := Exp[m] RealQuat[Sin[a]v      , Cos[a]] /; NumericQ[a]
*)


(**************************************************************************************************************
  SPECIAL RULES: bivector only
***************************************************************************************************************)

RealQuat /: Log[RealQuat[{x_,y_,z_}, 0] m_:1] := RealQuat[Pi/2 {x,y,z}, Log[m]];
RealQuat /: Log[RealQuat[v_,         0] m_:1] := RealQuat[Pi/2 v, Log[m]];


(**************************************************************************************************************
  SPECIAL RULES: quaternions in a simple orientation like format 
  WARNING: These rules can produce incorrect results, should be verifying that all symbolic contents of 'a' is
  do not exist in other portions.
  
  VERY IMPORTANT: These forms assume that the L2 norm is extracted:
  Q =   RealQuat[Sin[a] v, Cos[a]] -> |Q|=1, |v|=1         (same if v is a list)
  Q = m RealQuat[Sin[a] v, Cos[a]] -> |Q|=m, |v|=1, m>= 0  (same if v is a list)

***************************************************************************************************************)

(* Enable polar coordinate rules by default *)
RealQuatPolar := True;

(* L2 norm of the polar form quaternions -- from assumption of conical form *)
RealQuat /: Abs[RealQuat[Sin[a_]{x_,y_,z_}, Cos[a_]] m_:1] := m /; RealQuatPolar;
RealQuat /: Abs[RealQuat[Sin[a_]v_,         Cos[a_]] m_:1] := m /; RealQuatPolar;

RealQuat /: Norm[RealQuat[Sin[a_]{x_,y_,z_}, Cos[a_]] m_:1] := m m /; RealQuatPolar;
RealQuat /: Norm[RealQuat[Sin[a_]v_,         Cos[a_]] m_:1] := m m /; RealQuatPolar;

(* If it wasn't for the existance of the general Sqrt defination, simply appling Power would create the compact
  result. Since I need the general Sqrt, have to explicitly catch Sqrt's here *)

RealQuat /: Sqrt[RealQuat[Sin[a_]{x_,y_,z_}, Cos[a_]] m_:1] := Sqrt[m] RealQuat[Sin[a/2]{x,y,z}, Cos[a/2]] /; RealQuatPolar;
RealQuat /: Sqrt[RealQuat[Sin[a_]v_,         Cos[a_]] m_:1] := Sqrt[m] RealQuat[Sin[a/2]v,       Cos[a/2]] /; RealQuatPolar;

(* SPECIAL RULES: quaternions in a simple orientation like format with and without scale *)
RealQuat /: Power[RealQuat[Sin[a_]{x_,y_,z_}, Cos[a_]] m_:1, n_] := m^n RealQuat[Sin[n a]{x,y,z}, Cos[n a]] /; RealQuatPolar;
RealQuat /: Power[RealQuat[Sin[a_]v_,         Cos[a_]] m_:1, n_] := m^n RealQuat[Sin[n a]v,       Cos[n a]] /; RealQuatPolar;

(* SPECIAL RULES: inversion -- needed since generalize form as special rule for -1 power *)
RealQuat /: Power[RealQuat[Sin[a_]{x_,y_,z_}, Cos[a_]] m_:1, -1] := 1/m RealQuat[-Sin[a]{x,y,z}, Cos[a]] /; RealQuatPolar;
RealQuat /: Power[RealQuat[Sin[a_]v_,         Cos[a_]] m_:1, -1] := 1/m RealQuat[-Sin[a]v,       Cos[a]] /; RealQuatPolar;

RealQuat /: Log[RealQuat[Sin[a_]{x_,y_,z_}, Cos[a_]] m_:1] := RealQuat[a {x,y,z}, Log[m]] /; RealQuatPolar;
RealQuat /: Log[RealQuat[Sin[a_]v_,         Cos[a_]] m_:1] := RealQuat[a v,       Log[m]] /; RealQuatPolar;

RealQuat /: Exp[RealQuat[Sin[a_]{x_,y_,z_}, Cos[a_]] m_:1] := Exp[m Cos[a]] RealQuat[Sin[m Sin[a]]{x,y,z}, Cos[m Sin[a]]] /; RealQuatPolar;
RealQuat /: Exp[RealQuat[Sin[a_]v_,         Cos[a_]] m_:1] := Exp[m Cos[a]] RealQuat[Sin[m Sin[a]]v,       Cos[m Sin[a]]] /; RealQuatPolar;


(*********************************************************************************************************************
   Products of quaternions in polar form that only vary by scale factors and/or frequency:
 *)

RealQuat /: NonCommutativeMultiply[RealQuat[Sin[(m_:1)a_]v_, Cos[(m_:1)a_]] s_:1, 
                                   RealQuat[Sin[(n_:1)a_]v_, Cos[(n_:1)a_]] t_:1] := s t RealQuat[Sin[(m+n)a]v, Cos[(m+n)a]] /; RealQuatPolar;

RealQuat /: NonCommutativeMultiply[RealQuat[Sin[(m_:1)a_]{x_,y_,z_}, Cos[(m_:1)a_]] s_:1, 
                                   RealQuat[Sin[(n_:1)a_]{x_,y_,z_}, Cos[(n_:1)a_]] t_:1] := s t RealQuat[Sin[(m+n)a]{x,y,z}, Cos[(m+n)a]] /; RealQuatPolar;

(* As above, but dealing with negative powers: AN OPTIONAL SIGN PATTERN WOULD COLLAPSE THESE TO THE TWO ABOVE..humm.. *)
RealQuat /: NonCommutativeMultiply[RealQuat[-Sin[(m_:1)a_]v_, Cos[(m_:1)a_]] s_:1, 
                                   RealQuat[ Sin[(n_:1)a_]v_, Cos[(n_:1)a_]] t_:1] := s t RealQuat[Sin[(n-m)a]v, Cos[(n-m)a]] /; RealQuatPolar;

RealQuat /: NonCommutativeMultiply[RealQuat[ Sin[(m_:1)a_]v_, Cos[(m_:1)a_]] s_:1, 
                                   RealQuat[-Sin[(n_:1)a_]v_, Cos[(n_:1)a_]] t_:1] := s t RealQuat[Sin[(m-n)a]v, Cos[(m-n)a]] /; RealQuatPolar;

RealQuat /: NonCommutativeMultiply[RealQuat[-Sin[(m_:1)a_]{x_,y_,z_}, Cos[(m_:1)a_]] s_:1, 
                                   RealQuat[ Sin[(n_:1)a_]{x_,y_,z_}, Cos[(n_:1)a_]] t_:1] := s t RealQuat[Sin[(n-m)a]{x,y,z}, Cos[(n-m)a]] /; RealQuatPolar;

RealQuat /: NonCommutativeMultiply[RealQuat[ Sin[(m_:1)a_]{x_,y_,z_}, Cos[(m_:1)a_]] s_:1, 
                                   RealQuat[-Sin[(n_:1)a_]{x_,y_,z_}, Cos[(n_:1)a_]] t_:1] := s t RealQuat[Sin[(m-n)a]{x,y,z}, Cos[(m-n)a]] /; RealQuatPolar;


(* Reasonble for symbolic .. sucky for numeric .. fix *)
RealQuat /: Power[RealQuat[V_,S_], n_] := Module[{s = S, v = V,
       ri, r},
        ri = ComplexExpand[{(v Im[(s + I r)^n])/r, 
          Re[(s + I r)^n]}];
        r = Sqrt[Dot[v, v]];
        RealQuat @@ ri] /; IntegerQ[n]




(* Pass through -- SURELY THERE'S A BETTER METHOD *)
RealQuat /: TrigReduce[RealQuat[Q_,q_]] := RealQuat[TrigReduce[Q], TrigReduce[q]]
RealQuat /: TrigFactor[RealQuat[Q_,q_]] := RealQuat[TrigFactor[Q], TrigFactor[q]]
RealQuat /: Collect[RealQuat[Q_,q_],p_] := RealQuat[Collect[Q,p], Collect[q,p]]

(* uniform random unit quaternions. Lazy implementations *)
RealQuatRandomXY[] := With[{p=RandomPoint[Sphere[]]},RealQuat[{p[[1]],p[[2]],0},p[[3]]]];
RealQuatRandomZ[] := With[{p=RandomPoint[Circle[]]},RealQuat[{0,0,p[[2]]},p[[1]]]];
RealQuatRandom[] := (RealQuatRandomXY[]**RealQuatRandomZ[]);
RealQuatRandomZ::usage  ="Generates a uniform random unit quaterion about Z.";
RealQuatRandomXY::usage ="Generates a uniform random unit quaterion with axis in XY-plane.";
RealQuatRandom::usage   ="Generates a uniform random unit quaterion.";

(** SHOULD BE SETTING MORE ATTRIBUTES **)

(* SetAttributes[{Dot, Vee}, {Orderless,Flat,Listable}] *)

RealQuatPackageLoaded = True;

EndPackage[]
