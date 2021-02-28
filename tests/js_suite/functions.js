// cutest-ignore

let alertCounter = 1;

function showAlert(msg) {
	print("Alert #" + alertCounter + ": " + msg);
	alertCounter++;
}

const sayHello = function(name) {
	print("Hello, " + name + "!");
}

let helloWorld = function() {
	print(arguments);
	print("hello, world!");	
}

const arrowFunc = (arg) => {
	print(arg);
	return arg;
}

const arrowExpression = (arg) => print(arg);

const closureFunc = function() {
	const prefix = 'hello';

	function f(arg) {
		print(prefix + arg);
	}
	
	f(14);	
}

const closureArrow = () => {
	const prefix = 'bye';

	let f = (arg) => {
		print(prefix + arg);
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
