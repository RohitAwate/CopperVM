const age = 18;

if (age >= 18) {
	print("Allowed");
} else {
	print("Not allowed");
}

// if > else if > else if > else
const color = "red";

// == also performs type-aware comparison
if (color === "green") {
	print("tree");
// === can also be used for the same job 
} else if (color == "yellow") {
	print("sun");
} else if (color == "blue") {
	print("blue");
} else {
	print("apple");
}
