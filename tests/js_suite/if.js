const age = 18;

if (age >= 18) {
	console.log("Allowed");
} else {
	console.log("Not allowed");
}

// if > else if > else if > else
const color = "red";

// == also performs type-aware comparison
if (color == "green") {
	console.log("tree");
// === can also be used for the same job 
} else if (color === "yellow") {
	console.log("sun");
} else if (color == "blue") {
	console.log("blue");
} else {
	console.log("apple");
}
