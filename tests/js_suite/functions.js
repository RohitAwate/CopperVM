let alertCounter = 1;

function showAlert(msg) {
	console.log("Alert #" + alertCounter + ": " + msg);
	alertCounter++;
}

const sayHello = function(name) {
	console.log("Hello, " + name + "!");
}

let helloWorld = function() {
	console.log(arguments);
	console.log("hello, world!");	
}

const arrowFunc = (arg) => {
	console.log(arg);
	return arg;
}

const arrowExpression = (arg) => console.log(arg);

const closureFunc = function() {
	const prefix = 'hello';

	function f(arg) {
		console.log(prefix + arg);
	}
	
	f(14);	
}

const closureArrow = () => {
	const prefix = 'bye';

	let f = (arg) => {
		console.log(prefix + arg);
	}
	
	f(78);	
}

showAlert();
showAlert("hello");
showAlert("bye");

sayHello();
sayHello("Batman");
sayHello("Thor");

helloWorld();
helloWorld(1, true);
helloWorld("hello");

arrowFunc();
arrowFunc(12);

arrowExpression();
arrowExpression(13);

closureFunc();
closureArrow();
