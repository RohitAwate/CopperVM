let homoList = ["apples", "oranges", "watermelons"]; 
let heteroList = [1, 2, "hello", true];
console.log(heteroList[1])
console.log(heteroList[2]);

// Array of objects
let objects = [{name: "Rohit", age: 22}, {name: "Peter", age: 19}];
console.log(objects[0].name);
console.log(objects[0]["name"]);
console.log(objects[0]['name']);

// homogenous arrays in objects
let colors = {
	"primary": ["red", "yellow", "blue"],
	"secondary": ["green", "orange", "purple"],
	"tertiary": ["yellow-orange", "red-orange", "red-purple", "blue-purple", "blue-green", "yellow-green"],
};
console.log(colors["primary"][1]);
console.log(colors['tertiary'][2]);

// superhero name: real name in single quotes, is billionaire, 1 for Marvel/2 for DC
let superheroes = {
	"Batman": ['Bruce Wayne', true, 2],
	"Iron Man": ['Tony Stark', true, 1],
	"Spiderman": ['Peter Parker', false, 2],
	"Superman": ['Clark Kent', false, 2]
};
console.log(superheroes["Batman"]);
console.log(superheroes['Superman']);

// Multi-dimensional arrays
const matrix = [
	[1, 2, 3],
	[4, 5, 6],
	[7, 8, 9]
];
console.log(matrix[0][0]);
console.log(matrix[2][2]);