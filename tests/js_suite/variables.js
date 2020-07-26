// LET
let i = 100;
let f = 12.25;
let s = "hello world!";
let ss = 'bye world!';
let b = true;

let refers_to_i = i;
let refersToF = f;
let RefersToS = s;

i = 12.125;
f = 100;
b = "bye, world!";
s = false;

let swap = i;
i = f;
f = swap;

swap = b;
b = s;
s = swap;

// CONST
const ic = 100;
const fc = 12.25;
const sc = "hello world";
const ssc = 'bye world';
const bc = true;

let lic = ic;
let lfc = fc;
let lsc = sc;
let lssc = ssc;
let lbc = bc;

lfc = lic;
lsc = lssc;
lbc = lfc;
