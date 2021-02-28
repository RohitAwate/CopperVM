// cutest-ignore

// Object with non-strings as keys
const config = {
	port: 8080,
	https: true,
	domain: 'www.example.com',
};

// Object with non-strings as keys
let fruitColors = {
	"apple": "red",
	"orange": "orange",
	"watermelon": "green"
} // no semicolon

// Nested objects
let people = {
	"Rohit": {
		from: "India",
		github: "RohitAwate",
		age: 22
	},
	'Bruce': {
		'from': "Gotham",
		"github": "Batman",
		age: 40
	},
	Clark: {
		'from': "Metropolis",
		"github": "Superman",
		age: 40
	}, // trailing comma
}

let objects = {
	key1: {},
	"key2": {},
	'key3': {} // no trailing comma
} 
